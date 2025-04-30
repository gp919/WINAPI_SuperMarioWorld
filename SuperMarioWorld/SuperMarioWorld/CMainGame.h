#pragma once
#include "pch.h"
#include "CSceneMgr.h"
#include "CKeyMgr.h"

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
	HDC			m_hDC;
	DWORD		m_dwTime;
	int			m_iFPS;
	LONG		m_lMouseX;
	LONG		m_lMouseY;
	TCHAR		m_szBuffer[128];
	
	
};