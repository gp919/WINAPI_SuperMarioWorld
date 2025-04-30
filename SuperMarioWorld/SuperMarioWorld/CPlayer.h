#pragma once
#include "pch.h"
#include "CMainGame.h"
#include "CObject.h"
#include "CObjectMgr.h"
#include "CBmpMgr.h"
#include "CKeyMgr.h"


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

public:


private:
	void Key_Input();
	void Change_Motion();
	void Jump();

private:
	PLAYERSTATE     m_eCurState;
	PLAYERSTATE     m_ePreState;

};