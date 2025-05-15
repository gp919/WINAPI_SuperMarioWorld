
#include "pch.h"
#include "CMonster.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CLineMgr.h"
#include "CUiMgr.h"

// Static 데이터
const map<pair<MONSTERID, MONSTER_STATE>, FRAME> CMonster::m_mapFrame = {
    // Goomba
    {{MON_GOOMBA, MONSTER_WALK}, {0, 3, 0, 100, 0}},
    {{MON_GOOMBA, MONSTER_STOMPED}, {0, 3, 1, 25, 0}},

    // Green Koopa
    {{MON_GREENKOOPA, MONSTER_EJECTED}, {0, 1, 0, 100, 0}},    // 1열: EJECTED 움직임
    {{MON_GREENKOOPA, MONSTER_STOMPED}, {0, 0, 1, 0, 0}},      // 2열: 밟힌 모션
    {{MON_GREENKOOPA, MONSTER_SHELL_IDLE}, {0, 0, 2, 0, 0}},  // 3열: 껍질 idle
    {{MON_GREENKOOPA, MONSTER_SHELL_MOVE}, {0, 3, 2, 50, 0}}, // 3열: 껍질 회전
    {{MON_GREENKOOPA, MONSTER_WALK}, {0, 1, 3, 200, 0}},      // 4열: 걷기

    // Red Koopa
    {{MON_REDKOOPA, MONSTER_EJECTED}, {0, 1, 0, 100, 0}},
    {{MON_REDKOOPA, MONSTER_STOMPED}, {0, 0, 1, 0, 0}},
    {{MON_REDKOOPA, MONSTER_SHELL_IDLE}, {0, 0, 2, 0, 0}},
    {{MON_REDKOOPA, MONSTER_SHELL_MOVE}, {0, 3, 2, 50, 0}},
    {{MON_REDKOOPA, MONSTER_WALK}, {0, 1, 3, 200, 0}},

    // Mole
    {{MON_MOLE, MONSTER_IDLE}, {0, 1, 0, 200, 0}},

    // Piranha
    {{MON_PIRANHA, MONSTER_IDLE}, {0, 1, 0, 200, 0}}
};

// Static 데이터 - 방향별 이미지 키
const map<pair<MONSTERID, OBJECTDIR>, const TCHAR*> CMonster::m_mapImage = {
    // Goomba
    {{MON_GOOMBA, DIR_LEFT}, L"Goomba_LEFT"},
    {{MON_GOOMBA, DIR_RIGHT}, L"Goomba_RIGHT"},
    // Green Koopa
    {{MON_GREENKOOPA, DIR_LEFT}, L"GKoopa_LEFT"},
    {{MON_GREENKOOPA, DIR_RIGHT}, L"GKoopa_RIGHT"},
    // Red Koopa
    {{MON_REDKOOPA, DIR_LEFT}, L"RKoopa_LEFT"},
    {{MON_REDKOOPA, DIR_RIGHT}, L"RKoopa_RIGHT"},
    // Mole
    {{MON_MOLE, DIR_LEFT}, L"Mole_LEFT"},
    {{MON_MOLE, DIR_RIGHT}, L"Mole_RIGHT"},
    // Piranha (방향 구분 없음)
    {{MON_PIRANHA, DIR_LEFT}, L"Piranha"},
    {{MON_PIRANHA, DIR_RIGHT}, L"Piranha"}
};

CMonster::CMonster()
    : m_eMonID(MON_GOOMBA)
    , m_eState(MONSTER_IDLE)
    , m_bMove(false)
    , m_fFallSpeed(0.f)
    , m_dwDeadTime(0)
    , m_dwTime(0)
    , m_pHolder(nullptr)
{
    m_eId = OBJ_MONSTER;
    m_eDir = DIR_LEFT;
}

CMonster::CMonster(float _fX, float _fY, MONSTERID _eID)
    : CMonster()  // 위임 생성자 사용
{
    m_tInfo.fX = _fX;
    m_tInfo.fY = _fY;
    m_tInfo.iType = _eID;
    m_eMonID = _eID;
}

CMonster::CMonster(INFO _tInfo)
    : CMonster()
{
    m_tInfo = _tInfo;
    m_eMonID = (MONSTERID)m_tInfo.iType;
}

CMonster::~CMonster()
{
    Release();
}

void CMonster::Initialize()
{
    m_eMonID = (MONSTERID)m_tInfo.iType;

    // 상태가 이미 설정되어 있으면 그대로 유지 (EJECTED 등)
    if (m_eState != MONSTER_IDLE)
    {
        // 이미 상태가 설정되어 있는 경우 (예: EJECTED)
        // 해당 상태에 맞는 크기 설정
        switch (m_eState)
        {
        case MONSTER_EJECTED:
        case MONSTER_SHELL_IDLE:
        case MONSTER_SHELL_MOVE:
        case MONSTER_STOMPED:
            m_tInfo.fCX = TILECX * SCALE_FACTOR;
            m_tInfo.fCY = TILECX * SCALE_FACTOR;  // 미니쿠파 크기
            break;
        default:
            // 일반 크기
            switch (m_eMonID)
            {
            case MON_GREENKOOPA:
            case MON_REDKOOPA:
                m_tInfo.fCX = TILECX * SCALE_FACTOR;
                m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
                break;
            default:
                m_tInfo.fCX = TILECX * SCALE_FACTOR;
                m_tInfo.fCY = TILECY * SCALE_FACTOR;
                break;
            }
            break;
        }

        // 상태별 속도 및 설정
        switch (m_eState)
        {
        case MONSTER_EJECTED:
            m_fSpeed = 2.f;
            m_bMove = true;
            break;
        case MONSTER_SHELL_MOVE:
            m_fSpeed = 8.f;
            m_bMove = true;
            break;
        case MONSTER_SHELL_IDLE:
            m_fSpeed = 0.f;
            m_dwTime = GetTickCount();
            break;
        }
    }
    else
    {
        // 상태가 설정되지 않은 경우 기본 초기화
        // 크기 설정
        switch (m_eMonID)
        {
        case MON_GREENKOOPA:
        case MON_REDKOOPA:
            m_tInfo.fCX = TILECX * SCALE_FACTOR;
            m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
            break;
        default:
            m_tInfo.fCX = TILECX * SCALE_FACTOR;
            m_tInfo.fCY = TILECY * SCALE_FACTOR;
            break;
        }

        m_fSpeed = 1.f;

        // 상태 설정
        switch (m_eMonID)
        {
        case MON_MOLE:
        case MON_PIRANHA:
            Set_State(MONSTER_IDLE);
            m_bMove = false;
            break;
        default:
            Set_State(MONSTER_WALK);
            m_bMove = false;
            break;
        }
    }

    Update_ImageKey();
    Allign_Bottom(m_tInfo.fX, m_tInfo.fY);
}

int CMonster::Update()
{
    if (m_bDead)
    {
        if (GetTickCount() > m_dwDeadTime + 500)
        {
            CObject* pEffect = new CEffect(m_tInfo.fX, m_tInfo.fY, EFFECT_DEATH_STAR);
            CObjectMgr::Get_Instance()->Add_Object(OBJ_EFFECT, pEffect);

            CUiMgr::Get_Instance()->Set_Score(100);
            return DEAD;
        }

        Move_Frame();
        return NOEVENT;
    }

    // 플레이어에게 잡혀있는 경우
    if (m_pHolder)
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pHolder);

        // 플레이어가 잡기 키를 놓았는지 확인
        if (!pPlayer->Is_Grab())
        {
            Release_From_Holder();
            return NOEVENT;
        }

        // 플레이어와 껍질 위치 계산 (약간 겹치도록)
        float fPlayerHalfWidth = pPlayer->Get_Info()->fCX * 0.5f;
        float fShellHalfWidth = m_tInfo.fCX * 0.5f;
        float fOverlap = 15.f;  // 겹치는 정도 (숫자가 클수록 더 겹침)

        if (pPlayer->Get_Dir() == DIR_RIGHT)
            m_tInfo.fX = pPlayer->Get_Info()->fX + fPlayerHalfWidth + fShellHalfWidth - fOverlap;
        else
            m_tInfo.fX = pPlayer->Get_Info()->fX - fPlayerHalfWidth - fShellHalfWidth + fOverlap;

        // Y 좌표도 약간 위로 올리기
        m_tInfo.fY = pPlayer->Get_Info()->fY - 5.f;  // 5픽셀 위로

        Move_Frame();
        Update_Rect();
        return NOEVENT;
    }

    // 상태별 시간 체크
    if (m_eState == MONSTER_SHELL_IDLE)
    {
        if (GetTickCount() > m_dwTime + 7000)
        {
            Set_State(MONSTER_EJECTED);
        }
    }

    // 화면 범위 체크
#ifndef _DEBUG
// 화면에 들어왔을 때 움직임 시작 (한 번만 체크)
    if (!m_bMove && In_Screen())
    {
        switch (m_eMonID)
        {
        case MON_MOLE:
        case MON_PIRANHA:
            // 이 몬스터들은 특별한 조건에만 움직임
            break;
        default:
            // WALK 상태이거나 특수 상태일 때만 움직임 시작
            if (m_eState == MONSTER_WALK || m_eState == MONSTER_EJECTED || m_eState == MONSTER_SHELL_MOVE)
                m_bMove = true;
            break;
        }
    }
#else
// 디버그 모드에서는 움직이지 않음 (단, 특수 상태는 예외)
    if (m_eState == MONSTER_EJECTED || m_eState == MONSTER_SHELL_MOVE)
        m_bMove = true;
#endif

    // 움직임 업데이트
    if (m_bMove)
        Update_AI();

    Move_Frame();
    return NOEVENT;
}

void CMonster::Update_AI()
{
    if (m_eDir == DIR_LEFT)
        m_tInfo.fX -= m_fSpeed;
    else
        m_tInfo.fX += m_fSpeed;
}

void CMonster::Update_ImageKey()
{
    auto it = m_mapImage.find(make_pair(m_eMonID, m_eDir));
    if (it != m_mapImage.end())
    {
        m_pFrameKey = it->second;
    }
}

void CMonster::Late_Update()
{
    // 잡혀있는 상태면 중력과 충돌 체크 스킵
    if (m_pHolder)
    {
        Update_Rect();
        return;
    }

    On_Collision(OBJ_MONSTER);
    if (m_tInfo.fY >= 1272.f || m_tInfo.fX <= 0.f)
    {
        m_bDead = true;
        return;
    }

    // 등껍질 상태일 때 화면 밖으로 나가면 삭제
    if ((m_eState == MONSTER_SHELL_IDLE || m_eState == MONSTER_SHELL_MOVE) && !In_Screen())
    {
        m_bDead = true;
        return;
    }

    Apply_Gravity();
    Update_Rect();
}

void CMonster::Apply_Gravity()
{
    m_fFallSpeed += GRAVITY;

    if (m_fFallSpeed > MAX_FALL_SPEED)
        m_fFallSpeed = MAX_FALL_SPEED;

    m_tInfo.fY += m_fFallSpeed;

    float fY = 0.f;
    if (CLineMgr::Get_Instance()->Collision_Line(m_tInfo, &fY))
    {
        if (m_tInfo.fY + m_tInfo.fCY * 0.5f >= fY)
        {
            m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
            m_fFallSpeed = 0.f;
        }
    }
}

bool CMonster::In_Screen()
{
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
    float fMargin = 100.f;
    if (m_tInfo.fX + m_tInfo.fCX * 0.5f + fMargin < m_fScrollX ||
        m_tInfo.fX - m_tInfo.fCX * 0.5f - fMargin > m_fScrollX + WINCX ||
        m_tInfo.fY + m_tInfo.fCY * 0.5f + fMargin < m_fScrollY ||
        m_tInfo.fY - m_tInfo.fCY * 0.5f - fMargin > m_fScrollY + WINCY)
    {
        return false;
    }
    return true;
}

void CMonster::Render(HDC hDC)
{
    Update_ImageKey();
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

    GdiTransparentBlt(hDC,
        (int)(m_tInfo.fX - m_tInfo.fCX * 0.5f - m_fScrollX),
        (int)(m_tInfo.fY - m_tInfo.fCY * 0.5f - m_fScrollY),
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        m_tFrame.iStart * TILECX,
        m_tFrame.iMotion * TILECX,
        (int)(m_tInfo.fCX / SCALE_FACTOR),
        (int)(m_tInfo.fCY / SCALE_FACTOR),
        RGB(0, 255, 0));
}

void CMonster::Release()
{
}

void CMonster::On_Collision(EOBJECTID _id)
{
    if (m_bDead) return;

    switch (_id)
    {
    case OBJ_MONSTER:
    {
        list<CObject*>& rMonsterList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER);

        // 자신과 다른 몬스터들 간의 충돌을 체크
        for (auto& pMonster : rMonsterList)
        {
            if (pMonster == this || pMonster->Get_Dead())
                continue;

            CMonster* pOtherMonster = static_cast<CMonster*>(pMonster);

            // Check_Rect를 사용하여 충돌 확인
            float fWidth = 0.f, fHeight = 0.f;
            if (CCollisionMgr::Check_Rect(this, pOtherMonster, &fWidth, &fHeight))
            {
                // 움직이는 껍질 vs 일반 몬스터
                if (m_eState == MONSTER_SHELL_MOVE && pOtherMonster->Get_State() != MONSTER_SHELL_MOVE)
                {
                    // 충돌한 몬스터가 죽을 때 이펙트 생성
                    CObject* pEffect = new CEffect(pOtherMonster->Get_Info()->fX,
                        pOtherMonster->Get_Info()->fY,
                        EFFECT_DEATH_STAR);
                    CObjectMgr::Get_Instance()->Add_Object(OBJ_EFFECT, pEffect);

                    pOtherMonster->Set_Dead();
                    continue;
                }

                // 일반 몬스터 vs 움직이는 껍질
                if (m_eState != MONSTER_SHELL_MOVE && pOtherMonster->Get_State() == MONSTER_SHELL_MOVE)
                {
                    m_bDead = true;
                    return;
                }

                // 움직이는 껍질 vs 움직이는 껍질
                if (m_eState == MONSTER_SHELL_MOVE && pOtherMonster->Get_State() == MONSTER_SHELL_MOVE)
                {
                    // 충돌 시 양쪽 다 죽음
                    m_bDead = true;
                    pOtherMonster->Set_Dead();
                    return;
                }

                // 일반 몬스터끼리 충돌 시 방향 전환 (좌우 충돌일 때만)
                if (m_eState == MONSTER_WALK && pOtherMonster->Get_State() == MONSTER_WALK)
                {
                    // 충돌 방향 판단
                    if (fWidth > fHeight) // 상하 충돌은 무시
                        continue;

                    // 좌우 충돌 시 방향 전환
                    if (m_tInfo.fX < pOtherMonster->Get_Info()->fX)
                    {
                        // 내가 왼쪽에 있으면 왼쪽으로 전환
                        m_eDir = DIR_LEFT;
                        pOtherMonster->Set_Dir(DIR_RIGHT);
                    }
                    else
                    {
                        // 내가 오른쪽에 있으면 오른쪽으로 전환
                        m_eDir = DIR_RIGHT;
                        pOtherMonster->Set_Dir(DIR_LEFT);
                    }
                }
            }
        }
    }
    break;
    }
}

void CMonster::Set_State(MONSTER_STATE _eState)
{
    if (m_eState == _eState)
        return;

    m_eState = _eState;
    Init_Frame();

    // 쿠파일 경우 크기변경
    if (Is_Koopa())
    {
        switch (_eState)
        {
        case MONSTER_WALK:
            m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
            m_fSpeed = 1.f;
            break;

        case MONSTER_SHELL_IDLE:
        case MONSTER_SHELL_MOVE:
        case MONSTER_EJECTED:
        case MONSTER_STOMPED:
            m_tInfo.fCY = TILECX * SCALE_FACTOR;
            break;
        }
    }

    switch (_eState)
    {
    case MONSTER_SHELL_IDLE:
        m_fSpeed = 0.f;
        m_dwTime = GetTickCount();
        break;

    case MONSTER_SHELL_MOVE:
        m_fSpeed = 8.f;
        m_bMove = true;  // 껍질 움직임은 항상 활성화
        break;

    case MONSTER_EJECTED:
        m_fSpeed = 2.f;  // 슬라이딩 속도
        m_bMove = true;  // 항상 움직임
        break;

    case MONSTER_STOMPED:
        m_bDead = true;
        m_dwDeadTime = GetTickCount();
        break;
    }
}

void CMonster::Init_Frame()
{
    auto it = m_mapFrame.find(make_pair(m_eMonID, m_eState));
    if (it != m_mapFrame.end())
        m_tFrame = it->second;

    m_tFrame.dwTime = GetTickCount();
}

void CMonster::On_Stomped()
{
    // 이미 처리 중이면 return (중복 방지)
    static DWORD dwLastStompTime = 0;
    if (GetTickCount() - dwLastStompTime < 100)
        return;
    dwLastStompTime = GetTickCount();

    CSoundMgr::Get_Instance()->PlaySoundW(L"stomp.wav", SOUND_EFFECT, 0.1f);
    if (Is_Koopa())
    {
        switch (m_eState)
        {
        case MONSTER_WALK:
        {
            // 현재 위치 저장
            float fCurrentX = m_tInfo.fX;
            float fCurrentY = m_tInfo.fY;

            // 플레이어 방향 확인
            list<CObject*>& playerList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER);
            OBJECTDIR pushDir = DIR_RIGHT;

            if (!playerList.empty())
            {
                CObject* pPlayer = playerList.front();
                pushDir = (pPlayer->Get_Info()->fX < m_tInfo.fX) ? DIR_RIGHT : DIR_LEFT;
            }

            // 현재 쿠파를 껍질로 변경
            Set_State(MONSTER_SHELL_IDLE);

            // EJECTED 쿠파 생성 - 밀려난 위치로 직접 생성
            float fPushDistance = 30.f;
            float fEjectedX = fCurrentX + ((pushDir == DIR_RIGHT) ? fPushDistance : -fPushDistance);

            // EJECTED 쿠파 생성 - 상태를 생성자에서 직접 설정
            CMonster* pEjected = new CMonster();
            pEjected->m_tInfo.fX = fEjectedX;
            pEjected->m_tInfo.fY = fCurrentY;
            pEjected->m_tInfo.iType = m_eMonID;
            pEjected->m_eMonID = m_eMonID;
            pEjected->Set_State(MONSTER_EJECTED);  // Initialize 전에 상태 설정
            pEjected->Set_Dir(pushDir);
            pEjected->m_bMove = true;  // 바로 움직이도록 설정

            // 여기서는 Add_Object를 통한 자동 초기화를 피하기 위해
            // Initialize()를 직접 호출하지 않습니다.
            // AfterInit이 호출되면 그 때 처리됩니다.
            CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, pEjected);

            return;
        }

        case MONSTER_SHELL_IDLE:
        {
            // 껍질을 차는 처리
            list<CObject*>& playerList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER);
            if (!playerList.empty())
            {
                CObject* pPlayer = playerList.front();
                m_eDir = (pPlayer->Get_Info()->fX < m_tInfo.fX) ? DIR_RIGHT : DIR_LEFT;
            }
            Set_State(MONSTER_SHELL_MOVE);
        }
        return;

        case MONSTER_SHELL_MOVE:
            Set_State(MONSTER_SHELL_IDLE);
            return;

        case MONSTER_EJECTED:
            // EJECTED 상태에서 밟히면 바로 죽음
            Set_State(MONSTER_STOMPED);
            return;
        }
    }

    Set_State(MONSTER_STOMPED);
}
void CMonster::Release_From_Holder()
{
    if (!m_pHolder)
        return;

    CPlayer* pPlayer = static_cast<CPlayer*>(m_pHolder);
    m_pHolder = nullptr;

    // 껍질을 플레이어 방향으로 차기
    if (m_eState == MONSTER_SHELL_IDLE)
    {
        // 던질 때 위치 보정 (플레이어와 약간 떨어뜨림)
        float fThrowDistance = 10.f;
        if (pPlayer->Get_Dir() == DIR_RIGHT)
            m_tInfo.fX += fThrowDistance;
        else
            m_tInfo.fX -= fThrowDistance;

        On_Kicked(pPlayer->Get_Dir());
    }
}

void CMonster::On_Kicked(OBJECTDIR _dir)
{
    if (m_eState != MONSTER_SHELL_IDLE)
        return;

    CSoundMgr::Get_Instance()->PlaySoundW(L"kick.wav", SOUND_EFFECT, 0.1f);
    Set_State(MONSTER_SHELL_MOVE);
    m_eDir = _dir;
    m_bMove = true;

    // 차는 방향으로 초기 속도 부여
    if (_dir == DIR_RIGHT)
        m_tInfo.fX += 20.f;
    else
        m_tInfo.fX -= 20.f;
}
