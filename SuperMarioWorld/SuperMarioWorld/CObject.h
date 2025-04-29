#pragma once
#include "pch.h"
#include "Define.h"


class CObject
{
public:
	CObject();
	~CObject();

public:
	virtual void Initialize()PURE;
	virtual int Update()PURE;
	virtual void Late_Update()PURE;
	virtual void Render(HDC)PURE;
	virtual void Release()PURE;

private:
	bool m_bDead = false;
	bool m_bJump = false;
	float m_fSpeed = 1.f;
	float m_fAspeed = 1.f;

};