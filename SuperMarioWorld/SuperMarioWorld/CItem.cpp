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
	m_tInfo.fCX = TILECX * SCALE_FACTOR;
	m_tInfo.fCY = TILECY * SCALE_FACTOR;
	m_fSpeed = 1.f;

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
}

int CItem::Update()
{
	if (m_bDead)
	{
		CUiMgr::Get_Instance()->Set_Score(100);
		return DEAD;
	}

	if (m_bPopUp)
	{
		m_tInfo.fY -= 1.6f;
		m_iPopFrame++;

		if (m_iPopFrame >= 19)
		{
			m_bPopUp = false;
			m_bMove = true; // ���� �߷� ���� ����
		}

		return NOEVENT;
	}
	
	if (m_bMove)
	{
		// TODO : ������ �Լ�(�Ϻ� �����ۿ���)
	}

	CObject::Move_Frame();

	return NOEVENT;
}

void CItem::Late_Update()
{
	// ȭ��Ʒ��� ���� ���
	if (m_tInfo.fY >= 1700.f * SCALE_FACTOR || m_tInfo.fX == 0)
		m_bDead = true;

	// ����, ������ ��
	// TODO : �߷� ����

	// TODO : ���� �浹 �˻�


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


// ���� UI ��ġ ���游 �ݿ�
void CItem::Catch_Item(CObject* _obj)
{
	// TODO : ������ ���� ��ȭ �������� ���
	// TODO : üũ����Ʈ , ��������Ʈ�� ���
	// TODO : UI�� �ݿ��Ǵ� �������� ���

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
