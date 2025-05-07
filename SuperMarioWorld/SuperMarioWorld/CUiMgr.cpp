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
    Render_PlayerLife(hDC, 20.f, 16.f);                       // �»�� ����
    Render_Timer(hDC, WINCX - 270.f, 16.f);                   // ���� TIME
    Render_Score(hDC, WINCX - 130.f, 16.f);                   // ���� COIN + SCORE
}

void CUiMgr::Release()
{
}

void CUiMgr::Render_PlayerLife(HDC hDC, float _fx, float _fy)
{
    // "MARIO"
    HDC hText = CBmpMgr::Get_Instance()->Find_Image(L"MarioText");
    GdiTransparentBlt(hDC, _fx, _fy, 120, 24, hText, 0, 0, 40, 8, RGB(0, 0, 255));

    // "x" (�ؽ�Ʈ �Ʒ� ��)
    float fxSymbol = _fx;
    float fySymbol = _fy + 24.f; // 3�� ������ 24�ȼ���ŭ �Ʒ�

    HDC hX = CBmpMgr::Get_Instance()->Find_Image(L"X_Mark");
    GdiTransparentBlt(hDC, fxSymbol, fySymbol, 18, 21, hX, 0, 0, 6, 7, RGB(0, 0, 255));

    // ���� (x ������)
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
    // ���� ������ ��� (���� ����)
    HDC hCoinIcon = CBmpMgr::Get_Instance()->Find_Image(L"Coin_Icon");
    GdiTransparentBlt(hDC, _fx, _fy, 24, 24, hCoinIcon, 0, 0, 8, 8, RGB(0, 0, 255)); // 3�� Ȯ��

    // "x" ��ȣ ��� (������ ������)
    HDC hX = CBmpMgr::Get_Instance()->Find_Image(L"X_Mark");
    GdiTransparentBlt(hDC, _fx + 30.f, _fy + 2.f, 18, 21, hX, 0, 0, 6, 7, RGB(0, 0, 255));

    // ���� �� ��� (x ������)
    Render_Number(hDC, _fx + 54.f, _fy + 2.f, m_iCoin, SMALL_WHITE);

    // ���ھ� ��� (�Ʒ� ��)
    Render_Number(hDC, _fx, _fy + 30.f, m_iScore, SMALL_WHITE);
}

void CUiMgr::Render_Number(HDC hDC, float x, float y, int num, ENumberType type)
{
    HDC hNumDC = CBmpMgr::Get_Instance()->Find_Image(L"Number");
    wstring str = to_wstring(num);

    int srcY = 0;
    int srcH = 8;  // �⺻ ���� ����
    int dstW = 24, dstH = 24;  // �⺻ ��� ũ�� (8��8 �� 3)

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
        dstH = 48; // 16 �� 3
        break;
    }

    for (size_t i = 0; i < str.length(); ++i)
    {
        int digit = str[i] - L'0';

        GdiTransparentBlt(
            hDC,
            x + i * dstW, y,        // ��� ��ġ
            dstW, dstH,             // ��� ũ�� (3�� Ȯ��)
            hNumDC,
            digit * 8, srcY,        // ���� ��ġ
            8, srcH,
            RGB(0, 0, 255)
        );
    }
}
