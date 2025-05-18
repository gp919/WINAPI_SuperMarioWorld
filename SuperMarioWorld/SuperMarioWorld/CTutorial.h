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
	 pair<float, float> Get_MapSize() { return { 256.f, 224.f }; };
	 pair<float, float> Get_Spawn() { return { 0.f, 0.f }; }
	 
};

