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

	virtual void On_Collision(EOBJECTID _id);

public:
	INFO* Get_Info() { return &m_tInfo; };
	RECT* Get_Rect() { return &m_tRect; };
	EOBJECTID Get_ID() { return m_eId; }
	COLLISIONDIR Get_Col() { return m_eColDir; };
	bool Get_Dead() { return m_bDead; };
	OBJECTDIR Get_Dir() { return m_eDir; };

	void Set_PosX(float _fx) { m_tInfo.fX += _fx; };
	void Set_PosY(float _fy) { m_tInfo.fY += _fy; };
	void Set_ID(EOBJECTID _id) { m_eId = _id; };
	void Set_Col(COLLISIONDIR _dir) { m_eColDir = _dir; };
	void Set_Dead() { m_bDead = true; };
	void Set_TileCol() { m_bTileCol = true; };
	void Set_Dir(OBJECTDIR _dir) { m_eDir = _dir; };
	

protected:
	void		Update_Rect();
	void		Move_Frame();
	void		Allign_Bottom(float, float);

protected:
	float		m_fScrollX PURE;
	float		m_fScrollY PURE;
	float		m_fWorldX PURE;
	float		m_fWorldY PURE;
	float		m_fGridX PURE;
	float		m_fGridY PURE;

	bool m_bDead = false;
	float m_fSpeed = 1.f;
	float m_fAspeed = 1.f;

	INFO		m_tInfo;
	RECT		m_tRect;
	RECT		m_tHitBox;
	FRAME		m_tFrame;

	const TCHAR* m_pFrameKey;

	OBJECTDIR	m_eDir;
	EOBJECTID	m_eId;


	bool m_bJump;
	float m_fJumpSpeed;
	float m_fJumpTime;
	float m_fAccel;

	bool m_bTileCol=false;
	COLLISIONDIR m_eColDir=COL_NONE;

	float m_fCoolTime = 0.3f;
	DWORD m_dwStart = 0.f;
};