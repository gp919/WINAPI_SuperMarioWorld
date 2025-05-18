#include "pch.h"
#include "CSceneFinal.h"

CSceneFinal::CSceneFinal()
{
}

CSceneFinal::~CSceneFinal()
{
	Release();
}

void CSceneFinal::Initialize()
{
	CSoundMgr::Get_Instance()->StopAll();
    CSoundMgr::Get_Instance()->PlayBGM(L"43. Boss Battle.mp3", 0.5f);
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_MONSTER);
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_TILE);
    CObjectMgr::Get_Instance()->Delete_Object(OBJ_ITEM);
    CLineMgr::Get_Instance()->Release();

    m_bLogo = true;
    m_dwLogoStartTime = GetTickCount();

    // 좌측 세로
    CLineMgr::Get_Instance()->Add_Line({ 256 * SCALE_FACTOR,224 * SCALE_FACTOR }, { 256 * SCALE_FACTOR,448 * SCALE_FACTOR });
    // 상단
    CLineMgr::Get_Instance()->Add_Line({ 256 * SCALE_FACTOR,224 * SCALE_FACTOR }, { 512 * SCALE_FACTOR,224 * SCALE_FACTOR });
    // 우측 세로
    CLineMgr::Get_Instance()->Add_Line({ 512 * SCALE_FACTOR,224 * SCALE_FACTOR }, { 512 * SCALE_FACTOR,448 * SCALE_FACTOR });
    // 하단
    CLineMgr::Get_Instance()->Add_Line({ 256 * SCALE_FACTOR,448 * SCALE_FACTOR }, { 512 * SCALE_FACTOR,448 * SCALE_FACTOR });
    CLineMgr::Get_Instance()->Add_Line({ 256 * SCALE_FACTOR,432 * SCALE_FACTOR }, { 512 * SCALE_FACTOR,432 * SCALE_FACTOR });
    // 하단 라인 위에 타일 : Render X
    
    CTile* pTile;
    for (int i = 0; i < 18; ++i)
    {
        float fX = (256.f + i * 16.f) * SCALE_FACTOR;
        float fY = 416.f * SCALE_FACTOR + 32.f;

        CTile* pTile = new CTile(fX, fY, TILE_HIDDEN);
        CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, pTile);
    }
    
	
   /* pPlayer = dynamic_cast <CPlayer*>(CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER).front());
	pPlayer->Initialize();*/
    // 그 후 플레이어 생성 (타일 바닥 위)
    CObject* pPlayer;
    if (CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER).empty())
    {
        pPlayer = new CPlayer;
        CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, pPlayer);
    }
    else
    {
        pPlayer = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER).front();
        pPlayer->Initialize();
    }
        
    
    pPlayer->Get_Info()->fX = 273.f * SCALE_FACTOR;
    pPlayer->Get_Info()->fY = 400.f * SCALE_FACTOR;
    pPlayer->Update();
    pPlayer->Late_Update();

    m_fScrollX = pPlayer->Get_Info()->fX - WINCX * 0.42f;
    m_fScrollY = pPlayer->Get_Info()->fY - WINCY * 0.5f;
    const float minX = 256.f * SCALE_FACTOR;
    const float maxX = 512.f * SCALE_FACTOR - WINCX;
    const float minY = 224.f * SCALE_FACTOR;
    const float maxY = 448.f * SCALE_FACTOR - WINCY;
    m_fScrollX = clamp(m_fScrollX, minX, maxX);
    m_fScrollY = clamp(m_fScrollY, minY, maxY);
    CScrollMgr::Get_Instance()->Set_ScrollX(m_fScrollX);
    CScrollMgr::Get_Instance()->Set_ScrollY(m_fScrollY);


    // 쿠파 배치 좌표 (대략적으로 중앙 상단에 등장)
    float fBowserX = 384.f * SCALE_FACTOR;
    float fBowserY = 320.f * SCALE_FACTOR;
    CBowser* pBowser = new CBowser;
    pBowser->Set_PosX(fBowserX);  // 등장 위치
    pBowser->Set_PosY(fBowserY);
    CObjectMgr::Get_Instance()->Add_Object(OBJ_BOWSER, pBowser);


}

int CSceneFinal::Update()
{
    if (m_bLogo)
    {
        DWORD dwCurTime = GetTickCount();
        if (dwCurTime - m_dwLogoStartTime >= m_dwLogoDuration)
        {
            m_bLogo = false;
            m_bFadeIn = true;
            m_dwFadeInStartTime = GetTickCount();
        }
        return NOEVENT; // 로고 표시 중에는 다른 업데이트 중단
    }

    // 페이드인 체크
    if (m_bFadeIn)
    {
        DWORD dwCurTime = GetTickCount();
        if (dwCurTime - m_dwFadeInStartTime >= m_dwFadeInDuration)
        {
            m_bFadeIn = false; // 페이드인 완료
        }
    }
	CObjectMgr::Get_Instance()->Update();

	return NOEVENT;
}

void CSceneFinal::Late_Update()
{
	CObjectMgr::Get_Instance()->Late_Update();
}

void CSceneFinal::Render(HDC hDC)
{
    // 로고 표시
    if (m_bLogo)
    {
        // 검은 배경
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
        Rectangle(hDC, 0, 0, WINCX, WINCY);
        SelectObject(hDC, hOldBrush);
        DeleteObject(hBrush);

        // Logo_1 이미지 표시
        HDC hLogoMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Logo1");
        if (hLogoMemDC)
        {
            int iLogoWidth = 256; // 로고 이미지 크기 (실제 크기에 맞게 조정)
            int iLogoHeight = 224;
            int iDrawX = (WINCX - iLogoWidth * SCALE_FACTOR) / 2;
            int iDrawY = (WINCY - iLogoHeight * SCALE_FACTOR) / 2;

            GdiTransparentBlt(
                hDC,
                iDrawX, iDrawY,
                iLogoWidth * SCALE_FACTOR, iLogoHeight * SCALE_FACTOR,
                hLogoMemDC,
                0, 0,
                iLogoWidth, iLogoHeight,
                RGB(0, 255, 0));
        }
        return;
    }

    HDC hMemDC_back = CBmpMgr::Get_Instance()->Find_Image(L"BackF");
    if (hMemDC_back)
    {
        BitBlt(hDC, 0, 0, Get_MapSize().first, Get_MapSize().second + (int)m_fScrollY, hMemDC_back, 0, 0, SRCCOPY);
    }
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
    m_fWorldX = m_fScrollX + WINCX * 0.5f;
    m_fWorldY = m_fScrollY + WINCY * 0.5f;

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"SceneBoss");
    int srcX = static_cast<int>(CScrollMgr::Get_Instance()->Get_ScrollX() / SCALE_FACTOR);
    int srcY = static_cast<int>(CScrollMgr::Get_Instance()->Get_ScrollY() / SCALE_FACTOR);

    int srcW = WINCX / SCALE_FACTOR;
    int srcH = WINCY / SCALE_FACTOR;

    GdiTransparentBlt(
        hDC,
        0, 0,              // 화면 좌측 상단에 출력
        WINCX, WINCY,      // 확대된 출력 크기
        hMemDC,
        srcX, srcY,        // 이미지의 잘라낼 시작 위치
        srcW, srcH,
        RGB(0, 255, 0));

    CLineMgr::Get_Instance()->Render(hDC);
    CObjectMgr::Get_Instance()->Render(hDC);
    CUiMgr::Get_Instance()->Render(hDC);

    // 페이드인 효과
    if (m_bFadeIn)
    {
        DWORD dwCurTime = GetTickCount();
        DWORD dwElapsed = dwCurTime - m_dwFadeInStartTime;
        float fProgress = min(1.0f, static_cast<float>(dwElapsed) / m_dwFadeInDuration);

        // 어둡기 계산 (페이드인이므로 255에서 0으로)
        int iDarkness = static_cast<int>(255 * (1.0f - fProgress));

        if (iDarkness > 0)
        {
            // 검은색 오버레이
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            HDC hTempDC = CreateCompatibleDC(hDC);
            HBITMAP hBitmap = CreateCompatibleBitmap(hDC, WINCX, WINCY);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hBitmap);

            RECT rect = { 0, 0, WINCX, WINCY };
            FillRect(hTempDC, &rect, hBrush);

            BLENDFUNCTION bf = { 0 };
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = static_cast<BYTE>(iDarkness);
            bf.AlphaFormat = 0;

            AlphaBlend(hDC, 0, 0, WINCX, WINCY,
                hTempDC, 0, 0, WINCX, WINCY, bf);

            SelectObject(hTempDC, hOldBitmap);
            DeleteObject(hBitmap);
            DeleteDC(hTempDC);
            DeleteObject(hBrush);
        }
    }
}

void CSceneFinal::Release()
{
	CLineMgr::Get_Instance()->Destroy_Instance();
	CObjectMgr::Get_Instance()->Destroy_Instance();
}
