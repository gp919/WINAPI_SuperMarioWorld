#include "pch.h"
#include "CEditor.h"

CEditor* CEditor::m_pInstance = nullptr;

CEditor::CEditor()
{
}

CEditor::~CEditor()
{
    Release();
}


void CEditor::Initialize()
{
    CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
    CScrollMgr::Get_Instance()->Set_ScrollY(0.f);
    CScrollMgr::Get_Instance()->Set_Size(5120.f * SCALE_FACTOR, 432.f * SCALE_FACTOR);
    

}


int CEditor::Update()
{
    // ��ũ�� �����̱�
    Move_Scroll();
    // ���콺 ������ �ݿ�
    Handle_Mouse_Input();
    // Ű���� �Է¿� ���� ���, Ÿ�� ����
    Key_Input();

    CObjectMgr::Get_Instance()->Update();

     // ����/�ε�
    if (CKeyMgr::Get_Instance()->Key_Down('S') && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
        Save_Data();
    if (CKeyMgr::Get_Instance()->Key_Down('L') && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
        Load_Data();

    // ������ ����
    if (CKeyMgr::Get_Instance()->Key_Down(VK_ESCAPE)) {
        // ���� ���� Ȯ��
        if (MessageBox(g_hWnd, L"���� �۾� ���� �����͸� �����Ͻðڽ��ϱ�?",
            L"������ ����", MB_YESNO) == IDYES) {
            Save_Data();
        }
        return NOEVENT;
    }

    return NOEVENT;
}

void CEditor::Late_Update()
{
    CObjectMgr::Get_Instance()->Late_Update();
}

void CEditor::Render(HDC hDC)
{
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    swprintf_s(m_szBuffer, 128, L"mouseX : %ld, mouseY : %ld  gridX : %4.1f, gridY : %4.1f  MODE : %ls, TYPE : %d",
        m_lMouseX, m_lMouseY, m_fGridX, m_fGridY, m_wcMode.c_str(), m_iType);
    SetWindowText(g_hWnd, m_szBuffer);

    HDC hMemDC_back = CBmpMgr::Get_Instance()->Find_Image(L"BackGround01");
    if (hMemDC_back)
    {
        const int BACK_WIDTH = 512;
        const int BACK_HEIGHT = 432;

        // ���� ������ ��� ���� ���� �ε���
        int startIdx = (int)(m_fScrollX / (BACK_WIDTH * SCALE_FACTOR));

        // �ʿ��� ��� ��: ȭ�� �ʺ� Ŀ���� ��ŭ
        int numBacks = WINCX / (BACK_WIDTH * SCALE_FACTOR) + 2;

        for (int i = 0; i < numBacks; ++i)
        {
            int drawX = (startIdx + i) * BACK_WIDTH * SCALE_FACTOR - (int)m_fScrollX;

            BitBlt(
                hDC,
                drawX, 0,
                BACK_WIDTH * SCALE_FACTOR, BACK_HEIGHT * SCALE_FACTOR,
                hMemDC_back,
                0, 0,
                SRCCOPY);
        }
    }

    // �� 
    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Scene1");
    GdiTransparentBlt(
        hDC,
        0, 0,
        WINCX, WINCY,                          // ��� ũ��
        hMemDC,
        (int)(m_fScrollX / SCALE_FACTOR),        // �� �ε���
        (int)(m_fScrollY / SCALE_FACTOR),        // �� �ε��� (������ ����)
        WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,
        RGB(0, 255, 0));

    // �׸���
    const int GRID_SIZE = 48;
    for (int x = 0; x < WINCX; x += GRID_SIZE)
    {
        MoveToEx(hDC, x, 0, nullptr);
        LineTo(hDC, x, WINCY);
    }
    for (int y = 0; y < WINCY; y += GRID_SIZE)
    {
        MoveToEx(hDC, 0, y, nullptr);
        LineTo(hDC, WINCX, y);
    }

    

    // ���� : ������
    HPEN hLinePen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));  // ���������� ����
    HPEN hOldPen = (HPEN)SelectObject(hDC, hLinePen);

    for (auto& pLine : CLineMgr::Get_Instance()->Get_LinetList())
    {
        LINE* pLineInfo = pLine->Get_Line();
        float fX1 = pLineInfo->tLPoint.fX - m_fScrollX;
        float fY1 = pLineInfo->tLPoint.fY - m_fScrollY;
        float fX2 = pLineInfo->tRPoint.fX - m_fScrollX;
        float fY2 = pLineInfo->tRPoint.fY - m_fScrollY;

        // ȭ�� ���� ���� �ִ��� Ȯ�� (������ Ȯ��)
        if ((fX1 > -GRID_SIZE && fX1 < WINCX + GRID_SIZE) ||
            (fX2 > -GRID_SIZE && fX2 < WINCX + GRID_SIZE))
        {
            MoveToEx(hDC, int(fX1), int(fY1), nullptr);
            LineTo(hDC, int(fX2), int(fY2));
        }
    }

    // ���ҽ� ����
    SelectObject(hDC, hOldPen);
    DeleteObject(hLinePen);

    // ������Ʈ
    CObjectMgr::Get_Instance()->Render(hDC);

    // ���� ���õ� �׸��� ��ġ ǥ�� (�����)
    HBRUSH hCursorBrush = CreateSolidBrush(RGB(255, 255, 0));  // �����
    HPEN hCursorPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
    SelectObject(hDC, hCursorBrush);
    SelectObject(hDC, hCursorPen);

    float fCursorX = m_fGridX - m_fScrollX;
    float fCursorY = m_fGridY - m_fScrollY;

    if (fCursorX > -GRID_SIZE && fCursorX < WINCX + GRID_SIZE &&
        fCursorY > -GRID_SIZE && fCursorY < WINCY + GRID_SIZE)
    {
        // ���� ��忡 ���� �ٸ� ������� ǥ��
        if (m_eCurEdit == MODE_TILE)
            Rectangle(hDC, int(fCursorX - GRID_SIZE / 2), int(fCursorY - GRID_SIZE / 2),
                int(fCursorX + GRID_SIZE / 2), int(fCursorY + GRID_SIZE / 2));
        else if (m_eCurEdit == MODE_MONSTER)
            Ellipse(hDC, int(fCursorX - GRID_SIZE / 2), int(fCursorY - GRID_SIZE / 2),
                int(fCursorX + GRID_SIZE / 2), int(fCursorY + GRID_SIZE / 2));
        else if (m_eCurEdit == MODE_ITEM)
        {
            // ������ ���: ���̾Ƹ��(������) ������� ǥ��
            POINT points[4];
            points[0] = { int(fCursorX), int(fCursorY - GRID_SIZE / 2) };        // ���� ��
            points[1] = { int(fCursorX + GRID_SIZE / 2), int(fCursorY) };        // ������ ��
            points[2] = { int(fCursorX), int(fCursorY + GRID_SIZE / 2) };        // �Ʒ��� ��
            points[3] = { int(fCursorX - GRID_SIZE / 2), int(fCursorY) };        // ���� ��

            Polygon(hDC, points, 4);
        }
        else if (m_eCurEdit == MODE_LINE)
        {
            if(m_iType == LINE_HOR)
            {
                // ���� �������� ��Ÿ���� ȭ��ǥ ���(��)
                // ���� ��
                Rectangle(hDC, int(fCursorX - GRID_SIZE / 2), int(fCursorY - GRID_SIZE / 2),
                    int(fCursorX + GRID_SIZE / 2), int(fCursorY - GRID_SIZE / 2 + 5));

                // ȭ��ǥ �κ�
                MoveToEx(hDC, int(fCursorX), int(fCursorY - GRID_SIZE / 2), NULL);
                LineTo(hDC, int(fCursorX), int(fCursorY + GRID_SIZE / 3));

                // ȭ��ǥ �Ӹ�
                MoveToEx(hDC, int(fCursorX), int(fCursorY - GRID_SIZE / 2), NULL);
                LineTo(hDC, int(fCursorX - GRID_SIZE / 4), int(fCursorY - GRID_SIZE / 4));

                MoveToEx(hDC, int(fCursorX), int(fCursorY - GRID_SIZE / 2), NULL);
                LineTo(hDC, int(fCursorX + GRID_SIZE / 4), int(fCursorY - GRID_SIZE / 4));
            }
            else if(m_iType == LINE_VER)
            {
                // ���� �������� ��Ÿ���� ȭ��ǥ ���(��)
                // ������ ��
                Rectangle(hDC, int(fCursorX + GRID_SIZE / 2 - 5), int(fCursorY - GRID_SIZE / 2),
                    int(fCursorX + GRID_SIZE / 2), int(fCursorY + GRID_SIZE / 2));

                // ȭ��ǥ �κ�
                MoveToEx(hDC, int(fCursorX + GRID_SIZE / 2), int(fCursorY), NULL);
                LineTo(hDC, int(fCursorX - GRID_SIZE / 3), int(fCursorY));

                // ȭ��ǥ �Ӹ�
                MoveToEx(hDC, int(fCursorX + GRID_SIZE / 2), int(fCursorY), NULL);
                LineTo(hDC, int(fCursorX + GRID_SIZE / 4), int(fCursorY - GRID_SIZE / 4));

                MoveToEx(hDC, int(fCursorX + GRID_SIZE / 2), int(fCursorY), NULL);
                LineTo(hDC, int(fCursorX + GRID_SIZE / 4), int(fCursorY + GRID_SIZE / 4));
            }
            else
            {
                // ���� ���� ���� �� ǥ��
                if (m_bClick)
                {
                    // ù ������ Ŀ������ ��
                    MoveToEx(hDC, int(p1.fX - m_fScrollX), int(p1.fY - m_fScrollY), nullptr);
                    LineTo(hDC, int(fCursorX), int(fCursorY));

                    // ù ���� ���� ������ ǥ��
                    Ellipse(hDC,
                        int(p1.fX - m_fScrollX - 4), int(p1.fY - m_fScrollY - 4),
                        int(p1.fX - m_fScrollX + 4), int(p1.fY - m_fScrollY + 4));
                }
                else
                {
                    // ���� Ŀ�� ��ġ�� �� �̸�����
                    Ellipse(hDC,
                        int(fCursorX - 4), int(fCursorY - 4),
                        int(fCursorX + 4), int(fCursorY + 4));
                }
            }
        }
    }

    SelectObject(hDC, GetStockObject(WHITE_BRUSH));
    SelectObject(hDC, GetStockObject(BLACK_PEN));
    DeleteObject(hCursorBrush);
    DeleteObject(hCursorPen);

    // ���� ǥ�� (���� ����)
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0));
    TextOutW(hDC, 10, 10, L"���: 1-Ÿ��, 2-����, 3-����, 4-������", wcslen(L"���: 1-Ÿ��, 2-����, 3-����, 4-������"));
    TextOutW(hDC, 10, 30, L"Ÿ�� ����: Q-����, W-����", wcslen(L"Ÿ�� ����: Q-����, W-����"));
    TextOutW(hDC, 10, 50, L"����: Ctrl+S, �ҷ�����: Ctrl+L", wcslen(L"����: Ctrl+S, �ҷ�����: Ctrl+L"));
    TextOutW(hDC, 10, 70, L"����: ESC", wcslen(L"����: ESC"));

    // ����� ���� - ������Ʈ ���� Ȯ��
    auto& tileList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_TILE);
    auto& monsterList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER);
    auto& itemList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_ITEM);

    wchar_t szDebug[128];
    swprintf_s(szDebug, L"Ÿ��: %d, ����: %d, ������: %d",
        tileList.size(), monsterList.size(), itemList.size());
    TextOutW(hDC, 10, 90, szDebug, wcslen(szDebug));
}

void CEditor::Release()
{
    CLineMgr::Get_Instance()->Destroy_Instance();
    CObjectMgr::Get_Instance()->Destroy_Instance();
}

void CEditor::Move_Scroll()
{
    // Ÿ�̸� ����
    static DWORD dwLastMoveTime = 0;
    DWORD dwCurrentTime = GetTickCount();
    const DWORD MIN_INPUT_INTERVAL = 50;  // �ε巯�� �������� ���� ����

    if (dwCurrentTime - dwLastMoveTime < MIN_INPUT_INTERVAL)
        return;

    // ���� ��ũ�� ��ġ
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // �̵��� ���� - X�� �״��, Y�� ���� ����
    constexpr float SCROLL_STEP_X = 48.0f;  // Ÿ�� �� ĭ (48�ȼ�)
    constexpr float SCROLL_STEP_Y = 24.0f;  // Y���� X���� �������� ���� (���� ����)

    bool bMoved = false;
    float fDeltaX = 0.f;
    float fDeltaY = 0.f;

    // Ű �Է� ó��
    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
    {
        fDeltaX = -SCROLL_STEP_X;
        bMoved = true;
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
    {
        fDeltaX = SCROLL_STEP_X;
        bMoved = true;
    }

    if (GetAsyncKeyState(VK_UP) & 0x8000)
    {
        fDeltaY = -SCROLL_STEP_Y;
        bMoved = true;
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
    {
        fDeltaY = SCROLL_STEP_Y;
        bMoved = true;
    }

    // �������� ��쿡�� ������Ʈ
    if (bMoved)
    {
        // �� ��� ����
        float MAP_WIDTH = 5120.f * SCALE_FACTOR;
        float MAP_HEIGHT = 640.f * SCALE_FACTOR;

        // ���� ��ġ
        float fNewScrollX = m_fScrollX + fDeltaX;
        float fNewScrollY = m_fScrollY + fDeltaY;

        // ��� ����
        if (fNewScrollX < 0.f)
            fDeltaX = -m_fScrollX;
        else if (fNewScrollX > MAP_WIDTH - WINCX)
            fDeltaX = (MAP_WIDTH - WINCX) - m_fScrollX;

        if (fNewScrollY < 0.f)
            fDeltaY = -m_fScrollY;
        else if (fNewScrollY > MAP_HEIGHT - WINCY)
            fDeltaY = (MAP_HEIGHT - WINCY) - m_fScrollY;

        // ��ũ�� �Ŵ��� ������Ʈ
        if (fDeltaX != 0.f)
            CScrollMgr::Get_Instance()->Set_ScrollX(fDeltaX);
        if (fDeltaY != 0.f)
            CScrollMgr::Get_Instance()->Set_ScrollY(fDeltaY);

        // ������ �̵� �ð� ������Ʈ
        dwLastMoveTime = dwCurrentTime;
    }
}

void CEditor::Handle_Mouse_Input()
{
    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(g_hWnd, &pt);
    m_lMouseX = pt.x;
    m_lMouseY = pt.y;

    // ��ũ�� ��ġ ���
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // �׸��� ��ǥ ���
    Screen_To_World(m_lMouseX, m_lMouseY, &m_fWorldX, &m_fWorldY);
    World_To_Grid(m_fWorldX, m_fWorldY, &m_fGridX, &m_fGridY);

    // ��Ŭ�� - ��ġ
    if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON))
    {
        if (m_eCurEdit == MODE_LINE)
        {
            if (m_eCurLine != LINE_ANG)
            {
                Place_Line(m_fGridX, m_fGridY);
            }
            else
                Place_Point(m_fWorldX, m_fWorldY, m_fGridX, m_fGridY);
        }
        else
            Place_Object(m_fGridX, m_fGridY);
    }
    // ��Ŭ�� - ����
    else if (CKeyMgr::Get_Instance()->Key_Down(VK_RBUTTON))
    {
        if (m_eCurEdit == MODE_LINE)
        {
            CLineMgr::Get_Instance()->Sub_Line(m_fGridX, m_fGridY);
        }
        else
            CObjectMgr::Get_Instance()->Sub_Object(m_fGridX, m_fGridY);
        
    }
}

void CEditor::Key_Input()
{
    // ��� ���� : 1. Ÿ�� 2. ���� 3. ����
    if (CKeyMgr::Get_Instance()->Key_Pressing('1'))
    {
        m_eCurEdit = MODE_TILE;
        m_eCurTile = TILE_Q;
        m_wcMode = L"TILE";
        m_iType = m_eCurTile;
    }
    else if (CKeyMgr::Get_Instance()->Key_Pressing('2'))
    {
        m_eCurEdit = MODE_MONSTER;
        m_eCurMon = MON_GOOMBA;
        m_wcMode = L"MONSTER";
        m_iType = m_eCurMon;
    }
    else if (CKeyMgr::Get_Instance()->Key_Pressing('3'))
    {
        m_eCurEdit = MODE_LINE;
        m_wcMode = L"LINE";
        m_iType = m_eCurLine;
    }
    else if (CKeyMgr::Get_Instance()->Key_Pressing('4'))
    {
        m_eCurEdit = MODE_ITEM;
        m_wcMode = L"ITEM";
        m_iType = m_eCurItem;
    }

    // ���� Ÿ������ �̵�, ù��° �ε������ ������ �ε����� ��ȯ
    if (CKeyMgr::Get_Instance()->Key_Down('Q'))
    {
        if (m_eCurEdit == MODE_TILE)
        {
            if (m_eCurTile != TILE_Q)
            {
                m_eCurTile = (TILEID)(m_eCurTile - 1);
            }
            else
                m_eCurTile = (TILEID)(TILE_END - 1);
            m_iType = (int)m_eCurTile;
        }
        else if (m_eCurEdit == MODE_MONSTER)
        {
            if (m_eCurMon != MON_GOOMBA)
            {
                m_eCurMon = (MONSTERID)(m_eCurMon - 1);
            }
            else
                m_eCurMon = (MONSTERID)(MON_END - 1);
            m_iType = (int)m_eCurMon;
        }
        else if (m_eCurEdit == MODE_ITEM)
        {
            if (m_eCurItem != ITEM_COIN)
            {
                m_eCurItem = (ITEMID)(m_eCurItem - 1);
            }
            else
                m_eCurItem = (ITEMID)(ITEM_END - 1);
            m_iType = (int)m_eCurItem;
        }
    }

    // ���� Ÿ������ �̵�, ������ �ε������ ù��° �ε����� ��ȯ
    if (CKeyMgr::Get_Instance()->Key_Down('W'))
    {
        if (m_eCurEdit == MODE_TILE)
        {
            if (m_eCurTile != (TILEID)(TILE_END - 1))
            {
                m_eCurTile = (TILEID)(m_eCurTile + 1);
            }
            else
                m_eCurTile = TILE_Q;
            m_iType = m_eCurTile;
        }
        else if (m_eCurEdit == MODE_MONSTER)
        {
            if (m_eCurMon != (MONSTERID)(MON_END - 1))
            {
                m_eCurMon = (MONSTERID)(m_eCurMon + 1);
            }
            else
                m_eCurMon = MON_GOOMBA;
            m_iType = m_eCurMon;
        }
        else if (m_eCurEdit == MODE_ITEM)
        {
            if (m_eCurItem != (ITEMID)(ITEM_END - 1))
            {
                m_eCurItem = (ITEMID)(m_eCurItem + 1);
            }
            else
                m_eCurItem = ITEM_COIN;
            m_iType = m_eCurItem;
        }
    }

    // ���� Ÿ�� ���� : ���� -> ���� -> ���� ����
    if (CKeyMgr::Get_Instance()->Key_Down('E'))
    {
        if(m_eCurEdit == MODE_LINE)
        {
            if (m_eCurLine == LINE_HOR)
                m_eCurLine = LINE_VER;
            else if (m_eCurLine == LINE_VER)
                m_eCurLine = LINE_ANG;
            else if (m_eCurLine == LINE_ANG)
                m_eCurLine = LINE_HOR;

            m_iType = m_eCurLine;
        }
    }
}


void CEditor::Place_Object(float _fx, float _fy)
{
    CObject* pObject = nullptr;
    switch (m_eCurEdit)
    {
        case MODE_TILE:
            pObject = new CTile(_fx, _fy, m_eCurTile);
            CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, pObject);
            break;

        case MODE_MONSTER:
            pObject = new CMonster(_fx, _fy, m_eCurMon);
            CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, pObject);
            break;

        case MODE_ITEM:
            pObject = new CItem(_fx, _fy, m_eCurItem);
            CObjectMgr::Get_Instance()->Add_Object(OBJ_ITEM, pObject);
            break;
    }
}

void CEditor::Place_Line(float _fx, float _fy)
{
    float f1X, f1Y;
    float f2X, f2Y;

    if(m_eCurLine == LINE_HOR)
    {
        f1X = _fx - (TILECX * SCALE_FACTOR) / 2.f;
        f1Y = _fy - (TILECY * SCALE_FACTOR) / 2.f;
        f2X = _fx + (TILECX * SCALE_FACTOR) / 2.f;
        f2Y = _fy - (TILECY * SCALE_FACTOR) / 2.f;
    }
    else
    {
        f1X = _fx + (TILECX * SCALE_FACTOR) / 2.f;
        f1Y = _fy - (TILECY * SCALE_FACTOR) / 2.f;
        f2X = _fx + (TILECX * SCALE_FACTOR) / 2.f;
        f2Y = _fy + (TILECY * SCALE_FACTOR) / 2.f;
    }
    CLineMgr::Get_Instance()->Add_Line({ f1X,f1Y }, { f2X,f2Y });
}

// �׸��� ���� 
void CEditor::Place_Point(float _wx, float _wy, float _gx, float _gy)
{
    if (!m_bClick)
    {
        p1 = { 0,0 };
        // �߽���ǥ ���� �����̸� ���� X , �������̸� ������ X
        if (_wx <= _gx)
            p1.fX = _gx - TILECX * SCALE_FACTOR * 0.5f;
        else
            p1.fX = _gx + TILECX * SCALE_FACTOR * 0.5f;
        // y�� �������� : gdi��ǥ�迡 ����
        // �Ʒ��� �ִ°��
        if (_wy > _gy)
            p1.fY = _gy + TILECY * SCALE_FACTOR * 0.5f;
        // ���� �ִ°��
        else
            p1.fY = _gy - TILECY * SCALE_FACTOR * 0.5f;
        m_bClick = true;
    }
    else
    {
        p2 = { 0, 0 };
        // �߽���ǥ ���� �����̸� ���� X , �������̸� ������ X
        if (_wx <= _gx)
            p2.fX = _gx - TILECX * SCALE_FACTOR * 0.5f;
        else
            p2.fX = _gx + TILECX * SCALE_FACTOR * 0.5f;
        // y�� �������� : gdi��ǥ�迡 ����
        // �Ʒ��� �ִ°��
        if (_wy > _gy)
            p2.fY = _gy + TILECY * SCALE_FACTOR * 0.5f;
        // ���� �ִ°��
        else
            p2.fY = _gy - TILECY * SCALE_FACTOR * 0.5f;
        m_bClick = false;
        // �� ��ǥ�� ���� ������ ���� �߰�
        if ((p1.fX != p2.fX) && (p1.fY != p2.fY))
            CLineMgr::Get_Instance()->Add_Line(p1, p2);
    }
}

void CEditor::Screen_To_World(float screenX, float screenY, float* worldX, float* worldY)
{
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // ��� -> ���� ��ǥ
    *worldX = (screenX + m_fScrollX);
    *worldY = (screenY + m_fScrollY);
}

// editor ���� �������� �ʿ��Ѱ�?
void CEditor::World_To_Screen(float worldX, float worldY, float* screenX, float* screenY)
{
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // ��ũ�� ��ġ�� ����Ͽ� �׸��� ��ǥ ���
    *screenX = (worldX - m_fScrollX);
    *screenY = (worldY - m_fScrollY);
}

void CEditor::World_To_Grid(float worldX, float worldY, float* gridX, float* gridY)
{
    // ���� ��ǥ�� �׸��� �ε����� ��ȯ (���� �ε���)
    int gridIndexX = static_cast<int>(worldX / 48.0f);
    int gridIndexY = static_cast<int>(worldY / 48.0f);

    // �׸����� �߽���ǥ ���
    *gridX = (gridIndexX * 48.0f) + 24.0f;
    *gridY = (gridIndexY * 48.0f) + 24.0f;
}

void CEditor::Save_Data()
{
    const wchar_t* wc_Dir = L"../Data/";
    const wchar_t* wc_Dat = L".dat";
    for (auto i = 0; i < MODE_END; i++)
    {
        // 0. Ÿ�� 1. ���� 2. ���� 3. ������
        // enum Ÿ������ �ִ°� ����ϱ� �Ⱦ ������
        string s_Name = to_string(i);
        wstring ws_Name(s_Name.begin(), s_Name.end());
        wstring ws_FullPath = wc_Dir + ws_Name + wc_Dat;
        const wchar_t* wc_FullPath = ws_FullPath.c_str();

        HANDLE	hFile = CreateFile(wc_FullPath,		// ���� �̸��� ������ ���
            GENERIC_WRITE,			// ���� ���� ���(GENERIC_WRITE : ���� ����, GENERIC_READ : �б� ����)
            NULL,					// ���� ���(������ ����� ���¿��� �ٸ� ���μ������� ����ϰ��� �� �� �㰡�� ���ΰ�, null�� ��� �������� ����)
            NULL,					// ���� �Ӽ�(null�� ��� �⺻ ���� �ش��ϴ� ���� ���� ����)
            CREATE_ALWAYS,			// ���� ���(CREATE_ALWAYS : ������ ������ ����, ������ �����, OPEN_EXISTING : ������ ���� ��쿡�� �ҷ�����)
            FILE_ATTRIBUTE_NORMAL,  // ���� �Ӽ�(�Ϲ����� ����)
            NULL);					// ������ ������ �Ӽ��� ������ ���ø� ����(������� �ʱ� ������ null)

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MessageBox(g_hWnd, L"Save Error", _T("Error"), MB_OK);
            return;
        }

        DWORD	dwByte(0);
        // TILE, MONSTER, LINE, ITEM ������ ����
        switch (i)
        {
        case MODE_TILE:
            for (auto& pobj : CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_TILE))
            {
                WriteFile(hFile, (pobj->Get_Info()), sizeof(INFO), &dwByte, nullptr);
            }
            break;
        case MODE_MONSTER:
            for (auto& pobj : CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER))
            {
                //  TODO : ���� Ÿ�� üũ�ؼ� �ֱ� , �ٿ����� �ϵ��ڵ�
                WriteFile(hFile, (pobj->Get_Info()), sizeof(INFO), &dwByte, nullptr);
            }
            break;
        case MODE_LINE:
            for (auto& pLine : CLineMgr::Get_Instance()->Get_LinetList())
            {
               WriteFile(hFile, (pLine->Get_Line()), sizeof(LINE), &dwByte, nullptr);
            }
            break;
        case MODE_ITEM:
            for (auto& pobj : CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_ITEM))
            {
                WriteFile(hFile, (pobj->Get_Info()), sizeof(INFO), &dwByte, nullptr);
            }
            break;

        }
        
        CloseHandle(hFile);
        MessageBox(g_hWnd, wc_FullPath, _T("Success"), MB_OK);
    }
    
}

void CEditor::Load_Data( )
{
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_MONSTER);
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_TILE);
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_ITEM);
    CLineMgr::Get_Instance()->Release();
    const wchar_t* wc_Dir = L"../Data/";
    const wchar_t* wc_Dat = L".dat";

    for (auto i = 0; i < MODE_END; i++)
    {
        // 0. Ÿ�� 1. ���� 2. ���� 3. ������
        // enum Ÿ������ �ִ°� ����ϱ� �Ⱦ ������
        string s_Name = to_string(i);
        wstring ws_Name(s_Name.begin(), s_Name.end());
        wstring ws_FullPath = wc_Dir + ws_Name + wc_Dat;
        const wchar_t* wc_FullPath = ws_FullPath.c_str();
        HANDLE	hFile = CreateFile(wc_FullPath,		// ���� �̸��� ������ ���
            GENERIC_READ,			// ���� ���� ���(GENERIC_WRITE : ���� ����, GENERIC_READ : �б� ����)
            NULL,					// ���� ���(������ ����� ���¿��� �ٸ� ���μ������� ����ϰ��� �� �� �㰡�� ���ΰ�, null�� ��� �������� ����)
            NULL,					// ���� �Ӽ�(null�� ��� �⺻ ���� �ش��ϴ� ���� ���� ����)
            OPEN_EXISTING,			// ���� ���(CREATE_ALWAYS : ������ ������ ����, ������ �����, OPEN_EXISTING : ������ ���� ��쿡�� �ҷ�����)
            FILE_ATTRIBUTE_NORMAL,  // ���� �Ӽ�(�Ϲ����� ����)
            NULL);					// ������ ������ �Ӽ��� ������ ���ø� ����(������� �ʱ� ������ null)

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MessageBox(g_hWnd, L"Load Error", _T("Error"), MB_OK);
            return;
        }

        DWORD	dwByte(0);
        LINE	tLine{};
        INFO    tInfo{};

        //Release();
        switch (i)
        {
        case MODE_TILE:
            while (true)
            {
                ReadFile(hFile, &tInfo, sizeof(INFO), &dwByte, nullptr);

                if (0 == dwByte)
                    break;

                CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(tInfo));
            }
            break;
        case MODE_MONSTER:
            while (true)
            {
                ReadFile(hFile, &tInfo, sizeof(INFO), &dwByte, nullptr);

                if (0 == dwByte)
                    break;
                CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, new CMonster(tInfo));
            }
            break;
        case MODE_LINE:
            while (true)
            {
                ReadFile(hFile, &tLine, sizeof(LINE), &dwByte, nullptr);

                if (0 == dwByte)
                    break;

                CLineMgr::Get_Instance()->Get_LinetList().push_back(new CLine(tLine));
            }
            break;
        case MODE_ITEM:
            while (true)
            {
                ReadFile(hFile, &tInfo, sizeof(INFO), &dwByte, nullptr);

                if (0 == dwByte)
                    break;
                CObjectMgr::Get_Instance()->Add_Object(OBJ_ITEM, new CItem(tInfo));
            }
            break;
        }

        CloseHandle(hFile);
        MessageBox(g_hWnd, L"Load �Ϸ�", _T("Success"), MB_OK);
    }
}

