#include "pch.h"
#include "CMainGame.h"

CMainGame::CMainGame() : m_dwTime(GetTickCount()), m_iFPS(0)
{
	ZeroMemory(m_szFPS, sizeof(m_szFPS));
}

CMainGame::~CMainGame()
{
	Release();
}

void CMainGame::Initialize()
{
	m_hDC = GetDC(g_hWnd);
	CSceneMgr::Get_Instance()->Initialize();
}

void CMainGame::Update()
{
	//CSceneMgr::Get_Instance()->Update();
}

void CMainGame::Late_Update()
{
	//CSceneMgr::Get_Instance()->Late_Update();
}

void CMainGame::Render()
{
	// CSceneMgr::Get_Instance()->Render(m_hDC);
}

void CMainGame::Release()
{
	CSceneMgr::Get_Instance()->Destroy_Instance();
}
