#pragma once
#include "pch.h"
#include "CMonster.h"

class CGoomba : public CMonster
{
public:
	CGoomba(float ,float);
	virtual ~CGoomba();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;
	void On_Stomped()override;
	void Update_AI()override;
};

