#include "pch.h"
#include "CMonster.h"

CMonster::CMonster()
{
}

CMonster::CMonster(float _fx, float _fy, MONSTERID _monid )
{
	m_tInfo.fX = _fx;
	m_tInfo.fY = _fy;
	m_tInfo.iType = _monid;
}

CMonster::CMonster(INFO _info)
{
	m_tInfo = _info;
}


CMonster::~CMonster()
{
	Release();
}

void CMonster::Initialize()
{
	m_tInfo.fCX = TILECX * SCALE_FACTOR;
	m_tInfo.fCY = TILECY * SCALE_FACTOR;
	m_fSpeed = 1.f;

	switch (m_tInfo.iType)
	{
	case MON_KOOPA:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 1;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 200.f;
		break;
	case MON_GOOMBA:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 3;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 100.f;
		break;
	}

}

int CMonster::Update()
{

	if (m_bDead)
	{
		if (GetTickCount() > m_dwDeathTime + 500)  // 0.5초 후 제거
		{
			CUiMgr::Get_Instance()->Set_Score(100);
			return DEAD;
		}
		
		
		CObject::Move_Frame();
		return NOEVENT;
	}

	if(m_bMove)
		Update_AI();
	

	
	
	CObject::Move_Frame();
	
	return NOEVENT;
}

void CMonster::Late_Update()
{
	//	화면(1224)아래로 가면, 중심좌표가 0이면 사망
	if (m_tInfo.fY >= 1700.f * SCALE_FACTOR || m_tInfo.fX == 0.f)
		m_bDead = true;
	// 중력 적용
	m_tInfo.fY += 0.5f;
	
	// 라인 충돌 검사
	float fY = 0.f;
	if (CLineMgr::Get_Instance()->Collision_Line(this->m_tInfo, &fY))
	{
		
		// 착지 판단
		if (m_tInfo.fY + m_tInfo.fCY * 0.5f >= fY)
		{
			m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
			m_fFallSpeed = 0.f;
		}
		else
		{
			// 아직 공중에 있는 상태
			m_fFallSpeed += GRAVITY * 0.1f;
			if (m_fFallSpeed > 8.f)
				m_fFallSpeed = 8.f;
		}
	}
	else
	{
		// 공중에 있으면 계속 낙하
		m_fFallSpeed += GRAVITY * 0.1f;
		if (m_fFallSpeed > 8.f)
			m_fFallSpeed = 8.f;
	}
	
	CObject::Update_Rect();
}

void CMonster::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Koopa");
	switch (m_tInfo.iType)
	{
	case MON_KOOPA:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Koopa");
		break;
	case MON_GOOMBA:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Goomba");
		break;
	}

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	GdiTransparentBlt(hDC,
		(int)(m_tInfo.fX - m_tInfo.fCX * 0.5f - fScrollX),
		(int)(m_tInfo.fY - m_tInfo.fCY * 0.5f - fScrollY),
		(int)(m_tInfo.fCX),  // ← 몬스터 너비
		(int)(m_tInfo.fCY),  // ← 몬스터 높이
		hMemDC,
		m_tFrame.iStart * TILECX,
		m_tFrame.iMotion * TILECY,
		TILECX,
		TILECY,
		RGB(0, 255, 0));
}

void CMonster::Release()
{

}

void CMonster::On_Stomped()
{
	m_eState = MONSTER_STOMPED;
	m_bDead = true;

	switch (m_tInfo.iType)
	{
	case MON_KOOPA:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 1;  // 죽는 모션 프레임 (납작한 등)
		break;
	case MON_GOOMBA:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 3;
		m_tFrame.iMotion = 1;  // 죽는 모션 프레임 (납작한 등)
		m_tFrame.dwSpeed = 25.f;
		break;
	}
	m_dwDeathTime = GetTickCount();  // 죽은 시간 기록
}

void CMonster::Update_AI()
{
	if (m_eDir == DIR_LEFT)
		m_tInfo.fX -= m_fSpeed;
	else
		m_tInfo.fX += m_fSpeed;
}
