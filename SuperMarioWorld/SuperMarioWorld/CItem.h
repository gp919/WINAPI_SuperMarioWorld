#pragma once

#include "pch.h"
#include "CObject.h"
#include "CUiMgr.h"
#include "CScrollMgr.h"
#include "CUiMgr.h"


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

	void Catch_Item(CObject*);

private:
	bool m_bMove;
};

