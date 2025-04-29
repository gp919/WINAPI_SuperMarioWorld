#pragma once
#include "pch.h"
#include "CObjectMgr.h"

CObjectMgr* CObjectMgr::m_pInstance(nullptr);

CObjectMgr::CObjectMgr()
{
	//ZeroMemory(m_listObject, sizeof(m_listObject));
}

CObjectMgr::~CObjectMgr()
{
	Release();
}

void CObjectMgr::Initialize()
{
	for (auto i = 0; i < OBJ_END; i++)
	{
		for (auto& iter : m_listObject[i])
		{
			iter->Initialize();
		}
	}
}

int CObjectMgr::Update()
{
	for (auto i = 0; i < OBJ_END; i++)
	{
		for (auto& iter : m_listObject[i])
		{
			iter->Update();
		}
	}
	return NOEVENT;
}

void CObjectMgr::Late_Update()
{
	for (auto i = 0; i < OBJ_END; i++)
	{
		for (auto& iter : m_listObject[i])
		{
			iter->Late_Update();
		}
	}
}

void CObjectMgr::Render(HDC hDC)
{
	for (auto i = 0; i < OBJ_END; i++)
	{
		for (auto& iter : m_listObject[i])
		{
			iter->Render(hDC);
		}
	}
}

void CObjectMgr::Release()
{

}

void CObjectMgr::Add_Object(EOBJECTID _id, CObject* _pObj)
{
	m_listObject[_id].push_back(_pObj);
}

void CObjectMgr::Delete_Object(EOBJECTID _id)
{
	for (auto& pObj : m_listObject[_id])
	{
		Safe_Delete(pObj);
	}
	m_listObject[_id].clear();
}
