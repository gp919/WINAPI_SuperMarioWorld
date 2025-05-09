#include "pch.h"
#include "CEditor.h"

CEditor::CEditor()
{
}

CEditor::~CEditor()
{
}

void CEditor::Run()
{
    Initialize();

    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Scene1/Scene1.bmp", L"Scene1");
    MSG msg = {};
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Update();

        m_hDC = GetDC(g_hWnd);
        Render(m_hDC);
        ReleaseDC(g_hWnd, m_hDC);
    }

    Release();
}

void CEditor::Initialize()
{
    CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
    CScrollMgr::Get_Instance()->Set_ScrollY(0.f);
}

void CEditor::Update()
{
    static DWORD dwPrevTime = GetTickCount();
    DWORD dwNow = GetTickCount();

    // 입력 처리 주기를 제한 (100ms = 초당 10번)
    if (dwNow - dwPrevTime < 100)
        return;

    constexpr float SCROLL_STEP = 24.f; // 48단위의 절반 정도 (1칸씩 이동도 가능)

    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        fScrollX -= SCROLL_STEP;

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        fScrollX += SCROLL_STEP;

    if (GetAsyncKeyState(VK_UP) & 0x8000)
        fScrollY -= SCROLL_STEP;

    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        fScrollY += SCROLL_STEP;

    // 스크롤 한계
    float MAP_WIDTH = 5120.f * SCALE_FACTOR;
    float MAP_HEIGHT = 432.f * SCALE_FACTOR;

    fScrollX = clamp(fScrollX, 0.f, MAP_WIDTH - WINCX);
    fScrollY = clamp(fScrollY, 0.f, MAP_HEIGHT - WINCY);

    CScrollMgr::Get_Instance()->Set_ScrollX(fScrollX);
    CScrollMgr::Get_Instance()->Set_ScrollY(fScrollY);

    dwPrevTime = dwNow; // 다음 입력 시간 기준 갱신
}

void CEditor::Render(HDC hDC)
{
    HDC hBackDC = CreateCompatibleDC(hDC);
    HBITMAP hBackBitmap = CreateCompatibleBitmap(hDC, WINCX, WINCY);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

    PatBlt(hBackDC, 0, 0, WINCX, WINCY, WHITENESS);  // 화면 초기화

    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Scene1");
    int iSrcX = (int)(fScrollX / SCALE_FACTOR);
    int iSrcY = (int)((432.f * SCALE_FACTOR - WINCY - fScrollY) / SCALE_FACTOR);  // Y 반전
    int iSrcW = WINCX / SCALE_FACTOR;
    int iSrcH = WINCY / SCALE_FACTOR;

    GdiTransparentBlt(
        hBackDC,
        0, 0,
        WINCX, WINCY,
        hMemDC,
        iSrcX,
        iSrcY,
        iSrcW,
        iSrcH,
        RGB(0, 255, 0));

    const int GRID_SIZE = 48;
    for (int x = 0; x < WINCX; x += GRID_SIZE)
    {
        MoveToEx(hBackDC, x, 0, nullptr);
        LineTo(hBackDC, x, WINCY);
    }
    for (int y = 0; y < WINCY; y += GRID_SIZE)
    {
        MoveToEx(hBackDC, 0, y, nullptr);
        LineTo(hBackDC, WINCX, y);
    }


    // 실제 화면에 복사
    BitBlt(hDC, 0, 0, WINCX, WINCY, hBackDC, 0, 0, SRCCOPY);

    // 정리
    SelectObject(hBackDC, hOldBitmap);
    DeleteObject(hBackBitmap);
    DeleteDC(hBackDC);
}

void CEditor::Release()
{
    CBmpMgr::Destroy_Instance();
    CScrollMgr::Destroy_Instance();
}

