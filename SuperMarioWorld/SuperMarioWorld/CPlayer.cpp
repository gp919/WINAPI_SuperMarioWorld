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
	m_tInfo = { 400.f, 400.f , SMALLX * SCALE_FACTOR, SMALLY * SCALE_FACTOR };
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
	Key_Input();
	if (m_eCurState != IDLE)
	{
		Update_Gravity();
	}

	return NOEVENT;
}

void CPlayer::Late_Update()
{
	Offset();

	On_Collision(OBJ_TILE);
	CObject::Update_Rect();

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
			if(m_eCurState!=DUCK && m_eCurState!=RUN)
				m_eCurState = IDLE;

		}
	}

	

	Change_Motion();
	CObject::Move_Frame();
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
		(int)m_tInfo.fCY,                                 // 출력 크기 (3배)
		hMemDC,
		m_tFrame.iStart * SMALLX,                   // 열 인덱스 × 프레임 너비
		m_tFrame.iMotion * SMALLY,                  // 행 인덱스 × 프레임 높이
		SMALLX, SMALLY,                                 // 자를 원본 크기
		RGB(0, 255, 0));

	const float fFocusX = WINCX * 0.42f;
	const float fOffsetHalf = 48.f; // 오프셋 박스의 반너비

	const int iLeft = (int)(fFocusX - fOffsetHalf);
	const int iRight = (int)(fFocusX + fOffsetHalf);

	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // 빨간색
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	Rectangle(hDC, iLeft, 0, iRight, WINCY); // 화면 위부터 아래까지 박스

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hPen);
}

void CPlayer::Release()
{
}

void CPlayer::On_Collision(EOBJECTID _id)
{
	switch (_id)
	{
		// 1. 타일
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
		// 2. 몬스터(보스패턴)
		//case OBJ_MONSTER:
		//	CObject* pTarget = CCollisionMgr::Collision_RectEx(
		//		CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER),
		//		CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER)
		//	);
		//	if (!pTarget) break;
		//	// 위에서 밟은 경우
		//	if (Get_Col() == COL_BOTTOM)
		//	{
		//		// 플레이어 반동 점프
		//		m_bJump = true;
		//		m_fJumpTime = 0.f;
		//		m_fJumpSpeed = -BOUNCE_SPEED;  // 튕겨오르는 속도 상수

		//		// 몬스터 스톰프 처리
		//		static_cast<CMonster*>(pTarget)->On_Stomped();
		//	}
		//	else
		//	{
		//		// 피격 처리 (예: 작아지거나 사망)
		//		Take_Damage();
		//	}

		//	break;
	}
	

	// 3. 아이템

	// 4. 지형(용암,낭떠러지)
}

void CPlayer::Key_Input()
{

	bool bPressed = false;

	// 이동 : 좌우
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT) && (m_eCurState != DUCK))
	{
		m_eDir = DIR_LEFT;
		m_tInfo.fX -= m_fSpeed;
		if(!m_bJump && !m_bSpin)
			m_eCurState = WALK;
		bPressed = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT) && (m_eCurState != DUCK))
	{
		m_tInfo.fX += m_fSpeed;
		m_eDir = DIR_RIGHT;
		if (!m_bJump && !m_bSpin)
			m_eCurState = WALK;
		bPressed = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP))
	{
		if (!m_bJump && !m_bSpin)
			m_eCurState = LOOK_UP;
		bPressed = true;
	}
	
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN))
	{
		if(!m_bSpin)
			m_eCurState = DUCK;
		bPressed = true;
	}

	// 점프
	if (CKeyMgr::Get_Instance()->Key_Down(VK_JUMP) && !m_bJump)
	{
		m_bJump = true;
		m_fJumpTime = 0.1f;
		m_fJumpSpeed = -13.63f;
		m_eCurState = JUMP;
	}

	// 스핀점프
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPIN) && !m_bJump)
	{
		m_bJump = true;
		m_bSpin = true;
		m_fJumpTime = 0.1f;
		m_fJumpSpeed = -13.63f;
		m_eCurState = SPIN_JUMP;
	}

	// 달리기 : 방향키와 함께 누르는 동안에는 최대속도까지 가속하다가 떼는 순간 원래 속력
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RUN) && CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT) ||
		CKeyMgr::Get_Instance()->Key_Pressing(VK_RUN) && CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
	{

		if (m_fSpeed < 10.f)
		{
			m_fSpeed += 0.1f;
			//m_fJumpSpeed -= 0.1f;
		}
		else
		{
			m_fSpeed = 10.f;
			m_eCurState = RUN;
		}
		bPressed = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Up(VK_RUN) && CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT) ||
		CKeyMgr::Get_Instance()->Key_Up(VK_RUN) && CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
	{
		m_fSpeed = 4.f;
		m_eCurState = WALK;
		bPressed = true;
	}	
	

	if (!bPressed && !m_bJump && m_eCurState!=RUN)
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
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 0;
			m_tFrame.dwSpeed = 50;
			break;

		case LOOK_UP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 1;
			m_tFrame.dwSpeed = 50;
			break;

		case DUCK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 2;
			m_tFrame.dwSpeed = 50;
			break;

		case WALK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 3;
			m_tFrame.dwSpeed = 100;
			break;

		case RUN:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 4;
			m_tFrame.dwSpeed = 50;
			break;

		case JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 7;
			m_tFrame.dwSpeed = 50;
			break;

		case FALL:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 8;
			m_tFrame.dwSpeed = 50;
			break;


		case RUN_JUMP:
			break;

		case SPIN_JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 3;
			m_tFrame.iMotion = 10;
			m_tFrame.dwSpeed = 50;
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

		m_tFrame.dwTime = GetTickCount();
		m_ePreState = m_eCurState;
	}
}

// 점프 및 중력 처리
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
		if (m_fJumpSpeed > 0 && m_fJumpSpeed < 1.f && !m_bSpin)
			m_eCurState = FALL;
	}
		
		
		
	// t++
	m_fJumpTime += 0.1f;

}

void CPlayer::Offset()
{
	const float fFocusX = WINCX * 0.42f;          // 화면 내 기준 위치
	const float fOffsetBoxHalf = 24.f;            // 플레이어 너비 * 2 (96px)

	fOffsetBoxLeft = fFocusX - fOffsetBoxHalf;
	fOffsetBoxRight = fFocusX + fOffsetBoxHalf;

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fPlayerScreenX = m_tInfo.fX - fScrollX;

	// 오프셋 박스 기준 스크롤 처리
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