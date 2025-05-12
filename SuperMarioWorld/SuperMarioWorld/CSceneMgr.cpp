#pragma once
#include "pch.h"
#include "CSceneMgr.h"

CSceneMgr* CSceneMgr::m_pInstance = nullptr;

CSceneMgr::CSceneMgr() : m_pCurrentScene(nullptr)
{
}

CSceneMgr::~CSceneMgr()
{
	Release();
}

void CSceneMgr::Initialize()
{	
	#ifdef _DEBUG
	m_pCurrentScene = new CEditor;
	#else
	m_pCurrentScene = new CScene01;
	#endif
	
	m_pCurrentScene->Initialize();
}

int CSceneMgr::Update()
{
	m_pCurrentScene->Update();
	return NOEVENT;
}

void CSceneMgr::Late_Update()
{
	m_pCurrentScene->Late_Update();
	
}

void CSceneMgr::Render(HDC hDC)
{
	m_pCurrentScene->Render(hDC);
}

void CSceneMgr::Release()
{
	if (m_pCurrentScene)
		m_pCurrentScene->Release();

	Safe_Delete(m_pCurrentScene);
}

void CSceneMgr::Key_Input()
{
	
}

void CSceneMgr::Load_Data()
{
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_MONSTER);
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_TILE);
    CLineMgr::Get_Instance()->Release();
    const wchar_t* wc_Dir = L"../Data/";
    const wchar_t* wc_Dat = L".dat";

    for (auto i = 0; i < MODE_END; i++)
    {
        // 0. Ÿ�� 1. ���� 2. ����  // enum Ÿ������ �ִ°� ����ϱ� �Ⱦ ������
        string s_Name = to_string(i);
        wstring ws_Name(s_Name.begin(), s_Name.end());
        wstring ws_FullPath = wc_Dir + ws_Name + wc_Dat;
        const wchar_t* wc_FullPath = ws_FullPath.c_str();
        HANDLE	hFile = CreateFile(wc_FullPath,		// ���� �̸��� ������ ���
            GENERIC_READ,			// ���� ���� ���(GENERIC_WRITE : ���� ����, GENERIC_READ : �б� ����)
            NULL,					// ���� ���(������ ����� ���¿��� �ٸ� ���μ������� ����ϰ��� �� �� �㰡�� ���ΰ�, null�� ��� �������� ����)
            NULL,					// ���� �Ӽ�(null�� ��� �⺻ ���� �ش��ϴ� ���� ���� ����)
            OPEN_EXISTING,			// ���� ���(CREATE_ALWAYS : ������ ������ ����, ������ �����, OPEN_EXISTING : ������ ���� ��쿡�� �ҷ�����)
            FILE_ATTRIBUTE_NORMAL,  // ���� �Ӽ�(�Ϲ����� ����)
            NULL);					// ������ ������ �Ӽ��� ������ ���ø� ����(������� �ʱ� ������ null)

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MessageBox(g_hWnd, L"Load Error", _T("Error"), MB_OK);
            return;
        }

        DWORD	dwByte(0);
        LINE	tLine{};
        INFO    tInfo{};

        //Release();
        switch (i)
        {
        case MODE_TILE:
            while (true)
            {
                ReadFile(hFile, &tInfo, sizeof(INFO), &dwByte, nullptr);

                if (0 == dwByte)
                    break;

                CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(tInfo));
            }
            break;
        case MODE_MONSTER:
            while (true)
            {
                ReadFile(hFile, &tInfo, sizeof(INFO), &dwByte, nullptr);

                if (0 == dwByte)
                    break;
                CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, new CMonster(tInfo));
            }
            break;
        case MODE_LINE:
            while (true)
            {
                ReadFile(hFile, &tLine, sizeof(LINE), &dwByte, nullptr);

                if (0 == dwByte)
                    break;

                CLineMgr::Get_Instance()->Get_LinetList().push_back(new CLine(tLine));
            }
            break;
        }

        CloseHandle(hFile);
        MessageBox(g_hWnd, L"Load �Ϸ�", _T("Success"), MB_OK);
        CObjectMgr::Get_Instance()->Initialize();
    }
}

// SCENEID�� �޾� ���� SCENE�� ����Ű�� �����͸� ����
// ������ ����Ű�� �ִ� SCENE�� ����
void CSceneMgr::Change_Scene(SCENEID _id)
{
	if (m_pCurrentScene)
	{
		Safe_Delete(m_pCurrentScene);
	}

	switch (_id)
	{
	case SC_LOGO:
		//m_pCurrentScene = new CLogo;
		break;
	// ���� �ΰ�� ���� ������� ����
	case SC_MENU:
		break;
	case SC_TUTORIAL:
		m_pCurrentScene = new CTutorial;
		break;\
	case SC_WORLD:
		break;
	case SC_STAGE_ONE:
		break;
	case SC_STAGE_TWO:
		break;
	case SC_STAGE_FINAL:
		break;
	case SC_BOSS:
		break;
	default:
		// ���� �޼��� �ڵ�
		break;
	}

	// ������ ������ Initialize
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Initialize();
		pair<float, float> pSize = m_pCurrentScene->Get_MapSize();
		CScrollMgr::Get_Instance()->Set_Size(pSize.first, pSize.second);
		CScrollMgr::Get_Instance()->Scroll_Lock(); // �ʱ�ȭ ��ġ ����
	}
	
}
