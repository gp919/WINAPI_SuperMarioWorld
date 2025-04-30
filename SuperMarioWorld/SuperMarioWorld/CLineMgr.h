#pragma once

#include "CLine.h"
#include "CObject.h"

class CLineMgr
{
private:
	CLineMgr();
	~CLineMgr();

public:
	bool	Collision_Line(float _fX, float* pY);


public:
	void	Initialize();
	void	Render(HDC hDC);
	void	Release();

	void	Load_Data();

public:
	static CLineMgr* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CLineMgr;
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
	static CLineMgr* m_pInstance;
	list<CLine*>	m_Linelist;
};

