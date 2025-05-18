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
    // 스크롤 움직이기
    Move_Scroll();
    // 마우스 움직임 반영
    Handle_Mouse_Input();
    // 키보드 입력에 따른 모드, 타입 변경
    Key_Input();

    CObjectMgr::Get_Instance()->Update();

     // 저장/로드
    if (CKeyMgr::Get_Instance()->Key_Down('S') && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
        Save_Data();
    if (CKeyMgr::Get_Instance()->Key_Down('L') && (GetAsyncKeyState(VK_CONTROL) & 0x8000))
        Load_Data();

    // 에디터 종료
    if (CKeyMgr::Get_Instance()->Key_Down(VK_ESCAPE)) {
        // 저장 여부 확인
        if (MessageBox(g_hWnd, L"현재 작업 중인 데이터를 저장하시겠습니까?",
            L"에디터 종료", MB_YESNO) == IDYES) {
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

        // 현재 보여질 배경 영역 시작 인덱스
        int startIdx = (int)(m_fScrollX / (BACK_WIDTH * SCALE_FACTOR));

        // 필요한 배경 수: 화면 너비를 커버할 만큼
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

    // 맵 
    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Scene1");
    GdiTransparentBlt(
        hDC,
        0, 0,
        WINCX, WINCY,                          // 출력 크기
        hMemDC,
        (int)(m_fScrollX / SCALE_FACTOR),        // 열 인덱스
        (int)(m_fScrollY / SCALE_FACTOR),        // 행 인덱스 (오프셋 제거)
        WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,
        RGB(0, 255, 0));

    // 그리드
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

    

    // 라인 : 빨간색
    HPEN hLinePen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));  // 빨간색으로 변경
    HPEN hOldPen = (HPEN)SelectObject(hDC, hLinePen);

    for (auto& pLine : CLineMgr::Get_Instance()->Get_LinetList())
    {
        LINE* pLineInfo = pLine->Get_Line();
        float fX1 = pLineInfo->tLPoint.fX - m_fScrollX;
        float fY1 = pLineInfo->tLPoint.fY - m_fScrollY;
        float fX2 = pLineInfo->tRPoint.fX - m_fScrollX;
        float fY2 = pLineInfo->tRPoint.fY - m_fScrollY;

        // 화면 영역 내에 있는지 확인 (간단한 확인)
        if ((fX1 > -GRID_SIZE && fX1 < WINCX + GRID_SIZE) ||
            (fX2 > -GRID_SIZE && fX2 < WINCX + GRID_SIZE))
        {
            MoveToEx(hDC, int(fX1), int(fY1), nullptr);
            LineTo(hDC, int(fX2), int(fY2));
        }
    }

    // 리소스 정리
    SelectObject(hDC, hOldPen);
    DeleteObject(hLinePen);

    // 오브젝트
    CObjectMgr::Get_Instance()->Render(hDC);

    // 현재 선택된 그리드 위치 표시 (노란색)
    HBRUSH hCursorBrush = CreateSolidBrush(RGB(255, 255, 0));  // 노란색
    HPEN hCursorPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
    SelectObject(hDC, hCursorBrush);
    SelectObject(hDC, hCursorPen);

    float fCursorX = m_fGridX - m_fScrollX;
    float fCursorY = m_fGridY - m_fScrollY;

    if (fCursorX > -GRID_SIZE && fCursorX < WINCX + GRID_SIZE &&
        fCursorY > -GRID_SIZE && fCursorY < WINCY + GRID_SIZE)
    {
        // 현재 모드에 따라 다른 모양으로 표시
        if (m_eCurEdit == MODE_TILE)
            Rectangle(hDC, int(fCursorX - GRID_SIZE / 2), int(fCursorY - GRID_SIZE / 2),
                int(fCursorX + GRID_SIZE / 2), int(fCursorY + GRID_SIZE / 2));
        else if (m_eCurEdit == MODE_MONSTER)
            Ellipse(hDC, int(fCursorX - GRID_SIZE / 2), int(fCursorY - GRID_SIZE / 2),
                int(fCursorX + GRID_SIZE / 2), int(fCursorY + GRID_SIZE / 2));
        else if (m_eCurEdit == MODE_ITEM)
        {
            // 아이템 모드: 다이아몬드(마름모) 모양으로 표시
            POINT points[4];
            points[0] = { int(fCursorX), int(fCursorY - GRID_SIZE / 2) };        // 위쪽 점
            points[1] = { int(fCursorX + GRID_SIZE / 2), int(fCursorY) };        // 오른쪽 점
            points[2] = { int(fCursorX), int(fCursorY + GRID_SIZE / 2) };        // 아래쪽 점
            points[3] = { int(fCursorX - GRID_SIZE / 2), int(fCursorY) };        // 왼쪽 점

            Polygon(hDC, points, 4);
        }
        else if (m_eCurEdit == MODE_LINE)
        {
            if(m_iType == LINE_HOR)
            {
                // 윗면 라인임을 나타내는 화살표 모양(↑)
                // 위쪽 선
                Rectangle(hDC, int(fCursorX - GRID_SIZE / 2), int(fCursorY - GRID_SIZE / 2),
                    int(fCursorX + GRID_SIZE / 2), int(fCursorY - GRID_SIZE / 2 + 5));

                // 화살표 부분
                MoveToEx(hDC, int(fCursorX), int(fCursorY - GRID_SIZE / 2), NULL);
                LineTo(hDC, int(fCursorX), int(fCursorY + GRID_SIZE / 3));

                // 화살표 머리
                MoveToEx(hDC, int(fCursorX), int(fCursorY - GRID_SIZE / 2), NULL);
                LineTo(hDC, int(fCursorX - GRID_SIZE / 4), int(fCursorY - GRID_SIZE / 4));

                MoveToEx(hDC, int(fCursorX), int(fCursorY - GRID_SIZE / 2), NULL);
                LineTo(hDC, int(fCursorX + GRID_SIZE / 4), int(fCursorY - GRID_SIZE / 4));
            }
            else if(m_iType == LINE_VER)
            {
                // 세로 라인임을 나타내는 화살표 모양(→)
                // 오른쪽 선
                Rectangle(hDC, int(fCursorX + GRID_SIZE / 2 - 5), int(fCursorY - GRID_SIZE / 2),
                    int(fCursorX + GRID_SIZE / 2), int(fCursorY + GRID_SIZE / 2));

                // 화살표 부분
                MoveToEx(hDC, int(fCursorX + GRID_SIZE / 2), int(fCursorY), NULL);
                LineTo(hDC, int(fCursorX - GRID_SIZE / 3), int(fCursorY));

                // 화살표 머리
                MoveToEx(hDC, int(fCursorX + GRID_SIZE / 2), int(fCursorY), NULL);
                LineTo(hDC, int(fCursorX + GRID_SIZE / 4), int(fCursorY - GRID_SIZE / 4));

                MoveToEx(hDC, int(fCursorX + GRID_SIZE / 2), int(fCursorY), NULL);
                LineTo(hDC, int(fCursorX + GRID_SIZE / 4), int(fCursorY + GRID_SIZE / 4));
            }
            else
            {
                // 임의 라인 가상 선 표시
                if (m_bClick)
                {
                    // 첫 점에서 커서까지 선
                    MoveToEx(hDC, int(p1.fX - m_fScrollX), int(p1.fY - m_fScrollY), nullptr);
                    LineTo(hDC, int(fCursorX), int(fCursorY));

                    // 첫 점을 작은 원으로 표시
                    Ellipse(hDC,
                        int(p1.fX - m_fScrollX - 4), int(p1.fY - m_fScrollY - 4),
                        int(p1.fX - m_fScrollX + 4), int(p1.fY - m_fScrollY + 4));
                }
                else
                {
                    // 현재 커서 위치에 점 미리보기
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

    // 도움말 표시 (선택 사항)
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0));
    TextOutW(hDC, 10, 10, L"모드: 1-타일, 2-몬스터, 3-라인, 4-아이템", wcslen(L"모드: 1-타일, 2-몬스터, 3-라인, 4-아이템"));
    TextOutW(hDC, 10, 30, L"타입 변경: Q-이전, W-다음", wcslen(L"타입 변경: Q-이전, W-다음"));
    TextOutW(hDC, 10, 50, L"저장: Ctrl+S, 불러오기: Ctrl+L", wcslen(L"저장: Ctrl+S, 불러오기: Ctrl+L"));
    TextOutW(hDC, 10, 70, L"종료: ESC", wcslen(L"종료: ESC"));

    // 디버그 정보 - 오브젝트 개수 확인
    auto& tileList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_TILE);
    auto& monsterList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER);
    auto& itemList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_ITEM);

    wchar_t szDebug[128];
    swprintf_s(szDebug, L"타일: %d, 몬스터: %d, 아이템: %d",
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
    // 타이머 설정
    static DWORD dwLastMoveTime = 0;
    DWORD dwCurrentTime = GetTickCount();
    const DWORD MIN_INPUT_INTERVAL = 50;  // 부드러운 움직임을 위한 간격

    if (dwCurrentTime - dwLastMoveTime < MIN_INPUT_INTERVAL)
        return;

    // 현재 스크롤 위치
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // 이동량 설정 - X는 그대로, Y는 따로 조정
    constexpr float SCROLL_STEP_X = 48.0f;  // 타일 한 칸 (48픽셀)
    constexpr float SCROLL_STEP_Y = 24.0f;  // Y축은 X축의 절반으로 설정 (조정 가능)

    bool bMoved = false;
    float fDeltaX = 0.f;
    float fDeltaY = 0.f;

    // 키 입력 처리
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

    // 움직였을 경우에만 업데이트
    if (bMoved)
    {
        // 맵 경계 제한
        float MAP_WIDTH = 5120.f * SCALE_FACTOR;
        float MAP_HEIGHT = 640.f * SCALE_FACTOR;

        // 예상 위치
        float fNewScrollX = m_fScrollX + fDeltaX;
        float fNewScrollY = m_fScrollY + fDeltaY;

        // 경계 조정
        if (fNewScrollX < 0.f)
            fDeltaX = -m_fScrollX;
        else if (fNewScrollX > MAP_WIDTH - WINCX)
            fDeltaX = (MAP_WIDTH - WINCX) - m_fScrollX;

        if (fNewScrollY < 0.f)
            fDeltaY = -m_fScrollY;
        else if (fNewScrollY > MAP_HEIGHT - WINCY)
            fDeltaY = (MAP_HEIGHT - WINCY) - m_fScrollY;

        // 스크롤 매니저 업데이트
        if (fDeltaX != 0.f)
            CScrollMgr::Get_Instance()->Set_ScrollX(fDeltaX);
        if (fDeltaY != 0.f)
            CScrollMgr::Get_Instance()->Set_ScrollY(fDeltaY);

        // 마지막 이동 시간 업데이트
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

    // 스크롤 위치 고려
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // 그리드 좌표 계산
    Screen_To_World(m_lMouseX, m_lMouseY, &m_fWorldX, &m_fWorldY);
    World_To_Grid(m_fWorldX, m_fWorldY, &m_fGridX, &m_fGridY);

    // 좌클릭 - 배치
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
    // 우클릭 - 제거
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
    // 모드 변경 : 1. 타일 2. 몬스터 3. 라인
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

    // 이전 타입으로 이동, 첫번째 인덱스라면 마지막 인덱스로 순환
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

    // 다음 타입으로 이동, 마지막 인덱스라면 첫번째 인덱스로 순환
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

    // 라인 타입 변경 : 수평 -> 수직 -> 임의 루프
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

// 그리드 기준 
void CEditor::Place_Point(float _wx, float _wy, float _gx, float _gy)
{
    if (!m_bClick)
    {
        p1 = { 0,0 };
        // 중심좌표 기준 왼쪽이면 왼쪽 X , 오른쪽이면 오른쪽 X
        if (_wx <= _gx)
            p1.fX = _gx - TILECX * SCALE_FACTOR * 0.5f;
        else
            p1.fX = _gx + TILECX * SCALE_FACTOR * 0.5f;
        // y도 마찬가지 : gdi좌표계에 주의
        // 아래에 있는경우
        if (_wy > _gy)
            p1.fY = _gy + TILECY * SCALE_FACTOR * 0.5f;
        // 위에 있는경우
        else
            p1.fY = _gy - TILECY * SCALE_FACTOR * 0.5f;
        m_bClick = true;
    }
    else
    {
        p2 = { 0, 0 };
        // 중심좌표 기준 왼쪽이면 왼쪽 X , 오른쪽이면 오른쪽 X
        if (_wx <= _gx)
            p2.fX = _gx - TILECX * SCALE_FACTOR * 0.5f;
        else
            p2.fX = _gx + TILECX * SCALE_FACTOR * 0.5f;
        // y도 마찬가지 : gdi좌표계에 주의
        // 아래에 있는경우
        if (_wy > _gy)
            p2.fY = _gy + TILECY * SCALE_FACTOR * 0.5f;
        // 위에 있는경우
        else
            p2.fY = _gy - TILECY * SCALE_FACTOR * 0.5f;
        m_bClick = false;
        // 두 좌표가 같지 않으면 라인 추가
        if ((p1.fX != p2.fX) && (p1.fY != p2.fY))
            CLineMgr::Get_Instance()->Add_Line(p1, p2);
    }
}

void CEditor::Screen_To_World(float screenX, float screenY, float* worldX, float* worldY)
{
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // 상대 -> 절대 좌표
    *worldX = (screenX + m_fScrollX);
    *worldY = (screenY + m_fScrollY);
}

// editor 에서 렌더링시 필요한가?
void CEditor::World_To_Screen(float worldX, float worldY, float* screenX, float* screenY)
{
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // 스크롤 위치를 고려하여 그리드 좌표 계산
    *screenX = (worldX - m_fScrollX);
    *screenY = (worldY - m_fScrollY);
}

void CEditor::World_To_Grid(float worldX, float worldY, float* gridX, float* gridY)
{
    // 월드 좌표를 그리드 인덱스로 변환 (정수 인덱스)
    int gridIndexX = static_cast<int>(worldX / 48.0f);
    int gridIndexY = static_cast<int>(worldY / 48.0f);

    // 그리드의 중심좌표 계산
    *gridX = (gridIndexX * 48.0f) + 24.0f;
    *gridY = (gridIndexY * 48.0f) + 24.0f;
}

void CEditor::Save_Data()
{
    const wchar_t* wc_Dir = L"../Data/";
    const wchar_t* wc_Dat = L".dat";
    for (auto i = 0; i < MODE_END; i++)
    {
        // 0. 타일 1. 몬스터 2. 라인 3. 아이템
        // enum 타입으로 넣는것 고민하기 싫어서 정수로
        string s_Name = to_string(i);
        wstring ws_Name(s_Name.begin(), s_Name.end());
        wstring ws_FullPath = wc_Dir + ws_Name + wc_Dat;
        const wchar_t* wc_FullPath = ws_FullPath.c_str();

        HANDLE	hFile = CreateFile(wc_FullPath,		// 파일 이름을 포함한 경로
            GENERIC_WRITE,			// 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
            NULL,					// 공유 방식(파일이 개방된 상태에서 다른 프로세스에서 사용하고자 할 때 허가할 것인가, null인 경우 공유하지 않음)
            NULL,					// 보안 속성(null인 경우 기본 값에 해당하는 보안 상태 설정)
            CREATE_ALWAYS,			// 생성 방식(CREATE_ALWAYS : 파일이 없으면 생성, 있으면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 불러오기)
            FILE_ATTRIBUTE_NORMAL,  // 파일 속성(일반적인 파일)
            NULL);					// 생성될 파일의 속성을 제공할 템플릿 파일(사용하지 않기 때문에 null)

        if (INVALID_HANDLE_VALUE == hFile)
        {
            MessageBox(g_hWnd, L"Save Error", _T("Error"), MB_OK);
            return;
        }

        DWORD	dwByte(0);
        // TILE, MONSTER, LINE, ITEM 순으로 저장
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
                //  TODO : 몬스터 타입 체크해서 넣기 , 바우저는 하드코딩
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
        // 0. 타일 1. 몬스터 2. 라인 3. 아이템
        // enum 타입으로 넣는것 고민하기 싫어서 정수로
        string s_Name = to_string(i);
        wstring ws_Name(s_Name.begin(), s_Name.end());
        wstring ws_FullPath = wc_Dir + ws_Name + wc_Dat;
        const wchar_t* wc_FullPath = ws_FullPath.c_str();
        HANDLE	hFile = CreateFile(wc_FullPath,		// 파일 이름을 포함한 경로
            GENERIC_READ,			// 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
            NULL,					// 공유 방식(파일이 개방된 상태에서 다른 프로세스에서 사용하고자 할 때 허가할 것인가, null인 경우 공유하지 않음)
            NULL,					// 보안 속성(null인 경우 기본 값에 해당하는 보안 상태 설정)
            OPEN_EXISTING,			// 생성 방식(CREATE_ALWAYS : 파일이 없으면 생성, 있으면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 불러오기)
            FILE_ATTRIBUTE_NORMAL,  // 파일 속성(일반적인 파일)
            NULL);					// 생성될 파일의 속성을 제공할 템플릿 파일(사용하지 않기 때문에 null)

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
        MessageBox(g_hWnd, L"Load 완료", _T("Success"), MB_OK);
    }
}

