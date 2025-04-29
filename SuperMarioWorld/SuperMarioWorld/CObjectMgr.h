#pragma once

#include "pch.h"
#include "CObject.h"


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

	void Add_Object(EOBJECTID, CObject*);
	void Delete_Object(EOBJECTID);

private:
	static CObjectMgr* m_pInstance;
	list<CObject*> m_listObject[OBJ_END];

};

