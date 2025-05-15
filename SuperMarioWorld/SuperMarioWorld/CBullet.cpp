#include "pch.h"
#include "CBullet.h"

CBullet::CBullet(float _fx, float _fy, OBJECTDIR _dir)
	: m_fSpeed(7.f), m_fGravity(0.3f), m_fSpeedY(0.f)
{
	m_tInfo.fX = _fx;
	m_tInfo.fY = _fy;
	m_eBulletDir = _dir;
}

CBullet::~CBullet()
{
	Release();
}

void CBullet::Initialize()
{
	m_tInfo.fCX = 8.f * SCALE_FACTOR;  // 3�� Ȯ��
	m_tInfo.fCY = 8.f * SCALE_FACTOR;  // 3�� Ȯ��

	// ���̾�� �ʱ� �ӵ� ����
	m_fSpeedY = 3.f; // �������� ��¦ ������ ȿ��

	// ������ ���� (4���� �̹����� �̾�ٿ��� ����)
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 3;
	m_tFrame.iMotion = 0;
	m_tFrame.dwSpeed = 100; // ������ ��ȯ �ӵ�
	m_tFrame.dwTime = GetTickCount64();
}

int CBullet::Update()
{
	if (m_bDead)
		return DEAD;

	// ���� �̵� (���⿡ ����)
	if (m_eBulletDir == DIR_LEFT)
		m_tInfo.fX -= m_fSpeed;
	else
		m_tInfo.fX += m_fSpeed;

	// �߷� ����
	m_fSpeedY += m_fGravity;
	m_tInfo.fY += m_fSpeedY;

	// ȭ�� ������ ������ ����
	if (Out_Of_Screen())
		m_bDead = true;

	return NOEVENT;
}

void CBullet::Late_Update()
{
	// Ÿ�ϰ��� �浹 �˻�
	On_Collision(OBJ_TILE);
	// ���� �浹 �˻� (�������� �浹)
	float fY = 0.f;
	if (CLineMgr::Get_Instance()->Collision_Line(m_tInfo, &fY))
	{
		if (m_tInfo.fY + m_tInfo.fCY * 0.5f >= fY)
		{
			m_fSpeedY = -6.f; // �ٴڿ��� ƨ���
			m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
		}
	}

	On_Collision(OBJ_MONSTER);

	CObject::Update_Rect();
	CObject::Move_Frame();
}

void CBullet::Render(HDC hDC)
{
	m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Bullet");
	GdiTransparentBlt(hDC,
		(int)(m_tInfo.fX - m_tInfo.fCX * 0.5f - m_fScrollX),
		(int)(m_tInfo.fY - m_tInfo.fCY * 0.5f - m_fScrollY),
		(int)m_tInfo.fCX,
		(int)m_tInfo.fCY,
		hMemDC,
		m_tFrame.iStart * 8, // 8�ȼ��� �̵�
		m_tFrame.iMotion * 8,
		8,
		8,
		RGB(0, 255, 0));
}

void CBullet::Release()
{
}

void CBullet::On_Collision(EOBJECTID _id)
{
	CObject* pTarget = nullptr;

	switch (_id)
	{
	case OBJ_TILE:
		pTarget = CCollisionMgr::Collision_RectEx(
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_BULLET),
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_TILE)
		);

		if (pTarget)
		{
			// Ÿ�ϰ� �浹 �� ���� (���� �ε���)
			m_bDead = true;
		}
		break;

	case OBJ_MONSTER:
		pTarget = CCollisionMgr::Collision_RectEx(
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_BULLET),
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_MONSTER)
		);

		if (pTarget)
		{
			CMonster* pMonster = static_cast<CMonster*>(pTarget);
			if (pMonster->In_Screen())
			{
				m_bDead = true;
				pTarget->Set_Dead();

				// ������ ����
				CItem* pItem = new CItem(pTarget->Get_Info()->fX, pTarget->Get_Info()->fY, ITEM_COIN);
				CObjectMgr::Get_Instance()->Add_Object(OBJ_ITEM, pItem);
			}
		}
		break;
	}
}

bool CBullet::Out_Of_Screen()
{
	// ȭ�� ������ �������� Ȯ��
	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	if (m_tInfo.fX < fScrollX - 100.f || m_tInfo.fX > fScrollX + WINCX + 100.f ||
		m_tInfo.fY < fScrollY - 100.f || m_tInfo.fY > fScrollY + WINCY + 100.f)
		return true;

	return false;
}