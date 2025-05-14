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

	bool		m_bClear = false;

	// 사운드 재생 관련 변수
protected:
	vector<wstring> m_vecBGM;
	int m_iCurBGMIndex = 0;
	bool m_bLoopBGM = false;

	bool m_bLogo = true;
	DWORD m_dwLogoStartTime = 0;
	const DWORD m_dwLogoDuration = 1000;

	// 페이드
	bool m_bFadeIn = false; // 페이드인 시작 여부
	DWORD m_dwFadeInStartTime;
	const DWORD m_dwFadeInDuration = 500; // 0.5초
	bool m_bFadeOut = false;        // 페이드아웃 시작 여부
	DWORD m_dwFadeStartTime = 0;  // 페이드아웃 시작 시간
	const DWORD m_dwFadeDuration = 500; // 페이드아웃 지속 시간 (.5초)
};