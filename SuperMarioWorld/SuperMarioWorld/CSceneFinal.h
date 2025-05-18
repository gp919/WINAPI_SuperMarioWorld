#pragma once
#include "pch.h"
#include "CScene.h"
#include "CSoundMgr.h"
#include "CObjectMgr.h"
#include "CPlayer.h"

class CPlayer;

class CSceneFinal : public CScene
{
public:
	CSceneFinal();
	~CSceneFinal();
public:

	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;

	pair<float, float> Get_MapSize() { return { 2304.f, 1344.f }; };
	pair<float, float> Get_Spawn() { return { 273.f * SCALE_FACTOR,400.f * SCALE_FACTOR }; };


private:
	float		m_fScrollX PURE;
	float		m_fScrollY PURE;
	float		m_fWorldX PURE;
	float		m_fWorldY PURE;
	float		m_fGridX PURE;
	float		m_fGridY PURE;
	bool		m_bPipe = false;
	CPlayer*	pPlayer=nullptr;
};

