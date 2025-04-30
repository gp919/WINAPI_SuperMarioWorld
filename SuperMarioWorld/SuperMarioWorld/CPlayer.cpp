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
	m_tInfo = { SMALLX * SCALE_FACTOR * 0.5f, WINCY - (SMALLY * SCALE_FACTOR * 0.5f), SMALLX * SCALE_FACTOR, SMALLY * SCALE_FACTOR };
	m_fSpeed = 4.f;


	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/SmallMario_L.bmp", L"Player_LEFT");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Player/SmallMario_R.bmp", L"Player_RIGHT");

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
	//CObject::Update_HitBox();
	
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
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

	
	GdiTransparentBlt(
		hDC,
		m_tRect.left, m_tRect.top,
		(int)m_tInfo.fCX, (int)m_tInfo.fCY,                                 // 출력 크기 (3배)
		hMemDC,
		m_tFrame.iStart * SMALLX,                   // 열 인덱스 × 프레임 너비
		m_tFrame.iMotion * SMALLY,                  // 행 인덱스 × 프레임 높이
		SMALLX, SMALLY,                                 // 자를 원본 크기
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

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP))
	{
		m_eCurState = LOOK_UP;
	}
	
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN))
	{
		m_eCurState = DUCK;
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

	// 달리기
	else if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RUN))
	{
		m_fSpeed += 1.f;
		m_eCurState = RUN;
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

		case LOOK_UP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 1;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case DUCK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 2;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case WALK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 3;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case RUN:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 4;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 7;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;

		case FALL:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 8;
			m_tFrame.dwSpeed = 200;
			m_tFrame.dwTime = GetTickCount();
			break;


		case RUN_JUMP:
			break;
		case SKID:
			break;
		case KICK:
			break;
		case HOLD_IDLE:
			break;
		case HOLD_WALK:
			break;
		case DEATH:
			break;
		}
		m_ePreState = m_eCurState;

	}
}

// TODO : 
void CPlayer::Jump()
{
	float fGroundY = WINCY - m_tInfo.fCY * 0.5f;
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
		// 낙하할때 잠깐 FALL 상태
		if (m_fJumpSpeed >= 0.f && m_fJumpSpeed <= 5.4f)
			m_eCurState = FALL;
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
