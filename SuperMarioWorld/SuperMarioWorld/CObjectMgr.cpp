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

	for (auto& obj : m_listObject[OBJ_ITEM])
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

void CObjectMgr::AfterInit(CObject* _obj)
{
	_obj->Initialize();
}

void CObjectMgr::Sub_Object(float _fx, float _fy)
{
	
	for (auto iter = m_listObject[OBJ_MONSTER].begin(); iter != m_listObject[OBJ_MONSTER].end(); )
	{
		INFO* pInfo = (*iter)->Get_Info();
		// 지정된 좌표 근처에 있는 몬스터 찾기
		if (abs(pInfo->fX - _fx) < 24.0f && abs(pInfo->fY - _fy) < 24.0f)
		{
			Safe_Delete<CObject*>(*iter);
			iter = m_listObject[OBJ_MONSTER].erase(iter);
		}
		else
		{
			++iter;
		}
	}

	for (auto iter = m_listObject[OBJ_TILE].begin(); iter != m_listObject[OBJ_TILE].end(); )
	{
		INFO* pInfo = (*iter)->Get_Info();
		// 지정된 좌표 근처에 있는 타일 찾기
		if (abs(pInfo->fX - _fx) < 24.0f && abs(pInfo->fY - _fy) < 24.0f)
		{
			Safe_Delete<CObject*>(*iter);
			iter = m_listObject[OBJ_TILE].erase(iter);
		}
		else
		{
			++iter;
		}
	}

	for (auto iter = m_listObject[OBJ_ITEM].begin(); iter != m_listObject[OBJ_ITEM].end(); )
	{
		INFO* pInfo = (*iter)->Get_Info();
		// 지정된 좌표 근처에 있는 오브젝트 찾기
		if (abs(pInfo->fX - _fx) < 24.0f && abs(pInfo->fY - _fy) < 24.0f)
		{
			Safe_Delete<CObject*>(*iter);
			iter = m_listObject[OBJ_ITEM].erase(iter);
		}
		else
		{
			++iter;
		}
	}

}

void CObjectMgr::Delete_Object(EOBJECTID _id)
{
	for (auto& pObj : m_listObject[_id])
	{
		Safe_Delete(pObj);
	}
	m_listObject[_id].clear();
}
