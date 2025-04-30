#pragma once
#include "pch.h"
#include "CScene.h"
#include "CMyBmp.h"
#include "CBmpMgr.h"

class CTutorial : public CScene
{
public:
	CTutorial();
	virtual ~CTutorial();

public:
	 void Initialize()override;
	 int Update()override;
	 void Late_Update()override;
	 void Render(HDC)override;
	 void Release()override;
};

