#pragma once

#include "pch.h"
#include "CScene.h"
#include "CCollisionMgr.h"
#include "CTutorial.h"
#include "CScene01.h"
#include "CEditor.h"
#include "CMenu.h"
#include "CLogo.h"
#include "CWorld.h"



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
	void Key_Input();
	void Load_Data();
	void Set_Clear(bool _clear) { m_bClear = _clear; };
	bool Get_Clear() { return m_bClear; };

	CScene* Get_CurrentScene()
	{
		if (m_pCurrentScene)
			return m_pCurrentScene;
	}

public:
	void Change_Scene(SCENEID);

public:
	static CSceneMgr* m_pInstance;

private:
	bool m_bClear = false;
	bool m_bMusic = false;
	CScene* m_pCurrentScene;
	SCENEID m_eCurScene = SC_END;
};