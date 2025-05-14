#include "pch.h"
#include "CScene01.h"
#include "fmod_errors.h"

CScene01::CScene01()
{
}

CScene01::~CScene01()
{
	Release();
}

void CScene01::Initialize()
{
	m_vecBGM = { L"12. Overworld.mp3"};
	m_iCurBGMIndex = 0;

	m_bLogo = true;
	m_dwLogoStartTime = GetTickCount();

	CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
	CScrollMgr::Get_Instance()->Set_ScrollY(0.f);
	CScrollMgr::Get_Instance()->Set_Size(5120.f * SCALE_FACTOR, 432.f * SCALE_FACTOR);
	CSceneMgr::Get_Instance()->Load_Data(); // ���� ������ �ҷ�����
	
	// �� �� �÷��̾� ���� (Ÿ�� �ٴ� ��)
	CObject* pPlayer = new CPlayer;
	pPlayer->Initialize();  // ��������� �ʱ�ȭ
	pPlayer->Update();
	pPlayer->Late_Update();
	
	CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, pPlayer);

	// ��ũ�� ��ġ ����
	CScrollMgr::Get_Instance()->Set_ScrollX(pPlayer->Get_Info()->fX - WINCX * 0.42f);
	CScrollMgr::Get_Instance()->Set_ScrollY(pPlayer->Get_Info()->fY - WINCY * 0.5f);
	CScrollMgr::Get_Instance()->Scroll_Lock();
}

int CScene01::Update()
{
	if (m_bLogo)
	{
		DWORD dwCurTime = GetTickCount();
		if (dwCurTime - m_dwLogoStartTime >= m_dwLogoDuration)
		{
			m_bLogo = false;
			m_bFadeIn = true;
			m_dwFadeInStartTime = GetTickCount();

			// �ΰ� ������ BGM ����
			CSoundMgr::Get_Instance()->PlayBGM(m_vecBGM[m_iCurBGMIndex].c_str(), 0.5f);
		}
		return NOEVENT; // �ΰ� ǥ�� �߿��� �ٸ� ������Ʈ �ߴ�
	}

	// ���̵��� üũ
	if (m_bFadeIn)
	{
		DWORD dwCurTime = GetTickCount();
		if (dwCurTime - m_dwFadeInStartTime >= m_dwFadeInDuration)
		{
			m_bFadeIn = false; // ���̵��� �Ϸ�
		}
	}

	CObjectMgr::Get_Instance()->Update();
	// BGM ���� ���
	if (!CSoundMgr::Get_Instance()->Is_ChannelPlaying(SOUND_BGM))
	{
		++m_iCurBGMIndex;
		if (m_iCurBGMIndex >= m_vecBGM.size())
		{
			if (m_bLoopBGM)
				m_iCurBGMIndex = 0;
			else
				return NOEVENT; // ��
		}

		CSoundMgr::Get_Instance()->PlayBGM(m_vecBGM[m_iCurBGMIndex].c_str(), 0.5f);
	}
	return NOEVENT;
}

void CScene01::Late_Update()
{
	if(!m_bLogo)
	{
		CObjectMgr::Get_Instance()->Late_Update();
	}
}

void CScene01::Render(HDC hDC)
{
	// �ΰ� ǥ��
	if (m_bLogo)
	{
		// ���� ���
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
		Rectangle(hDC, 0, 0, WINCX, WINCY);
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);

		// Logo_1 �̹��� ǥ��
		HDC hLogoMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Logo1");
		if (hLogoMemDC)
		{
			int iLogoWidth = 256; // �ΰ� �̹��� ũ�� (���� ũ�⿡ �°� ����)
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

	m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
	m_fWorldX = m_fScrollX + WINCX * 0.5f;
	m_fWorldY = m_fScrollY + WINCY * 0.5f;

	// ��� �̹��� ��� (��ũ�� ���� or ��� ���� ���� ����)
	HDC hMemDC_back = CBmpMgr::Get_Instance()->Find_Image(L"BackGround01");
	if (hMemDC_back)
	{
		const int BACK_WIDTH = 512;
		const int BACK_HEIGHT = 432;

		int startIdx = (int)(m_fScrollX / (BACK_WIDTH * SCALE_FACTOR));
		int numBacks = WINCX / (BACK_WIDTH * SCALE_FACTOR) + 2;

		for (int i = 0; i < numBacks; ++i)
		{
			int drawX = (startIdx + i) * BACK_WIDTH * SCALE_FACTOR - (int)m_fScrollX;
			BitBlt(hDC, drawX, 0, BACK_WIDTH * SCALE_FACTOR, BACK_HEIGHT * SCALE_FACTOR, hMemDC_back, 0, 0, SRCCOPY);
		}
	}

	// Scene1 �̹��� ���
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Scene1");
	int srcX = static_cast<int>(CScrollMgr::Get_Instance()->Get_ScrollX() / SCALE_FACTOR);
	int srcY = static_cast<int>(CScrollMgr::Get_Instance()->Get_ScrollY() / SCALE_FACTOR);

	int srcW = WINCX / SCALE_FACTOR;
	int srcH = WINCY / SCALE_FACTOR;

	GdiTransparentBlt(
		hDC,
		0, 0,              // ȭ�� ���� ��ܿ� ���
		WINCX, WINCY,      // Ȯ��� ��� ũ��
		hMemDC,
		srcX, srcY,        // �̹����� �߶� ���� ��ġ
		srcW, srcH,
		RGB(0, 255, 0));

	// ������Ʈ, ����, UI�� ������ǥ ��� �� ������ �� ��ũ�� ����
	CLineMgr::Get_Instance()->Render(hDC);
	CObjectMgr::Get_Instance()->Render(hDC);
	CUiMgr::Get_Instance()->Render(hDC);

	// ���̵��� ȿ��
	if (m_bFadeIn)
	{
		DWORD dwCurTime = GetTickCount();
		DWORD dwElapsed = dwCurTime - m_dwFadeInStartTime;
		float fProgress = min(1.0f, static_cast<float>(dwElapsed) / m_dwFadeInDuration);

		// ��ӱ� ��� (���̵����̹Ƿ� 255���� 0����)
		int iDarkness = static_cast<int>(255 * (1.0f - fProgress));

		if (iDarkness > 0)
		{
			// ������ ��������
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

void CScene01::Release()
{
	CLineMgr::Get_Instance()->Destroy_Instance();
	CObjectMgr::Get_Instance()->Destroy_Instance();
}

