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
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_ITEM);
    CLineMgr::Get_Instance()->Release();
    const wchar_t* wc_Dir = L"../Data/";
    const wchar_t* wc_Dat = L".dat";

    for (auto i = 0; i < MODE_END; i++)
    {
        // 0. Ÿ�� 1. ���� 2. ���� 3. ������
        // enum Ÿ������ �ִ°� ����ϱ� �Ⱦ ������
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
        case MODE_ITEM:
            while (true)
            {
                ReadFile(hFile, &tInfo, sizeof(INFO), &dwByte, nullptr);

                if (0 == dwByte)
                    break;
                CObjectMgr::Get_Instance()->Add_Object(OBJ_ITEM, new CItem(tInfo));
            }
            break;
        }

        CloseHandle(hFile);
        MessageBox(g_hWnd, L"Load �Ϸ�", _T("Success"), MB_OK);
        CObjectMgr::Get_Instance()->Initialize();
    }
}
