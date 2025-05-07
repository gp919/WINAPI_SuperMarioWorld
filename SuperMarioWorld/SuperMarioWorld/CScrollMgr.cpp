#include "pch.h"
#include "CScrollMgr.h"

CScrollMgr* CScrollMgr::m_pInstance = nullptr;

CScrollMgr::CScrollMgr()
	: m_fScrollX(0.f), m_fScrollY(0.f), m_fStageCX(5120.f), m_fStageCY(432.f)
{
}

CScrollMgr::~CScrollMgr()
{
}

void CScrollMgr::Scroll_Lock()
{
    float fStageCX = m_fStageCX * SCALE_FACTOR - WINCX;
    float fStageCY = m_fStageCY * SCALE_FACTOR - WINCY;

    m_fScrollX = clamp(m_fScrollX, 0.f, fStageCX);
    m_fScrollY = clamp(m_fScrollY, 0.f, fStageCY);

}

void CScrollMgr::Follow_Target(float fTargetX, float fTargetY, float fSpeed)
{
    // 화면 내 플레이어 고정 위치 (예: 40% 지점)
    const float fFocusX = WINCX * 0.4f;

    float fOldScrollX = m_fScrollX;

    m_fScrollX = fTargetX - fFocusX;

    Scroll_Lock();

}
