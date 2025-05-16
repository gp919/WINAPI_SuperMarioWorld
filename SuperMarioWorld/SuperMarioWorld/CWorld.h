#pragma once

#include "CScene.h"
#include "CScrollMgr.h"
#include "CSoundMgr.h"
#include "CKeyMgr.h"
#include "CBmpMgr.h"
#include "CWorldPlayer.h"
#include "CSceneMgr.h"

class CWorld : public CScene
{
public:
	CWorld();
	~CWorld();

	void Initialize() override;
	int Update() override;
	void Late_Update() override;
	void Render(HDC) override;
	void Release() override;
	pair<float, float> Get_MapSize() { return { 5120.f, 432.f }; }
	void Set_Background(const wstring& str);

	void Set_World(float _fx, float _fy)
	{
		m_fWorldX = _fx;
		m_fWorldY = _fy;
	}
	float Get_WorldX() { return m_fWorldX; };
	float Get_WorldY() { return m_fWorldY; };

private:
	int			m_iWorld = 0;
	float		m_fScrollX;
	float		m_fScrollY;
	float		m_fGridX;
	float		m_fGridY;
	CWorldPlayer m_tPlayer;  
	wstring m_strCurBackground = L"World1";


	// ���� ������ǥ�� ���
	float		m_fWorldX;
	float		m_fWorldY;
	

	// ���̵�ƿ� ������
	bool        m_bFadeOut = false;
	DWORD       m_dwFadeStartTime = 0;
	DWORD       m_dwFadeDuration = 500; // 0.5�� ���� ���̵�ƿ�
	int         m_iFadeAlpha = 255;     // ���İ� (255: ������, 0: ����)
};
