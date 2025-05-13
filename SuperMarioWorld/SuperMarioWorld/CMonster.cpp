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
	case MON_GOOMBA:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 3;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 100.f;
		break;
	case MON_GREENKOOPA:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 1;
		m_tFrame.iMotion = 3;
		m_tFrame.dwSpeed = 200.f;
		m_tInfo.fCX = TILECX * SCALE_FACTOR;
		m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
		break;
	case MON_REDKOOPA:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 1;
		m_tFrame.iMotion = 3;
		m_tFrame.dwSpeed = 200.f;
		m_tInfo.fCX = TILECX * SCALE_FACTOR;
		m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
		break;
	case MON_MOLE:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 1;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 200.f;
		break;
	case MON_PIRANHA:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 1;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 200.f;
		break;

	}

	Allign_Bottom(m_tInfo.fX, m_tInfo.fY);

}

int CMonster::Update()
{

	if (m_bDead)
	{
		if (GetTickCount() > m_dwDeathTime + 500)  // 0.5�� �� ����
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
	//	ȭ��(1224)�Ʒ��� ����, �߽���ǥ�� 0�̸� ���
	if (m_tInfo.fY >= 1700.f * SCALE_FACTOR || m_tInfo.fX == 0.f)
		m_bDead = true;
	// �߷� ����
	m_tInfo.fY += 0.5f;
	
	// ���� �浹 �˻�
	float fY = 0.f;
	if (CLineMgr::Get_Instance()->Collision_Line(this->m_tInfo, &fY))
	{
		
		// ���� �Ǵ�
		if (m_tInfo.fY + m_tInfo.fCY * 0.5f >= fY)
		{
			m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
			m_fFallSpeed = 0.f;
		}
		else
		{
			// ���� ���߿� �ִ� ����
			m_fFallSpeed += GRAVITY * 0.1f;
			if (m_fFallSpeed > 8.f)
				m_fFallSpeed = 8.f;
		}
	}
	else
	{
		// ���߿� ������ ��� ����
		m_fFallSpeed += GRAVITY * 0.1f;
		if (m_fFallSpeed > 8.f)
			m_fFallSpeed = 8.f;
	}
	
	CObject::Update_Rect();
}

void CMonster::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Goomba");
	switch (m_tInfo.iType)
	{
	case MON_GOOMBA:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Goomba");
		break;
	case MON_GREENKOOPA:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"GKoopa");
		break;
	case MON_REDKOOPA:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"RKoopa");
		break;
	case MON_MOLE:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Mole");
		break;
	case MON_PIRANHA:
		hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Piranha");
		break;

	}

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	GdiTransparentBlt(hDC,
		(int)(m_tInfo.fX - m_tInfo.fCX * 0.5f - fScrollX),
		(int)(m_tInfo.fY - m_tInfo.fCY * 0.5f - fScrollY),
		(int)(m_tInfo.fCX),  // �� ���� �ʺ�
		(int)(m_tInfo.fCY),  // �� ���� ����
		hMemDC,
		m_tFrame.iStart * TILECX,
		m_tFrame.iMotion * TILECY,
		TILECX,
		(m_tInfo.fCY / (SCALE_FACTOR)),
		RGB(0, 255, 0));
}

void CMonster::Release()
{

}

void CMonster::Update_State()
{

}

void CMonster::On_Stomped()
{
	// �������� Ƣ�� ������ : ���� X
	if (
		((m_tInfo.iType == MON_GREENKOOPA) || (m_tInfo.iType == MON_REDKOOPA))
		&&	(m_eState != MONSTER_EJECTED)
		)
	{
		m_eState = MONSTER_EJECTED;
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 1;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 200.f;
		m_tInfo.fCY = TILECY * SCALE_FACTOR;
	}
	// ����
	else
	{
		m_eState = MONSTER_STOMPED;
		m_bDead = true;
		switch (m_tInfo.iType)
		{
		case MON_GOOMBA:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 3;
			m_tFrame.iMotion = 1;  // �״� ��� ������
			m_tFrame.dwSpeed = 25.f;
			break;
		case MON_GREENKOOPA:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 1;  // �״� ��� ������
			m_tInfo.fCY = TILECY * SCALE_FACTOR;
			break;
		case MON_REDKOOPA:
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 1;  // �״� ��� ������
			m_tInfo.fCY = TILECY * SCALE_FACTOR;
			break;

		}
		
		m_dwDeathTime = GetTickCount();  // ���� �ð� ���
	}

}

void CMonster::Update_AI()
{
	if (m_eDir == DIR_LEFT)
		m_tInfo.fX -= m_fSpeed;
	else
		m_tInfo.fX += m_fSpeed;
}
