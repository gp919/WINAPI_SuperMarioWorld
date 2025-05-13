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

// ���ο� ������Ʈ �߰��� ����
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

    // �׸��� ��ǥ�� ��ȯ �Լ�
    void Screen_To_World(float , float, float* , float* );
    void World_To_Screen(float, float, float* , float* );
    void World_To_Grid(float, float, float*, float*);

    void Save_Data();
    void Load_Data();

private:
    // ���� enum : ���⿡ ���ο� ������Ʈ �߰�
    EDITMODE m_eCurEdit = MODE_TILE;
    TILEID m_eCurTile = TILE_Q;
    MONSTERID m_eCurMon = MON_KOOPA;
    ITEMID m_eCurItem = ITEM_COIN;
    LINEDIR m_eCurLine = LINE_VER;
    // ����, Ÿ���� �����ؼ� ����� ���� Ÿ�� ���� ����
    int m_iType = 0;
    int m_CurStage = 0;
    list<CLine*>	m_Linelist;

private:
    DWORD		m_dwTime;
    TCHAR		m_szBuffer[128];
    wstring m_wcMode = L"TILE";

    LONG		m_lMouseX;
    LONG		m_lMouseY;
    // ȭ�鳻 ��� ��ǥ
    float		m_fScrollX;
    float		m_fScrollY;
    // ��ũ�� ������ �Ǿ��ִ� ����� ���� ��ǥ
    float		m_fWorldX;
    float		m_fWorldY;
    // �׸����� �߽���ǥ ���� ����
    float       m_fGridX;
    float       m_fGridY;

private:
    static CEditor* m_pInstance;



};

