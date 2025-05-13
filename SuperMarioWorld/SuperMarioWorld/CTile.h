#pragma once

#include "pch.h"
#include "CObject.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CItem.h"

class CTile : public CObject
{
public:
	CTile();
	CTile(float _fx, float _fy, TILEID _tileid);
	CTile(INFO);
	~CTile();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;

	void On_Hit(OBJECTDIR);

private:
	bool m_bHit = false;
	bool m_bBounce = false;
	float m_fBounceY = 0.f;
	float m_fOriginY = 0.f;
	TILEID m_eTileId = TILE_EMPTY;
};

