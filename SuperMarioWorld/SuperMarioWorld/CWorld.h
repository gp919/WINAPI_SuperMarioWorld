#pragma once
#include "CScene.h"
#include "CScrollMgr.h"
#include "CSoundMgr.h"
#include "CKeyMgr.h"

class CWorld : public CScene
{
public:
	CWorld();
	~CWorld();

public:

	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;
	pair<float, float> Get_MapSize() { return { 5120.f, 432.f }; };

private:
	int			m_iWorld = 0;
	float		m_fScrollX PURE;
	float		m_fScrollY PURE;
	float		m_fWorldX PURE;
	float		m_fWorldY PURE;
	float		m_fGridX PURE;
	float		m_fGridY PURE;

};

