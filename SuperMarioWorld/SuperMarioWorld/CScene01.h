#pragma once
#include "pch.h"
#include "CScene.h"
#include "CMyBmp.h"
#include "CBmpMgr.h"
#include "CObjectMgr.h"
#include "CTile.h"
#include "CPlayer.h"
#include "CUiMgr.h"
#include "CGoomba.h"
#include "CKoopa.h"

class CScene01 : public CScene
{
public:
	CScene01();
	virtual ~CScene01();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;
	pair<float, float> Get_MapSize() { return { 5120.f, 432.f }; };
};

