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
	//test
	// ��ũ�� ���� (�ʿ� ��)
	CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
	CScrollMgr::Get_Instance()->Set_ScrollY(0.f);

	m_vecBGM = { L"03.Yoshi's Island.mp3" };
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
		if (m_iWorld > 3)
			m_iWorld = 0;
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_LEFT))
	{
		CSoundMgr::Get_Instance()->PlaySoundW(L"fireball.wav", SOUND_EFFECT, 0.1f);
		--m_iWorld;
		if (m_iWorld < 0)
			m_iWorld = 3;
	}

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
			break;
		// ��������1
		case 1:
			break;
		// ��������2
		case 2:
			break;
		// ��������final
		case 3:
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

}

void CWorld::Release()
{

}