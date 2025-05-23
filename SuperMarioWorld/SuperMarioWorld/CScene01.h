#pragma once
#include "pch.h"
#include "CScene.h"
#include "CMyBmp.h"
#include "CBmpMgr.h"
#include "CObjectMgr.h"
#include "CTile.h"
#include "CPlayer.h"
#include "CUiMgr.h"
#include "CObject.h"
#include "CPipe.h"

class CPlayer;
class CScene01 : public CScene
{
public:
	CScene01();
	virtual ~CScene01();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;
	pair<float, float> Get_MapSize() { return { 5120.f, 1120.f }; };
	pair<float, float> Get_Spawn()
	{
		return { 64.f * SCALE_FACTOR, 352.f * SCALE_FACTOR };
	};

	void Set_Pipe(bool _pipe) { m_bPipe = _pipe; };

	bool Is_Player_Enterable(CObject* pPlayer);

private:
	float		m_fScrollX PURE;
	float		m_fScrollY PURE;
	float		m_fWorldX PURE;
	float		m_fWorldY PURE;
	float		m_fGridX PURE;
	float		m_fGridY PURE;

	bool		m_bPipe = false;
	bool m_bPipeScrolling = false;
	DWORD m_dwPipeScrollStart = 0;
	float m_fTargetScrollY = 0.f;
	float m_fRestoreScrollY = 0.f;
	CPlayer* pPlayer;

	


};

