#pragma once

#include "pch.h"
#include "Define.h"

class CScene abstract
{
public:
	CScene();
	~CScene();

public:
	virtual void Initialize()PURE;
	virtual int Update()PURE;
	virtual void Late_Update()PURE;
	virtual void Render(HDC)PURE;
	virtual void Release()PURE;
};