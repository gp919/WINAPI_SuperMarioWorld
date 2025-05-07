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

void CUiMgr::Render_PlayerLife(HDC hDC, float _fx, float _fy)
{
    // "MARIO"
    HDC hText = CBmpMgr::Get_Instance()->Find_Image(L"MarioText");
    GdiTransparentBlt(hDC, _fx, _fy, 120, 24, hText, 0, 0, 40, 8, RGB(0, 0, 255));

    // "x" (텍스트 아래 줄)
    float fxSymbol = _fx;
    float fySymbol = _fy + 24.f; // 3배 높이인 24픽셀만큼 아래

    HDC hX = CBmpMgr::Get_Instance()->Find_Image(L"X_Mark");
    GdiTransparentBlt(hDC, fxSymbol, fySymbol, 18, 21, hX, 0, 0, 6, 7, RGB(0, 0, 255));

    // 숫자 (x 오른쪽)
    Render_Number(hDC, fxSymbol + 24.f, fySymbol + 1.f, m_iLife, ENumberType::SMALL_WHITE);
}

void CUiMgr::Render_Timer(HDC hDC, float _fx, float _fy)
{
    HDC hTimeText = CBmpMgr::Get_Instance()->Find_Image(L"Time_Text");
    GdiTransparentBlt(hDC, _fx, _fy, 72, 21, hTimeText, 0, 0, 24, 7, RGB(0, 0, 255));

    Render_Number(hDC, _fx, _fy + 18.f, static_cast<int>(m_fTime), SMALL_YELLOW);
}

void CUiMgr::Render_Score(HDC hDC, float _fx, float _fy)
{
    // 동전 아이콘 출력 (좌측 정렬)
    HDC hCoinIcon = CBmpMgr::Get_Instance()->Find_Image(L"Coin_Icon");
    GdiTransparentBlt(hDC, _fx, _fy, 24, 24, hCoinIcon, 0, 0, 8, 8, RGB(0, 0, 255)); // 3배 확대

    // "x" 기호 출력 (아이콘 오른쪽)
    HDC hX = CBmpMgr::Get_Instance()->Find_Image(L"X_Mark");
    GdiTransparentBlt(hDC, _fx + 30.f, _fy + 2.f, 18, 21, hX, 0, 0, 6, 7, RGB(0, 0, 255));

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
            RGB(0, 0, 255)
        );
    }
}
