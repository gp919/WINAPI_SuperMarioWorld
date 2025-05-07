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
	m_eTileId = _tileid;
}

CTile::~CTile()
{
	Release();
}

void CTile::Initialize()
{
	m_tInfo.fCX = TILECX * SCALE_FACTOR;
	m_tInfo.fCY = TILECY * SCALE_FACTOR;
	switch (m_eTileId)
	{
	case TILE_Q:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 3;
		m_tFrame.iMotion = 0;
		break;
	case TILE_E:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 0;
		m_tFrame.iMotion = 1;
		break;
	case TILE_ROT:
		m_tFrame.iStart = 0;
		m_tFrame.iEnd = 3;
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
	CObject::Move_Frame();
	CObject::Update_Rect();
	return NOEVENT;
}

void CTile::Late_Update()
{
}

void CTile::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Tile");

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	GdiTransparentBlt(hDC,
		(int)(m_tRect.left - fScrollX),
		(int)(m_tRect.top - fScrollY),
		(int)(m_tInfo.fCX),  // ← 타일 너비
		(int)(m_tInfo.fCY),  // ← 타일 높이
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
