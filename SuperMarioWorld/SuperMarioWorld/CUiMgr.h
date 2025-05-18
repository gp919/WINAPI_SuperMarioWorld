#pragma once

#include "pch.h"
#include "CBmpMgr.h"
#include "CObjectMgr.h"
#include "CSoundMgr.h"

// 1��° ~ 7��°������ ����, 8��°�� 1UP
const int g_iScoreChain[] = {
	100, 200, 400, 800,
	1000, 2000, 4000,
	10000 // 8��°
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
	void Reset_KillChain();  // ������: ���¿�



private:
	static CUiMgr* m_pInstance;

	float m_fTime = 300.f;
	float m_dwTime = 0.f;
	int m_iLife = 100;
	int m_iCoin = 0;
	int m_iScore = 0;

	bool m_bPause = false;

	// CUiMgr �Ǵ� ���� ���� ���¸� �����ϴ� Ŭ������ �߰�
	struct KillChain {
		int iChainCount = 0;
		DWORD dwLastKillTime = 0;
	};
	KillChain m_KillChain;

	wstring m_strRecentEffect;  // �ֱ� ���� or "1UP" ǥ�ÿ�
	DWORD m_dwEffectTime = 0;   // ����Ʈ �ð� ����
	float m_fEffectX = 0.f;     // ���� ����Ʈ X ��ġ
	float m_fEffectY = 0.f;     // ���� ����Ʈ Y ��ġ



};

