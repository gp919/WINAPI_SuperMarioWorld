#pragma once
#include "pch.h"
#include "CSceneMgr.h"
#include "CKeyMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"

class CMainGame
{
public:
	CMainGame();
	~CMainGame();

public:
	void Initialize();
	void Update();
	void Late_Update();
	void Render();
	void Release();

private:
	HDC hMemDC;
	HBITMAP hBackBmp;
	HBITMAP hOldBmp;

private:
	HDC			m_hDC;
	DWORD		m_dwTime;
	int			m_iFPS;
	LONG		m_lMouseX;
	LONG		m_lMouseY;
	TCHAR		m_szBuffer[128];
	
	
};