#include "pch.h"
#include "CBowser.h"
#include "CObjectMgr.h"
#include "CScrollMgr.h"
#include "CBmpMgr.h"
#include "CItem.h"
#include "CBullet.h"
#include <random>
#include <cmath>

const map<BOWSER_STATE, FRAME> CBowser::m_mapBowserFrame = {
    { BOWSER_IDLE,            { 0, 3, 0, 250, 0 } },
    { BOWSER_SWING,           { 0, 3, 0, 100, 0 } },
    { BOWSER_STOMP,           { 0, 3, 0, 150, 0 } },
    { BOWSER_HIDE_DROP,       { 0, 3, 2, 300, 0 } },
    { BOWSER_FIRE_DROP_HIDE,  { 0, 3, 2, 300, 0 } },
    { BOWSER_DEAD_FLYAWAY,    { 0, 3, 3, 100, 0 } },
    { BOWSER_HIT,             { 0, 3, 5, 120, 0 } },
    { BOWSER_DESCEND,         { 0, 3, 0, 150, 0 } }
};

CBowser::CBowser()
    : m_eState(BOWSER_DESCEND), m_fSwingTime(0.f), m_fStompSpeed(0.f), m_bStompStarted(false),
    m_dwDropStartTime(0), m_bDropStarted(false), m_dwFireDropStartTime(0), m_bHideStarted(false),
    m_fDeadSpeedY(0.f), m_dwLastPatternTime(0), m_bWaitingForNextPattern(false), m_iPhase(0),
    m_dwStompStartDelay(0), m_bReturningCenter(false), m_fStompRecoverSpeed(0.f), m_fTargetX(0.f), m_bMovingSide(false)
{
    m_tInfo.fCX = 64.f * SCALE_FACTOR;
    m_tInfo.fCY = 96.f * SCALE_FACTOR;
    m_eDir = DIR_LEFT;
    m_tFrame = m_mapBowserFrame.at(BOWSER_DESCEND);
    m_tFrame.dwTime = GetTickCount();
    m_tInfo.fY = -64.f;
    m_tSpawnPos = { 384.f * SCALE_FACTOR, 320.f * SCALE_FACTOR };
}


CBowser::~CBowser() { Release(); }

void CBowser::Initialize()
{
    m_eState = BOWSER_DESCEND;
    m_tFrame = m_mapBowserFrame.at(m_eState);
    m_tFrame.dwTime = GetTickCount();
    m_dwLastPatternTime = GetTickCount();
    m_iPhase = 0;
}

int CBowser::Update()
{
    Update_State();
    CObject::Move_Frame();
    CObject::Update_Rect();
    return NOEVENT;
}

void CBowser::Late_Update() {}
void CBowser::Release() {}

void CBowser::Render(HDC hDC)
{
    Update_ImageKey();
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    const TCHAR* pFrameKey = (m_eDir == DIR_LEFT) ? L"Bowser_LEFT" : L"Bowser_RIGHT";
    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(pFrameKey);
    if (!hMemDC) return;

    GdiTransparentBlt(
        hDC,
        static_cast<int>(m_tInfo.fX - m_tInfo.fCX * 0.5f - m_fScrollX),
        static_cast<int>(m_tInfo.fY - m_tInfo.fCY * 0.5f - m_fScrollY),
        static_cast<int>(m_tInfo.fCX),
        static_cast<int>(m_tInfo.fCY),
        hMemDC,
        m_tFrame.iStart * 64,
        m_tFrame.iMotion * 96,
        64, 96,
        RGB(0, 255, 0));
}

void CBowser::Update_ImageKey()
{
    // 상태에 맞는 프레임키 설정
    m_pFrameKey = (m_eDir == DIR_LEFT) ? L"Bowser_LEFT" : L"Bowser_RIGHT";
   
}

void CBowser::Set_State(BOWSER_STATE eNewState)
{
    if (m_eState == eNewState) return;

    m_eState = eNewState;

    // 몬스터 클래스와 유사하게 프레임 초기화 로직 수정
    auto it = m_mapBowserFrame.find(m_eState);
    if (it != m_mapBowserFrame.end())
    {
        m_tFrame = it->second;
        m_tFrame.dwTime = GetTickCount();

        // 페이즈에 따라 모션 행 변경
        if (m_eState == BOWSER_HIT)
            m_tFrame.iMotion = 5;
        else if (m_iPhase >= 2)
        {
            if (m_eState == BOWSER_HIDE_DROP)
                m_tFrame.iMotion = 3;
            else if (m_eState == BOWSER_FIRE_DROP_HIDE || m_eState == BOWSER_DEAD_FLYAWAY)
                m_tFrame.iMotion = 4;
            else
                m_tFrame.iMotion = 1;
        }
        else
        {
            if (m_eState == BOWSER_HIDE_DROP)
                m_tFrame.iMotion = 2;
            else
                m_tFrame.iMotion = 0;
        }
    }

    switch (eNewState)
    {
    case BOWSER_DESCEND:         m_tInfo.fY = -64.f; break;
    case BOWSER_SWING:           m_fSwingTime = 0.f; m_tPivot = { m_tInfo.fX, m_tInfo.fY }; break;
    case BOWSER_STOMP:           m_fStompSpeed = -6.f * SCALE_FACTOR; m_dwStompStartDelay = GetTickCount(); break;
    case BOWSER_HIDE_DROP:       m_bDropStarted = false; m_dwDropStartTime = GetTickCount(); break;
    case BOWSER_FIRE_DROP_HIDE:  m_bHideStarted = false; m_dwFireDropStartTime = GetTickCount(); break;
    case BOWSER_DEAD_FLYAWAY:    m_fDeadSpeedY = -5.f; break;
    case BOWSER_HIT:             m_dwLastPatternTime = GetTickCount(); break;
    default: break;
    }
}

void CBowser::Update_State()
{
    if (m_eState == BOWSER_IDLE)
    {
        DWORD dwNow = GetTickCount();
        if (!m_bWaitingForNextPattern)
        {
            m_bWaitingForNextPattern = true;
            m_dwLastPatternTime = dwNow;
        }
        else if (dwNow - m_dwLastPatternTime > 1000)
        {
            Execute_Random_Pattern();
            m_bWaitingForNextPattern = false;
        }
    }
    else
    {
        switch (m_eState)
        {
        case BOWSER_DESCEND:         Pattern_Descend(); break;
        case BOWSER_SWING:           Pattern_Swing(); break;
        case BOWSER_STOMP:           Pattern_Stomp(); break;
        case BOWSER_HIDE_DROP:       Pattern_HideDrop(); break;
        case BOWSER_FIRE_DROP_HIDE:  Pattern_FireDrop(); break;
        case BOWSER_DEAD_FLYAWAY:    Pattern_DeadFlyAway(); break;
        default: break;
        }
    }

    // 자연스러운 좌우 이동 및 중앙 복귀 처리
    if (m_bReturningCenter || m_bMovingSide)
    {
        float dx = m_fTargetX - m_tInfo.fX;
        if (fabs(dx) < 2.f * SCALE_FACTOR)
        {
            m_tInfo.fX = m_fTargetX;
            m_bReturningCenter = false;
            m_bMovingSide = false;
        }
        else
        {
            m_tInfo.fX += dx * 0.1f;
        }
    }
}

void CBowser::Execute_Random_Pattern()
{
    int pattern = rand() % 5;
    switch (pattern)
    {
    case 0: Set_State(BOWSER_SWING); break;
    case 1: Set_State(BOWSER_STOMP); break;
    case 2: Set_State(BOWSER_HIDE_DROP); break;
    case 3:
        m_fTargetX = m_tSpawnPos.x;
        m_bReturningCenter = true;
        break;
    case 4:
        if (rand() % 2 == 0)
            m_fTargetX = m_tSpawnPos.x - 100.f * SCALE_FACTOR;
        else
            m_fTargetX = m_tSpawnPos.x + 100.f * SCALE_FACTOR;
        m_bMovingSide = true;
        break;
    }
}

void CBowser::On_Hit()
{
    ++m_iPhase;
    if (m_iPhase >= 3)
    {
        Set_State(BOWSER_DEAD_FLYAWAY);
        m_bDead = true;
    }
    else
    {
        Set_State(BOWSER_HIT);
    }
}

void CBowser::Pattern_Descend()
{
    m_tInfo.fY += 2.f;
    if (m_tInfo.fY >= m_tSpawnPos.y)
    {
        m_tInfo.fY = m_tSpawnPos.y;
        Set_State(BOWSER_IDLE);
    }
}

void CBowser::Pattern_Swing()
{
    constexpr float RADIUS = 50.f * SCALE_FACTOR;
    constexpr float ANGULAR_SPEED = 0.05f;
    m_fSwingTime += ANGULAR_SPEED;
    m_tInfo.fX = m_tPivot.x + RADIUS * cosf(m_fSwingTime);
    m_tInfo.fY = m_tPivot.y + RADIUS * sinf(m_fSwingTime);
    if (m_fSwingTime >= 2.f * 3.1415926f)
        Set_State(BOWSER_IDLE);
}

void CBowser::Pattern_Stomp()
{
    DWORD now = GetTickCount();
    const float stompBottomY = 432.f * SCALE_FACTOR - m_tInfo.fCY * 0.5f;

    // 사전 예고 단계
    if (now - m_dwStompStartDelay < 400)
    {
        m_tInfo.fY -= 0.5f * SCALE_FACTOR;
        return;
    }

    // 하강 단계
    if (!m_bStompStarted)
    {
        m_fStompSpeed += 2.f * SCALE_FACTOR;
        m_tInfo.fY += m_fStompSpeed;

        if (m_tInfo.fY >= stompBottomY)
        {
            m_tInfo.fY = stompBottomY;
            m_bStompStarted = true;
            m_fStompRecoverSpeed = -1.0f * SCALE_FACTOR;
            m_tFrame.iStart = 3; // 충돌 시 마지막 프레임
        }
    }
    // 복귀(상승) 단계
    else
    {
        m_tInfo.fY -= m_fStompRecoverSpeed;
        m_fStompRecoverSpeed += 0.05f * SCALE_FACTOR;

        if (m_tInfo.fY <= m_tSpawnPos.y)
        {
            m_tInfo.fY = m_tSpawnPos.y;
            m_bStompStarted = false;  //  다음 스톰을 위해 초기화
            m_fStompSpeed = 0.f;
            m_fStompRecoverSpeed = 0.f;
            Set_State(BOWSER_IDLE);
            return;
        }
    }

    // 프레임 애니메이션: 0 > 1 > 2 > 3 > 2 > 1 > 0
    static bool forward = true;
    DWORD tick = GetTickCount();
    if (tick - m_tFrame.dwTime > m_tFrame.dwSpeed)
    {
        if (forward)
        {
            ++m_tFrame.iStart;
            if (m_tFrame.iStart >= 3)
            {
                m_tFrame.iStart = 3;
                forward = false;
            }
        }
        else
        {
            --m_tFrame.iStart;
            if (m_tFrame.iStart <= 0)
            {
                m_tFrame.iStart = 0;
                forward = true;
            }
        }
        m_tFrame.dwTime = tick;
    }
}

void CBowser::Pattern_HideDrop()
{
    if (!m_bDropStarted)
    {
        // 프레임 수동 초기화는 여기서 하지 않음
        m_bDropStarted = true;
    }

    // 프레임 업데이트는 Update()에서 처리

    if (GetTickCount() > m_dwDropStartTime + 1000)
    {
        Set_State(BOWSER_IDLE);
    }
}


void CBowser::Pattern_FireDrop()
{
    if (!m_bHideStarted)
    {
        m_tFrame = m_mapBowserFrame.at(BOWSER_FIRE_DROP_HIDE);
        m_tFrame.dwTime = GetTickCount();
        m_bHideStarted = true;
    }

    if (GetTickCount() > m_dwFireDropStartTime + 1000)
    {
        Set_State(BOWSER_IDLE);
    }
}

void CBowser::Pattern_DeadFlyAway()
{
    m_tInfo.fX += 2.f * SCALE_FACTOR;
    m_tInfo.fY += m_fDeadSpeedY;
    m_fDeadSpeedY += 0.2f * SCALE_FACTOR;
}

void CBowser::Spawn_Mechakoopa(float fx) {}
void CBowser::Spawn_Fire(float fx) {}
