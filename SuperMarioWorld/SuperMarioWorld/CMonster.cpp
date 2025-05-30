
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
    {{MON_GOOMBA, MONSTER_STOMPED}, {0, 0, 1, 0, 0}},
    {{MON_GOOMBA, MONSTER_DEAD}, {0, 0, 1, 0, 0}},

    // Green Koopa
    {{MON_GREENKOOPA, MONSTER_EJECTED}, {0, 1, 0, 100, 0}},    // 1열: EJECTED 움직임
    {{MON_GREENKOOPA, MONSTER_STOMPED}, {0, 0, 1, 0, 0}},      // 2열: 밟힌 모션
    {{MON_GREENKOOPA, MONSTER_SHELL_IDLE}, {0, 0, 2, 0, 0}},  // 3열: 껍질 idle
    {{MON_GREENKOOPA, MONSTER_SHELL_MOVE}, {0, 3, 2, 50, 0}}, // 3열: 껍질 회전
    {{MON_GREENKOOPA, MONSTER_WALK}, {0, 1, 3, 200, 0}},      // 4열: 걷기
    {{MON_GREENKOOPA, MONSTER_DEAD}, {0, 0, 1, 0, 0}},

    // Red Koopa
    {{MON_REDKOOPA, MONSTER_EJECTED}, {0, 1, 0, 100, 0}},
    {{MON_REDKOOPA, MONSTER_STOMPED}, {0, 0, 1, 0, 0}},
    {{MON_REDKOOPA, MONSTER_SHELL_IDLE}, {0, 0, 2, 0, 0}},
    {{MON_REDKOOPA, MONSTER_SHELL_MOVE}, {0, 3, 2, 50, 0}},
    {{MON_REDKOOPA, MONSTER_WALK}, {0, 1, 3, 200, 0}},
    {{MON_REDKOOPA, MONSTER_DEAD}, {0, 0, 1, 0, 0}},

    // Mole
    {{MON_MOLE, MONSTER_IDLE},      {0, 1, 0, 300, 0}}, // 1열
    {{MON_MOLE, MONSTER_UP},        {0, 1, 1, 200, 0}}, // 2열
    {{MON_MOLE, MONSTER_EJECTED},   {0, 0, 2, 0, 0}},   // 3열
    {{MON_MOLE, MONSTER_WALK},      {0, 1, 3, 200, 0}}, // 4열
    {{MON_MOLE, MONSTER_STOMPED},   {0, 4, 4, 100, 0}}, // 5열
    {{MON_MOLE, MONSTER_DEAD},   {0, 4, 4, 100, 0}},

    // Piranha
    {{MON_PIRANHA, MONSTER_IDLE},   {0, 1, 0, 300, 0}}, // 기본 움직임 (2개)
    {{MON_PIRANHA, MONSTER_UP},     {0, 1, 0, 300, 0}},  
    {{MON_PIRANHA, MONSTER_DOWN},   {0, 1, 0, 300, 0}},
    {{MON_PIRANHA, MONSTER_HIDDEN}, {0, 0, 0, 0, 0}}, // 정지 이미지 재사용
    {{MON_REDKOOPA, MONSTER_DEAD}, {0, 0, 1, 0, 0}},

    // MekaKoopa
    {{MON_MEKAKOOPA, MONSTER_IDLE}, {0, 3, 0, 100, 0}},
    {{MON_MEKAKOOPA, MONSTER_SHELL_IDLE}, {0, 0, 1, 0, 0}},
    {{MON_MEKAKOOPA, MONSTER_SHELL_MOVE}, {0, 2, 1, 133, 0}},
    {{MON_MEKAKOOPA, MONSTER_DEAD}, {0, 5, 2, 66, 0}},
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
    {{MON_PIRANHA, DIR_RIGHT}, L"Piranha"},
    // Meka Koopa
    {{MON_MEKAKOOPA, DIR_LEFT }, L"MekaKoopa_LEFT" },
    {{MON_MEKAKOOPA, DIR_RIGHT}, L"MekaKoopa_RIGHT"}
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
            // 메카쿠파일 경우 크기 예외 처리
            if (m_eMonID == MON_MEKAKOOPA)
            {
                m_tInfo.fCX = 96.f;  // 명확한 크기 지정
                m_tInfo.fCY = 72.f;  // 명확한 크기 지정
            }
            else
            {
                m_tInfo.fCX = TILECX * SCALE_FACTOR;
                m_tInfo.fCY = TILECX * SCALE_FACTOR;  // 미니쿠파 크기
            }
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
            case MON_MEKAKOOPA:
                m_tInfo.fCX = 96.f;  // 명확한 크기 지정
                m_tInfo.fCY = 72.f;  // 명확한 크기 지정
                m_fSpeed = 2.f;
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
        case MON_MEKAKOOPA:
            m_tInfo.fCX = 96.f;  // 명확한 크기 지정
            m_tInfo.fCY = 72.f;  // 명확한 크기 지정
            m_fSpeed = 2.f;
            m_bMove = true;
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
            Set_State(MONSTER_IDLE);
            m_fUpTargetY = m_tInfo.fY - 30.f;
            m_bMove = false;
            break;
        case MON_PIRANHA:
            m_fPipeBottomY = m_tInfo.fY;
            m_fPipeTopY = m_tInfo.fY - 40.f;
            m_bMove = false;
            break;
        case MON_MEKAKOOPA:
            Set_State(MONSTER_IDLE);
            m_fSpeed = 2.f;
            m_bMove = true;
            break;
        default:
            Set_State(MONSTER_WALK);
            m_bMove = false;
            break;
        }
    }

    // 몬스터별 추가 위치 세팅
    if (m_eMonID == MON_PIRANHA)
    {
        m_fPipeBottomY = m_tInfo.fY;        // 현재 위치를 파이프 아래로 지정
        m_fPipeTopY = m_tInfo.fY - 100.f;    // 올라올 최대 위치
        Set_State(MONSTER_HIDDEN);
    }

    if (m_eMonID == MON_MOLE)
    {
        m_fUpTargetY = m_tInfo.fY - 60.f;   // 두더지가 튀어오를 목표 높이
        Set_State(MONSTER_IDLE);
    }

    Update_ImageKey();
    Allign_Bottom(m_tInfo.fX, m_tInfo.fY);
}

int CMonster::Update()
{
    // 메카쿠파가 화면 밖에서 들어올 때 속도 제한
    if (m_eMonID == MON_MEKAKOOPA && !m_bMove && In_Screen())
    {
        m_bMove = true;
        if (m_eState == MONSTER_IDLE)
        {
            // 활성화되면 느린 속도로 시작
            m_fSpeed = 1.5f;
        }
    }

    if (m_bDead)
    {
        if (m_eState != MONSTER_DEAD)
        {
            Set_State(MONSTER_DEAD);
        }

        if (GetTickCount() > m_dwDeadTime + 500)
        {
            CUiMgr::Get_Instance()->Set_Score(1, m_tInfo.fX, m_tInfo.fY);
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

    if (m_eMonID == MON_MOLE || m_eMonID == MON_PIRANHA)
        Update_AI();
    else if (m_bMove)
        Update_AI();

    Move_Frame();
    return NOEVENT;
}

void CMonster::Update_AI()
{
    // 두더지
    if (m_eMonID == MON_MOLE)
    {
        switch (m_eState)
        {
        case MONSTER_IDLE:
            if (Player_In_Range(150.f))
                Set_State(MONSTER_UP);
            break;

        case MONSTER_UP:
            m_tInfo.fY -= 3.f;
            if (!m_bSpringSoundPlayed && m_tInfo.fY >= 0.f)
            {
                CSoundMgr::Get_Instance()->PlaySoundW(L"spring.wav", SOUND_EFFECT, 0.5f);
                m_bSpringSoundPlayed = true;
            }
            if (m_tInfo.fY <= m_fUpTargetY)
            {
                Set_State(MONSTER_WALK);
            }

            break;

        case MONSTER_WALK:
            if (m_eDir == DIR_LEFT) m_tInfo.fX -= m_fSpeed;
            else m_tInfo.fX += m_fSpeed;

            if (GetTickCount() > m_dwTime + 4000)  // 4초 지나면 삭제
                m_bDead = true;
            break;
        }
        return;
    }

    // 피라냐
    if (m_eMonID == MON_PIRANHA)
    {
        switch (m_eState)
        {
        case MONSTER_HIDDEN:
            if (Player_Distance_High())
            {
                DWORD now = GetTickCount();
                if (now - m_dwTime > 2000)
                    Set_State(MONSTER_UP);
            }
            break;

        case MONSTER_UP:
            m_tInfo.fY -= 2.5f;
            if (m_tInfo.fY <= m_fPipeTopY)
                Set_State(MONSTER_IDLE);
            break;

        case MONSTER_IDLE:
            if (GetTickCount() > m_dwTime + 3000)
                Set_State(MONSTER_DOWN);
            break;

        case MONSTER_DOWN:
            m_tInfo.fY += 2.5f;
            if (m_tInfo.fY >= m_fPipeBottomY)
                Set_State(MONSTER_HIDDEN);
            break;
        }
        return;
    }

    // 메카쿠파 처리
    if (m_eMonID == MON_MEKAKOOPA)
    {
        // 위로 던져진 SHELL_MOVE 상태일 때만 특별 처리
        if (m_bUpThrown && m_eState == MONSTER_SHELL_MOVE)
        {
            // 상승 단계
            if (m_fFallSpeed < 0)
            {
                // 중력 적용
                m_fFallSpeed += GRAVITY * 0.5f;
                m_tInfo.fY += m_fFallSpeed;

                // 최고점 도달 시 하강 모드로 전환
                if (m_fFallSpeed >= 0)
                {
                    m_bDescending = true;
                }
            }
            // 하강 단계
            else if (m_bDescending)
            {
                // 화면 하단 체크만 수행 (라인, 타일, 플레이어 무시)
                m_fFallSpeed += GRAVITY * 0.5f;

                if (m_fFallSpeed > MAX_FALL_SPEED)
                    m_fFallSpeed = MAX_FALL_SPEED;

                m_tInfo.fY += m_fFallSpeed;

                // 라인 충돌 체크
                float fY = 0.f;
                if (CLineMgr::Get_Instance()->Collision_Line(m_tInfo, &fY))
                {
                    if (m_tInfo.fY + m_tInfo.fCY * 0.5f >= fY)
                    {
                        m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
                        m_fFallSpeed = 0.f;
                        m_bUpThrown = false;
                        m_bDescending = false;
                        Set_State(MONSTER_IDLE);
                        m_fSpeed = 2.f;
                        return;
                    }
                }
            }

            // 기본 이동 (X축)
            // 기존 코드...
        }

        // IDLE, SHELL_IDLE, 일반 SHELL_MOVE 상태 처리
        // 파이널씬 경계 체크
        float fLeftBound = 256.f * SCALE_FACTOR + m_tInfo.fCX * 0.5f;
        float fRightBound = 512.f * SCALE_FACTOR - m_tInfo.fCX * 0.5f;
        float fGroundY = (432.f - 16.f) * SCALE_FACTOR - m_tInfo.fCY * 0.5f;

        // IDLE 상태 (일반 걷기)
        if (m_eState == MONSTER_IDLE)
        {
            // 좌우 이동
            if (m_eDir == DIR_LEFT)
                m_tInfo.fX -= m_fSpeed;
            else
                m_tInfo.fX += m_fSpeed;

            // 경계 체크
            if (m_tInfo.fX <= fLeftBound)
            {
                m_tInfo.fX = fLeftBound;
                m_eDir = DIR_RIGHT;
                Update_ImageKey();
            }
            else if (m_tInfo.fX >= fRightBound)
            {
                m_tInfo.fX = fRightBound;
                m_eDir = DIR_LEFT;
                Update_ImageKey();
            }

            // 항상 땅 위에 있도록 함
            m_tInfo.fY = fGroundY;
        }

        return; // 메카쿠파 처리 완료
    }

    // 일반 몬스터 이동
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

    // 화면 밖으로 나가면 삭제
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

#ifndef _DEBUG
    // 위로 던진 SHELL_MOVE 상태일 때만 특별 처리
    if (m_eMonID == MON_MEKAKOOPA && m_bUpThrown && m_eState == MONSTER_SHELL_MOVE)
    {
        // 중력 처리 스킵 (Update_AI에서 직접 처리)
    }
    else
    {
        // 두더지와 피라냐는 위로 올라오는 중이면 중력 무시
        bool bIgnoreGravity =
            (m_eMonID == MON_MOLE && m_eState == MONSTER_IDLE) ||
            (m_eMonID == MON_MOLE && m_eState == MONSTER_UP) ||
            (m_eMonID == MON_PIRANHA && m_eState == MONSTER_HIDDEN) ||
            (m_eMonID == MON_PIRANHA && m_eState == MONSTER_UP);

        if (!bIgnoreGravity)
            Apply_Gravity();
    }
#else
    // 디버그 모드에선 중력 무시
#endif

    Update_Rect();
}

void CMonster::Apply_Gravity()
{
    // 메카쿠파일 경우 충돌 검사 반복 횟수 증가
    int nSteps = (m_eMonID == MON_MEKAKOOPA) ? 6 : 1;
    float fStepSize = GRAVITY / nSteps;

    // 한 번에 큰 변화 대신 작은 단계로 나누어 처리
    for (int i = 0; i < nSteps; ++i)
    {
        // 점진적으로 속도 증가
        m_fFallSpeed += fStepSize;

        if (m_fFallSpeed > MAX_FALL_SPEED)
            m_fFallSpeed = MAX_FALL_SPEED;

        // 점진적으로 위치 변경
        float fMoveStep = m_fFallSpeed / nSteps;
        m_tInfo.fY += fMoveStep;

        // 매 단계마다 충돌 검사
        float fY = 0.f;
        if (CLineMgr::Get_Instance()->Collision_Line(m_tInfo, &fY))
        {
            float fCollisionMargin = (m_eMonID == MON_MEKAKOOPA) ? 2.0f : 0.5f;
            if (m_tInfo.fY + m_tInfo.fCY * 0.5f >= fY - fCollisionMargin)
            {
                m_tInfo.fY = fY - m_tInfo.fCY * 0.5f - 1.0f;  // 약간의 추가 마진
                m_fFallSpeed = 0.f;

                // 충돌 발생하면 루프 종료
                break;
            }
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
    if (m_eMonID == MON_MEKAKOOPA)
    {
        // 메카쿠파 원본 크기
        const int srcWidth = 32;
        const int srcHeight = 24;

        // 출력 크기는 정확히 96x72 (3배)
        const int drawWidth = 96;
        const int drawHeight = 72;

        GdiTransparentBlt(hDC,
            (int)(m_tInfo.fX - drawWidth * 0.5f - m_fScrollX),  // 중앙 기준 출력 위치 X
            (int)(m_tInfo.fY - drawHeight * 0.5f - m_fScrollY), // 중앙 기준 출력 위치 Y
            drawWidth,  // 출력 너비
            drawHeight, // 출력 높이
            hMemDC,
            m_tFrame.iStart * srcWidth,    // 소스 X 오프셋
            m_tFrame.iMotion * srcHeight,  // 소스 Y 오프셋
            srcWidth,  // 소스 너비
            srcHeight, // 소스 높이
            RGB(0, 255, 0));
    }
    else
    {
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
            if ((pOtherMonster->Get_MonsterID() == MON_MEKAKOOPA) && pOtherMonster->Is_Descending())
            {
                break;
            }

            // Check_Rect를 사용하여 충돌 확인
            float fWidth = 0.f, fHeight = 0.f;
            if (CCollisionMgr::Check_Rect(this, pOtherMonster, &fWidth, &fHeight))
            {
                // 움직이는 껍질 vs 일반 몬스터
                if (m_eState == MONSTER_SHELL_MOVE && pOtherMonster->Get_State() != MONSTER_SHELL_MOVE)
                {
                    pOtherMonster->Set_Dead();
                    CSoundMgr::Get_Instance()->PlaySoundW(L"kick.wav",SOUND_EFFECT, 0.5f);
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
                    CSoundMgr::Get_Instance()->PlaySoundW(L"kick.wav", SOUND_EFFECT, 0.5f);
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
    {
        // 피라냐나 두더지의 경우 같은 상태여도 프레임 재설정이 필요함
        switch (_eState)
        {
        case MONSTER_IDLE:
        case MONSTER_HIDDEN:
        case MONSTER_UP:
        case MONSTER_DOWN:
            Init_Frame();  // 재시작
            m_dwTime = GetTickCount();  // 타이머 초기화
            return;

        default:
            return;
        }
    }

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
        case MONSTER_DEAD:
            m_tInfo.fCY = TILECX * SCALE_FACTOR;
            m_dwDeadTime = GetTickCount();  // 죽은 시점 기록
            m_fSpeed = 0.f;
            m_bMove = false;
            break;
        }
    }

    // 메카쿠파 상태 변경 시 특별 처리
    if (m_eMonID == MON_MEKAKOOPA)
    {
        switch (_eState)
        {
        case MONSTER_IDLE:
            m_tInfo.fCX = 96.f;
            m_tInfo.fCY = 72.f;
            m_fSpeed = 2.f;
            m_bMove = true;
            break;

        case MONSTER_SHELL_IDLE:
            m_tInfo.fCX = 96.f;
            m_tInfo.fCY = 72.f;
            m_fSpeed = 0.f;
            m_dwTime = GetTickCount();
            break;

        case MONSTER_SHELL_MOVE:
            // 크기 유지하면서 속도만 변경
            m_tInfo.fCX = 96.f;
            m_tInfo.fCY = 72.f;
            m_fSpeed = 6.f; // 메카쿠파 껍질 이동 속도
            m_bMove = true;
            break;

        case MONSTER_STOMPED:
            m_bDead = true;
            m_dwDeadTime = GetTickCount();
            break;

        case MONSTER_DEAD:
            m_dwDeadTime = GetTickCount();
            m_fSpeed = 0.f;
            m_bMove = false;
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

    case MONSTER_UP:
    case MONSTER_DOWN:
    case MONSTER_HIDDEN:
    case MONSTER_IDLE:
        m_dwTime = GetTickCount();
        break;

    case MONSTER_WALK:
        m_fSpeed = 2.f;
        m_dwTime = GetTickCount();
        break;
    case MONSTER_DEAD:
        if (m_eMonID == MON_PIRANHA) m_tInfo.fCY = 16.f;
        m_dwDeadTime = GetTickCount();  // 죽은 시점 기록
        m_fSpeed = 0.f;
        m_bMove = false;
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

    // 메카쿠파 처리
    if (m_eMonID == MON_MEKAKOOPA)
    {
        // 현재 상태가 IDLE이면 SHELL_IDLE로 변경
        if (m_eState == MONSTER_IDLE)
        {
            Set_State(MONSTER_SHELL_IDLE);

            // 바닥 위치 계산
            float fY = 0.f;
            if (CLineMgr::Get_Instance()->Collision_Line(m_tInfo, &fY))
            {
                // 바닥에 정확히 붙이기 (크기는 Set_State에서 이미 조정됨)
                m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
                m_fFallSpeed = 0.f;  // 중력 초기화
            }
            return;
        }
    }

    // 쿠파 처리
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

        // 메카쿠파는 위로 던지기 동작 추가
        if (m_eMonID == MON_MEKAKOOPA)
        {
            // 위로 던지기
            Set_State(MONSTER_SHELL_MOVE);
            m_fSpeed = 4.f;
            m_eDir = pPlayer->Get_Dir();
            m_bUpThrown = true;
            m_bDescending = false;  // 처음엔 올라가야 하므로 하강 모드 false
            m_fFallSpeed = -12.f;   // 초기 점프 속도
            CSoundMgr::Get_Instance()->PlaySoundW(L"meka_throw.wav", SOUND_EFFECT, 0.5f);
        }
        else
        {
            // 일반 쿠파는 그냥 차기
            On_Kicked(pPlayer->Get_Dir());
        }
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

bool CMonster::Player_In_Range(float range)
{
    list<CObject*>& playerList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER);
    if (playerList.empty()) return false;

    CObject* pPlayer = playerList.front();
    float dx = fabs(pPlayer->Get_Info()->fX - m_tInfo.fX);
    return dx < range;
}

bool CMonster::Player_Distance_High()
{
    list<CObject*>& playerList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER);
    if (playerList.empty()) return true;

    CObject* pPlayer = playerList.front();
    float dx = fabs(pPlayer->Get_Info()->fX - m_tInfo.fX);
    return dx > 50.f;
}