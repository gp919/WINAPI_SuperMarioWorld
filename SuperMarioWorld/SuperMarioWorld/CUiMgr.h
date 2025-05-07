#pragma once

#include "pch.h"
#include "CBmpMgr.h"

enum ENumberType { SMALL_WHITE, SMALL_YELLOW, BIG_WHITE };

class CUiMgr
{
public:
	CUiMgr();
	~CUiMgr();

public:
	static CUiMgr* Get_Instance()
	{
		if (!m_pInstance)
			m_pInstance = new CUiMgr;

		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			Safe_Delete(m_pInstance);
		}
	}

public:
	void Initialize();
	void Render(HDC);
	void Release();

	void Set_Time(float _time) { m_fTime = _time; };
	void Set_Life(float _life) { m_iLife = _life; };
	void Set_Coin(float _coin) { m_iCoin = _coin; };

	void Render_PlayerLife(HDC, float, float);
	void Render_Timer(HDC, float, float);
	void Render_Score(HDC, float, float);
	void Render_Number(HDC, float, float, int, ENumberType);



private:
	static CUiMgr* m_pInstance;

	float m_fTime = 999.f;
	int m_iLife = 100;
	int m_iCoin = 10;
	int m_iScore = 1000;


};

