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
	m_tInfo = { 400.f, 492.f , SMALLX * SCALE_FACTOR, SMALLY * SCALE_FACTOR };
	m_fSpeed = 4.f;

	m_bJump = true;
	m_eDir = DIR_RIGHT;
	m_pFrameKey = (L"Player_RIGHT");

	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 0;
	m_tFrame.iMotion = 0;
	m_tFrame.dwSpeed = 200;
	m_tFrame.dwTime = GetTickCount();

	m_eCurState = JUMP;
	m_ePreState = END;

	m_fJumpSpeed = -10.f;
	m_fJumpTime = 0.1f;

}

int CPlayer::Update()
{
	if (m_bDead)
	{
		if (m_eCurState != DEATH)
		{
			m_eCurState = DEATH;
			m_fJumpTime = 0.1f;
			m_fJumpSpeed = -5.63f;
		}
		Update_Gravity();
		return NOEVENT;
	}
		
	Key_Input();
	if (m_eCurState != IDLE)
	{
		Update_Gravity();
	}

	return NOEVENT;
}

void CPlayer::Late_Update()
{

	Change_Motion();
	CObject::Move_Frame();
	CObject::Update_Rect();
	Offset();

	if (m_tInfo.fY > 1000.f)
	{
		// ������ ó��
		m_tInfo.fX = 400.f;
		m_tInfo.fY = 492.f;
		m_fJumpSpeed = 0.f;
		m_fJumpTime = 0.f;
		m_bJump = false;
		m_bSpin = false;
		m_bDead = false;
		m_eCurState = IDLE;
		m_ePreState = END;

		// �浹�ڽ��� �ٽ� ������� ��
		CObject::Update_Rect();
	}

	if (m_bDead) return;

	On_Collision(OBJ_TILE);
	On_Collision(OBJ_MONSTER);
	

	if(m_fJumpSpeed > 0.f)
	{
		float fY(0);
		if (CLineMgr::Get_Instance()->Collision_Line(this->m_tInfo, &fY))
		{
			m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
			m_fJumpSpeed = 0.f;
			m_fJumpTime = 0.f;
			m_bJump = false;
			m_bSpin = false;

			if (m_eCurState != DUCK && m_eCurState != DEATH)
			{
				// �̵� �� ���� ���� �Ǻ��� �ӵ��� ó��
				if (fabsf(m_fSpeed) > 8.0f)
					m_eCurState = RUN;
				else if (fabsf(m_fSpeed) > 0.1f)
					m_eCurState = WALK;
				else
					m_eCurState = IDLE;
			}

		}
	}
	
}

void CPlayer::Render(HDC hDC)
{
	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();


	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);
	GdiTransparentBlt(
		hDC,
		(int)(m_tRect.left - fScrollX),
		(int)(m_tRect.top),
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,                                 // ��� ũ�� (3��)
		hMemDC,
		m_tFrame.iStart * SMALLX,                   // �� �ε��� �� ������ �ʺ�
		m_tFrame.iMotion * SMALLY,                  // �� �ε��� �� ������ ����
		SMALLX, SMALLY,                                 // �ڸ� ���� ũ��
		RGB(0, 255, 0));

	//const float fFocusX = WINCX * 0.42f;
	//const float fOffsetHalf = 48.f; // ������ �ڽ��� �ݳʺ�

	//const int iLeft = (int)(fFocusX - fOffsetHalf);
	//const int iRight = (int)(fFocusX + fOffsetHalf);

	//HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // ������
	//HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
	//HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	//Rectangle(hDC, iLeft, 0, iRight, WINCY); // ȭ�� ������ �Ʒ����� �ڽ�

	//SelectObject(hDC, hOldPen);
	//SelectObject(hDC, hOldBrush);
	//DeleteObject(hPen);
}

void CPlayer::Release()
{

}

void CPlayer::On_Collision(EOBJECTID _id)
{
	if (m_bDead) return;
	CObject* pTarget;
	switch (_id)
	{
		// 1. Ÿ��
		case OBJ_TILE:
				pTarget = CCollisionMgr::Collision_RectEx(
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
		// 2. ����(��������)
		case OBJ_MONSTER:
				pTarget = CCollisionMgr::Collision_RectEx(
				CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER),
				CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER)
			);
			if (!pTarget) break;
			// ������ ���� ���
			if (Get_Col() == COL_BOTTOM)
			{
				// �÷��̾� �ݵ� ����
				m_bJump = true;
				m_fJumpTime = 0.f;
				m_fJumpSpeed = -BOUNCE_SPEED;  // ƨ�ܿ����� �ӵ� ���

				// ���� ������ ó��
				static_cast<CMonster*>(pTarget)->On_Stomped();
			}
			else
			{
				m_bDead = true;
			}

			break;
	}
	

	// 3. ������

	// 4. ����(���,��������)
}

void CPlayer::Key_Input()
{
	if (m_bDead) return;

	bool bPressed = false;

	const bool bLeft = CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT);
	const bool bRight = CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT);
	const bool bUp = CKeyMgr::Get_Instance()->Key_Pressing(VK_UP);
	const bool bDown = CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN);
	const bool bRun = CKeyMgr::Get_Instance()->Key_Pressing(VK_RUN);

	// �̵� - ��
	if (bLeft && m_eCurState != DUCK)
	{
		m_eDir = DIR_LEFT;
		m_tInfo.fX -= m_fSpeed;

		if (!m_bJump && !m_bSpin)
		{
			if (bRun)
			{
				if (m_fSpeed < 10.f)
					m_fSpeed += 0.1f;
				else
					m_fSpeed = 10.f;

				if (m_eCurState != RUN)
					m_eCurState = RUN;
			}
			else
			{
				m_fSpeed = 4.f;
				if (m_eCurState != WALK)
					m_eCurState = WALK;
			}
		}
		bPressed = true;
	}

	// �̵� - ��
	if (bRight && m_eCurState != DUCK)
	{
		m_eDir = DIR_RIGHT;
		m_tInfo.fX += m_fSpeed;

		if (!m_bJump && !m_bSpin)
		{
			if (bRun)
			{
				if (m_fSpeed < 10.f)
					m_fSpeed += 0.1f;
				else
					m_fSpeed = 10.f;

				if (m_eCurState != RUN)
					m_eCurState = RUN;
			}
			else
			{
				m_fSpeed = 4.f;
				if (m_eCurState != WALK)
					m_eCurState = WALK;
			}
		}
		bPressed = true;
	}

	// �� ����
	if (bUp && !m_bJump && !m_bSpin && m_eCurState != LOOK_UP)
	{
		m_eCurState = LOOK_UP;
		bPressed = true;
	}

	// �ɱ�
	if (bDown && !m_bSpin && m_eCurState != DUCK)
	{
		m_eCurState = DUCK;
		bPressed = true;
	}

	// ����
	if (CKeyMgr::Get_Instance()->Key_Down(VK_JUMP) && !m_bJump)
	{
		m_bJump = true;
		m_fJumpTime = 0.1f;
		m_fJumpSpeed = -13.63f;
		m_eCurState = JUMP;
		CSoundMgr::Get_Instance()->PlaySoundW(L"jump.wav", SOUND_EFFECT, 0.5f);
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

	// ����Ű�� �ƹ��͵� ������ �ʰ� ���ߵ� �ƴ� �� �� IDLE
	if (!bPressed && !m_bJump && m_eCurState != IDLE && m_eCurState != DEATH)
	{
		m_eCurState = IDLE;
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
		m_tFrame.iStart = 0;
		m_tFrame.dwTime = GetTickCount();
		m_ePreState = m_eCurState;

		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 0;
			m_tFrame.dwSpeed = 200;
			break;

		case LOOK_UP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 1;
			m_tFrame.dwSpeed = 20;
			break;

		case DUCK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 2;
			m_tFrame.dwSpeed = 20;
			break;

		case WALK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 3;
			m_tFrame.dwSpeed = 130;
			break;

		case RUN:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 4;
			m_tFrame.dwSpeed = 120;
			break;

		case JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 7;
			m_tFrame.dwSpeed = 200;
			break;

		case FALL:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 8;
			m_tFrame.dwSpeed = 20;
			break;


		case RUN_JUMP:
			break;

		case SPIN_JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 3;
			m_tFrame.iMotion = 10;
			m_tFrame.dwSpeed = 20;
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
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 22;
			m_tFrame.dwSpeed = 500;
			break;
		}
	}
}

// ���� �� �߷� ó��
void CPlayer::Update_Gravity()
{


	// y = y0 + vt
	m_tInfo.fY += m_fJumpSpeed * m_fJumpTime;
	// v = v0 + at
	if (m_fJumpSpeed >= 8.0f)
		m_fJumpSpeed = 8.0f;
	else if (m_fJumpSpeed <= -20.f)
		m_fJumpSpeed = -20.f;
	else
	{
		m_fJumpSpeed += GRAVITY * m_fJumpTime;
		if (m_fJumpSpeed > 0 && m_fJumpSpeed < 1.f && !m_bSpin && !m_bDead && m_eCurState != FALL)
			m_eCurState = FALL;
	}
		
		
		
	// t++
	m_fJumpTime += 0.1f;

}

void CPlayer::Offset()
{
	const float fFocusX = WINCX * 0.42f;          // ȭ�� �� ���� ��ġ
	const float fOffsetBoxHalf = 24.f;            // �÷��̾� �ʺ� * 2 (96px)

	fOffsetBoxLeft = fFocusX - fOffsetBoxHalf;
	fOffsetBoxRight = fFocusX + fOffsetBoxHalf;

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fPlayerScreenX = m_tInfo.fX - fScrollX;

	// ������ �ڽ� ���� ��ũ�� ó��
	if (fPlayerScreenX < fOffsetBoxLeft)
	{
		float fDelta = fOffsetBoxLeft - fPlayerScreenX;
		CScrollMgr::Get_Instance()->Set_ScrollX(-fDelta);
	}
	else if (fPlayerScreenX > fOffsetBoxRight)
	{
		float fDelta = fPlayerScreenX - fOffsetBoxRight;
		CScrollMgr::Get_Instance()->Set_ScrollX(fDelta);
	}

	float fTargetScrollY = CScrollMgr::Get_Instance()->Get_ScrollY() * SCALE_FACTOR - WINCY;
	CScrollMgr::Get_Instance()->Set_ScrollY(fTargetScrollY);


	CScrollMgr::Get_Instance()->Scroll_Lock();
}