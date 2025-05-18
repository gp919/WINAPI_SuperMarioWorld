#pragma once

#include "pch.h"
#include "CBmpMgr.h"
#include "CObjectMgr.h"
#include "CSoundMgr.h"

// 1번째 ~ 7번째까지는 점수, 8번째는 1UP
const int g_iScoreChain[] = {
	100, 200, 400, 800,
	1000, 2000, 4000,
	10000 // 8번째
};
const int g_iScoreChainMax = sizeof(g_iScoreChain) / sizeof(int);

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
	void Update();

	void Set_Time(float _time) { m_fTime = _time; };
	void Set_Life(float _life) { m_iLife += _life; };
	void Set_Coin(float _coin) { m_iCoin += _coin; };
	void Set_Score(int _score);
	void Set_Score(int _score, float _fx, float _fy);

	void Render_PlayerLife(HDC, float, float);
	void Render_Timer(HDC, float, float);
	void Render_Score(HDC, float, float);
	void Render_Number(HDC, float, float, int, ENumberType);
	void Render_ScoreEffect(HDC, float, float);

	void Add_KillScore();

	void Show_ScoreEffect(int iScore);
	void Show_1UpEffect();
	void Add_Life() { ++m_iLife; };
	void Reset_KillChain();  // 선택적: 리셋용



private:
	static CUiMgr* m_pInstance;

	float m_fTime = 300.f;
	float m_dwTime = 0.f;
	int m_iLife = 100;
	int m_iCoin = 0;
	int m_iScore = 0;

	bool m_bPause = false;

	// CUiMgr 또는 게임 진행 상태를 저장하는 클래스에 추가
	struct KillChain {
		int iChainCount = 0;
		DWORD dwLastKillTime = 0;
	};
	KillChain m_KillChain;

	wstring m_strRecentEffect;  // 최근 점수 or "1UP" 표시용
	DWORD m_dwEffectTime = 0;   // 이펙트 시간 저장
	float m_fEffectX = 0.f;     // 점수 이펙트 X 위치
	float m_fEffectY = 0.f;     // 점수 이펙트 Y 위치



};

