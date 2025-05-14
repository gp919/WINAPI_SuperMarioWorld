#include "pch.h"
#include "CLogo.h"

CLogo::CLogo()
{
}

CLogo::~CLogo()
{
	Release();
}

void CLogo::Initialize()
{
	m_vecBGM = { L"01. Nintendo Logo.mp3" };
	m_iCurBGMIndex = 0;
	CSoundMgr::Get_Instance()->PlayBGM(m_vecBGM[m_iCurBGMIndex].c_str(), 0.5f);
	
}

int CLogo::Update()
{
    if (!CSoundMgr::Get_Instance()->Is_ChannelPlaying(SOUND_BGM))
    {
        ++m_iCurBGMIndex;
        if (m_iCurBGMIndex >= m_vecBGM.size())
        {
            if (!m_bFadeOut)
            {
                // 페이드아웃 시작
                m_bFadeOut = true;
                m_dwFadeStartTime = GetTickCount();
            }

            // 페이드아웃 완료 확인
            if (m_bFadeOut)
            {
                DWORD dwCurTime = GetTickCount();
                if (dwCurTime - m_dwFadeStartTime >= m_dwFadeDuration)
                {
                    return DEAD; // 페이드아웃 완료 후 씬 종료
                }
            }
        }
        else
        {
            // 범위 내에 있을 때만 재생
            CSoundMgr::Get_Instance()->PlayBGM(m_vecBGM[m_iCurBGMIndex].c_str(), 0.5f);
        }
    }
    return NOEVENT;
}

void CLogo::Late_Update()
{
}

void CLogo::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Logo");
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

    int iWidth = WINCX / SCALE_FACTOR;
    int iHeight = WINCY / SCALE_FACTOR;

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
        // 일반 렌더링
        GdiTransparentBlt(
            hDC,
            0, 0,
            WINCX, WINCY,
            hMemDC,
            0, 0,
            iWidth, iHeight,
            RGB(0, 255, 0));
    }
}

void CLogo::Release()
{
}
