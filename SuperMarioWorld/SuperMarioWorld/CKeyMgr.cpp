#pragma once

#include "pch.h"
#include "CKeyMgr.h"

CKeyMgr* CKeyMgr::m_pInstance = nullptr;

CKeyMgr::CKeyMgr()
{
	ZeroMemory(m_bKeyState, sizeof(m_bKeyState));
}

CKeyMgr::~CKeyMgr()
{
}

// Ű������ ����
bool CKeyMgr::Key_Pressing(int iKey)
{
	if (GetAsyncKeyState(iKey) & 0x8000)
		return true;

	return false;
}

// ���� �����ӿ� ���� ���� ����
// ���� �����ӿ� ���� ����
bool CKeyMgr::Key_Down(int iKey)
{
	if ((!m_bKeyState[iKey]) && (GetAsyncKeyState(iKey) & 0x8000))
	{
		m_bKeyState[iKey] = !m_bKeyState[iKey];
		return true;
	}
	return false;
}

// 
bool CKeyMgr::Key_Up(int iKey)
{
	if ((m_bKeyState[iKey]) && !(GetAsyncKeyState(iKey) & 0x8000))
	{
		m_bKeyState[iKey] = !m_bKeyState[iKey];
		return true;
	}

	return false;
}

void CKeyMgr::Key_Update()
{
	for (int i = 0; i < VK_MAX; ++i)
	{
		m_bKeyState[i] = (GetAsyncKeyState(i) & 0x8000);
	}

}
