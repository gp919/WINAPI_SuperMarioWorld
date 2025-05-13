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


private:
	void Key_Input();
	void Change_State();
	void Update_Gravity();
	void Update_Speed();
	void Offset();

private:
	PLAYERSTATE     m_eCurState;
	PLAYERSTATE     m_ePreState;
	bool m_bSpin = false;
	bool m_bDuck = false;
	float fOffsetBoxLeft;
	float fOffsetBoxRight;

};