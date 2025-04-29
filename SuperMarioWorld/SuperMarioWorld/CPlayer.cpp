#pragma once
#include "pch.h"
#include "CPlayer.h"

CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	Release();
}

void CPlayer::Initialize()
{
	m_tInfo = { 8.f, WINCY-8.f, 16.f, 16.f };
}

int CPlayer::Update()
{
	Key_Input();
	CObject::Update_Rect();
	
	return 0;
}

void CPlayer::Late_Update()
{
}

void CPlayer::Render(HDC hDC)
{
	Rectangle(hDC, m_tInfo.fX - (m_tInfo.fCX / 2), m_tInfo.fY - (m_tInfo.fCY / 2), m_tInfo.fX + (m_tInfo.fCX / 2), m_tInfo.fY + (m_tInfo.fCY / 2));
}

void CPlayer::Release()
{
}

void CPlayer::Key_Input()
{
	// �̵� : �¿�
	if (GetAsyncKeyState(VK_LEFT))
	{
		m_tInfo.fX -= m_fSpeed;
	}

	else if (GetAsyncKeyState(VK_RIGHT))
	{
		m_tInfo.fX += m_fSpeed;
	}

	// �÷�����, ��ũ����
	else if (GetAsyncKeyState(VK_UP))
	{

	}
	
	else if (GetAsyncKeyState(VK_DOWN))
	{

	}

	// ����, ��������, ���
	else if (GetAsyncKeyState('X'))
	{
		
	}

	else if (GetAsyncKeyState(VK_LSHIFT))
	{

	}

	else if (GetAsyncKeyState('A'))
	{

	}

}
