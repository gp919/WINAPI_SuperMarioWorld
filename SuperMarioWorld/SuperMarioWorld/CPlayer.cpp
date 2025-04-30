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
	m_tInfo = { TILECX / 2.f + 0.1f, WINCY - (TILECY / 2.f + 0.1f), TILECX, TILECY };
	m_fSpeed = 10.f;
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/Mario_LMove.bmp", L"Player_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/Mario_RMove.bmp", L"Player_RIGHT");

	m_eDir = DIR_RIGHT;
	m_pFrameKey = L"Player_RIGHT";

	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 3;
	m_tFrame.iMotion = 0;
	m_tFrame.dwSpeed = 200;
	m_tFrame.dwTime = GetTickCount();


}

int CPlayer::Update()
{
	Key_Input();
	Change_Motion();
	CObject::Move_Frame();
	CObject::Update_Rect();
	
	return NOEVENT;
}

void CPlayer::Late_Update()
{
	
}

void CPlayer::Render(HDC hDC)
{
	Rectangle(hDC, m_tInfo.fX - (m_tInfo.fCX / 2), m_tInfo.fY - (m_tInfo.fCY / 2), m_tInfo.fX + (m_tInfo.fCX / 2), m_tInfo.fY + (m_tInfo.fCY / 2));

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

	GdiTransparentBlt(hDC,
		m_tRect.left,
		m_tRect.top,
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,
		hMemDC,
		// 복사할 이미지의 비트맵 시작 좌표
		m_tFrame.iStart * (int)m_tInfo.fCX,
		m_tFrame.iMotion * (int)m_tInfo.fCY,	
		// 복사할 이미지의 가로, 세로 사이즈
		(int)m_tInfo.fCX,	
		(int)m_tInfo.fCY,
		RGB(0, 255, 0));
}

void CPlayer::Release()
{
}

void CPlayer::Key_Input()
{

	// 이동 : 좌우
	if (GetAsyncKeyState(VK_LEFT))
	{
		m_tInfo.fX -= m_fSpeed;
		m_eDir = DIR_LEFT;
		m_eCurState = WALK;
	}

	else if (GetAsyncKeyState(VK_RIGHT))
	{
		m_tInfo.fX += m_fSpeed;
		m_eDir = DIR_RIGHT;
		m_eCurState = WALK;
	}

	// 올려보기, 웅크리기
	else if (GetAsyncKeyState(VK_UP))
	{

	}
	
	else if (GetAsyncKeyState(VK_DOWN))
	{

	}

	// 점프, 스핀점프, 대시
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

void CPlayer::Change_Motion()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 0;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case WALK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 1;
			m_tFrame.iMotion = 1;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case RUN:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 1;
			m_tFrame.iMotion = 2;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case JUMP:
			break;

		case JUMP_ACCEL:
			break;

		case STOMP:
			break;
		case CROUCH:
			break;
		case BRAKE:
			break;
		case KICK:
			break;
		case HOLD_IDLE:
			break;
		case HOLD_MOVE:
			break;
		case PDEAD:
			break;
		}
		m_ePreState = m_eCurState;

		if(m_eDir=DIR_RIGHT)
			m_pFrameKey = L"Player_RIGHT";
		else if(m_eDir = DIR_LEFT)
			m_pFrameKey = L"Player_LEFT";
	}

	
}
