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
	m_tInfo.fY = 1152.f - (SMALLY * SCALE_FACTOR * 0.5f);  // 바닥 위에 정확히 착지
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
	// 무적 상태 해제
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
	// 파워업/파워다운 시작도 상태 변경 전에 처리
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
		// 리스폰 처리
		m_tInfo.fX = 264.f;
		m_tInfo.fY = 1152.f - (SMALLY * SCALE_FACTOR * 0.5f);
		m_fJumpSpeed = 0.f;
		m_fJumpTime = 0.f;
		m_bJump = false;
		m_bSpin = false;
		m_bDead = false;
		m_eCurState = IDLE;
		m_ePreState = END;

		// 충돌박스도 다시 맞춰줘야 함
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
				// 이동 중 착지 상태 판별을 속도로 처리
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
	// 무적시 깜빡임
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
		(int)m_tInfo.fCY,                              // 출력 크기 (3배)
		hMemDC,
		m_tFrame.iStart * SMALLX,                   // 열 인덱스 × 프레임 너비
		m_tFrame.iMotion * srcY,                  // 행 인덱스 × 프레임 높이
		SMALLX, srcY,                                 // 자를 원본 크기
		RGB(0, 255, 0));
}

void CPlayer::Release()
{

}

void CPlayer::On_Collision(EOBJECTID _id)
{
	if (m_bDead) return;

	// 충돌 전 위치 저장
	float fOriginalX = m_tInfo.fX;
	float fOriginalY = m_tInfo.fY;

	CObject* pTarget;

	switch (_id)
	{
		// 1. 타일
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

	// 2. 몬스터
	case OBJ_MONSTER:
	{
		pTarget = CCollisionMgr::Collision_RectEx(
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER),
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER)
		);

		if (!pTarget) break;

		CMonster* pMonster = static_cast<CMonster*>(pTarget);

		// 이미 이 플레이어가 잡고 있는 껍질이면 위치 복원
		if (pMonster->Get_Holder() == this)
		{
			m_tInfo.fX = fOriginalX;
			m_tInfo.fY = fOriginalY;
			Update_Rect();
			break;
		}

		MONSTER_STATE monsterState = pMonster->Get_State();

		// 움직이는 껍질과의 충돌은 무시
		if (monsterState == MONSTER_SHELL_MOVE)
		{
			break;
		}

		// 정지된 껍질인 경우
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

		// 일반 몬스터 처리
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

	// 3. 아이템
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

	// 앉기
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
		
	// 점프
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

	// 이동 - 좌
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

	// 이동 - 우
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

	// 위 보기
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_UP) && !m_bJump && !m_bSpin &&
		m_eCurState != LOOK_UP &&
		!CKeyMgr::Get_Instance()->Key_Pressing(VK_LEFT) &&
		!CKeyMgr::Get_Instance()->Key_Pressing(VK_RIGHT))
	{
		m_eCurState = LOOK_UP;
		bPressed = true;
	}

	// 스핀점프
	if (CKeyMgr::Get_Instance()->Key_Down(VK_SPIN) && !m_bJump)
	{
		m_bJump = true;
		m_bSpin = true;
		m_fJumpTime = 0.1f;
		m_fJumpSpeed = -13.63f;
		m_eCurState = SPIN_JUMP;
		CSoundMgr::Get_Instance()->PlaySoundW(L"spin.wav", SOUND_EFFECT, 0.1f);
	}

	// 잡기 키 (예: X키)
	if (CKeyMgr::Get_Instance()->Key_Pressing(VK_GRAB))
	{
		m_bGrab = true;
		// 현재 상태를 HOLD 상태로 전환
		Update_Hold(true);
	}
	else
	{
		// 잡기 키를 놓았을 때
		if (CKeyMgr::Get_Instance()->Key_Up(VK_GRAB))
		{
			m_bGrab = false;
			// 일반 상태로 복귀
			Update_Hold(false);
		}
	}

	if (CKeyMgr::Get_Instance()->Key_Down(VK_FIRE))
	{
		if(m_eMarioState==MARIO_FLOWER)
		{
			// 최대 파이어볼 개수 체크 (일반적으로 2개)
			list<CObject*>& bulletList = CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_BULLET);
			if (bulletList.size() < 2)  // 화면에 2개 미만일 때만 발사
			{
				// 파이어볼 위치 조정 (플레이어보다 약간 앞쪽)
				float fBulletX = m_tInfo.fX;
				float fBulletY = m_tInfo.fY - 10.f;  // 플레이어 중심보다 약간 위

				// 방향에 따른 오프셋
				if (m_eDir == DIR_LEFT)
					fBulletX -= 15.f;
				else
					fBulletX += 15.f;

				CBullet* pBullet = new CBullet(fBulletX, fBulletY, m_eDir);
				CObjectMgr::Get_Instance()->Add_Object(OBJ_BULLET, pBullet);

				// 발사 사운드 재생
				CSoundMgr::Get_Instance()->PlaySound(L"fireball.wav", SOUND_EFFECT, 0.5f);
			}
		}
	}

	// 방향키가 아무것도 눌리지 않고 공중도 아닐 때 → IDLE
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

		// 특정 속도 이상일때 반대 방향 키를 누르면 스키드 모션 + 스키드 이펙트
		// 특정 속도 이상일때 더킹을 하면 스키드 이펙트만
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
		// WALK, RUN, JUMP 동일
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
	// 좌우 키 입력시 가속 증가
	// 일정 가속 수치에 도달하면 등가속
	// 이에 따른 속도 증가도 일정 수치에 도달하면 등속
	// 키를 뗏을때 가속도 감소 : 속도가 0이 될때까지
	// 속도가 0이 되면 가속도는 0으로 초기화

	// v = at

}

void CPlayer::Offset()
{
	const float fFocusX = WINCX * 0.42f;
	const float fOffsetBoxHalf = 24.f;

	fOffsetBoxLeft = fFocusX - fOffsetBoxHalf;
	fOffsetBoxRight = fFocusX + fOffsetBoxHalf;

	// X축: 오프셋 박스를 벗어나면 스크롤 이동
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

	// Y축: 플레이어를 화면 중앙에 고정
	float fTargetScrollY = m_tInfo.fY - WINCY * 0.5f;
	float fCurrentScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();
	float fDeltaY = fTargetScrollY - fCurrentScrollY;
	//ScrollMgr::Get_Instance()->Set_ScrollY(fDeltaY);

	CScrollMgr::Get_Instance()->Scroll_Lock();
}

void CPlayer::Update_Hold(bool _hold)
{
	if (_hold)  // 잡기 상태로 전환
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
	else  // 일반 상태로 복귀
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
