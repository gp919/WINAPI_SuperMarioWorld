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

public:
	INFO Get_Info() { return m_tInfo; };

protected:
	void		Update_Rect();
	void		Update_HitBox();
	void		Move_Frame();

protected:
	
	bool m_bDead = false;
	float m_fSpeed = 1.f;
	float m_fAspeed = 1.f;

	INFO		m_tInfo;
	RECT		m_tRect;
	RECT		m_tHitBox;
	FRAME		m_tFrame;

	const TCHAR* m_pFrameKey;

	OBJECTDIR m_eDir;


	bool m_bJump;
	float m_fJumpSpeed;
	float m_fJumpTime;
	float m_fAccel;
};