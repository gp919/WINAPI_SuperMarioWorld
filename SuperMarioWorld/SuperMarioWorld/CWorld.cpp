#include "pch.h"
#include "CWorld.h"

CWorld::CWorld()
{
}

CWorld::~CWorld()
{
}

void CWorld::Initialize()
{
	CSoundMgr::Get_Instance()->StopAll();
	m_tPlayer.Initialize(this);
	//test
	// ��ũ�� ���� (�ʿ� ��)
	CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
	CScrollMgr::Get_Instance()->Set_ScrollY(0.f);

	m_vecBGM = { L"03. Yoshi's Island.mp3" };
	m_iCurBGMIndex = 0;
	m_bLoopBGM = true;
	CSoundMgr::Get_Instance()->PlayBGM(m_vecBGM[m_iCurBGMIndex].c_str(), 0.5f);

}

int CWorld::Update()
{
	if (m_bFadeOut)
	{
		DWORD dwCurTime = GetTickCount();
		if (dwCurTime - m_dwFadeStartTime >= m_dwFadeDuration)
		{
			return DEAD; // ���̵�ƿ� �Ϸ� �� �� ����
		}
	}

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

	// ���̵�ƿ� ���̸� �Է� ����
	if (m_bFadeOut)
		return NOEVENT;

	if (CKeyMgr::Get_Instance()->Key_Down(VK_RIGHT))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"fireball.wav", SOUND_EFFECT, 0.1f);
		++m_iWorld;
		if (m_iWorld > 3) m_iWorld = 0;
		m_tPlayer.Set_Target(m_iWorld);
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_LEFT))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"fireball.wav", SOUND_EFFECT, 0.1f);
		--m_iWorld;
		if (m_iWorld < 0) m_iWorld = 3;
		m_tPlayer.Set_Target(m_iWorld);
	}
	m_tPlayer.Update();
	// Space �Է� �� ���� Ȯ��
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPACE))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"coin.wav", SOUND_EFFECT, 0.1f);
		// ���̵�ƿ� ����
		m_bFadeOut = true;
		m_dwFadeStartTime = GetTickCount();
		switch (m_iWorld)
		{
		// Ʃ�丮��
		case 0:
			CSceneMgr::Get_Instance()->Change_Scene(SC_MENU);
			break;
		// ��������1
		case 1:
			CSceneMgr::Get_Instance()->Change_Scene(SC_STAGE_ONE);
			break;
		// ��������2
		case 2:
			CSceneMgr::Get_Instance()->Change_Scene(SC_STAGE_TWO);
			break;
		// ��������final
		case 3:
			CSceneMgr::Get_Instance()->Change_Scene(SC_BOSS);
			break;
		}
		return DEAD;
	}

	return NOEVENT;
}

void CWorld::Late_Update()
{

}
void CWorld::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_strCurBackground.c_str());
	if (!hMemDC) return;

	int iAlpha = 255;
	if (m_bFadeOut)
	{
		DWORD dwCurTime = GetTickCount();
		DWORD dwElapsed = dwCurTime - m_dwFadeStartTime;
		if (dwElapsed >= m_dwFadeDuration)
		{
			iAlpha = 0;
		}
		else
		{
			// ���� �������� ���İ� ��� (255���� 0����)
			float fProgress = static_cast<float>(dwElapsed) / m_dwFadeDuration;
			iAlpha = static_cast<int>(255 * (1.0f - fProgress));
		}
	}

	// GDI���� ���� ȿ���� �����ϴ� ���
	if (iAlpha < 255)
	{
		// ���� ��� �׸���
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
		Rectangle(hDC, 0, 0, WINCX, WINCY);
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);

		// ���� ���� ȿ�� (������ ����)
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = iAlpha;
		bf.AlphaFormat = 0;

		AlphaBlend(hDC, 0, 0, WINCX, WINCY,
			hMemDC, 0, 0, 256, 224, bf);

		// �÷��̾ ���� ���İ����� ������
		if (iAlpha > 0)
		{
			HDC hTempDC = CreateCompatibleDC(hDC);
			HBITMAP hTempBitmap = CreateCompatibleBitmap(hDC, WINCX, WINCY);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hTempBitmap);

			// ���� ������� �ʱ�ȭ
			HBRUSH hTransBrush = CreateSolidBrush(RGB(0, 255, 0));
			HBRUSH hOldTransBrush = (HBRUSH)SelectObject(hTempDC, hTransBrush);
			Rectangle(hTempDC, 0, 0, WINCX, WINCY);
			SelectObject(hTempDC, hOldTransBrush);
			DeleteObject(hTransBrush);

			// �÷��̾� �׸���
			m_tPlayer.Render(hTempDC);

			// ���ĺ���� �÷��̾� �׸���
			AlphaBlend(hDC, 0, 0, WINCX, WINCY,
				hTempDC, 0, 0, WINCX, WINCY, bf);

			// ���ҽ� ����
			SelectObject(hTempDC, hOldBitmap);
			DeleteObject(hTempBitmap);
			DeleteDC(hTempDC);
		}
	}
	else
	{
		// �Ϲ� ������
		GdiTransparentBlt(hDC, 0, 0, WINCX, WINCY,
			hMemDC, 0, 0, 256, 224, RGB(0, 255, 0));
		m_tPlayer.Render(hDC);
	}
}

void CWorld::Release()
{

}

void CWorld::Set_Background(const wstring& str)
{
	m_strCurBackground = str;
}
