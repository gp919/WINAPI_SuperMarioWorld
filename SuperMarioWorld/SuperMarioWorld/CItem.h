#pragma once

#include "pch.h"
#include "CObject.h"
#include "CUiMgr.h"
#include "CScrollMgr.h"
#include "CUiMgr.h"
#include "CSoundMgr.h"
#include "CSceneMgr.h"


class CItem : public CObject
{
public:
	CItem(float, float, ITEMID);
	CItem(INFO);
	virtual ~CItem();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;

	void On_Collision(EOBJECTID);
	void Line_Collision();
	void Update_Gravitiy();

	void Catch_Item(CObject*);
	void Set_Pop(bool _pop)
	{
		m_bPopUp = _pop;
		m_iPopFrame = 0;
		m_fStartY = m_tInfo.fY;
	}

private:
	bool m_bMove;
	bool m_bPopUp = false;     // ���� ��
	int  m_iPopFrame = 0;     // �� ������ �ö�����
	float m_fStartY;          // ���� ��ġ ���

	bool m_bMoveReady = false;
};

