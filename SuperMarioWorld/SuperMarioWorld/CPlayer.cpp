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
	m_tInfo.fX = 264.f;
	m_tInfo.fY = 1152.f - (SMALLY * SCALE_FACTOR * 0.5f);  // �ٴ� ���� ��Ȯ�� ����
	m_tInfo.fCX = SMALLX * SCALE_FACTOR;
	m_tInfo.fCY = SMALLY * SCALE_FACTOR;
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
	Offset();

}

int CPlayer::Update()
{
	DWORD dwCurrent = GetTickCount();
	// ���� ���� ����
	if (m_bInvi && (dwCurrent - m_dwHitTime >= m_fCoolTime * 1000.f))
		m_bInvi = false;

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
	//if (m_eCurState != IDLE)
	{
		Update_Gravity();
	}
	
	return NOEVENT;
}

void CPlayer::Late_Update()
{
	// �Ŀ���/�Ŀ��ٿ� ���۵� ���� ���� ���� ó��
	if (m_eCurState == POWERUP)
	{
		m_dwFadeStartTime = GetTickCount();
		CSoundMgr::Get_Instance()->PlaySound(L"powerup.wav", SOUND_POWER, 0.5f);
		m_ePreState = m_eCurState;
		m_eCurState = IDLE;
	}
	else if (m_eCurState == POWERDOWN)
	{
		m_dwFadeStartTime = GetTickCount();
		CSoundMgr::Get_Instance()->PlaySound(L"pipe.wav", SOUND_POWER, 0.5f);
		m_ePreState = m_eCurState;
		m_eCurState = IDLE;

	}

	Change_State();
	CObject::Move_Frame();
	CObject::Update_Rect();
	Offset();
		

	if (m_tInfo.fY > 1700.f * SCALE_FACTOR)
	{
		// ������ ó��
		m_tInfo.fX = 264.f;
		m_tInfo.fY = 1152.f - (SMALLY * SCALE_FACTOR * 0.5f);
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
	On_Collision(OBJ_ITEM);
	float fX(0);
	float fY(0);
	
	if (CLineMgr::Get_Instance()->Collision_Vertical(this->m_tInfo, &fX))
	{
		float fLeft = m_tInfo.fX - m_tInfo.fCX * 0.5f;
		float fRight = m_tInfo.fX + m_tInfo.fCX * 0.5f;

		if (m_eDir == DIR_LEFT && fX >= fLeft && fX < fRight - 0.00013f)
			m_tInfo.fX = fX + m_tInfo.fCX * 0.5f;
		else if (m_eDir == DIR_RIGHT && fX <= fRight && fX > fLeft + 0.00013f)
			m_tInfo.fX = fX - m_tInfo.fCX * 0.5f;
	}

	if(m_fJumpSpeed > 0.f)
	{
		
		if (CLineMgr::Get_Instance()->Collision_Line(this->m_tInfo, &fY))
		{
			m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
			m_fJumpSpeed = 0.f;
			m_fJumpTime = 0.f;
			m_bJump = false;
			m_bSpin = false;

			if (m_eCurState != DEATH)
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
	// ������ ������
	if (m_bInvi && (GetTickCount() % 2) && (m_eCurState != DEATH))	return;


	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	float drawX = m_tInfo.fX - m_tInfo.fCX * 0.5f - fScrollX;
	float drawY = m_tInfo.fY - m_tInfo.fCY * 0.5f - fScrollY;

	int srcY;
	if (m_eMarioState == MARIO_SMALL)
		srcY = SMALLY;
	else
		srcY = BIGY;

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);
	GdiTransparentBlt(
		hDC,
		(int)drawX,
		(int)drawY,
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,                              // ��� ũ�� (3��)
		hMemDC,
		m_tFrame.iStart * SMALLX,                   // �� �ε��� �� ������ �ʺ�
		m_tFrame.iMotion * srcY,                  // �� �ε��� �� ������ ����
		SMALLX, srcY,                                 // �ڸ� ���� ũ��
		RGB(0, 255, 0));
}

void CPlayer::Release()
{

}

void CPlayer::On_Collision(EOBJECTID _id)
{
	if (m_bDead) return;

	// �浹 �� ��ġ ����
	float fOriginalX = m_tInfo.fX;
	float fOriginalY = m_tInfo.fY;

	CObject* pTarget;

	switch (_id)
	{
		// 1. Ÿ��
	case OBJ_TILE:
	{
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
			if (m_tInfo.fX <= pTarget->Get_Info()->fX)
				static_cast<CTile*>(pTarget)->On_Hit(DIR_RIGHT);
			else
				static_cast<CTile*>(pTarget)->On_Hit(DIR_LEFT);

			if (m_fJumpSpeed < 0.f)
				m_fJumpSpeed *= -1.f;

			if ((pTarget->Get_Info()->iType) == TILE_EMPTY)
			{
				CSoundMgr::Get_Instance()->PlaySoundW(L"bump.wav", SOUND_EFFECT, 0.1f);
			}
				
		}
	}
	break;

	// 2. ����
	case OBJ_MONSTER:
	{
		pTarget = CCollisionMgr::Collision_RectEx(
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER),
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER)
		);

		if (!pTarget) break;

		CMonster* pMonster = static_cast<CMonster*>(pTarget);

		// �̹� �� �÷��̾ ��� �ִ� �����̸� ��ġ ����
		if (pMonster->Get_Holder() == this)
		{
			m_tInfo.fX = fOriginalX;
			m_tInfo.fY = fOriginalY;
			Update_Rect();
			break;
		}

		MONSTER_STATE monsterState = pMonster->Get_State();

		// �����̴� �������� �浹�� ����
		if (monsterState == MONSTER_SHELL_MOVE)
		{
			break;
		}

		// ������ ������ ���
		if (monsterState == MONSTER_SHELL_IDLE)
		{
			if (m_bGrab)
			{
				if (!pMonster->Is_Held())
				{
					pMonster->Set_Holder(this);
				}
				break;
			}
			else
			{
				if (Get_Col() == COL_BOTTOM)
				{
					m_bJump = true;
					m_fJumpTime = 0.f;
					m_fJumpSpeed = -BOUNCE_SPEED;
					pMonster->On_Stomped();
				}
				else
				{
					pMonster->On_Kicked(m_eDir);
				}
			}
			break;
		}

		if (m_bInvi) break;

		// �Ϲ� ���� ó��
		if (Get_Col() == COL_BOTTOM)
		{
			m_bJump = true;
			m_fJumpTime = 0.f;
			m_fJumpSpeed = -BOUNCE_SPEED;
			pMonster->On_Stomped();

		}
		else
		{
				if (m_eMarioState == MARIO_FLOWER)
					Change_Mario(MARIO_BIG);
				else if (m_eMarioState == MARIO_BIG)
					Change_Mario(MARIO_SMALL);
				else
					m_bDead = true;

				m_bInvi = true;
				m_dwHitTime = GetTickCount();
		}
	}
	break;

	// 3. ������
	case OBJ_ITEM:
	{
		pTarget = CCollisionMgr::Collision_RectEx(
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER),
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_ITEM)
		);

		if (!pTarget) break;

		CItem* pItem = static_cast<CItem*>(pTarget);
		pItem->Catch_Item(pItem);
		if (pItem->Get_Info()->iType == ITEM_MUSH)
		{
			if (this->m_eMarioState == MARIO_SMALL)
			{
				m_ePreState = m_eCurState;
				m_eCurState = POWERUP;
				Change_Mario(MARIO_BIG);
			}
		}
		else if (pItem->Get_Info()->iType == ITEM_FLOWER)
		{
				m_ePreState = m_eCurState;
				m_eCurState = POWERUP;
				Change_Mario(MARIO_FLOWER);
		}
	}
	break;
	}
}

void CPlayer::Key_Input()
{
	if (m_bDead) return;

	bool bPressed = false;
	const bool bRun = CKeyMgr::Get_Instance()->Key_Pressing(VK_RUN);

	// �ɱ�
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_DOWN) && !m_bSpin && m_eCurState != DUCK)
	{
		m_eCurState = DUCK;
		m_bDuck = true;
		bPressed = true;
	}

	if (CKeyMgr::Get_Instance()->Key_Up(VK_DOWN))
	{
		m_bDuck = false;
		if (m_eCurState == DUCK)
			m_eCurState = IDLE;
	}
		
	// ����
	if (CKeyMgr::Get_Instance()->Key_Down(VK_JUMP) && !m_bJump)
	{
		m_bJump = true;
		m_fJumpTime = 0.1f;
		m_fJumpSpeed = -13.63f;
		if (!m_bDuck)
		{
			m_eCurState = JUMP;
		}
			
		CSoundMgr::Get_Instance()->PlaySoundW(L"jump.wav", SOUND_EFFECT, 0.1f);
	}

	// �̵� - ��
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT) && (!m_bDuck || m_bJump))
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

				if (m_eCurState != RUN && m_fSpeed>=10.f)
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
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT) && (!m_bDuck || m_bJump))
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

				if (m_eCurState != RUN && m_fSpeed>=10.f)
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
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP) && !m_bJump && !m_bSpin &&
		m_eCurState != LOOK_UP &&
		!CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT) &&
		!CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
	{
		m_eCurState = LOOK_UP;
		bPressed = true;
	}

	// ��������
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPIN) && !m_bJump)
	{
		m_bJump = true;
		m_bSpin = true;
		m_fJumpTime = 0.1f;
		m_fJumpSpeed = -13.63f;
		m_eCurState = SPIN_JUMP;
		CSoundMgr::Get_Instance()->PlaySoundW(L"spin.wav", SOUND_EFFECT, 0.1f);
	}

	// ��� Ű (��: XŰ)
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_GRAB))
	{
		m_bGrab = true;
		// ���� ���¸� HOLD ���·� ��ȯ
		Update_Hold(true);
	}
	else
	{
		// ��� Ű�� ������ ��
		if (CKeyMgr::Get_Instance()->Key_Up(VK_GRAB))
		{
			m_bGrab = false;
			// �Ϲ� ���·� ����
			Update_Hold(false);
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_FIRE))
	{
		if(m_eMarioState==MARIO_FLOWER)
		{
			// �ִ� ���̾ ���� üũ (�Ϲ������� 2��)
			list<CObject*>& bulletList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_BULLET);
			if (bulletList.size() < 2)  // ȭ�鿡 2�� �̸��� ���� �߻�
			{
				// ���̾ ��ġ ���� (�÷��̾�� �ణ ����)
				float fBulletX = m_tInfo.fX;
				float fBulletY = m_tInfo.fY - 10.f;  // �÷��̾� �߽ɺ��� �ణ ��

				// ���⿡ ���� ������
				if (m_eDir == DIR_LEFT)
					fBulletX -= 15.f;
				else
					fBulletX += 15.f;

				CBullet* pBullet = new CBullet(fBulletX, fBulletY, m_eDir);
				CObjectMgr::Get_Instance()->Add_Object(OBJ_BULLET, pBullet);

				// �߻� ���� ���
				CSoundMgr::Get_Instance()->PlaySound(L"fireball.wav", SOUND_EFFECT, 0.5f);
			}
		}
	}

	// ����Ű�� �ƹ��͵� ������ �ʰ� ���ߵ� �ƴ� �� �� IDLE
	if (!bPressed && !m_bJump && m_eCurState != IDLE && m_eCurState != DEATH && m_eCurState != LOOK_UP && m_eCurState != DUCK)
	{
		m_eCurState = IDLE;
	}
}
		

void CPlayer::Change_State()
{
	switch (m_eMarioState)
	{
	case MARIO_SMALL:
		if (m_eDir == DIR_RIGHT)
			m_pFrameKey = L"Player_RIGHT";
		else if (m_eDir == DIR_LEFT)
			m_pFrameKey = L"Player_LEFT";
		break;
	case MARIO_BIG:
		if (m_eDir == DIR_RIGHT)
			m_pFrameKey = L"SPlayer_RIGHT";
		else if (m_eDir == DIR_LEFT)
			m_pFrameKey = L"SPlayer_LEFT";
		break;
	case MARIO_FLOWER:
		if (m_eDir == DIR_RIGHT)
			m_pFrameKey = L"FPlayer_RIGHT";
		else if (m_eDir == DIR_LEFT)
			m_pFrameKey = L"FPlayer_LEFT";
		break;
	}
	

	
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
			break;

		case LOOK_UP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 1;
			break;

		case DUCK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 2;
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
			m_tFrame.dwSpeed = 120;
			break;

		case JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 7;
			break;

		case FALL:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 8;
			break;


		case RUN_JUMP:
			break;

		case SPIN_JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 3;
			m_tFrame.iMotion = 10;
			m_tFrame.dwSpeed = 20;
			break;

		// Ư�� �ӵ� �̻��϶� �ݴ� ���� Ű�� ������ ��Ű�� ��� + ��Ű�� ����Ʈ
		// Ư�� �ӵ� �̻��϶� ��ŷ�� �ϸ� ��Ű�� ����Ʈ��
		case SKID:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 11;
			break;

		case KICK:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 12;
			break;
		case HOLD_IDLE:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 15;
			break;
		// WALK, RUN, JUMP ����
		case HOLD_WALK:
		case HOLD_RUN:
		case HOLD_JUMP:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 18;
			break;

		case POWERUP:
		case POWERDOWN:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 2;
			m_tFrame.iMotion = 19;
			m_tFrame.dwSpeed = 666;
			break;
		case DEATH:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 22;
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
		if (m_bJump && m_fJumpSpeed > 0 && m_fJumpSpeed < 1.f && !m_bSpin)
		{
			if(!m_bDead && m_eCurState != FALL && !m_bDuck)
				m_eCurState = FALL;
		}
			
	}
	// t++
	m_fJumpTime += 0.1f;
}

void CPlayer::Update_Speed()
{
	float fAccel;
	// �¿� Ű �Է½� ���� ����
	// ���� ���� ��ġ�� �����ϸ� ���
	// �̿� ���� �ӵ� ������ ���� ��ġ�� �����ϸ� ���
	// Ű�� ������ ���ӵ� ���� : �ӵ��� 0�� �ɶ�����
	// �ӵ��� 0�� �Ǹ� ���ӵ��� 0���� �ʱ�ȭ

	// v = at

}

void CPlayer::Offset()
{
	const float fFocusX = WINCX * 0.42f;
	const float fOffsetBoxHalf = 24.f;

	fOffsetBoxLeft = fFocusX - fOffsetBoxHalf;
	fOffsetBoxRight = fFocusX + fOffsetBoxHalf;

	// X��: ������ �ڽ��� ����� ��ũ�� �̵�
	float fPlayerScreenX, fPlayerScreenY;
	CScrollMgr::Get_Instance()->World_To_Screen(m_tInfo.fX, m_tInfo.fY, &fPlayerScreenX, &fPlayerScreenY);

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

	// Y��: �÷��̾ ȭ�� �߾ӿ� ����
	float fTargetScrollY = m_tInfo.fY - WINCY * 0.5f;
	float fCurrentScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
	float fDeltaY = fTargetScrollY - fCurrentScrollY;
	//ScrollMgr::Get_Instance()->Set_ScrollY(fDeltaY);

	CScrollMgr::Get_Instance()->Scroll_Lock();
}

void CPlayer::Update_Hold(bool _hold)
{
	if (_hold)  // ��� ���·� ��ȯ
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_eCurState = HOLD_IDLE;
			break;
		case WALK:
			m_eCurState = HOLD_WALK;
			break;
		case RUN:
			m_eCurState = HOLD_RUN;
			break;
		case JUMP:
		case FALL:
			m_eCurState = HOLD_JUMP;
			break;
		case DUCK:
			m_eCurState = HOLD_DUCK;
			break;
		case LOOK_UP:
			m_eCurState = HOLD_LOOKUP;
			break;
		}
	}
	else  // �Ϲ� ���·� ����
	{
		switch (m_eCurState)
		{
		case HOLD_IDLE:
			m_eCurState = IDLE;
			break;
		case HOLD_WALK:
			m_eCurState = WALK;
			break;
		case HOLD_RUN:
			m_eCurState = RUN;
			break;
		case HOLD_JUMP:
			m_eCurState = m_bJump ? JUMP : FALL;
			break;
		case HOLD_DUCK:
			m_eCurState = DUCK;
			break;
		case HOLD_LOOKUP:
			m_eCurState = LOOK_UP;
			break;
		}
	}
}

void CPlayer::Change_Mario(MARIOSTATE _state)
{
	m_eMarioState = _state;
	switch (m_eMarioState)
	{
	case MARIO_SMALL:
		m_tInfo.fCX = TILECX * SCALE_FACTOR;
		m_tInfo.fCY = TILECY * SCALE_FACTOR * 1.5f;
		break;
	case MARIO_BIG:
	case MARIO_FLOWER:
		m_tInfo.fCX = TILECX * SCALE_FACTOR;
		m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
		break;
		
	}
	CObject::Update_Rect();
}
