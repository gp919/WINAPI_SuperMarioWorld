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

const map<MONSTERID, LPCTSTR> CMonster::m_mapImage = {
    {MON_GOOMBA, L"Goomba"},
    {MON_GREENKOOPA, L"GKoopa"},
    {MON_REDKOOPA, L"RKoopa"},
    {MON_MOLE, L"Mole"},
    {MON_PIRANHA, L"Piranha"}
};

CMonster::CMonster()
    : m_eMonID(MON_GOOMBA)
    , m_eState(MONSTER_IDLE)
    , m_bMove(false)
    , m_fFallSpeed(0.f)
    , m_dwDeadTime(0)
    , m_dwTime(0)
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

    switch (m_eMonID)
    {
    case MON_MOLE:
    case MON_PIRANHA:
        Set_State(MONSTER_IDLE);
        m_bMove = false;
        break;
    default:
        Set_State(MONSTER_WALK);
        m_bMove = true;
        break;
    }

    Allign_Bottom(m_tInfo.fX, m_tInfo.fY);
}

int CMonster::Update()
{
    if (m_bDead)
    {
        if (GetTickCount() > m_dwDeadTime + 500)
        {
            CUiMgr::Get_Instance()->Set_Score(100);
            return DEAD;
        }

        Move_Frame();
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
    else if (m_eState == MONSTER_EJECTED)
    {
        if (GetTickCount() > m_dwTime + 2000)
        {
            m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
            Set_State(MONSTER_WALK);
        }
    }

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

void CMonster::Late_Update()
{
    if (m_tInfo.fY >= WINCY * 2.5f || m_tInfo.fX <= 0.f)
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

void CMonster::Render(HDC hDC)
{
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_mapImage.at(m_eMonID));

    GdiTransparentBlt(hDC,
        (int)(m_tInfo.fX - m_tInfo.fCX * 0.5f - m_fScrollX),
        (int)(m_tInfo.fY - m_tInfo.fCY * 0.5f - m_fScrollY),
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        m_tFrame.iStart * TILECX,
        m_tFrame.iMotion * TILECX,
        TILECX,
        (int)(m_tInfo.fCY / SCALE_FACTOR),
        RGB(0, 255, 0));
}

void CMonster::Release()
{
}

void CMonster::On_Collision(EOBJECTID _id)
{
    switch (_id)
    {
    case OBJ_PLAYER:
        if (m_eColDir == COL_TOP)
            On_Stomped();
        break;
    }
}

void CMonster::Set_State(MONSTER_STATE _eState)
{
    if (m_eState == _eState)
        return;

    m_eState = _eState;
    Init_Frame();

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
        break;

    case MONSTER_EJECTED:
        m_fSpeed = 0.5f;
        m_fJumpSpeed = -5.f;
        m_dwTime = GetTickCount();
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
    if (Is_Koopa())
    {
        switch (m_eState)
        {
        case MONSTER_WALK:
            Set_State(MONSTER_SHELL_IDLE);
            return;

        case MONSTER_SHELL_IDLE:
            Set_State(MONSTER_SHELL_MOVE);
            return;

        case MONSTER_SHELL_MOVE:
            Set_State(MONSTER_SHELL_IDLE);
            return;

        case MONSTER_EJECTED:
            Set_State(MONSTER_STOMPED);
            return;
        }
    }

    Set_State(MONSTER_STOMPED);
}