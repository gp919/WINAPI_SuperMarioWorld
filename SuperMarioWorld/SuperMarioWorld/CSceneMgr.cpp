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
        // 0. 타일 1. 몬스터 2. 라인  // enum 타입으로 넣는것 고민하기 싫어서 정수로
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
        }

        CloseHandle(hFile);
        MessageBox(g_hWnd, L"Load 완료", _T("Success"), MB_OK);
        CObjectMgr::Get_Instance()->Initialize();
    }
}

// SCENEID를 받아 현재 SCENE을 가리키는 포인터를 변경
// 기존에 가리키고 있던 SCENE은 제거
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
	// 이하 로고와 같은 방식으로 동작
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
		// 에러 메세지 코드
		break;
	}

	// 포인터 변경후 Initialize
	if (m_pCurrentScene)
	{
		m_pCurrentScene->Initialize();
		pair<float, float> pSize = m_pCurrentScene->Get_MapSize();
		CScrollMgr::Get_Instance()->Set_Size(pSize.first, pSize.second);
		CScrollMgr::Get_Instance()->Scroll_Lock(); // 초기화 위치 제한
	}
	
}
