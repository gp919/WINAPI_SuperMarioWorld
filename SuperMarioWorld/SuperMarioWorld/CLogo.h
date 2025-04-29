#pragma once
#include "pch.h"
#include "Define.h"
#include "CScene.h"

class CLogo : public CScene
{
public:
	CLogo();
	~CLogo();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;
};