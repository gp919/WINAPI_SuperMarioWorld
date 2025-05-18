#pragma once
#include "pch.h"
#include "CObject.h"

enum BOWSER_STATE
{
    BOWSER_DESCEND,           // ���� �ִϸ��̼�
    BOWSER_SWING,             // �¿� ����
    BOWSER_STOMP,             // ���� ����
    BOWSER_HIDE_DROP,         // ���� ���¿��� ��ī���� ���
    BOWSER_FIRE_DROP_HIDE,    // ���� ���¿��� �� ���
    BOWSER_DEAD_FLYAWAY,      // ��� �� ���� ���ư�
    BOWSER_IDLE,              // �⺻ ��� �ִϸ��̼�
    BOWSER_HIT,               // ��ī���Ŀ� �¾��� �� ����
    BOWSER_PEACH_HELP,
    BOWSER_END
};

struct POINTF
{
    float x;
    float y;
    POINTF() : x(0.f), y(0.f) {}
    POINTF(float _x, float _y) : x(_x), y(_y) {}
};

class CBowser : public CObject
{
public:
    CBowser();
    virtual ~CBowser();

    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Late_Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
    virtual void On_Collision(EOBJECTID)override;

    void Set_State(BOWSER_STATE eNewState);
    void Update_State();
    void Update_ImageKey();
    void Execute_Random_Pattern();
    void On_Hit();

private:
    bool m_bReturningCenter;
    BOWSER_STATE m_eState;

    DWORD m_dwPeachHelpStart;
    bool m_bItemDropped;

    POINTF m_tSpawnPos;

    float m_fSwingTime;
    POINTF m_tPivot;

    float m_fStompRecoverSpeed;
    DWORD m_dwStompStartDelay;
    float m_fStompSpeed;
    bool  m_bStompStarted;

    DWORD m_dwDropStartTime;
    bool  m_bDropStarted;

    DWORD m_dwFireDropStartTime;
    bool  m_bHideStarted;

    float m_fDeadSpeedY;
    DWORD m_dwLastPatternTime;
    bool  m_bWaitingForNextPattern;

    int m_iPhase;             // ���� ������ (0~3)

    float m_fTargetX;
    bool m_bMovingSide;

    DWORD m_dwHitTime;        // ���� �ð� ���� ����
    bool m_bPhasePending;     // ������ ��ȯ ���� ����
    bool m_bHurry = false;

    bool m_bPatternReady;        // �̵� �� ���� ���� �غ��
    bool m_bMovingForPattern;    // ���� ������ ���� �̵� ��
    int  m_iPendingPattern;      // ���� ������ ���� ��ȣ

    static const std::map<BOWSER_STATE, FRAME> m_mapBowserFrame;

    void Pattern_Descend();
    void Pattern_Swing();
    void Pattern_Stomp();
    void Pattern_HideDrop();
    void Pattern_FireDrop();
    void Pattern_DeadFlyAway();
    void Pattern_PeachHelp();

    void Spawn_Mechakoopa(float fx);
    void Spawn_Fire(float fx);
};
