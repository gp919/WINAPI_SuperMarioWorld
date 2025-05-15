#include "pch.h"
#include "CWorldPlayer.h"
#include "CWorld.h"

void CWorldPlayer::Initialize(CWorld* pOwnerWorld)
{
    m_vecWorldPos = {
        {519, 642}, // World 0
        {265, 642}, // World 1
        {417, 321}, // World 2
        {105,  86}  // World 3
    };

    m_fX = m_vecWorldPos[0].x;
    m_fY = m_vecWorldPos[0].y;
    m_pOwnerWorld = pOwnerWorld;
}

void CWorldPlayer::Set_Target(int iWorld)
{
    if (iWorld >= 0 && iWorld < m_vecWorldPos.size())
    {
        POINT cur = { (LONG)m_fX, (LONG)m_fY };
        POINT next = m_vecWorldPos[iWorld];

        m_quePath = {};
        m_iCurWorld = iWorld;

        // 2 → 3 특수 이동
        if (iWorld == 3 && cur.x == 417 && cur.y == 321)
        {
            m_quePath.push({ 417, 270 });       // World1 기준 위로 살짝
            m_bChangeToWorld2 = true;           // 다음 도착에서 전환
        }
        else
        {
            if (cur.y != next.y) m_quePath.push({ cur.x, next.y });
            if (cur.x != next.x) m_quePath.push(next);
        }

        m_bMoving = true;
        m_tTarget = m_quePath.front();
        m_quePath.pop();
    }
}






void CWorldPlayer::Update()
{
    if (m_bMoving)
    {
        Move_Towards_Target();

        // 프레임 갱신
        DWORD dwCur = GetTickCount();
        if (dwCur - m_dwFrameTime >= m_dwFrameSpeed)
        {
            m_dwFrameTime = dwCur;
            m_iFrame = (m_iFrame + 1) % m_iMaxFrame;
        }
    }
    else
    {
        m_iFrame = 0;  // 정지 시 첫 프레임
    }
}

void CWorldPlayer::Move_Towards_Target()
{
    float dx = m_tTarget.x - m_fX;
    float dy = m_tTarget.y - m_fY;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist <= 1.f)
    {
        m_fX = m_tTarget.x;
        m_fY = m_tTarget.y;

        if (m_bChangeToWorld2 && m_pOwnerWorld)
        {
            m_pOwnerWorld->Set_Background(L"World2");
            CSoundMgr::Get_Instance()->StopAll();
            CSoundMgr::Get_Instance()->PlayBGM(L"08. Valley of Bowser.mp3", 0.5f);
            m_bChangeToWorld2 = false;

            // 강제 위치 설정 (World2 시작 위치)
            m_fX = 690.f;  // 230 * 3
            m_fY = 588.f;  // 196 * 3

            // 새 경로: World2 내부 이동
            while (!m_quePath.empty()) m_quePath.pop();
            m_quePath.push({ 318, 588 });  // 1. 가로 이동
            m_quePath.push({ 318, 270 });  // 2. 세로 이동

            m_bMoving = true;
            m_tTarget = m_quePath.front(); m_quePath.pop();
            return;
        }

        if (!m_quePath.empty())
        {
            m_tTarget = m_quePath.front();
            m_quePath.pop();
        }
        else
        {
            CSoundMgr::Get_Instance()->PlaySoundW(L"map-spot.wav", SOUND_EFFECT, 0.5f);
            m_bMoving = false;
            
        }
        return;
    }

    m_fX += m_fSpeed * dx / dist;
    m_fY += m_fSpeed * dy / dist;
}

void CWorldPlayer::Switch_To_World2()
{
    // 강제 시작 위치: 230, 196 → 스케일 적용
    m_fX = 690.f;
    m_fY = 588.f;

    while (!m_quePath.empty()) m_quePath.pop();

    // 이동 경로: 도착 위치 106, 90 → 318, 270
    m_quePath.push({ 318, 270 });

    m_bMoving = true;
    m_tTarget = m_quePath.front();
    m_quePath.pop();
}


void CWorldPlayer::Render(HDC hDC)
{
    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"WorldPlayer");
    int iFrameWidth = 16;
    int iFrameHeight = 16;

    int iDrawX = static_cast<int>(m_fX - (iFrameWidth * SCALE_FACTOR / 2.f));
    int iDrawY = static_cast<int>(m_fY - (iFrameHeight * SCALE_FACTOR / 2.f));

    GdiTransparentBlt(
        hDC,
        iDrawX,
        iDrawY,
        iFrameWidth * SCALE_FACTOR,
        iFrameHeight * SCALE_FACTOR,
        hMemDC,
        iFrameWidth * m_iFrame, 0,
        iFrameWidth, iFrameHeight,
        RGB(0, 255, 0));
}