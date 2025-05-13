#include "pch.h"
#include "CItem.h"

CItem::CItem(float _fx, float _fy, ITEMID _id)
{
	m_tInfo.fX = _fx;
	m_tInfo.fY = _fy;
	m_tInfo.iType = _id;
}

CItem::CItem(INFO _info)
{
	m_tInfo = _info;
}

CItem::~CItem()
{
	Release();
}

void CItem::Initialize()
{
	m_bDead = false;
	m_bJump = false;
	m_bMove = false;
	m_fJumpSpeed = 0.f;
	m_fJumpTime = 0.f;

	m_tInfo.fCX = TILECX * SCALE_FACTOR;
	m_tInfo.fCY = TILECY * SCALE_FACTOR;
	m_fSpeed = 2.f;

	switch (m_tInfo.iType)
	{
	case ITEM_COIN:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 3;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 100.f;
		break;
	case ITEM_YOSHI:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 4;
		m_tFrame.iMotion = 1;
		m_tFrame.dwSpeed = 80.f;
		m_tInfo.fCX = TILECX * SCALE_FACTOR;
		m_tInfo.fCY = TILECY * SCALE_FACTOR * 2.f;
		break;
	case ITEM_MUSH:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 2;
		break;
	case ITEM_LEV:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 3;
		break;
	case ITEM_CHECK:
		m_tInfo.fCX = TILECX * SCALE_FACTOR;
		m_tInfo.fCY = TILECY * SCALE_FACTOR * 0.5f;
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 4;
		break;
	case ITEM_CLEAR:
		m_tInfo.fCX = TILECX * SCALE_FACTOR * 1.5f;
		m_tInfo.fCY = TILECY * SCALE_FACTOR * 0.5f;
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 5;
		break;
	}
	Allign_Bottom(m_tInfo.fX, m_tInfo.fY);
}

int CItem::Update()
{
	if (m_bMoveReady)
	{
		m_bMoveReady = false;
		m_bMove = true;
		// 다음 프레임부터 m_bMove 조건이 발동되도록 지연
	}

	if (m_bDead)
	{
		CUiMgr::Get_Instance()->Set_Score(100);
		return DEAD;
	}

	if (m_bPopUp)
	{
		m_tInfo.fY -= 1.f;
		m_iPopFrame++;

		if (m_iPopFrame >= 23)
		{
			m_bPopUp = false;
			m_iPopFrame = 0.f;
			if (m_tInfo.iType == ITEM_MUSH || m_tInfo.iType == ITEM_LEV)
			{
				m_bMoveReady = true;
				m_bJump = true;
				m_fJumpTime = 0.1f;
				m_fJumpSpeed = 8.f;
			}
				
		}
	}
	
	if (m_bMove && !m_bPopUp)
	{
		// TODO : 움직임 함수(일부 아이템에만)
		if (m_eDir == DIR_RIGHT)
		{
			m_tInfo.fX += m_fSpeed;
		}
		else
		{
			m_tInfo.fX -= m_fSpeed;
		}
	}

	

	return NOEVENT;
}

void CItem::Late_Update()
{
	// 화면아래로 가면 사망
	if (m_tInfo.fY >= 1700.f * SCALE_FACTOR || m_tInfo.fX == 0)
		m_bDead = true;

	if (m_bDead) return;
	if(!m_bPopUp && m_bMove)
		On_Collision(OBJ_TILE);
	// 버섯, 레벨업 만 중력 적용
	if (m_bMove && m_bJump && !m_bPopUp && (m_tInfo.iType == ITEM_MUSH || m_tInfo.iType == ITEM_LEV))
	{
		Update_Gravitiy();
	}
	if (m_bMove)
		Line_Collision();

	// TODO : 라인 충돌 검사

	CObject::Move_Frame();
	CObject::Update_Rect();
}

void CItem::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Item");
	m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	int iCutY = 0;
	switch (m_tInfo.iType)
	{
	case ITEM_COIN:  iCutY = 0; break;
	case ITEM_YOSHI: iCutY = 16; break;
	case ITEM_MUSH:  iCutY = 48; break;
	case ITEM_LEV:   iCutY = 64; break;
	case ITEM_CHECK: iCutY = 80; break;
	case ITEM_CLEAR: iCutY = 96; break;
	}

	GdiTransparentBlt(
		hDC,
		(int)(m_tInfo.fX - m_tInfo.fCX * 0.5f - m_fScrollX),
		(int)(m_tInfo.fY - m_tInfo.fCY * 0.5f - m_fScrollY),
		(int)(m_tInfo.fCX),
		(int)(m_tInfo.fCY),
		hMemDC,
		m_tFrame.iStart * (int)(m_tInfo.fCX / SCALE_FACTOR),
		iCutY,
		(int)(m_tInfo.fCX / SCALE_FACTOR),
		(int)(m_tInfo.fCY / SCALE_FACTOR),
		RGB(0, 255, 0));
}

void CItem::Release()
{
}

void CItem::On_Collision(EOBJECTID _id)
{
	if (m_bDead) return;
	CObject* pTarget = nullptr;
	switch (_id)
	{
	case OBJ_TILE:
		pTarget = CCollisionMgr::Collision_RectEx(
			CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_ITEM),
			CObjectMgr::Get_Instance()->Get_ObjectList(_id)
		);
		if (pTarget && Get_Col() == COL_BOTTOM)
		{
			m_bJump = false;
			m_fJumpTime = 0.f;
			m_fJumpSpeed = 0.f;
			m_tInfo.fY = pTarget->Get_Rect()->top - m_tInfo.fCY * 0.5f;
		}
		else if (pTarget && (Get_Col() == COL_LEFT || Get_Col() == COL_RIGHT))
		{
			if (Get_Col() == COL_LEFT)
				m_eDir = DIR_RIGHT;
			else
				m_eDir = DIR_LEFT;
			
			m_bJump = true;
			m_fJumpTime = 0.1f;
			m_fJumpSpeed = 8.f;
		}
		// TODO : fix
		else if (!pTarget)
		{
			m_bJump = true;
			m_fJumpTime = 0.1f;
			m_fJumpSpeed = 8.f;
		}
		break;
	}
}

void CItem::Line_Collision()
{
	float fX(0);
	float fY(0);
	if (CLineMgr::Get_Instance()->Collision_Vertical(this->m_tInfo, &fX))
	{
		float fLeft = m_tInfo.fX - m_tInfo.fCX * 0.5f;
		float fRight = m_tInfo.fX + m_tInfo.fCX * 0.5f;

		if (m_eDir == DIR_LEFT && fX >= fLeft && fX < fRight - 0.00013f)
		{
			m_tInfo.fX = fX + m_tInfo.fCX * 0.5f;
			m_eDir = DIR_RIGHT;
		}
		else if (m_eDir == DIR_RIGHT && fX <= fRight && fX > fLeft + 0.00013f)
		{
			m_tInfo.fX = fX - m_tInfo.fCX * 0.5f;
			m_eDir = DIR_LEFT;
		}
	}
	if (m_fJumpSpeed > 0.f)
	{

		if (CLineMgr::Get_Instance()->Collision_Line(this->m_tInfo, &fY))
		{
			m_tInfo.fY = fY - m_tInfo.fCY * 0.5f;
			m_bJump = false;
			m_fJumpTime = 0.f;
			m_fJumpSpeed = 0.f;
		}
		else
		{
			m_bJump = true;
			m_fJumpTime = 0.1f;
			m_fJumpSpeed = 8.f;
		}
	}
}

void CItem::Update_Gravitiy()
{
	// y = y0 + vt
	m_tInfo.fY += m_fJumpSpeed * m_fJumpTime;
	// v = v0 + at
	if (m_fJumpSpeed >= 24.0f)
		m_fJumpSpeed = 24.0f;
	else if (m_fJumpSpeed <= -2.f)
		m_fJumpSpeed = -2.f;
	else
	{
		m_fJumpSpeed += GRAVITY * m_fJumpTime;
	}
	// t++
	m_fJumpTime += 0.1f;
}


// 현재 UI 수치 변경만 반영
void CItem::Catch_Item(CObject* _obj)
{
	// TODO : 마리오 상태 변화 아이템일 경우
	// TODO : 체크포인트 , 엔드포인트일 경우
	// TODO : UI에 반영되는 아이템일 경우

	switch (m_tInfo.iType)
	{
		case ITEM_COIN:
			CUiMgr::Get_Instance()->Set_Score(100);
			CUiMgr::Get_Instance()->Set_Coin(1);
			break;
		case ITEM_YOSHI:
			CUiMgr::Get_Instance()->Set_Score(1000);
			CUiMgr::Get_Instance()->Set_Coin(1);
			break;
		case ITEM_MUSH:
			CUiMgr::Get_Instance()->Set_Score(500);
			break;
		case ITEM_LEV:
			CUiMgr::Get_Instance()->Set_Score(500);
			CUiMgr::Get_Instance()->Set_Life(1);
			break;
		case ITEM_CHECK:
			CUiMgr::Get_Instance()->Set_Score(10000);
			break;
		case ITEM_CLEAR:
			CUiMgr::Get_Instance()->Set_Score(10000);
			break;
	}
	m_bDead = true;
}
