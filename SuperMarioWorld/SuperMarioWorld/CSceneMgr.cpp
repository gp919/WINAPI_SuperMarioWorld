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
	CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, new CPlayer);
	m_pCurrentScene = new CLogo;
}

int CSceneMgr::Update()
{
	return NOEVENT;
}

void CSceneMgr::Late_Update()
{
}

void CSceneMgr::Render(HDC hDC)
{
}

void CSceneMgr::Release()
{
	CObjectMgr::Get_Instance()->Delete_Object(OBJ_PLAYER);
	Safe_Delete(m_pCurrentScene);
}

// SCENEID를 받아 현재 SCENE을 가리키는 포인터를 변경
// 기존에 가리키고 있던 SCENE은 제거
void CSceneMgr::Change_Scene(SCENEID _id)
{
	if(m_pCurrentScene)
		Safe_Delete(m_pCurrentScene);

	switch (_id)
	{
	case SC_LOGO:
		m_pCurrentScene = new CLogo;
		break;
	// 이하 로고와 같은 방식으로 동작
	case SC_MENU:
		break;
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
	m_pCurrentScene->Initialize();
}
