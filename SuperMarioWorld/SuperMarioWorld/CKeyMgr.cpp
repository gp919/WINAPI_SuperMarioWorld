#pragma once

#include "pch.h"
#include "CKeyMgr.h"

CKeyMgr* CKeyMgr::m_pInstance = nullptr;

CKeyMgr::CKeyMgr()
{
	ZeroMemory(m_bCurState, sizeof(m_bCurState));
	ZeroMemory(m_bPrevState, sizeof(m_bPrevState));
}

CKeyMgr::~CKeyMgr()
{
}

// Ű������ ����
bool CKeyMgr::Key_Pressing(int iKey)
{
	return m_bCurState[iKey];
}

// ���� �����ӿ� ���� ���� ����
// ���� �����ӿ� ���� ����
bool CKeyMgr::Key_Down(int iKey)
{
	return !m_bPrevState[iKey] && m_bCurState[iKey];

}

// ���� �����ӿ� ���� ���� �ְ�
// ���� �����ӿ� ������ ���� ����
bool CKeyMgr::Key_Up(int iKey)
{
	return m_bPrevState[iKey] && !m_bCurState[iKey];
}

void CKeyMgr::Key_Update()
{
    // ���� �������� Ű ���¸� ���� �������� ���·� ����
    memcpy(m_bPrevState, m_bCurState, sizeof(m_bCurState));

    // ���� �������� Ű ���� ����
    for (int i = 0; i < VK_MAX; ++i)
    {
        m_bCurState[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }

}
