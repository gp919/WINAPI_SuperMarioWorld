#pragma once
#include "pch.h"
#include "CObject.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CUiMgr.h"

enum MONSTER_STATE {MONSTER_STAND ,MONSTER_WALK, MONSTER_STOMPED, MONSTER_DEAD, MONSTER_END};

class CMonster : public CObject
{
public:
	CMonster();
	CMonster(float, float, MONSTERID);
	CMonster(INFO);
	~CMonster();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;

	virtual void On_Stomped();
	virtual void Update_AI();

private:
	MONSTER_STATE m_eState = MONSTER_STAND;
	DWORD m_dwDeathTime = 0.f;
	float m_fFallSpeed = 0.f;
	float m_bMove = false;


};

