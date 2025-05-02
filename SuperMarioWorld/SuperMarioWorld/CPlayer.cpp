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
	m_tInfo = { WINCX * 0.5f+100.f, 590.f , SMALLX * SCALE_FACTOR, SMALLY * SCALE_FACTOR };
	m_fSpeed = 4.f;

	m_bJump = false;
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
	Key_Input();
	if (m_eCurState != IDLE)
		Update_Gravity();

	CObject::Update_Rect();
	CObject::Move_Frame();

	return NOEVENT;
}

void CPlayer::Late_Update()
{
	if (m_tInfo.fY < g_iMaxHeight)
		g_iMaxHeight = m_tInfo.fY;
	g_iHeight = g_iMaxHeight;

	On_Collision(OBJ_TILE);
	
	float fY;
	bool bLine = CLineMgr::Get_Instance()->Collision_Line(m_tInfo.fX, &fY);
	if (bLine && (fY - m_tInfo.fCY * 0.5f <= m_tInfo.fY))
	{
		m_bJump = false;
		m_fJumpTime = 0.f;
		m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
	}

	Change_Motion();
		
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

void CPlayer::On_Collision(EOBJECTID _id)
{
	switch (_id)
	{
		// 1. Ÿ��
		case OBJ_TILE:
				CObject* pTarget = CCollisionMgr::Collision_RectEx(
				CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER),
				CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_TILE)
				);
				if (pTarget && Get_Col() == COL_BOTTOM)
				{
					m_bJump = false;
					m_bSpin = false;
					m_fJumpSpeed = 0.f;
					m_fJumpTime = 0.f;
					m_eCurState = IDLE;

					m_tInfo.fY = pTarget->Get_Rect()->top - m_tInfo.fCY * 0.5f;
				}
				else if (pTarget && Get_Col() == COL_TOP)
				{
					if (m_fJumpSpeed < 0.f)
						m_fJumpSpeed *= -1.f;
				}

			break;
	}
	// 2. ����(��������)

	// 3. ������

	// 4. ����(���,��������)
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
		m_fJumpTime = 0.1f;
		m_fJumpSpeed = -13.63f;
		m_eCurState = JUMP;
	}

	// ��������
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPIN) && !m_bJump)
	{
		m_bJump = true;
		m_bSpin = true;
		m_fJumpTime = 0.1f;
		m_fJumpSpeed = -13.63f;
		m_eCurState = SPIN_JUMP;
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

// ���� �� �߷� ó��
void CPlayer::Update_Gravity()
{
	// y = y0 + vt
	m_tInfo.fY += m_fJumpSpeed * m_fJumpTime;
	// v = v0 + at
	if (m_fJumpSpeed < 13.6f)
		m_fJumpSpeed += GRAVITY * m_fJumpTime;
		
		
	// t++
	m_fJumpTime += 0.1f;

}
