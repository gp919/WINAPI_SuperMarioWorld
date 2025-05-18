#pragma once
#include "pch.h"
#include "CMainGame.h"
#include "CObject.h"
#include "CObjectMgr.h"
#include "CBmpMgr.h"
#include "CKeyMgr.h"
#include "CLineMgr.h"
#include "CMonster.h"
#include "CItem.h"
#include "CBullet.h"

class CPlayer : public CObject
{

public:
	CPlayer();
	~CPlayer();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;

	void On_Collision(EOBJECTID _id)override;

public:
	bool Is_Grab() 
	{ 
		return m_bGrab;
	};

	MARIOSTATE Get_Mario() { return m_eMarioState; };
	void Offset();

private:
	void Key_Input();
	void Change_State();
	void Update_Gravity();
	void Update_Speed();
	
	void Update_Hold(bool);
	void Change_Mario(MARIOSTATE);

private:
	MARIOSTATE		m_eMarioState = MARIO_SMALL;
	PLAYERSTATE     m_eCurState;
	PLAYERSTATE     m_ePreState;
	bool m_bSpin = false;
	bool m_bDuck = false;
	bool m_bGrab = false;
	DWORD m_dwFadeStartTime = 0.f;
	float fOffsetBoxLeft;
	float fOffsetBoxRight;

	bool m_bInvi = false;               // 무적 상태 여부
	DWORD m_dwHitTime = 0;             // 마지막 피격 시간
	float m_fCoolTime = 1.5f;          // 무적 지속 시간 (1.5초)

};