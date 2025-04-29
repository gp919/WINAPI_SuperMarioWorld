#pragma once

#include "pch.h"
#include "Define.h"
#include "CScene.h"
#include "CLogo.h"



class CSceneMgr
{
public:
	CSceneMgr();
	~CSceneMgr();

public:
	static CSceneMgr* Get_Instance()
	{
		if (!m_pInstance)
			m_pInstance = new CSceneMgr;

		return m_pInstance;
	}

	static void Destroy_Instance()
	{
		if(m_pInstance)
			Safe_Delete(m_pInstance);
	}

public:
	void Initialize();
	int Update();
	void Late_Update();
	void Render(HDC hDC);
	void Release();

public:
	void Change_Scene(SCENEID);

public:
	static CSceneMgr* m_pInstance;

private:
	CScene* m_pCurrentScene;
};