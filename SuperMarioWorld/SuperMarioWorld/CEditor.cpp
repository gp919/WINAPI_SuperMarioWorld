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

    // 스테이지 로딩 부분 : 나중에 enum으로 관리
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
    m_CurStage = 1;
    CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
    CScrollMgr::Get_Instance()->Set_ScrollY(0.f);

    CScrollMgr::Get_Instance()->Set_Size(5120.f * SCALE_FACTOR, 432.f * SCALE_FACTOR);

    CObjectMgr::Get_Instance()->Initialize();

}

void CEditor::Update()
{
    // 스크롤 움직이기
    Move_Scroll();

    // 모드 변경 : 1. 타일 2. 몬스터 3. 라인
    if (CKeyMgr::Get_Instance()->Key_Down('1'))
        m_eCurEdit = MODE_TILE;
    else if (CKeyMgr::Get_Instance()->Key_Down('2'))
        m_eCurEdit = MODE_MONSTER;
    else if (CKeyMgr::Get_Instance()->Key_Down('3'))
        m_eCurEdit = MODE_LINE;;

    // 타일 종류 선택 1. Q. 물음표 E. 느낌표 R. 회전 C. 구름 H. 투명 O. 기본
    if (m_eCurEdit == MODE_TILE) {
        if (CKeyMgr::Get_Instance()->Key_Down('Q')) m_eCurTile = TILE_Q;
        else if (CKeyMgr::Get_Instance()->Key_Down('E')) m_eCurTile = TILE_E;
        else if (CKeyMgr::Get_Instance()->Key_Down('R')) m_eCurTile = TILE_ROT;
        else if (CKeyMgr::Get_Instance()->Key_Down('C')) m_eCurTile = TILE_CLOUD;
        else if (CKeyMgr::Get_Instance()->Key_Down('H')) m_eCurTile = TILE_HIDDEN;
        else if (CKeyMgr::Get_Instance()->Key_Down('0')) m_eCurTile = TILE_EMPTY;
    }

    
    // TODO : 몬스터 종류 선택 (숫자 키 4-9)
    //if (m_eMode == EditorMode::MONSTER_MODE) {
    //    for (int i = '4'; i <= '9'; ++i) {
    //        if (CKeyMgr::Get_Instance()->Key_Down(i)) {
    //            m_iSelectedMonster = i - '4';  // 0부터 시작하는 인덱스로 변환
    //        }
    //    }
    //}


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
        return;
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

void CEditor::Move_Scroll()
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

//void CEditor::Handle_Mouse_Input()
//{
//    // 마우스 좌표 얻기
//    POINT ptMouse = {};
//    GetCursorPos(&ptMouse);
//    ScreenToClient(g_hWnd, &ptMouse);
//
//    // 스크롤 위치 고려
//    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
//    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
//
//    // 그리드 좌표 계산
//    int gridX, gridY;
//    Screen_To_Grid(ptMouse.x, ptMouse.y, &gridX, &gridY);
//
//    // 좌클릭 - 배치
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
//    // 우클릭 - 제거
//    else if (CKeyMgr::Get_Instance()->Key_Down(VK_RBUTTON)) {
//        Remove_Object(gridX, gridY);
//    }
//}

void CEditor::Screen_To_Grid(float screenX, float screenY, float* gridX, float* gridY)
{
    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // 스크롤 위치를 고려하여 그리드 좌표 계산
    *gridX = (screenX + fScrollX) / (TILECX * SCALE_FACTOR);
    *gridY = (screenY + fScrollY) / (TILECY * SCALE_FACTOR);
}

void CEditor::Grid_To_Screen(float gridX, float gridY, float* screenX, float* screenY)
{
    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    // 스크롤 위치를 고려하여 그리드 좌표 계산
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
        // TILE, MONSTER, LINE 순으로 저장
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
        // 0. 타일 1. 몬스터 2. 라인
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
        MessageBox(g_hWnd, L"Load 완료", _T("Success"), MB_OK);
    }
}

