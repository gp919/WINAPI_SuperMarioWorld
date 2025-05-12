#pragma once

#include "pch.h"

class CScrollMgr
{
private:
	CScrollMgr();
	~CScrollMgr();

public:
	float		Get_ScrollX() { return m_fScrollX; }
	float		Get_ScrollY() { return m_fScrollY; }

	void		Set_ScrollX(float _fX) { m_fScrollX += _fX; }
	void		Set_ScrollY(float _fY) { m_fScrollY += _fY; }
	void		Set_AbsScrollY(float _fY) { m_fScrollY = _fY; }
	void		Set_AbsScrollX(float _fX) { m_fScrollX = _fX; }

	void		Scroll_Lock();

	void		Set_Size(float _fx, float _fy) { m_fStageCX = _fx, m_fStageCY = _fy; };
	void		Follow_Target(float, float, float);

	void Screen_To_World(float, float, float*, float*);
	void World_To_Screen(float, float, float*, float*);
	void World_To_Grid(float, float, float*, float*);

public:
	static CScrollMgr* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CScrollMgr;
		}

		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	static CScrollMgr* m_pInstance;

	float		m_fScrollX PURE;
	float		m_fScrollY PURE;
	float		m_fWorldX PURE;
	float		m_fWorldY PURE;
	float		m_fGridX PURE;
	float		m_fGridY PURE;

	float m_fStageCX;
	float m_fStageCY;

};

