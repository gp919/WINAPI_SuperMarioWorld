#include "pch.h"
#include "CMenu.h"
#include "CSoundMgr.h"

CMenu::CMenu() : m_iSelected(0), m_iIndex(0), m_fScrollX(0.f), m_fScrollY(0.f)
{
}

CMenu::~CMenu()
{
	Release();
}

void CMenu::Initialize()
{
	// 스크롤 고정 (필요 시)
	CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
	CScrollMgr::Get_Instance()->Set_ScrollY(0.f);

	m_vecBGM = { L"02. Title.mp3"};
	m_iCurBGMIndex = 0;
	m_bLoopBGM = true;
	CSoundMgr::Get_Instance()->PlayBGM(m_vecBGM[m_iCurBGMIndex].c_str(), 0.5f);
}

int CMenu::Update()
{
	if (m_bFadeOut)
	{
		DWORD dwCurTime = GetTickCount();
		if (dwCurTime - m_dwFadeStartTime >= m_dwFadeDuration)
		{
			return DEAD; // 페이드아웃 완료 후 씬 종료
		}
	}

	// BGM 순차 재생
	if (!CSoundMgr::Get_Instance()->Is_ChannelPlaying(SOUND_BGM))
	{
		++m_iCurBGMIndex;
		if (m_iCurBGMIndex >= m_vecBGM.size())
		{
			if (m_bLoopBGM)
				m_iCurBGMIndex = 0;
			else
				return NOEVENT; // 끝
		}

		CSoundMgr::Get_Instance()->PlayBGM(m_vecBGM[m_iCurBGMIndex].c_str(), 0.5f);
	}

	// 페이드아웃 중이면 입력 무시
	if (m_bFadeOut)
		return NOEVENT;

	if (CKeyMgr::Get_Instance()->Key_Down(VK_DOWN))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"fireball.wav", SOUND_EFFECT, 0.1f);
		++m_iSelected;
		if (m_iSelected > 3)
			m_iSelected = 0;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_UP))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"fireball.wav", SOUND_EFFECT, 0.1f);
		--m_iSelected;
		if (m_iSelected < 0)
			m_iSelected = 3;
	}

    // Space 입력 → 선택 확정
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPACE))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"coin.wav", SOUND_EFFECT, 0.1f);
		switch (m_iIndex)
		{
		case 0:
			m_iIndex++;
			m_iSelected = 0;
			break;

		case 1:
			if (m_iSelected == 0)
			{
				m_iIndex += 2;
				m_iSelected = 0;
				break;
			}
			else if (m_iSelected == 3)
			{
				m_iIndex++;
				m_iSelected = 0;
				break;
			}
			else
			{
				m_iIndex--;
				m_iSelected = 0;
				break;
			}

		case 2:
			if (m_iSelected == 3)
			{
				m_iIndex--;
				m_iSelected = 0;
				break;
			}
			break;

		case 3:
			if (m_iSelected == 0)
			{
				// 페이드아웃 시작
				m_bFadeOut = true;
				m_dwFadeStartTime = GetTickCount();
			}
			else if (m_iSelected == 1)
			{
				// editmode : 미구현
			}
			break;
		}
	}

    return NOEVENT;
}

void CMenu::Late_Update()
{
}

void CMenu::Render(HDC hDC)
{
	int iWidth = WINCX / SCALE_FACTOR;
	int iHeight = WINCY / SCALE_FACTOR;
	float fOffsetX(0.f);
	float fOffsetY(0.f);

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"BackLogo");
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
			// 선형 보간으로 알파값 계산 (255에서 0으로)
			float fProgress = static_cast<float>(dwElapsed) / m_dwFadeDuration;
			iAlpha = static_cast<int>(255 * (1.0f - fProgress));
		}
	}
	// GDI에서 투명도 효과를 구현하는 방법
	if (iAlpha < 255)
	{
		// 검은 배경 그리기
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
		Rectangle(hDC, 0, 0, WINCX, WINCY);
		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);

		// 알파 블렌드 효과 (간단한 구현)
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = iAlpha;
		bf.AlphaFormat = 0;

		AlphaBlend(hDC, 0, 0, WINCX, WINCY,
			hMemDC, 0, 0, iWidth, iHeight, bf);
	}
	else
	{

		GdiTransparentBlt(
			hDC,
			0, 0,
			WINCX, WINCY,
			hMemDC,
			0, 0,
			WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,
			RGB(0, 255, 0));

		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Menu");



		switch (m_iIndex)
		{
		case 0:
			fOffsetX = 40.f;
			fOffsetY = 144.f + m_iSelected * 16.f;
			break;
		case 1:
			fOffsetX = 64.f;  // 320 - 256
			fOffsetY = 120.f + m_iSelected * 16.f;
			break;
		case 2:
			fOffsetX = 40.f;  // 552 - 512
			fOffsetY = 120.f + m_iSelected * 16.f;
			break;
		case 3:
			fOffsetX = 64.f;  // 832 - 768
			fOffsetY = 128.f + m_iSelected * 16.f;
			break;
		}

		// 비트맵 기준 커서 Y 위치 보정
		int bmpOffsetY = m_iIndex * iHeight;

		GdiTransparentBlt(
			hDC,
			0, 0,
			WINCX, WINCY,
			hMemDC,
			m_iIndex * iWidth,  // ← X좌표 기준 잘라야 함!!
			0,                  // ← Y는 항상 0
			iWidth, iHeight,
			RGB(0, 255, 0));

		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Cursor");

		GdiTransparentBlt(
			hDC,
			static_cast<int>(fOffsetX * SCALE_FACTOR),
			static_cast<int>(fOffsetY * SCALE_FACTOR),
			static_cast<int>(TILECX * 0.5f * SCALE_FACTOR),
			static_cast<int>(TILECY * 0.5f * SCALE_FACTOR),
			hMemDC,
			0, 0,
			static_cast<int>(TILECX * 0.5f), static_cast<int>(TILECY * 0.5f),
			RGB(0, 255, 0));
	}
}

void CMenu::Release()
{
}
