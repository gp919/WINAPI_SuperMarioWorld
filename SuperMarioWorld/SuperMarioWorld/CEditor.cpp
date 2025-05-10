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
        // 메인 루프에서 키 상태 업데이트
        CKeyMgr::Get_Instance()->Key_Update();
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
    // 타이머 설정
    static DWORD dwLastMoveTime = 0;
    DWORD dwCurrentTime = GetTickCount();

    // 최소 입력 간격 (ms) - 이 값을 조정하여 입력 민감도 조절
    const DWORD MIN_INPUT_INTERVAL = 200;

    // 충분한 시간이 지났는지 확인 (이동 속도 제한)
    if (dwCurrentTime - dwLastMoveTime < MIN_INPUT_INTERVAL)
        return;  // 아직 입력 간격이 충분하지 않으면 처리하지 않음

    // 현재 스크롤 위치
    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // 고정된 이동 거리 (타일 2칸)
    constexpr float TILE_SIZE = 48.f;
    constexpr float SCROLL_STEP = TILE_SIZE * 2; // 96픽셀씩 이동

    // 키 입력 확인 및 처리
    bool bMoved = false;
    float fDeltaX = 0.f;
    float fDeltaY = 0.f;

    // GetAsyncKeyState는 현재 키가 눌렸는지만 확인
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        fDeltaX = -SCROLL_STEP; // 왼쪽으로 이동 (X 감소)
        bMoved = true;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        fDeltaX = SCROLL_STEP; // 오른쪽으로 이동 (X 증가)
        bMoved = true;
    }

    // GDI 좌표계에서 위로 이동은 Y 감소, 아래로 이동은 Y 증가
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        fDeltaY = SCROLL_STEP; // 위로 이동 (Y 감소)
        bMoved = true;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        fDeltaY = SCROLL_STEP * (-1); // 아래로 이동 (Y 증가)
        bMoved = true;
    }

    // 움직였을 경우에만 시간과 위치 업데이트
    if (bMoved)
    {
        // 맵 경계 제한 (스크롤 매니저에 설정하기 전에 계산)
        float MAP_WIDTH = 5120.f * SCALE_FACTOR;
        float MAP_HEIGHT = 432.f * SCALE_FACTOR;

        // 예상되는 새 위치
        float fNewScrollX = fScrollX + fDeltaX;
        float fNewScrollY = fScrollY + fDeltaY;

        // 경계를 벗어나는 경우 델타값 조정
        if (fNewScrollX < 0.f)
            fDeltaX = -fScrollX;
        else if (fNewScrollX > MAP_WIDTH - WINCX)
            fDeltaX = (MAP_WIDTH - WINCX) - fScrollX;

        if (fNewScrollY < 0.f)
            fDeltaY = -fScrollY;
        else if (fNewScrollY > MAP_HEIGHT - WINCY)
            fDeltaY = (MAP_HEIGHT - WINCY) - fScrollY;

        // 스크롤 매니저 업데이트
        if (fDeltaX != 0.f)
            CScrollMgr::Get_Instance()->Set_ScrollX(fDeltaX);
        if (fDeltaY != 0.f)
            CScrollMgr::Get_Instance()->Set_ScrollY(fDeltaY);

        // 마지막 이동 시간 업데이트
        dwLastMoveTime = dwCurrentTime;
    }
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
    CKeyMgr::Destroy_Instance();
}

