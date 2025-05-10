#pragma once
#include "pch.h"
#include "Define.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CKeyMgr.h"
#include "CLineMgr.h"

enum class EditorMode {
    TILE_MODE,
    MONSTER_MODE,
    LINE_MODE
};

class CEditor
{
public:
    CEditor();
    ~CEditor();

public:
    void Run();
    void Initialize();
    void Update();
    void Render(HDC hDC);
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

