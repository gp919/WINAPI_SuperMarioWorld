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

// 키누르는 상태
bool CKeyMgr::Key_Pressing(int iKey)
{
	return m_bCurState[iKey];
}

// 이전 프레임에 누른 적이 없고
// 현재 프레임에 누른 상태
bool CKeyMgr::Key_Down(int iKey)
{
	return !m_bPrevState[iKey] && m_bCurState[iKey];

}

// 이전 프레임에 누른 적이 있고
// 현재 프레임에 누르지 않은 상태
bool CKeyMgr::Key_Up(int iKey)
{
	return m_bPrevState[iKey] && !m_bCurState[iKey];
}

void CKeyMgr::Key_Update()
{
    // 이전 프레임의 키 상태를 현재 프레임의 상태로 갱신
    memcpy(m_bPrevState, m_bCurState, sizeof(m_bCurState));

    // 현재 프레임의 키 상태 갱신
    for (int i = 0; i < VK_MAX; ++i)
    {
        m_bCurState[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }

}
