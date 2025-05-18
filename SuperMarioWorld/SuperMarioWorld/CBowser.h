#pragma once
#include "pch.h"
#include "CObject.h"

enum BOWSER_STATE
{
    BOWSER_DESCEND,           // 등장 애니메이션
    BOWSER_SWING,             // 좌우 스윙
    BOWSER_STOMP,             // 낙하 공격
    BOWSER_HIDE_DROP,         // 숨은 상태에서 메카쿠파 드롭
    BOWSER_FIRE_DROP_HIDE,    // 숨은 상태에서 불 드롭
    BOWSER_DEAD_FLYAWAY,      // 사망 시 우상단 날아감
    BOWSER_IDLE,              // 기본 대기 애니메이션
    BOWSER_HIT,               // 메카쿠파에 맞았을 때 연출
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

    int m_iPhase;             // 현재 페이즈 (0~3)

    float m_fTargetX;
    bool m_bMovingSide;

    DWORD m_dwHitTime;        // 맞은 시간 저장 변수
    bool m_bPhasePending;     // 페이즈 전환 보류 상태
    bool m_bHurry = false;

    bool m_bPatternReady;        // 이동 후 패턴 실행 준비됨
    bool m_bMovingForPattern;    // 패턴 실행을 위한 이동 중
    int  m_iPendingPattern;      // 다음 실행할 패턴 번호

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
