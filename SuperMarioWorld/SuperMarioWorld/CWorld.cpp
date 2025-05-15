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
	// 스크롤 고정 (필요 시)
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
	// Space 입력 → 선택 확정
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPACE))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"coin.wav", SOUND_EFFECT, 0.1f);
		// 페이드아웃 시작
		m_bFadeOut = true;
		m_dwFadeStartTime = GetTickCount();
		switch (m_iWorld)
		{
		// 튜토리얼
		case 0:
			CSceneMgr::Get_Instance()->Change_Scene(SC_MENU);
			break;
		// 스테이지1
		case 1:
			CSceneMgr::Get_Instance()->Change_Scene(SC_STAGE_ONE);
			break;
		// 스테이지2
		case 2:
			CSceneMgr::Get_Instance()->Change_Scene(SC_STAGE_TWO);
			break;
		// 스테이지final
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
			hMemDC, 0, 0, 256, 224, bf);

		// 플레이어도 같은 알파값으로 렌더링
		if (iAlpha > 0)
		{
			HDC hTempDC = CreateCompatibleDC(hDC);
			HBITMAP hTempBitmap = CreateCompatibleBitmap(hDC, WINCX, WINCY);
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hTempDC, hTempBitmap);

			// 투명 배경으로 초기화
			HBRUSH hTransBrush = CreateSolidBrush(RGB(0, 255, 0));
			HBRUSH hOldTransBrush = (HBRUSH)SelectObject(hTempDC, hTransBrush);
			Rectangle(hTempDC, 0, 0, WINCX, WINCY);
			SelectObject(hTempDC, hOldTransBrush);
			DeleteObject(hTransBrush);

			// 플레이어 그리기
			m_tPlayer.Render(hTempDC);

			// 알파블렌드로 플레이어 그리기
			AlphaBlend(hDC, 0, 0, WINCX, WINCY,
				hTempDC, 0, 0, WINCX, WINCY, bf);

			// 리소스 정리
			SelectObject(hTempDC, hOldBitmap);
			DeleteObject(hTempBitmap);
			DeleteDC(hTempDC);
		}
	}
	else
	{
		// 일반 렌더링
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
