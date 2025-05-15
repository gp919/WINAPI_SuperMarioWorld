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
        // 클리어 상태이고 클리어 음악을 아직 재생하지 않았으면
            if (Get_Clear() && !m_bMusic)
            {
                // 모든 이펙트 사운드와 BGM 중지
                CSoundMgr::Get_Instance()->StopAll();
                CSoundMgr::Get_Instance()->PlayBGM(L"47. Course Clear.mp3", 0.5f);
                m_bMusic = true;
            }
        // 클리어 음악이 재생되었고, 음악이 끝났으면
            else if (m_bMusic && !CSoundMgr::Get_Instance()->Is_ChannelPlaying(SOUND_BGM))
            {
                CSoundMgr::Get_Instance()->PlayBGM(L"48. Fade Out!.mp3", 0.5f);
                // 상태 초기화
                Set_Clear(false);
                m_bMusic = false;
                Change_Scene(SC_LOGO);
            }
        break;
        case SC_STAGE_FINAL:
            Change_Scene(SC_BOSS);   // 최종 스테이지 → 보스
            break;
        case SC_BOSS:
            Change_Scene(SC_END);    // 보스 클리어 후 엔딩
            break;
        case SC_END:
            Change_Scene(SC_MENU);   // 엔딩 이후 메뉴로
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
        // 0. 타일 1. 몬스터 2. 라인 3. 아이템
        // enum 타입으로 넣는것 고민하기 싫어서 정수로
        string s_Name = to_string(i);
        wstring ws_Name(s_Name.begin(), s_Name.end());
        wstring ws_FullPath = wc_Dir + ws_Name + wc_Dat;
        const wchar_t* wc_FullPath = ws_FullPath.c_str();
        HANDLE	hFile = CreateFile(wc_FullPath,		// 파일 이름을 포함한 경로
            GENERIC_READ,			// 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
            NULL,					// 공유 방식(파일이 개방된 상태에서 다른 프로세스에서 사용하고자 할 때 허가할 것인가, null인 경우 공유하지 않음)
            NULL,					// 보안 속성(null인 경우 기본 값에 해당하는 보안 상태 설정)
            OPEN_EXISTING,			// 생성 방식(CREATE_ALWAYS : 파일이 없으면 생성, 있으면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 불러오기)
            FILE_ATTRIBUTE_NORMAL,  // 파일 속성(일반적인 파일)
            NULL);					// 생성될 파일의 속성을 제공할 템플릿 파일(사용하지 않기 때문에 null)

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

    // 씬 변경 시 클리어 관련 상태 초기화
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
