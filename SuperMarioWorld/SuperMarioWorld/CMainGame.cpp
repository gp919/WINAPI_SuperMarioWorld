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
	HDC hMemDC = CreateCompatibleDC(m_hDC);
	HBITMAP hBackBmp = CreateCompatibleBitmap(m_hDC, WINCX, WINCY);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBackBmp);
	
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC, GetStockObject(WHITE_BRUSH));
	Rectangle(hMemDC, 0, 0, WINCX, WINCY);
	SelectObject(hMemDC, hOldBrush);

	++m_iFPS;
	if (m_dwTime + 1000 < GetTickCount())
	{
		swprintf_s(m_szBuffer, 128, L"FPS : %d\tHeigt : %d\tX : %ld Y : %ld\t", m_iFPS, g_iHeight-25, m_lMouseX, m_lMouseY);
		SetWindowText(g_hWnd, m_szBuffer);

		m_iFPS = 0;
		m_dwTime = GetTickCount();
	}

	CSceneMgr::Get_Instance()->Render(hMemDC);
	CObjectMgr::Get_Instance()->Render(hMemDC);

	BitBlt(m_hDC, 0, 0, WINCX, WINCY, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBackBmp);
	DeleteDC(hMemDC);
}

void CMainGame::Release()
{
	CSceneMgr::Destroy_Instance();
	CObjectMgr::Destroy_Instance();
	CBmpMgr::Destroy_Instance();
	CKeyMgr::Destroy_Instance();
	ReleaseDC(g_hWnd, m_hDC);


}
