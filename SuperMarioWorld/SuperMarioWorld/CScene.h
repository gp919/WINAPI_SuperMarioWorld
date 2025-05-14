#pragma once

#include "pch.h"



class CScene
{
public:
	CScene();
	virtual ~CScene();

public:
	virtual void Initialize()PURE;
	virtual int Update()PURE;
	virtual void Late_Update()PURE;
	virtual void Render(HDC)PURE;
	virtual void Release()PURE;
	
	virtual pair<float, float> Get_MapSize()PURE;



private:
	float		m_fScrollX PURE;
	float		m_fScrollY PURE;
	float		m_fWorldX PURE;
	float		m_fWorldY PURE;
	float		m_fGridX PURE;
	float		m_fGridY PURE;

	// 사운드 재생 관련 변수
protected:
	vector<wstring> m_vecBGM;
	int m_iCurBGMIndex = 0;
	bool m_bLoopBGM = true;
};