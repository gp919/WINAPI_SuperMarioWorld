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
#include "CScene.h"
#include "CItem.h"

// 새로운 오브젝트 추가시 변경
enum EDITMODE { MODE_TILE, MODE_MONSTER, MODE_LINE, MODE_ITEM, MODE_END };
enum LINEDIR { LINE_HOR, LINE_VER, LINE_ANG, LINE_END };

class CEditor : public CScene
{
public:
    CEditor();
    virtual ~CEditor();


public:
    void Initialize()override;
    int Update()override;
    void Late_Update()override;
    void Render(HDC)override;
    void Release()override;
    pair<float, float> Get_MapSize() { return { 5120.f, 432.f }; };

public:
    void Move_Scroll();
    void Handle_Mouse_Input();
    void Key_Input();
    void Place_Object(float, float);
    void Place_Line(float, float);

    // 그리드 좌표계 변환 함수
    void Screen_To_World(float , float, float* , float* );
    void World_To_Screen(float, float, float* , float* );
    void World_To_Grid(float, float, float*, float*);

    void Save_Data();
    void Load_Data();

private:
    // 이하 enum : 여기에 새로운 오브젝트 추가
    EDITMODE m_eCurEdit = MODE_TILE;
    TILEID m_eCurTile = TILE_Q;
    MONSTERID m_eCurMon = MON_KOOPA;
    ITEMID m_eCurItem = ITEM_COIN;
    LINEDIR m_eCurLine = LINE_VER;
    // 몬스터, 타일을 통합해서 출력을 위한 타입 저장 변수
    int m_iType = 0;
    int m_CurStage = 0;
    list<CLine*>	m_Linelist;

private:
    DWORD		m_dwTime;
    TCHAR		m_szBuffer[128];
    wstring m_wcMode = L"TILE";

    LONG		m_lMouseX;
    LONG		m_lMouseY;
    // 화면내 상대 좌표
    float		m_fScrollX;
    float		m_fScrollY;
    // 스크롤 보정이 되어있는 월드맵 절대 좌표
    float		m_fWorldX;
    float		m_fWorldY;
    // 그리드의 중심좌표 관리 변수
    float       m_fGridX;
    float       m_fGridY;

private:
    static CEditor* m_pInstance;



};

