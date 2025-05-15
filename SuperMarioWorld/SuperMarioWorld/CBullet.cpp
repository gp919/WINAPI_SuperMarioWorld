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
	m_tInfo.fCX = 8.f * SCALE_FACTOR;  // 3배 확대
	m_tInfo.fCY = 8.f * SCALE_FACTOR;  // 3배 확대

	// 파이어볼의 초기 속도 설정
	m_fSpeedY = 3.f; // 위쪽으로 살짝 던지는 효과

	// 프레임 설정 (4개의 이미지가 이어붙여진 상태)
	m_tFrame.iStart = 0;
	m_tFrame.iEnd = 3;
	m_tFrame.iMotion = 0;
	m_tFrame.dwSpeed = 100; // 프레임 전환 속도
	m_tFrame.dwTime = GetTickCount64();
}

int CBullet::Update()
{
	if (m_bDead)
		return DEAD;

	// 수평 이동 (방향에 따라)
	if (m_eBulletDir == DIR_LEFT)
		m_tInfo.fX -= m_fSpeed;
	else
		m_tInfo.fX += m_fSpeed;

	// 중력 적용
	m_fSpeedY += m_fGravity;
	m_tInfo.fY += m_fSpeedY;

	// 화면 밖으로 나가면 삭제
	if (Out_Of_Screen())
		m_bDead = true;

	return NOEVENT;
}

void CBullet::Late_Update()
{
	// 타일과의 충돌 검사
	On_Collision(OBJ_TILE);
	// 라인 충돌 검사 (지형과의 충돌)
	float fY = 0.f;
	if (CLineMgr::Get_Instance()->Collision_Line(m_tInfo, &fY))
	{
		if (m_tInfo.fY + m_tInfo.fCY * 0.5f >= fY)
		{
			m_fSpeedY = -6.f; // 바닥에서 튕기기
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
		m_tFrame.iStart * 8, // 8픽셀씩 이동
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
			// 타일과 충돌 시 삭제 (벽에 부딪힘)
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

				// 아이템 생성
				CItem* pItem = new CItem(pTarget->Get_Info()->fX, pTarget->Get_Info()->fY, ITEM_COIN);
				CObjectMgr::Get_Instance()->Add_Object(OBJ_ITEM, pItem);
			}
		}
		break;
	}
}

bool CBullet::Out_Of_Screen()
{
	// 화면 밖으로 나갔는지 확인
	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	if (m_tInfo.fX < fScrollX - 100.f || m_tInfo.fX > fScrollX + WINCX + 100.f ||
		m_tInfo.fY < fScrollY - 100.f || m_tInfo.fY > fScrollY + WINCY + 100.f)
		return true;

	return false;
}