#include "pch.h"
#include "CKoopa.h"

CKoopa::CKoopa(float _fx, float _fy)
{
	m_tInfo.fX = _fx;
	m_tInfo.fY = _fy;
}

CKoopa::~CKoopa()
{
	Release();
}

void CKoopa::Initialize()
{
	m_tInfo.fCX = TILECX * SCALE_FACTOR;
	m_tInfo.fCY = TILECY * SCALE_FACTOR;
	m_fSpeed = 1.f;
}

int CKoopa::Update()
{
	if (m_bDead)
	{
		if (GetTickCount() > m_dwDeathTime + 500)  // 0.5초 후 제거
		{
			CUiMgr::Get_Instance()->Set_Score(100);
			return DEAD;
		}

		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 1;  // 죽는 모션 프레임 (납작한 등)

		CObject::Move_Frame();
		return NOEVENT;
	}

	Update_AI();

	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 1;
	m_tFrame.iMotion = 0;
	m_tFrame.dwSpeed = 500;

	CObject::Move_Frame();

	return NOEVENT;
}

void CKoopa::Late_Update()
{
	
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

void CKoopa::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Koopa");

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	GdiTransparentBlt(hDC,
		(int)(m_tRect.left - fScrollX),
		(int)(m_tRect.top - fScrollY),
		(int)(m_tInfo.fCX),  // ← 몬스터 너비
		(int)(m_tInfo.fCY),  // ← 몬스터 높이
		hMemDC,
		m_tFrame.iStart * TILECX,
		m_tFrame.iMotion * TILECY,
		TILECX,
		TILECY,
		RGB(0, 255, 0));
}

void CKoopa::Release()
{
}

void CKoopa::On_Stomped()
{
	m_eState = MONSTER_STOMPED;
	m_bDead = true;

	m_dwDeathTime = GetTickCount();  // 죽은 시간 기록
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 0;
	m_tFrame.iMotion = 2;  // ← 납작한 모션 (스프라이트에 따라 조정)
}

void CKoopa::Update_AI()
{
	if (m_eDir == DIR_LEFT)
		m_tInfo.fX -= m_fSpeed;
	else
		m_tInfo.fX += m_fSpeed;
}
