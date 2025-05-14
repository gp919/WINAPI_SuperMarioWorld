#pragma once
#include "CScene.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CKeyMgr.h"
#include "CSoundMgr.h"

class CLogo : public CScene
{
public:
	CLogo();
	virtual ~CLogo();
public:

	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;
	pair<float, float> Get_MapSize() { return { 5120.f, 432.f }; };

private:
	float		m_fScrollX PURE;
	float		m_fScrollY PURE;
	float		m_fWorldX PURE;
	float		m_fWorldY PURE;
	float		m_fGridX PURE;
	float		m_fGridY PURE;

};

