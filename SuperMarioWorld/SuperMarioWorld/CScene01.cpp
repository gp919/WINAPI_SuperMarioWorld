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
	CSoundMgr::Get_Instance()->StopAll();
	m_vecBGM = { L"12. Overworld.mp3"};
	m_iCurBGMIndex = 0;

	m_bLogo = true;
	m_dwLogoStartTime = GetTickCount();

	CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
	CScrollMgr::Get_Instance()->Set_ScrollY(0.f);
	CScrollMgr::Get_Instance()->Set_Size(5120.f * SCALE_FACTOR, 1120.f * SCALE_FACTOR);
	CSceneMgr::Get_Instance()->Load_Data(); // ���� ������ �ҷ�����
	
	// �� �� �÷��̾� ���� (Ÿ�� �ٴ� ��)
	pPlayer = new CPlayer;
	
	CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, pPlayer);

	pPlayer->Update();
	pPlayer->Late_Update();


	// ��ũ�� ��ġ ����
	CScrollMgr::Get_Instance()->Set_ScrollX(pPlayer->Get_Info()->fX - WINCX * 0.42f);
	CScrollMgr::Get_Instance()->Set_ScrollY(pPlayer->Get_Info()->fY - WINCY * 0.7f);
	CScrollMgr::Get_Instance()->Scroll_Lock();
}

int CScene01::Update()
{
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN))
	{
		float fx = pPlayer->Get_Info()->fX;
		float fy = pPlayer->Get_Info()->fY;

		if (fx >= 3872 * 3.f && fx <= 3904 * 3.f &&
			fy >= 1000.f && fy <= 1100.f)
		{
			pPlayer->Set_PosY(1488.f);

			CScrollMgr::Get_Instance()->Set_AbsScrollY(pPlayer->Get_Info()->fY - WINCY * 0.5f);
			CScrollMgr::Get_Instance()->Scroll_Lock();
			pPlayer->Set_ForceScroll(true);
		}
	}

	// ������ B: ���� �� ����
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP))
	{
		float fx = pPlayer->Get_Info()->fX;
		float fy = pPlayer->Get_Info()->fY;

		if (fx >= 4576 * 3.f - 10.f && fx <= 4576 * 3.f + 10.f &&
			fy >= 1776.f && fy <= 1872.f)
		{
			pPlayer->Set_PosX((4048 - 4576) * 3.f);
			pPlayer->Set_PosY((336 - 624) * 3.f);
			pPlayer->Set_ForceScroll(true);
			CSoundMgr::Get_Instance()->PlaySoundW(L"pipe.wav", SOUND_EFFECT, 0.1f);
		}
	}

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

	const auto& pipeList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PIPE);
	for (auto& pipeObj : pipeList)
	{
		CPipe* pPipe = dynamic_cast<CPipe*>(pipeObj);
		if (!pPipe) continue;

		if (pPipe->Is_Player_Enterable(pPlayer))
		{
			const POINTF& dest = pPipe->Get_DestPos();
			//pPlayer->Set_Pos(dest.x, dest.y);

			// �ɼ�: �� ���峪 ȿ�� �ֱ�
			CSoundMgr::Get_Instance()->PlaySoundW(L"pipe.wav", SOUND_EFFECT, 0.1f);

			break;
		}
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

	// TODO : �������� ������ ������Ʈ�� �浹 ������ ĳ���Ϳ� ��ũ���� �̵� <-> �ٽ� ���� ���� 
	// �������� ������ �»�� �߽���ǥ 11640, 1368
	// ������ ������ ���ϴ� �߽���ǥ 13800, 1848
	if (m_bPipe)
	{

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

	// BMP ������ ����� �ʵ��� ����
	const int maxBmpH = 1120;
	srcY = static_cast<int>(CScrollMgr::Get_Instance()->Get_ScrollY() / SCALE_FACTOR);
	srcH = WINCY / SCALE_FACTOR;

	// clamp ����ؼ� srcY�� �����ϰ� ����
	srcY = clamp(srcY, 0, maxBmpH - srcH);

	wchar_t szLog[256];
	swprintf(szLog, 256, L"[RENDER] ScrollY=%.2f srcY=%d srcH=%d\n",
		CScrollMgr::Get_Instance()->Get_ScrollY(), srcY, srcH);
	OutputDebugStringW(szLog);

	GdiTransparentBlt(
		hDC,
		0, 0,
		WINCX, WINCY,
		hMemDC,
		srcX, srcY,
		srcW, srcH,
		RGB(0, 255, 0));

	// ������Ʈ, ����, UI�� ������ǥ ��� �� ������ �� ��ũ�� ����
	CLineMgr::Get_Instance()->Render(hDC);
	CObjectMgr::Get_Instance()->Render(hDC);
	CUiMgr::Get_Instance()->Render(hDC);
	CUiMgr::Get_Instance()->Render_ScoreEffect(hDC, 0, 0);

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

