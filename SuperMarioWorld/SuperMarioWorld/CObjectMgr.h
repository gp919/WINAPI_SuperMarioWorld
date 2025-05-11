#pragma once

#include "pch.h"
#include "Define.h"
#include "CObject.h"
#include "CCollisionMgr.h"
#include "CLineMgr.h"


class CObjectMgr : public CObject
{
public:
	CObjectMgr();
	~CObjectMgr();

public:
	void Initialize();
	int Update();
	void Late_Update();
	void Render(HDC);
	void Release();

public:
	static CObjectMgr* Get_Instance()
	{
		if (!m_pInstance)
			m_pInstance = new CObjectMgr;

		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if (m_pInstance)
			Safe_Delete(m_pInstance);
	}

	list<CObject*>& Get_ObjectList(EOBJECTID _id) { return m_listObject[_id]; };
	void Add_Object(EOBJECTID, CObject*);
	void Sub_Object(float, float);
	void Delete_Object(EOBJECTID);

private:
	static CObjectMgr* m_pInstance;
	list<CObject*> m_listObject[OBJ_END];

};

