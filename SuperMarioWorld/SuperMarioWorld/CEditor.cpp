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

void CEditor::Run()
{
    Initialize();

    // �������� �ε� �κ� : ���߿� enum���� ����
    CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Scene1/Scene1.bmp", L"Scene1");
    Load_Data();
    


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
    m_CurStage = 1;
    CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
    CScrollMgr::Get_Instance()->Set_ScrollY(0.f);

    CScrollMgr::Get_Instance()->Set_Size(5120.f * SCALE_FACTOR, 432.f * SCALE_FACTOR);

    CObjectMgr::Get_Instance()->Initialize();

}

void CEditor::Update()
{
    // ��ũ�� �����̱�
    Move_Scroll();

    // ��� ���� : 1. Ÿ�� 2. ���� 3. ����
    if (CKeyMgr::Get_Instance()->Key_Down('1'))
        m_eCurEdit = MODE_TILE;
    else if (CKeyMgr::Get_Instance()->Key_Down('2'))
        m_eCurEdit = MODE_MONSTER;
    else if (CKeyMgr::Get_Instance()->Key_Down('3'))
        m_eCurEdit = MODE_LINE;;

    // Ÿ�� ���� ���� 1. Q. ����ǥ E. ����ǥ R. ȸ�� C. ���� H. ���� O. �⺻
    if (m_eCurEdit == MODE_TILE) {
        if (CKeyMgr::Get_Instance()->Key_Down('Q')) m_eCurTile = TILE_Q;
        else if (CKeyMgr::Get_Instance()->Key_Down('E')) m_eCurTile = TILE_E;
        else if (CKeyMgr::Get_Instance()->Key_Down('R')) m_eCurTile = TILE_ROT;
        else if (CKeyMgr::Get_Instance()->Key_Down('C')) m_eCurTile = TILE_CLOUD;
        else if (CKeyMgr::Get_Instance()->Key_Down('H')) m_eCurTile = TILE_HIDDEN;
        else if (CKeyMgr::Get_Instance()->Key_Down('0')) m_eCurTile = TILE_EMPTY;
    }

    
    // TODO : ���� ���� ���� (���� Ű 4-9)
    //if (m_eMode == EditorMode::MONSTER_MODE) {
    //    for (int i = '4'; i <= '9'; ++i) {
    //        if (CKeyMgr::Get_Instance()->Key_Down(i)) {
    //            m_iSelectedMonster = i - '4';  // 0���� �����ϴ� �ε����� ��ȯ
    //        }
    //    }
    //}


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
        return;
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

void CEditor::Move_Scroll()
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

//void CEditor::Handle_Mouse_Input()
//{
//    // ���콺 ��ǥ ���
//    POINT ptMouse = {};
//    GetCursorPos(&ptMouse);
//    ScreenToClient(g_hWnd, &ptMouse);
//
//    // ��ũ�� ��ġ ���
//    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
//    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
//
//    // �׸��� ��ǥ ���
//    int gridX, gridY;
//    Screen_To_Grid(ptMouse.x, ptMouse.y, &gridX, &gridY);
//
//    // ��Ŭ�� - ��ġ
//    if (CKeyMgr::Get_Instance()->Key_Down(VK_LBUTTON)) {
//        switch (m_eCurEdit) {
//        case MODE_TILE:
//            Place_Tile(gridX, gridY);
//            break;
//        case MODE_MONSTER:
//            Place_Monster(gridX, gridY);
//            break;
//        case MODE_LINE:
//            Place_Line(gridX, gridY);
//            break;
//        }
//    }
//    // ��Ŭ�� - ����
//    else if (CKeyMgr::Get_Instance()->Key_Down(VK_RBUTTON)) {
//        Remove_Object(gridX, gridY);
//    }
//}

void CEditor::Screen_To_Grid(float screenX, float screenY, float* gridX, float* gridY)
{
    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // ��ũ�� ��ġ�� ����Ͽ� �׸��� ��ǥ ���
    *gridX = (screenX + fScrollX) / (TILECX * SCALE_FACTOR);
    *gridY = (screenY + fScrollY) / (TILECY * SCALE_FACTOR);
}

void CEditor::Grid_To_Screen(float gridX, float gridY, float* screenX, float* screenY)
{
    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // ��ũ�� ��ġ�� ����Ͽ� �׸��� ��ǥ ���
    *screenX = (gridX + fScrollX) / (TILECX * SCALE_FACTOR);
    *screenY = (gridY + fScrollY) / (TILECY * SCALE_FACTOR);
}

void CEditor::Save_Data()
{
    const wchar_t* wc_Dir = L"../Data/";
    const wchar_t* wc_Dat = L".dat";
    
    for (auto i = 0; i < MODE_END; i++)
    {
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
        // TILE, MONSTER, LINE ������ ����
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
                WriteFile(hFile, (pobj->Get_Info()), sizeof(INFO), &dwByte, nullptr);
            }
            break;
        case MODE_LINE:
            for (auto& pLine : CLineMgr::Get_Instance()->Get_LinetList())
            {
               WriteFile(hFile, (pLine->Get_Line()), sizeof(LINE), &dwByte, nullptr);
            }
            break;
        }
        
        CloseHandle(hFile);
        MessageBox(g_hWnd, wc_FullPath, _T("Success"), MB_OK);
    }
    
}

void CEditor::Load_Data( )
{
    const wchar_t* wc_Dir = L"../Data/";
    const wchar_t* wc_Dat = L".dat";

    for (auto i = 0; i < MODE_END; i++)
    {
        // 0. Ÿ�� 1. ���� 2. ����
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
                CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, new CKoopa(tInfo));
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
        }

        CloseHandle(hFile);
        MessageBox(g_hWnd, L"Load �Ϸ�", _T("Success"), MB_OK);
    }
}

