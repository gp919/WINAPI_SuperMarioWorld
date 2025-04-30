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
	m_tInfo = { TILECX * 0.5f, WINCY - (TILECY * 0.5f), TILECX, TILECY };
	m_fSpeed = 4.f;


	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/Mario_LMove.bmp", L"Player_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/Mario_RMove.bmp", L"Player_RIGHT");

	m_eDir = DIR_RIGHT;
	m_pFrameKey = (L"Player_RIGHT");

	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 0;
	m_tFrame.iMotion = 0;
	m_tFrame.dwSpeed = 200;
	m_tFrame.dwTime = GetTickCount();

}

int CPlayer::Update()
{
	m_eCurState = IDLE;
	Key_Input();
	CKeyMgr::Get_Instance()->Key_Update();
	Jump();

	Change_Motion();
	CObject::Move_Frame();
	CObject::Update_Rect();
	CObject::Update_HitBox();
	
	// 점프 높이 기록용
	if(m_tInfo.fY < g_iMaxHeight)
		g_iMaxHeight = m_tInfo.fY;
	g_iHeight = g_iMaxHeight;
	

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
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT))
	{
		m_tInfo.fX -= m_fSpeed;
		m_eDir = DIR_LEFT;
		m_eCurState = WALK;
	}

	else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
	{
		m_tInfo.fX += m_fSpeed;
		m_eDir = DIR_RIGHT;
		m_eCurState = WALK;
	}

	// 올려보기, 웅크리기
	if (GetAsyncKeyState(VK_UP))
	{
		// 이미지 X
	}
	
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN))
	{
		m_eCurState = CROUCH;
	}

	// 점프
	if (CKeyMgr::Get_Instance()->Key_Down(VK_JUMP) && !m_bJump)
	{
		m_bJump = true;
		m_fJumpSpeed = -13.63f;
		m_fJumpTime = 0.f;
	}
	// 스핀점프
	else if (GetAsyncKeyState(VK_SPIN))
	{

	}
	// 대시
	else if (GetAsyncKeyState(VK_DASH))
	{

	}

}

void CPlayer::Change_Motion()
{
	if (m_eDir == DIR_RIGHT)
		m_pFrameKey = L"Player_RIGHT";
	else if (m_eDir == DIR_LEFT)
		m_pFrameKey = L"Player_LEFT";

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
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 3;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case JUMP_ACCEL:
			break;

		case STOMP:
			break;
		// Y크기 50px > 32px 로 감소 (미반영)
		case CROUCH:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 6;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
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

	}
}

void CPlayer::Jump()
{
	float fGroundY = WINCY - TILECY * 0.5f;
	float fGravity;

	if (m_bJump)
	{
		m_eCurState = JUMP;
		if (CKeyMgr::Get_Instance()->Key_Pressing(VK_JUMP))
			fGravity = GRAVITY * 1.f;
		else
			fGravity = GRAVITY * 1.5f;
	
			
		m_fAccel = fGravity;
		m_fJumpSpeed += m_fAccel;
		m_tInfo.fY += m_fJumpSpeed;

		
		m_fJumpTime += 0.1f;

		if (fGroundY < m_tInfo.fY)
		{
			m_bJump = false;
			m_fJumpTime = 0.f;
			m_tInfo.fY = fGroundY;
		}
	}
}
