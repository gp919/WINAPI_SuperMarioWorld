#pragma once
#include "CObject.h"

class CMouse : public CObject
{
public:
	CMouse();
	virtual ~CMouse();

public:
	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC hDC) override;
	void Release() override;
};

