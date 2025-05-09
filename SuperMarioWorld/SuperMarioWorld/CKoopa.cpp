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
		if (GetTickCount() > m_dwDeathTime + 500)  // 0.5�� �� ����
		{
			CUiMgr::Get_Instance()->Set_Score(100);
			return DEAD;
		}

		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 1;  // �״� ��� ������ (������ ��)

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

void CKoopa::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Koopa");

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	GdiTransparentBlt(hDC,
		(int)(m_tRect.left - fScrollX),
		(int)(m_tRect.top - fScrollY),
		(int)(m_tInfo.fCX),  // �� ���� �ʺ�
		(int)(m_tInfo.fCY),  // �� ���� ����
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

	m_dwDeathTime = GetTickCount();  // ���� �ð� ���
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 0;
	m_tFrame.iMotion = 2;  // �� ������ ��� (��������Ʈ�� ���� ����)
}

void CKoopa::Update_AI()
{
	if (m_eDir == DIR_LEFT)
		m_tInfo.fX -= m_fSpeed;
	else
		m_tInfo.fX += m_fSpeed;
}
