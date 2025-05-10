#pragma once
#include "pch.h"
#include "CMonster.h"
#include "CLineMgr.h"

class CKoopa : public CMonster
{
public:
	CKoopa(float, float);
	CKoopa(INFO);
	virtual ~CKoopa();
public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;
	void On_Stomped()override;
	void Update_AI()override;

private:
	float m_fFallSpeed = 0.f;
	OBJECTDIR m_eDir = DIR_LEFT;
	DWORD m_dwDeathTime = 0;
};

