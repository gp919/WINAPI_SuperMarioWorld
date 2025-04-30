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
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/back.bmp", L"Back");
	CSceneMgr::Get_Instance()->Initialize();
	CObjectMgr::Get_Instance()->Initialize();
}

void CMainGame::Update()
{
	// 커서 좌표 표시
	POINT pt{};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	m_lMouseX = pt.x;
	m_lMouseY = pt.y;

	CSceneMgr::Get_Instance()->Update();
	CObjectMgr::Get_Instance()->Update();
}

void CMainGame::Late_Update()
{
	CSceneMgr::Get_Instance()->Late_Update();
	CObjectMgr::Get_Instance()->Late_Update();
}

void CMainGame::Render()
{
	Rectangle(m_hDC, 0, 0, WINCX, WINCY);
	++m_iFPS;
	if (m_dwTime + 000 < GetTickCount())
	{
		swprintf_s(m_szBuffer, 128, L"FPS : %d\tHeigt : %d\tX : %ld Y : %ld\t", m_iFPS, g_iHeight-25, m_lMouseX, m_lMouseY);
		SetWindowText(g_hWnd, m_szBuffer);

		m_iFPS = 0;
		m_dwTime = GetTickCount();
	}

	HDC hBackDC = CBmpMgr::Get_Instance()->Find_Image(L"Back");

	
	//BitBlt(m_hDC, 0, 0, WINCX, WINCY, hBackDC, 0, 0, SRCCOPY);
	//GdiTransparentBlt(m_hDC, 0, 0, WINCX, WINCY, hBackDC, 0, 0, WINCX, WINCY, RGB(255, 255, 255));
	CSceneMgr::Get_Instance()->Render(m_hDC);
	CObjectMgr::Get_Instance()->Render(m_hDC);
}

void CMainGame::Release()
{
	CSceneMgr::Destroy_Instance();
	CObjectMgr::Destroy_Instance();
	CBmpMgr::Destroy_Instance();
	CKeyMgr::Destroy_Instance();
	ReleaseDC(g_hWnd, m_hDC);
}
