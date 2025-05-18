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
    { BOWSER_DESCEND,         { 0, 3, 0, 150, 0 } },
    { BOWSER_PEACH_HELP,      { 0, 3, 4, 400, 0 } }, // 5행: 피치 공주 등장 모션
};

CBowser::CBowser()
    : m_eState(BOWSER_DESCEND), m_fSwingTime(0.f), m_fStompSpeed(0.f), m_bStompStarted(false),
    m_dwDropStartTime(0), m_bDropStarted(false), m_dwFireDropStartTime(0), m_bHideStarted(false),
    m_fDeadSpeedY(0.f), m_dwLastPatternTime(0), m_bWaitingForNextPattern(false), m_iPhase(0),
    m_dwStompStartDelay(0), m_bReturningCenter(false), m_fStompRecoverSpeed(0.f), m_fTargetX(0.f), m_bMovingSide(false),
    m_dwHitTime(0), m_bPhasePending(false)
{
    m_tInfo.fCX = 64.f * SCALE_FACTOR;
    m_tInfo.fCY = 96.f * SCALE_FACTOR;
    m_eDir = DIR_LEFT;
    m_tFrame = m_mapBowserFrame.at(BOWSER_DESCEND);
    m_tFrame.dwTime = GetTickCount();
    m_tInfo.fY = -64.f;
    m_tSpawnPos = { 384.f * SCALE_FACTOR, 320.f * SCALE_FACTOR - 96.f };
}


CBowser::~CBowser() { Release(); }

void CBowser::Initialize()
{
    m_eState = BOWSER_DESCEND;
    m_tFrame = m_mapBowserFrame.at(m_eState);
    m_tFrame.dwTime = GetTickCount();
    m_dwLastPatternTime = GetTickCount();
    m_iPhase = 0;

    m_bPatternReady = false;
    m_bMovingForPattern = false;
    m_iPendingPattern = -1;

    m_bItemDropped = false;
    m_dwPeachHelpStart = 0;
}

int CBowser::Update()
{

    if (m_bDead)
    {
        CSoundMgr::Get_Instance()->StopSound(SOUND_BGM);
        CSoundMgr::Get_Instance()->PlaySoundW(L"Boss Death.wav", SOUND_EFFECT, 0.5f);
        CObjectMgr::Get_Instance()->Delete_Object(OBJ_MONSTER);
        return DEAD;
    }

    if (m_iPhase >= 2)
    {
        if (!m_bHurry)
        {
            CSoundMgr::Get_Instance()->StopSound(SOUND_BGM);
            CSoundMgr::Get_Instance()->PlayBGM(L"55. Bowser's Last Attack.mp3", 0.5f);
            m_bHurry = true;
        }

    }


    Update_State();
    // 숨는 모션(HIDE_DROP)에 대한 특별 처리
    if (m_eState == BOWSER_HIDE_DROP)
    {
        // 특별한 애니메이션 처리는 Pattern_HideDrop()에서 직접 수행
    }
    else
    {
        // 일반적인 프레임 이동
        CObject::Move_Frame();
    }

    
    CObject::Update_Rect();
    return NOEVENT;
}

void CBowser::Late_Update()
{
    On_Collision(OBJ_MONSTER);
}
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
    case BOWSER_PEACH_HELP:
        m_tFrame.iMotion = 4;
        m_dwPeachHelpStart = GetTickCount(); // 등장 타이머 시작
        break;
    default: break;
    }
}

void CBowser::Update_State()
{
    // 바운더리 제한
    const float fLeft = 256.f * SCALE_FACTOR + m_tInfo.fCX * 0.5f;
    const float fRight = 512.f * SCALE_FACTOR - m_tInfo.fCX * 0.5f;
    const float fTop = 224.f * SCALE_FACTOR + m_tInfo.fCY * 0.5f;
    const float fBottom = (432.f - 16.f) * SCALE_FACTOR - m_tInfo.fCY * 0.5f;

    if (m_tInfo.fX < fLeft)         m_tInfo.fX = fLeft;
    else if (m_tInfo.fX > fRight)   m_tInfo.fX = fRight;

    if (m_tInfo.fY < fTop)          m_tInfo.fY = fTop;
    else if (m_tInfo.fY > fBottom && m_eState != BOWSER_DESCEND)
        m_tInfo.fY = fBottom;

    // 페이즈 업 대기 중이면 HIT 상태 유지
    if (m_bPhasePending)
    {
        if (m_eState != BOWSER_HIT)
            Set_State(BOWSER_HIT);

        if (GetTickCount() - m_dwHitTime > 3000)
        {
            m_bPhasePending = false;
            ++m_iPhase;

            if (m_iPhase >= 3)
            {
                m_bDead = true;
                Set_State(BOWSER_DEAD_FLYAWAY);
            }
            else
            {
                Set_State(BOWSER_IDLE);
            }
        }
        return; // HIT 중에는 다른 상태 처리 안 함
    }

    // 현재 상태별 패턴 처리
    switch (m_eState)
    {
    case BOWSER_DESCEND:         Pattern_Descend(); break;
    case BOWSER_SWING:           Pattern_Swing(); break;
    case BOWSER_STOMP:           Pattern_Stomp(); break;
    case BOWSER_HIDE_DROP:       Pattern_HideDrop(); break;
    case BOWSER_FIRE_DROP_HIDE:  Pattern_FireDrop(); break;
    case BOWSER_DEAD_FLYAWAY:    Pattern_DeadFlyAway(); break;
    case BOWSER_PEACH_HELP:      Pattern_PeachHelp(); break;

    case BOWSER_IDLE:
    {
        DWORD now = GetTickCount();
        if (!m_bWaitingForNextPattern)
        {
            m_bWaitingForNextPattern = true;
            m_dwLastPatternTime = now;
        }
        else
        {
            DWORD dwDelay = (m_iPhase >= 3) ? 700 : 1500;
            if (now - m_dwLastPatternTime > dwDelay)
            {
                m_bWaitingForNextPattern = false;

                if (m_iPhase >= 2 && rand() % 4 == 0)
                {
                    Set_State(BOWSER_PEACH_HELP);
                }
                else
                {
                    Execute_Random_Pattern(); // 드롭, 스윙, 스톰프 중 하나
                }
            }
        }
    }
    break;
    }

    // 패턴 실행 전 이동 중인 경우
    if (m_bMovingForPattern)
    {
        const float fBaseY = m_tSpawnPos.y;

        float dx = m_fTargetX - m_tInfo.fX;
        float dy = fBaseY - m_tInfo.fY;

        // X, Y 둘 다 보정
        if (fabs(dx) < 16.f && fabs(dy) < 16.f)
        {
            m_tInfo.fX = m_fTargetX;
            m_tInfo.fY = fBaseY;
            m_bMovingForPattern = false;
            m_bPatternReady = true;
        }
        else
        {
            m_tInfo.fX += dx * 0.1f;
            m_tInfo.fY += dy * 0.1f;
        }
    }
    // 이동 완료 후 실제 패턴 진입
    else if (m_bPatternReady)
    {
        m_bPatternReady = false;

        switch (m_iPendingPattern)
        {
        case 0: Set_State(BOWSER_HIDE_DROP); break;
        case 1: Set_State(BOWSER_STOMP); break;
        case 2: Set_State(BOWSER_SWING); break;
        }
    }
}

void CBowser::Execute_Random_Pattern()
{
    // 위치 랜덤 선택: 0=왼쪽, 1=중앙, 2=오른쪽
    int pos = rand() % 3;
    const float fLeftBound = 256.f * SCALE_FACTOR + m_tInfo.fCX * 0.5f;
    const float fRightBound = 512.f * SCALE_FACTOR - m_tInfo.fCX * 0.5f;
    const float fCenter = m_tSpawnPos.x;

    switch (pos)
    {
    case 0: m_fTargetX = fLeftBound; break;
    case 1: m_fTargetX = fCenter; break;
    case 2: m_fTargetX = fRightBound; break;
    }
    bool bMekakoopaExists = !CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER).empty();
    // 3. 드롭 패턴 제외한 랜덤 선택
    while (true)
    {
        int pattern = rand() % 3; // 0 = DROP, 1 = STOMP, 2 = SWING
        if (pattern == 0 && bMekakoopaExists)
            continue;

        m_iPendingPattern = pattern;
        break;
    }

    // 이동 상태 설정
    m_bMovingForPattern = true;
    m_bPatternReady = false;
}

void CBowser::On_Hit()
{
    // 이미 히트 상태이거나 페이즈 전환 중이면 무시
    if (m_eState == BOWSER_HIT || m_bPhasePending)
        return;

    // 페이즈 전환 보류 상태로 설정, 실제 페이즈 증가는 나중에 수행
    m_bPhasePending = true;
    m_dwHitTime = GetTickCount();  // 맞은 시간 기록

    // 먼저 맞는 모션으로 변경
    Set_State(BOWSER_HIT);

    // 효과음 재생
    CSoundMgr::Get_Instance()->PlaySoundW(L"stun.wav", SOUND_EFFECT, 0.5f);
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
    // 땅 높이 조정
    const float stompBottomY = (432.f - 16.f) * SCALE_FACTOR - m_tInfo.fCY * 0.5f;

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
            CSoundMgr::Get_Instance()->PlaySoundW(L"slam.wav", SOUND_EFFECT, 0.5f);
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
        m_bDropStarted = true;

        // 바우저 현재 방향 기준으로 떨어뜨리기
        Spawn_Mechakoopa(0.f); // 매개변수는 무시됨(내부에서 방향 기준으로 계산)
    }

    // 모션 애니메이션 특별 처리 - HIDE_DROP 상태일 때 프레임 처리
    DWORD dwCurrentTime = GetTickCount();
    if (dwCurrentTime > m_tFrame.dwTime + m_tFrame.dwSpeed)
    {
        m_tFrame.dwTime = dwCurrentTime;

        static bool bForward = true;

        if (bForward)
        {
            ++m_tFrame.iStart;
            if (m_tFrame.iStart >= m_tFrame.iEnd)
            {
                m_tFrame.iStart = m_tFrame.iEnd;
                bForward = false;
            }
        }
        else
        {
            --m_tFrame.iStart;
            if (m_tFrame.iStart <= 0)
            {
                m_tFrame.iStart = 0;
                bForward = true;
            }
        }
    }

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

void CBowser::Pattern_PeachHelp()
{
    DWORD now = GetTickCount();

    // 아이템 드롭 전 대기
    if (!m_bItemDropped && now > m_dwPeachHelpStart + 800)
    {
        m_bItemDropped = true;

        // 버섯 아이템 생성 (CItem(float x, float y, ITEMID id) 사용)
        CItem* pItem = new CItem(m_tInfo.fX, m_tInfo.fY - m_tInfo.fCY * 0.5f, ITEM_MUSH);
        pItem->Initialize();
        pItem->m_bPopUp = true;
        CObjectMgr::Get_Instance()->Add_Object(OBJ_ITEM, pItem);
        CSoundMgr::Get_Instance()->PlaySoundW(L"item_spawn.wav", SOUND_EFFECT, 0.5f);
    }

    // 일정 시간 후 IDLE 상태로 전환
    if (now > m_dwPeachHelpStart + 1600)
    {
        m_bItemDropped = false;
        Set_State(BOWSER_IDLE);
    }
}

void CBowser::Spawn_Mechakoopa(float fx)
{
    const auto& Objlist = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER);
    if (!Objlist.empty()) return;

    // MekaKoopa 생성
    CMonster* pMekaKoopa = new CMonster();

    // 바우저 방향으로 위치 설정 (플레이어 위치 무시)
    float fSpawnX;
    if (m_eDir == DIR_LEFT)
        fSpawnX = m_tInfo.fX - 80.f; // 바우저 왼쪽에 생성
    else
        fSpawnX = m_tInfo.fX + 80.f; // 바우저 오른쪽에 생성

    // 위치와 크기 설정
    float fSpawnY = m_tInfo.fY - m_tInfo.fCY * 0.5f - 64.f;

    pMekaKoopa->Get_Info()->fX = fSpawnX;
    pMekaKoopa->Get_Info()->fY = fSpawnY;
    pMekaKoopa->Get_Info()->iType = MON_MEKAKOOPA;
    pMekaKoopa->Get_Info()->fCX = 96.f;
    pMekaKoopa->Get_Info()->fCY = 72.f;
    // 방향 설정 - 바우저와 같은 방향으로
    // 기본 상태 설정 - IDLE로 시작 (중력으로 천천히 떨어짐)
    pMekaKoopa->Set_Dir(m_eDir);
    pMekaKoopa->Set_State(MONSTER_IDLE);


    // 초기 낙하 속도 매우 작게 설정 (자연스러운 낙하를 위해)
    pMekaKoopa->Set_FallSpeed(0.5f);
    pMekaKoopa->Set_Jump(true);

    // OBJ_MONSTER에 추가
    CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, pMekaKoopa);

    // 효과음 재생
    // CSoundMgr::Get_Instance()->PlaySoundW(L"meka_spawn.wav", SOUND_EFFECT, 0.5f);
}
void CBowser::Spawn_Fire(float fx) {}


void CBowser::On_Collision(EOBJECTID _id)
{
    // 이미 죽었으면 처리하지 않음
    if (m_bDead) return;

    switch (_id)
    {
    case OBJ_MONSTER:
    {
        list<CObject*>& rMonsterList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER);

        for (auto& pMonster : rMonsterList)
        {
            if (pMonster->Get_Dead())
                continue;

            CMonster* pOtherMonster = static_cast<CMonster*>(pMonster);

            // 메카쿠파인지 확인
            if (pOtherMonster->Get_MonsterID() != MON_MEKAKOOPA)
                continue;

            // 위로 던져졌거나 SHELL_MOVE 상태인지 확인
            if (!(pOtherMonster->Is_UpThrown() ||
                (pOtherMonster->Get_State() == MONSTER_SHELL_MOVE && !pOtherMonster->Is_UpThrown())))
                continue;

            // 충돌 확인
            float fWidth = 0.f, fHeight = 0.f;
            if (CCollisionMgr::Check_Rect(this, pOtherMonster, &fWidth, &fHeight))
            {
                // 바우저는 피격
                On_Hit();

                // 메카쿠파는 소멸
                pOtherMonster->Set_State(MONSTER_DEAD);
                pOtherMonster->Set_Dead();
                // 효과음 재생
                CSoundMgr::Get_Instance()->PlaySoundW(L"boss_hit.wav", SOUND_EFFECT, 0.5f);

                // 한 번만 처리
                break;
            }
        }
    }
    break;
    }
}