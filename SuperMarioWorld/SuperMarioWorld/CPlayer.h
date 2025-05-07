#pragma once
#include "pch.h"
#include "CMainGame.h"
#include "CObject.h"
#include "CObjectMgr.h"
#include "CBmpMgr.h"
#include "CKeyMgr.h"
#include "CLineMgr.h"


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
	void Change_Motion();
	void Update_Gravity();
	void Offset();

private:
	PLAYERSTATE     m_eCurState;
	PLAYERSTATE     m_ePreState;
	bool m_bSpin = false;

	bool m_bLand = false;
	float fOffsetBoxLeft;
	float fOffsetBoxRight;

};