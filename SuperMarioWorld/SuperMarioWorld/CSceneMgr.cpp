#pragma once
#include "pch.h"
#include "CSceneMgr.h"

CSceneMgr* CSceneMgr::m_pInstance = nullptr;

CSceneMgr::CSceneMgr() : m_pCurrentScene(nullptr)
{
    #ifdef _DEBUG
        m_pCurrentScene = new CEditor;
        m_eCurScene = SC_EDIT;
    #else
        m_pCurrentScene = new CLogo;
        m_eCurScene = SC_LOGO;
    #endif
}

CSceneMgr::~CSceneMgr()
{
	Release();
}

void CSceneMgr::Initialize()
{	
	m_pCurrentScene->Initialize();
}

int CSceneMgr::Update()
{
    if (m_pCurrentScene->Update() || Get_Clear())
    {
        switch (m_eCurScene)
        {
        case SC_LOGO:
            Change_Scene(SC_MENU);
            break;
        case SC_MENU:
            Change_Scene(SC_STAGE_ONE);
            break;
        case SC_STAGE_ONE:
        case SC_STAGE_TWO:
        // Ŭ���� �����̰� Ŭ���� ������ ���� ������� �ʾ�����
            if (Get_Clear() && !m_bMusic)
            {
                // ��� ����Ʈ ����� BGM ����
                CSoundMgr::Get_Instance()->StopAll();
                CSoundMgr::Get_Instance()->PlayBGM(L"47. Course Clear.mp3", 0.5f);
                m_bMusic = true;
            }
        // Ŭ���� ������ ����Ǿ���, ������ ��������
            else if (m_bMusic && !CSoundMgr::Get_Instance()->Is_ChannelPlaying(SOUND_BGM))
            {
                CSoundMgr::Get_Instance()->PlayBGM(L"48. Fade Out!.mp3", 0.5f);
                // ���� �ʱ�ȭ
                Set_Clear(false);
                m_bMusic = false;
                Change_Scene(SC_LOGO);
            }
        break;
        case SC_STAGE_FINAL:
            Change_Scene(SC_BOSS);   // ���� �������� �� ����
            break;
        case SC_BOSS:
            Change_Scene(SC_END);    // ���� Ŭ���� �� ����
            break;
        case SC_END:
            Change_Scene(SC_MENU);   // ���� ���� �޴���
            break;
        }
    }
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
        CObjectMgr::Get_Instance()->Initialize();
    }
}

void CSceneMgr::Change_Scene(SCENEID eID)
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Release();
        Safe_Delete(m_pCurrentScene);
    }

    // �� ���� �� Ŭ���� ���� ���� �ʱ�ȭ
    Set_Clear(false);
    m_bMusic = false;

    switch (eID)
    {
    case SC_LOGO:
        m_pCurrentScene = new CLogo;
        break;
    case SC_MENU:
        m_pCurrentScene = new CMenu;
        break;
    case SC_EDIT:
        m_pCurrentScene = new CEditor;
        break;
    case SC_WORLD:
     //   m_pCurrentScene = new CWorld;
        break;
    case SC_STAGE_ONE:
        m_pCurrentScene = new CScene01;
        break;
    case SC_STAGE_TWO:
     //   m_pCurrentScene = new CScene02;
        break;
    case SC_STAGE_FINAL:
     //   m_pCurrentScene = new CSceneFinal;
        break;
    case SC_BOSS:
     //   m_pCurrentScene = new CBossStage;
        break;
    case SC_END:
     //   m_pCurrentScene = new CEnding;
        break;
    }

    m_eCurScene = eID;
    m_pCurrentScene->Initialize();
}
