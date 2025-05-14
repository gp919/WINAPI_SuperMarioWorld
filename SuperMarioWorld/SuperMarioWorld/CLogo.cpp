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
                // ���̵�ƿ� ����
                m_bFadeOut = true;
                m_dwFadeStartTime = GetTickCount();
            }

            // ���̵�ƿ� �Ϸ� Ȯ��
            if (m_bFadeOut)
            {
                DWORD dwCurTime = GetTickCount();
                if (dwCurTime - m_dwFadeStartTime >= m_dwFadeDuration)
                {
                    return DEAD; // ���̵�ƿ� �Ϸ� �� �� ����
                }
            }
        }
        else
        {
            // ���� ���� ���� ���� ���
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
            // ���� �������� ���İ� ��� (255���� 0����)
            float fProgress = static_cast<float>(dwElapsed) / m_dwFadeDuration;
            iAlpha = static_cast<int>(255 * (1.0f - fProgress));
        }
    }

    int iWidth = WINCX / SCALE_FACTOR;
    int iHeight = WINCY / SCALE_FACTOR;

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
            hMemDC, 0, 0, iWidth, iHeight, bf);
    }
    else
    {
        // �Ϲ� ������
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
