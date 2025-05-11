#pragma once
#include "pch.h"
#include "CMainGame.h"

CMainGame::CMainGame() : m_dwTime(GetTickCount()), m_iFPS(0), m_hDC(0)
{
	ZeroMemory(m_szBuffer, sizeof(m_szBuffer));
}

CMainGame::~CMainGame()
{
	Release();
}

void CMainGame::Initialize()
{

	m_hDC = GetDC(g_hWnd);

	hMemDC = CreateCompatibleDC(m_hDC);
	hBackBmp = CreateCompatibleBitmap(m_hDC, WINCX, WINCY);
	hOldBmp = (HBITMAP)SelectObject(hMemDC, hBackBmp);

	CSoundMgr::Get_Instance()->Initialize();

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/BackGround.bmp", L"BackGround");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Tile.bmp", L"Tile");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Tutorial/Yoshi.bmp", L"Tutorial");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Scene1/Scene1.bmp", L"Scene1");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Scene1/back01.bmp", L"BackGround01");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/SmallMario_L.bmp", L"Player_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/SmallMario_R.bmp", L"Player_RIGHT");
	
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/MarioText.bmp", L"MarioText");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/XMark.bmp", L"X_Mark");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/Time_Text.bmp", L"Time_Text");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/Coin_Icon.bmp", L"Coin_Icon");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/Number.bmp", L"Number");

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Koopa.bmp", L"Koopa");
	

	CLineMgr::Get_Instance()->Initialize();
	CSceneMgr::Get_Instance()->Initialize();
	
	CBmpMgr::Get_Instance()->PreWarm(hMemDC);


	//#ifdef _DEBUG

	//	if (::AllocConsole() == TRUE)
	//	{
	//		FILE* nfp[3];
	//		freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
	//		freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
	//		freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
	//		std::ios::sync_with_stdio();
	//	}

	//#endif // _DEBUG
}

void CMainGame::Update()
{
	CSceneMgr::Get_Instance()->Update();
	CKeyMgr::Get_Instance()->Key_Update();
	CUiMgr::Get_Instance()->Update();
}

void CMainGame::Late_Update()
{
	CSceneMgr::Get_Instance()->Late_Update();
}

void CMainGame::Render()
{
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC, GetStockObject(WHITE_BRUSH));
	Rectangle(hMemDC, 0, 0, WINCX, WINCY);
	SelectObject(hMemDC, hOldBrush);

	++m_iFPS;
	if (m_dwTime + 1000 < GetTickCount())
	{
		swprintf_s(m_szBuffer, 128, L"FPS : %d", m_iFPS);
		SetWindowText(g_hWnd, m_szBuffer);

		m_iFPS = 0;
		m_dwTime = GetTickCount();
	}

	CSceneMgr::Get_Instance()->Render(hMemDC);

	BitBlt(m_hDC, 0, 0, WINCX, WINCY, hMemDC, 0, 0, SRCCOPY);

	
}

void CMainGame::Release()
{
	//#ifdef _DEBUG
	//
	//	FreeConsole();
	//
	//#endif // _DEBUG
	CSoundMgr::Destroy_Instance();
	CSceneMgr::Destroy_Instance();
	CBmpMgr::Destroy_Instance();
	CKeyMgr::Destroy_Instance();
	CScrollMgr::Destroy_Instance();
	CUiMgr::Destroy_Instance();
	
	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBackBmp);
	DeleteDC(hMemDC);

	ReleaseDC(g_hWnd, m_hDC);


}
