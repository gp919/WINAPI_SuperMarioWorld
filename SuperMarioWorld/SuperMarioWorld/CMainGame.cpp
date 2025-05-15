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

	// 배경
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/BackGround.bmp", L"BackGround");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Tutorial/Yoshi.bmp", L"Tutorial");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Menu/Menu.bmp", L"Menu");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Menu/Cursor.bmp", L"Cursor");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Menu/Logo_menu.bmp", L"BackLogo");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Logo/Logo.bmp", L"Logo");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/World/World1.bmp", L"World1");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/World/world2.bmp", L"World2");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/World/worldplayer.bmp", L"WorldPlayer");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Scene1/Scene1.bmp", L"Scene1");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Scene1/Logo_1.bmp", L"Logo1");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Scene1/back01.bmp", L"BackGround01");
	
	// 오브젝트
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Tile.bmp", L"Tile");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Item.bmp", L"Item");

	// 마리오 이미지 (좌우)
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/SmallMario_L.bmp", L"Player_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/SmallMario_R.bmp", L"Player_RIGHT");
	// 슈퍼마리오 이미지 (좌우)
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/SuperMario_L.bmp", L"SPlayer_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/SuperMario_R.bmp", L"SPlayer_RIGHT");
	// 불꽃마리오 이미지 (좌우)
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/FlowerMario_L.bmp", L"FPlayer_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/FlowerMario_R.bmp", L"FPlayer_RIGHT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Bullet.bmp", L"Bullet");
	// HUD
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/MarioText.bmp", L"MarioText");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/XMark.bmp", L"X_Mark");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/Time_Text.bmp", L"Time_Text");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/Coin_Icon.bmp", L"Coin_Icon");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/UI/Number.bmp", L"Number");

	// 몬스터 목록
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Goomba_L.bmp", L"Goomba_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Green_Koopa_L.bmp", L"GKoopa_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Red_Koopa_L.bmp", L"RKoopa_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Monty_Mole_L.bmp", L"Mole_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Goomba_R.bmp", L"Goomba_RIGHT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Green_Koopa_R.bmp", L"GKoopa_RIGHT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Red_Koopa_R.bmp", L"RKoopa_RIGHT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Monty_Mole_R.bmp", L"Mole_RIGHT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Monsters/Piranha.bmp", L"Piranha");

	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Death_Star.bmp", L"Death_Star");
	
	CLineMgr::Get_Instance()->Initialize();
	CSceneMgr::Get_Instance()->Initialize();
	
	CBmpMgr::Get_Instance()->PreWarm(hMemDC);
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
