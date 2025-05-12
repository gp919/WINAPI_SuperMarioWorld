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
    float maxX = m_fStageCX * SCALE_FACTOR - WINCX;
    float maxY = (432.f * SCALE_FACTOR) - WINCY; // 여기가 너무 작으면 바닥이 안 보임

    m_fScrollX = clamp(m_fScrollX, 0.f, maxX);
    m_fScrollY = clamp(m_fScrollY, 0.f, maxY); // ← 이 부분을 이미지 높이에 맞춰 조정

}

void CScrollMgr::Follow_Target(float fTargetX, float fTargetY, float fSpeed)
{
    // 화면 내 플레이어 고정 위치 (예: 40% 지점)
    const float fFocusX = WINCX * 0.4f;

    float fOldScrollX = m_fScrollX;

    m_fScrollX = fTargetX - fFocusX;

    Scroll_Lock();

}

void CScrollMgr::Screen_To_World(float _screenX, float _screenY, float* _worldX, float* _worldY)
{
    // 상대 -> 절대 좌표
    *_worldX = (_screenX + m_fScrollX);
    *_worldY = (_screenY + m_fScrollY);
}

void CScrollMgr::World_To_Screen(float _worldX, float _worldY, float* _screenX, float* _screenY)
{

    // 스크롤 위치를 고려하여 그리드 좌표 계산
    *_screenX = (_worldX - m_fScrollX);
    *_screenY = (_worldY - m_fScrollY);
}

void CScrollMgr::World_To_Grid(float _worldX, float _worldY, float* _gridX, float* _gridY)
{
    // 월드 좌표를 그리드 인덱스로 변환 (정수 인덱스)
    int gridIndexX = static_cast<int>(_worldX / 48.0f);
    int gridIndexY = static_cast<int>(_worldY / 48.0f);

    // 그리드의 중심좌표 계산
    *_gridX = (gridIndexX * 48.0f) + 24.0f;
    *_gridY = (gridIndexY * 48.0f) + 24.0f;
}
