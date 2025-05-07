#pragma once
#include "pch.h"
#include "CObject.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"

enum MONSTER_STATE {MONSTER_WALK, MONSTER_STOMPED, MONSTER_DEAD, MONSTER_END};

class CMonster : public CObject
{
public:
	CMonster();
	~CMonster();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;

	virtual void On_Stomped()PURE;   // π‚«˚¿ª ∂ß
	virtual void Update_AI()PURE;    // ¿Ãµø AI

protected:
	MONSTER_STATE m_eState;

};

