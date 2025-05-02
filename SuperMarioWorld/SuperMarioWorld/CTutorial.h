#pragma once
#include "pch.h"
#include "CScene.h"
#include "CMyBmp.h"
#include "CBmpMgr.h"
#include "CObjectMgr.h"
#include "CTile.h"
#include "CPlayer.h"


class CTutorial : public CScene
{
public:
	CTutorial();
	~CTutorial();

public:
	 void Initialize()override;
	 int Update()override;
	 void Late_Update()override;
	 void Render(HDC)override;
	 void Release()override;
};

