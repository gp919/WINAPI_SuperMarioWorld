#pragma once
#include "pch.h"
#include "Define.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CKeyMgr.h"
#include "CLine.h"
#include "CLineMgr.h"
#include "CTile.h"
#include "CMonster.h"
#include "CKoopa.h"

enum EDITMODE { MODE_TILE,  MODE_MONSTER,   MODE_LINE, MODE_END  };

class CEditor
{
public:
    CEditor();
    ~CEditor();

public:
    static CEditor* Get_Instance()
    {
        if (!m_pInstance)
            m_pInstance = new CEditor;
        return m_pInstance;
    }

    static void Destroy_Instance()
    {
        if (m_pInstance)
        {
            Safe_Delete(m_pInstance);
        }
    }

public:
    void Run();
    void Initialize();
    void Update();
    void Render(HDC hDC);
    void Release();

public:
    void Move_Scroll();
    void Handle_Mouse_Input();
    void Place_Tile(float , float);
    void Place_Monster(float, float);
    void Place_Line(float , float );
    void Remove_Object(float , float );

    // 그리드 좌표계 변환 함수
    void Screen_To_Grid(LONG , LONG, float* , float* );
    void Grid_To_Screen(float, float, LONG* , LONG* );

    void Save_Data();
    void Load_Data();

private:
    // 이하 enum
    EDITMODE m_eCurEdit;
    TILEID m_eCurTile;
    // 나중에 enum 타입으로 수정
    int m_iCurMonster;
    int m_CurStage;
    

    list<CLine*>	m_Linelist;

    //// 그리드 설정
    int m_iTileSize;   // 타일 크기 (픽셀)
    int m_iMapWidth;   // 맵 너비 (픽셀)
    int m_iMapHeight;  // 맵 높이 (픽셀)


    

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

private:
    static CEditor* m_pInstance;



};

