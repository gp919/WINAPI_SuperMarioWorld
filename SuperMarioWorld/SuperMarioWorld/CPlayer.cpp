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
	m_tInfo = { WINCX * 0.5f, WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f) , SMALLX * SCALE_FACTOR, SMALLY * SCALE_FACTOR };
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
	
	// ���� ���� ��Ͽ�
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
		(int)m_tInfo.fCX, (int)m_tInfo.fCY,                                 // ��� ũ�� (3��)
		hMemDC,
		m_tFrame.iStart * SMALLX,                   // �� �ε��� �� ������ �ʺ�
		m_tFrame.iMotion * SMALLY,                  // �� �ε��� �� ������ ����
		SMALLX, SMALLY,                                 // �ڸ� ���� ũ��
		RGB(0, 255, 0));
	

	
	
}

void CPlayer::Release()
{
}

void CPlayer::Key_Input()
{

	// �̵� : �¿�
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT))
	{
		m_tInfo.fX -= m_fSpeed;
		m_eDir = DIR_LEFT;
		m_eCurState = WALK;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
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

	// ����
	if (CKeyMgr::Get_Instance()->Key_Down(VK_JUMP) && !m_bJump)
	{
		m_bJump = true;
		m_fJumpTime = 0.f;
		m_fJumpSpeed = -13.63f;
	}

	// ��������
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPIN) && !m_bJump)
	{
		m_bJump = true;
		m_bSpin = true;
		m_fJumpTime = 0.f;
		m_fJumpSpeed = -13.63f;
	}

	
	// �޸��� : Ű�� ������ ���ȿ��� �ִ�ӵ����� �����ϴٰ� ���� ���� ���� �ӷ�
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RUN))
	{
		if (m_fSpeed < SPEED_MAX)
		{
			m_fSpeed += RUN_ACCEL;
			m_fJumpSpeed -= RUN_ACCEL;
		}
		else
			m_eCurState = RUN;
	}

	if (CKeyMgr::Get_Instance()->Key_Up(VK_RUN))
	{
		m_fSpeed = 4.f;
		m_fJumpSpeed = -13.63f;
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
			m_tFrame.dwSpeed = 50;
			m_tFrame.dwTime = GetTickCount();
			break;

		case LOOK_UP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 1;
			m_tFrame.dwSpeed = 50;
			m_tFrame.dwTime = GetTickCount();
			break;

		case DUCK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 2;
			m_tFrame.dwSpeed = 50;
			m_tFrame.dwTime = GetTickCount();
			break;

		case WALK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 3;
			m_tFrame.dwSpeed = 100;
			m_tFrame.dwTime = GetTickCount();
			break;

		case RUN:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 4;
			m_tFrame.dwSpeed = 50;
			m_tFrame.dwTime = GetTickCount();
			break;

		case JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 7;
			m_tFrame.dwSpeed = 50;
			m_tFrame.dwTime = GetTickCount();
			break;

		case FALL:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 8;
			m_tFrame.dwSpeed = 50;
			m_tFrame.dwTime = GetTickCount();
			break;


		case RUN_JUMP:
			break;

		case SPIN_JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 3;
			m_tFrame.iMotion = 10;
			m_tFrame.dwSpeed = 50;
			m_tFrame.dwTime = GetTickCount();
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
	float fGroundY = WINCY - (16.f * SCALE_FACTOR + m_tInfo.fCY * 0.5f + TILECY * 0.5f);
	float fGravity(0);

	bool bLineCol = CLineMgr::Get_Instance()->Collision_Line(m_tInfo.fX, &fGroundY);

	if (m_bJump)		
	{
		
		m_eCurState = JUMP;
		if (CKeyMgr::Get_Instance()->Key_Pressing((VK_JUMP)))
		{
			fGravity = GRAVITY * 1.f;
		}
		else
			fGravity = GRAVITY * 1.5f;

		if (m_bSpin)
		{
			m_eCurState = SPIN_JUMP;
			if (CKeyMgr::Get_Instance()->Key_Pressing((VK_SPIN)))
			{
				fGravity = GRAVITY * 1.f;
			}
			else
				fGravity = GRAVITY * 1.5f;
		}
		
	

		m_fAccel = fGravity;
		m_fJumpSpeed += m_fAccel;

		// �����Ҷ� ��� FALL ����
		if(m_eCurState==JUMP)
		{
			if (m_fJumpSpeed >= 0.f && m_fJumpSpeed <= 5.4f)
				m_eCurState = FALL;
		}

		m_tInfo.fY += m_fJumpSpeed;

		
		m_fJumpTime += 0.1f;

		if (fGroundY < m_tInfo.fY + m_tInfo.fCY * 0.5f && m_fJumpSpeed >= 0.f)
		{
			m_bJump = false;
			m_bSpin = false;
			m_fJumpTime = 0.f;
			m_tInfo.fY = fGroundY- m_tInfo.fCY * 0.5f;
		}

		else if (bLineCol)
		{
			m_tInfo.fY = fGroundY;
		}
	}
}
