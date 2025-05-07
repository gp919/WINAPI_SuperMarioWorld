#pragma once

#include "pch.h"
#include "CObject.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"

class CTile : public CObject
{
public:
	CTile();
	CTile(float _fx, float _fy, TILEID _tileid);
	~CTile();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;

private:


private:
	TILEID m_eTileId = TILE_EMPTY;
};

