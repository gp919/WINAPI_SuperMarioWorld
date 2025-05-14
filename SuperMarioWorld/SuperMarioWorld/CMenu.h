#pragma once
#include "CScene.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"
#include "CKeyMgr.h"

class CMenu : public CScene
{
public:
	CMenu();
	virtual ~CMenu();
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

private:
	int m_iSelected;              // 현재 선택된 메뉴 인덱스 (0~3)
	int m_iIndex;

};

