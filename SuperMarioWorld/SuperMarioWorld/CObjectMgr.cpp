#pragma once
#include "pch.h"
#include "CObjectMgr.h"

CObjectMgr* CObjectMgr::m_pInstance(nullptr);

CObjectMgr::CObjectMgr()
{
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
		for (auto iter = m_listObject[i].begin(); iter != m_listObject[i].end(); )
		{
			int iResult = (*iter)->Update();
			if (iResult == DEAD)
			{
				Safe_Delete<CObject*>(*iter);
				iter = m_listObject[i].erase(iter);
			}
			else
			{
				++iter;
			}
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
	for (auto& obj : m_listObject[OBJ_TILE])
		obj->Render(hDC);

	for (auto& obj : m_listObject[OBJ_MONSTER])
		obj->Render(hDC);

	for (auto& obj : m_listObject[OBJ_PLAYER])
		obj->Render(hDC);
}

void CObjectMgr::Release()
{
	for (UINT i = 0; i < OBJ_END; ++i)
	{
		for_each(m_listObject[i].begin(), m_listObject[i].end(), Safe_Delete<CObject*>);
			m_listObject[i].clear();
	}
}

void CObjectMgr::Add_Object(EOBJECTID _id, CObject* _pObj)
{
	m_listObject[_id].push_back(_pObj);
	m_listObject[_id].back()->Set_ID(_id);
}


void CObjectMgr::Delete_Object(EOBJECTID _id)
{
	for (auto& pObj : m_listObject[_id])
	{
		Safe_Delete(pObj);
	}
	m_listObject[_id].clear();
}
