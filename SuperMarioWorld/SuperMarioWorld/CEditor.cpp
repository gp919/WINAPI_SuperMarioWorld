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
        // ���� �������� Ű ���� ������Ʈ
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
    // Ÿ�̸� ����
    static DWORD dwLastMoveTime = 0;
    DWORD dwCurrentTime = GetTickCount();

    // �ּ� �Է� ���� (ms) - �� ���� �����Ͽ� �Է� �ΰ��� ����
    const DWORD MIN_INPUT_INTERVAL = 200;

    // ����� �ð��� �������� Ȯ�� (�̵� �ӵ� ����)
    if (dwCurrentTime - dwLastMoveTime < MIN_INPUT_INTERVAL)
        return;  // ���� �Է� ������ ������� ������ ó������ ����

    // ���� ��ũ�� ��ġ
    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // ������ �̵� �Ÿ� (Ÿ�� 2ĭ)
    constexpr float TILE_SIZE = 48.f;
    constexpr float SCROLL_STEP = TILE_SIZE * 2; // 96�ȼ��� �̵�

    // Ű �Է� Ȯ�� �� ó��
    bool bMoved = false;
    float fDeltaX = 0.f;
    float fDeltaY = 0.f;

    // GetAsyncKeyState�� ���� Ű�� ���ȴ����� Ȯ��
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        fDeltaX = -SCROLL_STEP; // �������� �̵� (X ����)
        bMoved = true;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        fDeltaX = SCROLL_STEP; // ���������� �̵� (X ����)
        bMoved = true;
    }

    // GDI ��ǥ�迡�� ���� �̵��� Y ����, �Ʒ��� �̵��� Y ����
    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        fDeltaY = SCROLL_STEP; // ���� �̵� (Y ����)
        bMoved = true;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        fDeltaY = SCROLL_STEP * (-1); // �Ʒ��� �̵� (Y ����)
        bMoved = true;
    }

    // �������� ��쿡�� �ð��� ��ġ ������Ʈ
    if (bMoved)
    {
        // �� ��� ���� (��ũ�� �Ŵ����� �����ϱ� ���� ���)
        float MAP_WIDTH = 5120.f * SCALE_FACTOR;
        float MAP_HEIGHT = 432.f * SCALE_FACTOR;

        // ����Ǵ� �� ��ġ
        float fNewScrollX = fScrollX + fDeltaX;
        float fNewScrollY = fScrollY + fDeltaY;

        // ��踦 ����� ��� ��Ÿ�� ����
        if (fNewScrollX < 0.f)
            fDeltaX = -fScrollX;
        else if (fNewScrollX > MAP_WIDTH - WINCX)
            fDeltaX = (MAP_WIDTH - WINCX) - fScrollX;

        if (fNewScrollY < 0.f)
            fDeltaY = -fScrollY;
        else if (fNewScrollY > MAP_HEIGHT - WINCY)
            fDeltaY = (MAP_HEIGHT - WINCY) - fScrollY;

        // ��ũ�� �Ŵ��� ������Ʈ
        if (fDeltaX != 0.f)
            CScrollMgr::Get_Instance()->Set_ScrollX(fDeltaX);
        if (fDeltaY != 0.f)
            CScrollMgr::Get_Instance()->Set_ScrollY(fDeltaY);

        // ������ �̵� �ð� ������Ʈ
        dwLastMoveTime = dwCurrentTime;
    }
}

void CEditor::Render(HDC hDC)
{
    HDC hBackDC = CreateCompatibleDC(hDC);
    HBITMAP hBackBitmap = CreateCompatibleBitmap(hDC, WINCX, WINCY);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);

    PatBlt(hBackDC, 0, 0, WINCX, WINCY, WHITENESS);  // ȭ�� �ʱ�ȭ

    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Scene1");
    int iSrcX = (int)(fScrollX / SCALE_FACTOR);
    int iSrcY = (int)((432.f * SCALE_FACTOR - WINCY - fScrollY) / SCALE_FACTOR);  // Y ����
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


    // ���� ȭ�鿡 ����
    BitBlt(hDC, 0, 0, WINCX, WINCY, hBackDC, 0, 0, SRCCOPY);

    // ����
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

