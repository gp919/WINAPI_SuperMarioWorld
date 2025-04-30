#pragma once
#include "pch.h"


class CObject
{
public:
	CObject();
	virtual ~CObject();

public:
	virtual void Initialize()PURE;
	virtual int Update()PURE;
	virtual void Late_Update()PURE;
	virtual void Render(HDC)PURE;
	virtual void Release()PURE;

protected:
	void		Update_Rect();
	void		Move_Frame();

protected:
	
	bool m_bDead = false;
	int m_iJump = 0;
	float m_fSpeed = 1.f;
	float m_fAspeed = 1.f;

	INFO		m_tInfo;
	RECT		m_tRect;
	FRAME		m_tFrame;

	const TCHAR* m_pFrameKey;

	OBJECTDIR m_eDir;
};