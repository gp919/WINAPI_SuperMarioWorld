#pragma once
#include "pch.h"
#include "CObject.h"

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

private:
	void Key_Input();

};