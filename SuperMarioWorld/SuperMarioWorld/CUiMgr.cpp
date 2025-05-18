#include "pch.h"
#include "CUiMgr.h"

CUiMgr* CUiMgr::m_pInstance(nullptr);

CUiMgr::CUiMgr()
{
}

CUiMgr::~CUiMgr()
{
}

void CUiMgr::Initialize()
{
}

void CUiMgr::Render(HDC hDC)
{
    Render_PlayerLife(hDC, 20.f, 16.f);                       // 좌상단 여백
    Render_Timer(hDC, WINCX - 270.f, 16.f);                   // 우상단 TIME
    Render_Score(hDC, WINCX - 130.f, 16.f);                   // 우상단 COIN + SCORE
}

void CUiMgr::Release()
{
}

void CUiMgr::Update()
{
    list<CObject*> ObjectList;
    if (m_bPause)    return;

    DWORD dwNow = GetTickCount();

    if (dwNow - m_dwTime >= 1000) // 1초 경과
    {
        ObjectList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER);
        for (auto it : ObjectList)
        {
            if (it->Get_Dead())
                --m_iLife;
            if (m_iLife <= 0)
            {
                // 사망처리
            }
        }
        ObjectList.clear();

        m_dwTime = dwNow;

        if (m_fTime > 0)
            --m_fTime;

        if (m_fTime < 100)
        {
            // 배경음 변겅
        }

        if (m_fTime <= 0)
        {
            m_fTime = 0;
            // 예: 게임 오버
        }
    }
}

void CUiMgr::Set_Score(int _score)
{
    DWORD dwNow = GetTickCount();

    if (_score == 1) // 몬스터 처치용 호출
    {
        if (dwNow - m_KillChain.dwLastKillTime <= 2000)
            ++m_KillChain.iChainCount;
        else
            m_KillChain.iChainCount = 1;

        m_KillChain.dwLastKillTime = dwNow;

        int iIndex = min(m_KillChain.iChainCount - 1, g_iScoreChainMax - 1);
        int iBonus = g_iScoreChain[iIndex];
        m_iScore += iBonus;

        // 이펙트 텍스트 설정
        if (iIndex < g_iScoreChainMax - 1)
            m_strRecentEffect = to_wstring(iBonus);
        else
        {
            m_strRecentEffect = L"1UP";
            ++m_iLife;
            CSoundMgr::Get_Instance()->PlaySoundW(L"1up.wav", SOUND_EFFECT, 0.3f);
        }

        m_dwEffectTime = dwNow;

        // 점수 표시 위치는 임의로 설정하거나 외부에서 좌표 전달받도록 확장 가능
        m_fEffectX = WINCX / 2.f - 20.f;
        m_fEffectY = 200.f;
    }
    else
    {
        m_iScore += _score;
        m_strRecentEffect = to_wstring(_score);
        m_dwEffectTime = dwNow;

        m_fEffectX = WINCX / 2.f - 20.f;
        m_fEffectY = 200.f;
    }
}

void CUiMgr::Set_Score(int _score, float _fx, float _fy)
{
    DWORD dwNow = GetTickCount();

    if (_score == 1)
    {
        if (dwNow - m_KillChain.dwLastKillTime <= 2000)
            ++m_KillChain.iChainCount;
        else
            m_KillChain.iChainCount = 1;

        m_KillChain.dwLastKillTime = dwNow;

        int iIndex = min(m_KillChain.iChainCount - 1, g_iScoreChainMax - 1);
        int iBonus = g_iScoreChain[iIndex];
        m_iScore += iBonus;

        if (iIndex < g_iScoreChainMax - 1)
        {
            m_strRecentEffect = to_wstring(iBonus);
            wchar_t szSoundName[64];
            swprintf_s(szSoundName, L"Shell Combo-%02d.wav", iIndex + 1);
            CSoundMgr::Get_Instance()->PlaySoundW(szSoundName, SOUND_EFFECT, 0.3f);
        }
        else
        {
            m_strRecentEffect = L"1UP";
            ++m_iLife;
            CSoundMgr::Get_Instance()->PlaySoundW(L"1up.wav", SOUND_EFFECT, 0.3f);
        }
    }
    else
    {
        m_iScore += _score;
        m_strRecentEffect = to_wstring(_score);
    }

    m_dwEffectTime = dwNow;

    // 월드 좌표 → 화면 좌표 변환
    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    m_fEffectX = _fx - fScrollX;
    m_fEffectY = _fy - fScrollY;
}

void CUiMgr::Render_PlayerLife(HDC hDC, float _fx, float _fy)
{
    // "MARIO"
    HDC hText = CBmpMgr::Get_Instance()->Find_Image(L"MarioText");
    GdiTransparentBlt(hDC, _fx, _fy, 120, 24, hText, 0, 0, 40, 8, RGB(0, 255, 0));

    // "x" (텍스트 아래 줄)
    float fxSymbol = _fx;
    float fySymbol = _fy + 24.f; // 3배 높이인 24픽셀만큼 아래

    HDC hX = CBmpMgr::Get_Instance()->Find_Image(L"X_Mark");
    GdiTransparentBlt(hDC, fxSymbol, fySymbol, 18, 21, hX, 0, 0, 6, 7, RGB(0, 255, 0));

    // 숫자 (x 오른쪽)
    Render_Number(hDC, fxSymbol + 24.f, fySymbol + 1.f, m_iLife, ENumberType::SMALL_WHITE);
}

void CUiMgr::Render_Timer(HDC hDC, float _fx, float _fy)
{
    HDC hTimeText = CBmpMgr::Get_Instance()->Find_Image(L"Time_Text");
    GdiTransparentBlt(hDC, _fx, _fy, 72, 21, hTimeText, 0, 0, 24, 7, RGB(0, 255, 0));

    Render_Number(hDC, _fx, _fy + 18.f, static_cast<int>(m_fTime), SMALL_YELLOW);
}

void CUiMgr::Render_Score(HDC hDC, float _fx, float _fy)
{
    // 동전 아이콘 출력 (좌측 정렬)
    HDC hCoinIcon = CBmpMgr::Get_Instance()->Find_Image(L"Coin_Icon");
    GdiTransparentBlt(hDC, _fx, _fy, 24, 24, hCoinIcon, 0, 0, 8, 8, RGB(0, 255, 0)); // 3배 확대

    // "x" 기호 출력 (아이콘 오른쪽)
    HDC hX = CBmpMgr::Get_Instance()->Find_Image(L"X_Mark");
    GdiTransparentBlt(hDC, _fx + 30.f, _fy + 2.f, 18, 21, hX, 0, 0, 6, 7, RGB(0, 255, 0));

    // 코인 수 출력 (x 오른쪽)
    Render_Number(hDC, _fx + 54.f, _fy + 2.f, m_iCoin, SMALL_WHITE);

    // 스코어 출력 (아래 줄)
    Render_Number(hDC, _fx, _fy + 30.f, m_iScore, SMALL_WHITE);
}

void CUiMgr::Render_Number(HDC hDC, float x, float y, int num, ENumberType type)
{
    HDC hNumDC = CBmpMgr::Get_Instance()->Find_Image(L"Number");
    wstring str = to_wstring(num);

    int srcY = 0;
    int srcH = 8;  // 기본 숫자 높이
    int dstW = 24, dstH = 24;  // 기본 출력 크기 (8×8 × 3)

    switch (type)
    {
    case ENumberType::SMALL_WHITE:
        srcY = 0;
        break;
    case ENumberType::SMALL_YELLOW:
        srcY = 8;
        break;
    case ENumberType::BIG_WHITE:
        srcY = 16;
        srcH = 16;
        dstH = 48; // 16 × 3
        break;
    }


    for (size_t i = 0; i < str.length(); ++i)
    {
        int digit = str[i] - L'0';

        GdiTransparentBlt(
            hDC,
            x + i * dstW, y,        // 출력 위치
            dstW, dstH,             // 출력 크기 (3배 확대)
            hNumDC,
            digit * 8, srcY,        // 원본 위치
            8, srcH,
            RGB(0, 255, 0)
        );
    }
}

void CUiMgr::Render_ScoreEffect(HDC hDC, float fX, float fY)
{
    DWORD dwNow = GetTickCount();
    if (dwNow - m_dwEffectTime < 700 && !m_strRecentEffect.empty())
    {
        float fOffset = (GetTickCount() - m_dwEffectTime) * 0.05f;  // 시간당 위로 이동
        TextOut(hDC, (int)m_fEffectX, (int)(m_fEffectY - fOffset), m_strRecentEffect.c_str(), m_strRecentEffect.length());
    }
}

void CUiMgr::Add_KillScore()
{
    DWORD dwNow = GetTickCount();

    // 연속 타이머 2초로 설정 (2000ms)
    if (dwNow - m_KillChain.dwLastKillTime <= 2000)
    {
        ++m_KillChain.iChainCount;
    }
    else
    {
        m_KillChain.iChainCount = 1;
    }

    m_KillChain.dwLastKillTime = dwNow;

    int iIndex = min(m_KillChain.iChainCount - 1, g_iScoreChainMax - 1);
    int iScore = g_iScoreChain[iIndex];

    if (iIndex < g_iScoreChainMax - 1)
    {
        Set_Score(iScore);  // 점수 추가
        Show_ScoreEffect(iScore);  // 숫자 이펙트
    }
    else
    {
        Set_Score(10000);
        Show_ScoreEffect(10000);
        Show_1UpEffect();     // 1UP 이펙트 출력
        Add_Life();           // 라이프 증가
    }
}

void CUiMgr::Show_ScoreEffect(int iScore)
{
    m_strRecentEffect = to_wstring(iScore);
    m_dwEffectTime = GetTickCount();
    m_fEffectX = WINCX / 2.f - 20.f;
    m_fEffectY = 200.f;
}

void CUiMgr::Show_1UpEffect()
{
    m_strRecentEffect = L"1UP";
    m_dwEffectTime = GetTickCount();
    m_fEffectX = WINCX / 2.f - 20.f;
    m_fEffectY = 200.f;
}

void CUiMgr::Reset_KillChain()
{
    m_KillChain.iChainCount = 0;
    m_KillChain.dwLastKillTime = 0;
}
