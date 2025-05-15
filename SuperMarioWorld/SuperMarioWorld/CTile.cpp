#pragma once

#include "pch.h"
#include "CTile.h"

CTile::CTile()
{
}

CTile::CTile(float _fx, float _fy, TILEID _tileid)
{
	m_tInfo.fX = _fx;
	m_tInfo.fY = _fy;
	m_tInfo.iType = (int)(_tileid);
	m_eTileId = _tileid;
}

CTile::CTile(INFO _info)
{
	m_tInfo = _info;
	m_eTileId = (TILEID)(_info.iType);
}

CTile::~CTile()
{
	Release();
}

void CTile::Initialize()
{
	m_tInfo.fCX = TILECX * SCALE_FACTOR;
	m_tInfo.fCY = TILECY * SCALE_FACTOR;
	m_fOriginY = m_tInfo.fY;
	
	
	m_tFrame.dwTime = GetTickCount();

	switch (m_eTileId)
	{
	case TILE_Q:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 3;
		m_tFrame.iMotion = 0;
		m_tFrame.dwSpeed = 100.f;
		break;
	case TILE_E:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 1;
		break;
	case TILE_ROT:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 2;
		break;
	case TILE_CLOUD:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 3;
		break;
	case TILE_EMPTY:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 4;
		break;
	case TILE_HIDDEN:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 5;
		break;
	default:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 4;
		break;

	}

	CObject::Update_Rect();
}

int CTile::Update()
{

	/*m_tFrame.dwSpeed = 100.f;
	m_tFrame.dwTime = GetTickCount();*/
	if (m_bBounce)
	{
		m_tInfo.fY -= 4.f; // 올라감
		if (m_tInfo.fY <= m_fBounceY)
			m_bBounce = false;
	}
	else if (m_bHit && m_tInfo.fY < m_fOriginY)
	{
		m_tInfo.fY += 4.f; // 다시 내려옴
		if (m_tInfo.fY >= m_fOriginY)
		{
			m_tInfo.fY = m_fOriginY;
			m_eTileId = TILE_EMPTY;
			m_tInfo.iType = TILE_EMPTY;
			m_tFrame.iStart = 0;
			m_tFrame.iEnd = 0;
			m_tFrame.iMotion = 4;
		}
			
	}

	return NOEVENT;
}

void CTile::Late_Update()
{
	
	CObject::Move_Frame();
	CObject::Update_Rect();
}

void CTile::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Tile");

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	float drawX = m_tInfo.fX - m_tInfo.fCX * 0.5f - fScrollX;
	float drawY = m_tInfo.fY - m_tInfo.fCY * 0.5f - fScrollY;

	GdiTransparentBlt(hDC,
		int(drawX), int(drawY),
		int(m_tInfo.fCX), int(m_tInfo.fCY),
		hMemDC,
		m_tFrame.iStart * TILECX,
		m_tFrame.iMotion * TILECY,
		TILECX,
		TILECY,
		RGB(0, 255, 0));
}

void CTile::Release()
{
}

void CTile::On_Hit(OBJECTDIR _dir)
{

	if (m_bHit) return;

	m_bHit = true;

	if (m_eTileId == TILE_Q)
	{
		m_bBounce = true;  // 튀는 애니메이션
		m_fBounceY = m_tInfo.fY - 24.f; // 위로 24px 튀기기
		CSoundMgr::Get_Instance()->PlaySoundW(L"coin.wav", SOUND_EFFECT, 0.1f);
	}

	else if (m_eTileId == TILE_E)
	{
		// 아이템 생성
		CItem* pItem = nullptr;
		// TODO : 코드 리팩터링
		switch (m_eTileId)
		{
		//case TILE_Q: pItem = new CItem(m_tInfo.fX, m_tInfo.fY - m_tInfo.fCY * 0.5f, ITEM_LEV); break;
		case TILE_E:
			CPlayer* pPlayer = static_cast<CPlayer*>(CObjectMgr::Get_Instance()->Get_ObjectList(OBJ_PLAYER).front());
			if(pPlayer->Get_Mario()==MARIO_SMALL)
				pItem = new CItem(m_tInfo.fX, m_tInfo.fY - m_tInfo.fCY * 0.5f, ITEM_MUSH);
			else
				pItem = new CItem(m_tInfo.fX, m_tInfo.fY - m_tInfo.fCY * 0.5f, ITEM_FLOWER);
			break;
		}
		pItem->Set_Pop(true);
		pItem->Set_Dir(_dir);
		CObjectMgr::Get_Instance()->Add_Object(OBJ_ITEM, pItem);
		CSoundMgr::Get_Instance()->PlaySoundW(L"sprout-item.wav", SOUND_EFFECT, 0.1f);

		// 빈 타일로 변경
		m_eTileId = TILE_EMPTY;
		m_tInfo.iType = TILE_EMPTY;
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 4;
	}
	
	else if (m_eTileId == TILE_ROT)
	{
		m_bDead = true;
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 3;
		m_tFrame.iMotion = 2;
		m_tFrame.dwSpeed = 100;
	}
}
