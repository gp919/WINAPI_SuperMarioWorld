
#include "pch.h"
#include "CMonster.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CLineMgr.h"
#include "CUiMgr.h"

// Static ������
const map<pair<MONSTERID, MONSTER_STATE>, FRAME> CMonster::m_mapFrame = {
    // Goomba
    {{MON_GOOMBA, MONSTER_WALK}, {0, 3, 0, 100, 0}},
    {{MON_GOOMBA, MONSTER_STOMPED}, {0, 0, 1, 0, 0}},
    {{MON_GOOMBA, MONSTER_DEAD}, {0, 0, 1, 0, 0}},

    // Green Koopa
    {{MON_GREENKOOPA, MONSTER_EJECTED}, {0, 1, 0, 100, 0}},    // 1��: EJECTED ������
    {{MON_GREENKOOPA, MONSTER_STOMPED}, {0, 0, 1, 0, 0}},      // 2��: ���� ���
    {{MON_GREENKOOPA, MONSTER_SHELL_IDLE}, {0, 0, 2, 0, 0}},  // 3��: ���� idle
    {{MON_GREENKOOPA, MONSTER_SHELL_MOVE}, {0, 3, 2, 50, 0}}, // 3��: ���� ȸ��
    {{MON_GREENKOOPA, MONSTER_WALK}, {0, 1, 3, 200, 0}},      // 4��: �ȱ�
    {{MON_GREENKOOPA, MONSTER_DEAD}, {0, 0, 1, 0, 0}},

    // Red Koopa
    {{MON_REDKOOPA, MONSTER_EJECTED}, {0, 1, 0, 100, 0}},
    {{MON_REDKOOPA, MONSTER_STOMPED}, {0, 0, 1, 0, 0}},
    {{MON_REDKOOPA, MONSTER_SHELL_IDLE}, {0, 0, 2, 0, 0}},
    {{MON_REDKOOPA, MONSTER_SHELL_MOVE}, {0, 3, 2, 50, 0}},
    {{MON_REDKOOPA, MONSTER_WALK}, {0, 1, 3, 200, 0}},
    {{MON_REDKOOPA, MONSTER_DEAD}, {0, 0, 1, 0, 0}},

    // Mole
    {{MON_MOLE, MONSTER_IDLE},      {0, 1, 0, 300, 0}}, // 1��
    {{MON_MOLE, MONSTER_UP},        {0, 1, 1, 200, 0}}, // 2��
    {{MON_MOLE, MONSTER_EJECTED},   {0, 0, 2, 0, 0}},   // 3��
    {{MON_MOLE, MONSTER_WALK},      {0, 1, 3, 200, 0}}, // 4��
    {{MON_MOLE, MONSTER_STOMPED},   {0, 4, 4, 100, 0}}, // 5��
    {{MON_MOLE, MONSTER_DEAD},   {0, 4, 4, 100, 0}},

    // Piranha
    {{MON_PIRANHA, MONSTER_IDLE},   {0, 1, 0, 300, 0}}, // �⺻ ������ (2��)
    {{MON_PIRANHA, MONSTER_UP},     {0, 1, 0, 300, 0}},  
    {{MON_PIRANHA, MONSTER_DOWN},   {0, 1, 0, 300, 0}},
    {{MON_PIRANHA, MONSTER_HIDDEN}, {0, 0, 0, 0, 0}}, // ���� �̹��� ����
    {{MON_REDKOOPA, MONSTER_DEAD}, {0, 0, 1, 0, 0}},

    // MekaKoopa
    {{MON_MEKAKOOPA, MONSTER_IDLE}, {0, 3, 0, 100, 0}},
    {{MON_MEKAKOOPA, MONSTER_SHELL_IDLE}, {0, 0, 1, 0, 0}},
    {{MON_MEKAKOOPA, MONSTER_SHELL_MOVE}, {0, 2, 1, 133, 0}},
    {{MON_MEKAKOOPA, MONSTER_DEAD}, {0, 5, 2, 66, 0}},
};

// Static ������ - ���⺰ �̹��� Ű
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
    // Piranha (���� ���� ����)
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
    : CMonster()  // ���� ������ ���
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

    // ���°� �̹� �����Ǿ� ������ �״�� ���� (EJECTED ��)
    if (m_eState != MONSTER_IDLE)
    {
        // �̹� ���°� �����Ǿ� �ִ� ��� (��: EJECTED)
        // �ش� ���¿� �´� ũ�� ����
        switch (m_eState)
        {
        case MONSTER_EJECTED:
        case MONSTER_SHELL_IDLE:
        case MONSTER_SHELL_MOVE:
        case MONSTER_STOMPED:
            // ��ī������ ��� ũ�� ���� ó��
            if (m_eMonID == MON_MEKAKOOPA)
            {
                m_tInfo.fCX = 96.f;  // ��Ȯ�� ũ�� ����
                m_tInfo.fCY = 72.f;  // ��Ȯ�� ũ�� ����
            }
            else
            {
                m_tInfo.fCX = TILECX * SCALE_FACTOR;
                m_tInfo.fCY = TILECX * SCALE_FACTOR;  // �̴����� ũ��
            }
            break;
        
        default:
            // �Ϲ� ũ��
            switch (m_eMonID)
            {
            case MON_GREENKOOPA:
            case MON_REDKOOPA:
                m_tInfo.fCX = TILECX * SCALE_FACTOR;
                m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
                break;
            case MON_MEKAKOOPA:
                m_tInfo.fCX = 96.f;  // ��Ȯ�� ũ�� ����
                m_tInfo.fCY = 72.f;  // ��Ȯ�� ũ�� ����
                m_fSpeed = 2.f;
                break;
            default:
                m_tInfo.fCX = TILECX * SCALE_FACTOR;
                m_tInfo.fCY = TILECY * SCALE_FACTOR;
                break;
            }
            break;
        }

        // ���º� �ӵ� �� ����
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
        // ���°� �������� ���� ��� �⺻ �ʱ�ȭ
        // ũ�� ����
        switch (m_eMonID)
        {
        case MON_GREENKOOPA:
        case MON_REDKOOPA:
            m_tInfo.fCX = TILECX * SCALE_FACTOR;
            m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
            break;
        case MON_MEKAKOOPA:
            m_tInfo.fCX = 96.f;  // ��Ȯ�� ũ�� ����
            m_tInfo.fCY = 72.f;  // ��Ȯ�� ũ�� ����
            m_fSpeed = 2.f;
            m_bMove = true;
            break;
        default:
            m_tInfo.fCX = TILECX * SCALE_FACTOR;
            m_tInfo.fCY = TILECY * SCALE_FACTOR;
            break;
        }

        m_fSpeed = 1.f;

        // ���� ����
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

    // ���ͺ� �߰� ��ġ ����
    if (m_eMonID == MON_PIRANHA)
    {
        m_fPipeBottomY = m_tInfo.fY;        // ���� ��ġ�� ������ �Ʒ��� ����
        m_fPipeTopY = m_tInfo.fY - 100.f;    // �ö�� �ִ� ��ġ
        Set_State(MONSTER_HIDDEN);
    }

    if (m_eMonID == MON_MOLE)
    {
        m_fUpTargetY = m_tInfo.fY - 60.f;   // �δ����� Ƣ����� ��ǥ ����
        Set_State(MONSTER_IDLE);
    }

    Update_ImageKey();
    Allign_Bottom(m_tInfo.fX, m_tInfo.fY);
}

int CMonster::Update()
{
    // ��ī���İ� ȭ�� �ۿ��� ���� �� �ӵ� ����
    if (m_eMonID == MON_MEKAKOOPA && !m_bMove && In_Screen())
    {
        m_bMove = true;
        if (m_eState == MONSTER_IDLE)
        {
            // Ȱ��ȭ�Ǹ� ���� �ӵ��� ����
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

    // �÷��̾�� �����ִ� ���
    if (m_pHolder)
    {
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pHolder);

        // �÷��̾ ��� Ű�� ���Ҵ��� Ȯ��
        if (!pPlayer->Is_Grab())
        {
            Release_From_Holder();
            return NOEVENT;
        }

        // �÷��̾�� ���� ��ġ ��� (�ణ ��ġ����)
        float fPlayerHalfWidth = pPlayer->Get_Info()->fCX * 0.5f;
        float fShellHalfWidth = m_tInfo.fCX * 0.5f;
        float fOverlap = 15.f;  // ��ġ�� ���� (���ڰ� Ŭ���� �� ��ħ)

        if (pPlayer->Get_Dir() == DIR_RIGHT)
            m_tInfo.fX = pPlayer->Get_Info()->fX + fPlayerHalfWidth + fShellHalfWidth - fOverlap;
        else
            m_tInfo.fX = pPlayer->Get_Info()->fX - fPlayerHalfWidth - fShellHalfWidth + fOverlap;

        // Y ��ǥ�� �ణ ���� �ø���
        m_tInfo.fY = pPlayer->Get_Info()->fY - 5.f;  // 5�ȼ� ����

        Move_Frame();
        Update_Rect();
        return NOEVENT;
    }

    // ���º� �ð� üũ
    if (m_eState == MONSTER_SHELL_IDLE)
    {
        if (GetTickCount() > m_dwTime + 7000)
        {
            Set_State(MONSTER_EJECTED);
        }
    }

    // ȭ�� ���� üũ
#ifndef _DEBUG
// ȭ�鿡 ������ �� ������ ���� (�� ���� üũ)
    if (!m_bMove && In_Screen())
    {
        switch (m_eMonID)
        {
        case MON_MOLE:
        case MON_PIRANHA:
            // �� ���͵��� Ư���� ���ǿ��� ������
            break;
        default:
            // WALK �����̰ų� Ư�� ������ ���� ������ ����
            if (m_eState == MONSTER_WALK || m_eState == MONSTER_EJECTED || m_eState == MONSTER_SHELL_MOVE)
                m_bMove = true;
            break;
        }
    }
#else
// ����� ��忡���� �������� ���� (��, Ư�� ���´� ����)
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
    // �δ���
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

            if (GetTickCount() > m_dwTime + 4000)  // 4�� ������ ����
                m_bDead = true;
            break;
        }
        return;
    }

    // �Ƕ��
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

    // ��ī���� ó��
    if (m_eMonID == MON_MEKAKOOPA)
    {
        // ���� ������ SHELL_MOVE ������ ���� Ư�� ó��
        if (m_bUpThrown && m_eState == MONSTER_SHELL_MOVE)
        {
            // ��� �ܰ�
            if (m_fFallSpeed < 0)
            {
                // �߷� ����
                m_fFallSpeed += GRAVITY * 0.5f;
                m_tInfo.fY += m_fFallSpeed;

                // �ְ��� ���� �� �ϰ� ���� ��ȯ
                if (m_fFallSpeed >= 0)
                {
                    m_bDescending = true;
                }
            }
            // �ϰ� �ܰ�
            else if (m_bDescending)
            {
                // ȭ�� �ϴ� üũ�� ���� (����, Ÿ��, �÷��̾� ����)
                m_fFallSpeed += GRAVITY * 0.5f;

                if (m_fFallSpeed > MAX_FALL_SPEED)
                    m_fFallSpeed = MAX_FALL_SPEED;

                m_tInfo.fY += m_fFallSpeed;

                // ���� �浹 üũ
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

            // �⺻ �̵� (X��)
            // ���� �ڵ�...
        }

        // IDLE, SHELL_IDLE, �Ϲ� SHELL_MOVE ���� ó��
        // ���̳ξ� ��� üũ
        float fLeftBound = 256.f * SCALE_FACTOR + m_tInfo.fCX * 0.5f;
        float fRightBound = 512.f * SCALE_FACTOR - m_tInfo.fCX * 0.5f;
        float fGroundY = (432.f - 16.f) * SCALE_FACTOR - m_tInfo.fCY * 0.5f;

        // IDLE ���� (�Ϲ� �ȱ�)
        if (m_eState == MONSTER_IDLE)
        {
            // �¿� �̵�
            if (m_eDir == DIR_LEFT)
                m_tInfo.fX -= m_fSpeed;
            else
                m_tInfo.fX += m_fSpeed;

            // ��� üũ
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

            // �׻� �� ���� �ֵ��� ��
            m_tInfo.fY = fGroundY;
        }

        return; // ��ī���� ó�� �Ϸ�
    }

    // �Ϲ� ���� �̵�
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
    // �����ִ� ���¸� �߷°� �浹 üũ ��ŵ
    if (m_pHolder)
    {
        Update_Rect();
        return;
    }

    On_Collision(OBJ_MONSTER);

    // ȭ�� ������ ������ ����
    if (m_tInfo.fY >= 1272.f || m_tInfo.fX <= 0.f)
    {
        m_bDead = true;
        return;
    }

    // ��� ������ �� ȭ�� ������ ������ ����
    if ((m_eState == MONSTER_SHELL_IDLE || m_eState == MONSTER_SHELL_MOVE) && !In_Screen())
    {
        m_bDead = true;
        return;
    }

#ifndef _DEBUG
    // ���� ���� SHELL_MOVE ������ ���� Ư�� ó��
    if (m_eMonID == MON_MEKAKOOPA && m_bUpThrown && m_eState == MONSTER_SHELL_MOVE)
    {
        // �߷� ó�� ��ŵ (Update_AI���� ���� ó��)
    }
    else
    {
        // �δ����� �Ƕ�Ĵ� ���� �ö���� ���̸� �߷� ����
        bool bIgnoreGravity =
            (m_eMonID == MON_MOLE && m_eState == MONSTER_IDLE) ||
            (m_eMonID == MON_MOLE && m_eState == MONSTER_UP) ||
            (m_eMonID == MON_PIRANHA && m_eState == MONSTER_HIDDEN) ||
            (m_eMonID == MON_PIRANHA && m_eState == MONSTER_UP);

        if (!bIgnoreGravity)
            Apply_Gravity();
    }
#else
    // ����� ��忡�� �߷� ����
#endif

    Update_Rect();
}

void CMonster::Apply_Gravity()
{
    // ��ī������ ��� �浹 �˻� �ݺ� Ƚ�� ����
    int nSteps = (m_eMonID == MON_MEKAKOOPA) ? 6 : 1;
    float fStepSize = GRAVITY / nSteps;

    // �� ���� ū ��ȭ ��� ���� �ܰ�� ������ ó��
    for (int i = 0; i < nSteps; ++i)
    {
        // ���������� �ӵ� ����
        m_fFallSpeed += fStepSize;

        if (m_fFallSpeed > MAX_FALL_SPEED)
            m_fFallSpeed = MAX_FALL_SPEED;

        // ���������� ��ġ ����
        float fMoveStep = m_fFallSpeed / nSteps;
        m_tInfo.fY += fMoveStep;

        // �� �ܰ踶�� �浹 �˻�
        float fY = 0.f;
        if (CLineMgr::Get_Instance()->Collision_Line(m_tInfo, &fY))
        {
            float fCollisionMargin = (m_eMonID == MON_MEKAKOOPA) ? 2.0f : 0.5f;
            if (m_tInfo.fY + m_tInfo.fCY * 0.5f >= fY - fCollisionMargin)
            {
                m_tInfo.fY = fY - m_tInfo.fCY * 0.5f - 1.0f;  // �ణ�� �߰� ����
                m_fFallSpeed = 0.f;

                // �浹 �߻��ϸ� ���� ����
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
        // ��ī���� ���� ũ��
        const int srcWidth = 32;
        const int srcHeight = 24;

        // ��� ũ��� ��Ȯ�� 96x72 (3��)
        const int drawWidth = 96;
        const int drawHeight = 72;

        GdiTransparentBlt(hDC,
            (int)(m_tInfo.fX - drawWidth * 0.5f - m_fScrollX),  // �߾� ���� ��� ��ġ X
            (int)(m_tInfo.fY - drawHeight * 0.5f - m_fScrollY), // �߾� ���� ��� ��ġ Y
            drawWidth,  // ��� �ʺ�
            drawHeight, // ��� ����
            hMemDC,
            m_tFrame.iStart * srcWidth,    // �ҽ� X ������
            m_tFrame.iMotion * srcHeight,  // �ҽ� Y ������
            srcWidth,  // �ҽ� �ʺ�
            srcHeight, // �ҽ� ����
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

        // �ڽŰ� �ٸ� ���͵� ���� �浹�� üũ
        for (auto& pMonster : rMonsterList)
        {
            if (pMonster == this || pMonster->Get_Dead())
                continue;
            
            CMonster* pOtherMonster = static_cast<CMonster*>(pMonster);
            if ((pOtherMonster->Get_MonsterID() == MON_MEKAKOOPA) && pOtherMonster->Is_Descending())
            {
                break;
            }

            // Check_Rect�� ����Ͽ� �浹 Ȯ��
            float fWidth = 0.f, fHeight = 0.f;
            if (CCollisionMgr::Check_Rect(this, pOtherMonster, &fWidth, &fHeight))
            {
                // �����̴� ���� vs �Ϲ� ����
                if (m_eState == MONSTER_SHELL_MOVE && pOtherMonster->Get_State() != MONSTER_SHELL_MOVE)
                {
                    pOtherMonster->Set_Dead();
                    CSoundMgr::Get_Instance()->PlaySoundW(L"kick.wav",SOUND_EFFECT, 0.5f);
                    continue;
                }

                // �Ϲ� ���� vs �����̴� ����
                if (m_eState != MONSTER_SHELL_MOVE && pOtherMonster->Get_State() == MONSTER_SHELL_MOVE)
                {
                    m_bDead = true;
                    return;
                }

                // �����̴� ���� vs �����̴� ����
                if (m_eState == MONSTER_SHELL_MOVE && pOtherMonster->Get_State() == MONSTER_SHELL_MOVE)
                {
                    // �浹 �� ���� �� ����
                    m_bDead = true;
                    pOtherMonster->Set_Dead();
                    CSoundMgr::Get_Instance()->PlaySoundW(L"kick.wav", SOUND_EFFECT, 0.5f);
                    return;
                }

                // �Ϲ� ���ͳ��� �浹 �� ���� ��ȯ (�¿� �浹�� ����)
                if (m_eState == MONSTER_WALK && pOtherMonster->Get_State() == MONSTER_WALK)
                {
                    // �浹 ���� �Ǵ�
                    if (fWidth > fHeight) // ���� �浹�� ����
                        continue;

                    // �¿� �浹 �� ���� ��ȯ
                    if (m_tInfo.fX < pOtherMonster->Get_Info()->fX)
                    {
                        // ���� ���ʿ� ������ �������� ��ȯ
                        m_eDir = DIR_LEFT;
                        pOtherMonster->Set_Dir(DIR_RIGHT);
                    }
                    else
                    {
                        // ���� �����ʿ� ������ ���������� ��ȯ
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
        // �Ƕ�ĳ� �δ����� ��� ���� ���¿��� ������ �缳���� �ʿ���
        switch (_eState)
        {
        case MONSTER_IDLE:
        case MONSTER_HIDDEN:
        case MONSTER_UP:
        case MONSTER_DOWN:
            Init_Frame();  // �����
            m_dwTime = GetTickCount();  // Ÿ�̸� �ʱ�ȭ
            return;

        default:
            return;
        }
    }

    m_eState = _eState;
    Init_Frame();

    // ������ ��� ũ�⺯��
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
            m_dwDeadTime = GetTickCount();  // ���� ���� ���
            m_fSpeed = 0.f;
            m_bMove = false;
            break;
        }
    }

    // ��ī���� ���� ���� �� Ư�� ó��
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
            // ũ�� �����ϸ鼭 �ӵ��� ����
            m_tInfo.fCX = 96.f;
            m_tInfo.fCY = 72.f;
            m_fSpeed = 6.f; // ��ī���� ���� �̵� �ӵ�
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
        m_bMove = true;  // ���� �������� �׻� Ȱ��ȭ
        break;

    case MONSTER_EJECTED:
        m_fSpeed = 2.f;  // �����̵� �ӵ�
        m_bMove = true;  // �׻� ������
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
        m_dwDeadTime = GetTickCount();  // ���� ���� ���
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
    // �̹� ó�� ���̸� return (�ߺ� ����)
    static DWORD dwLastStompTime = 0;
    if (GetTickCount() - dwLastStompTime < 100)
        return;
    dwLastStompTime = GetTickCount();

    CSoundMgr::Get_Instance()->PlaySoundW(L"stomp.wav", SOUND_EFFECT, 0.1f);

    // ��ī���� ó��
    if (m_eMonID == MON_MEKAKOOPA)
    {
        // ���� ���°� IDLE�̸� SHELL_IDLE�� ����
        if (m_eState == MONSTER_IDLE)
        {
            Set_State(MONSTER_SHELL_IDLE);

            // �ٴ� ��ġ ���
            float fY = 0.f;
            if (CLineMgr::Get_Instance()->Collision_Line(m_tInfo, &fY))
            {
                // �ٴڿ� ��Ȯ�� ���̱� (ũ��� Set_State���� �̹� ������)
                m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
                m_fFallSpeed = 0.f;  // �߷� �ʱ�ȭ
            }
            return;
        }
    }

    // ���� ó��
    if (Is_Koopa())
    {
        switch (m_eState)
        {
        case MONSTER_WALK:
        {
            // ���� ��ġ ����
            float fCurrentX = m_tInfo.fX;
            float fCurrentY = m_tInfo.fY;

            // �÷��̾� ���� Ȯ��
            list<CObject*>& playerList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER);
            OBJECTDIR pushDir = DIR_RIGHT;

            if (!playerList.empty())
            {
                CObject* pPlayer = playerList.front();
                pushDir = (pPlayer->Get_Info()->fX < m_tInfo.fX) ? DIR_RIGHT : DIR_LEFT;
            }

            // ���� ���ĸ� ������ ����
            Set_State(MONSTER_SHELL_IDLE);

            // EJECTED ���� ���� - �з��� ��ġ�� ���� ����
            float fPushDistance = 30.f;
            float fEjectedX = fCurrentX + ((pushDir == DIR_RIGHT) ? fPushDistance : -fPushDistance);

            // EJECTED ���� ���� - ���¸� �����ڿ��� ���� ����
            CMonster* pEjected = new CMonster();
            pEjected->m_tInfo.fX = fEjectedX;
            pEjected->m_tInfo.fY = fCurrentY;
            pEjected->m_tInfo.iType = m_eMonID;
            pEjected->m_eMonID = m_eMonID;
            pEjected->Set_State(MONSTER_EJECTED);  // Initialize ���� ���� ����
            pEjected->Set_Dir(pushDir);
            pEjected->m_bMove = true;  // �ٷ� �����̵��� ����

            // ���⼭�� Add_Object�� ���� �ڵ� �ʱ�ȭ�� ���ϱ� ����
            // Initialize()�� ���� ȣ������ �ʽ��ϴ�.
            // AfterInit�� ȣ��Ǹ� �� �� ó���˴ϴ�.
            CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, pEjected);

            return;
        }

        case MONSTER_SHELL_IDLE:
        {
            // ������ ���� ó��
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
            // EJECTED ���¿��� ������ �ٷ� ����
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

    // ������ �÷��̾� �������� ����
    if (m_eState == MONSTER_SHELL_IDLE)
    {
        // ���� �� ��ġ ���� (�÷��̾�� �ణ ����߸�)
        float fThrowDistance = 10.f;
        if (pPlayer->Get_Dir() == DIR_RIGHT)
            m_tInfo.fX += fThrowDistance;
        else
            m_tInfo.fX -= fThrowDistance;

        // ��ī���Ĵ� ���� ������ ���� �߰�
        if (m_eMonID == MON_MEKAKOOPA)
        {
            // ���� ������
            Set_State(MONSTER_SHELL_MOVE);
            m_fSpeed = 4.f;
            m_eDir = pPlayer->Get_Dir();
            m_bUpThrown = true;
            m_bDescending = false;  // ó���� �ö󰡾� �ϹǷ� �ϰ� ��� false
            m_fFallSpeed = -12.f;   // �ʱ� ���� �ӵ�
            CSoundMgr::Get_Instance()->PlaySoundW(L"meka_throw.wav", SOUND_EFFECT, 0.5f);
        }
        else
        {
            // �Ϲ� ���Ĵ� �׳� ����
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

    // ���� �������� �ʱ� �ӵ� �ο�
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