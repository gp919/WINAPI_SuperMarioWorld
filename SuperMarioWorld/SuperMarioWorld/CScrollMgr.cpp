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
    float maxY = (432.f * SCALE_FACTOR) - WINCY; // ���Ⱑ �ʹ� ������ �ٴ��� �� ����

    m_fScrollX = clamp(m_fScrollX, 0.f, maxX);
    m_fScrollY = clamp(m_fScrollY, 0.f, maxY); // �� �� �κ��� �̹��� ���̿� ���� ����

}

void CScrollMgr::Follow_Target(float fTargetX, float fTargetY, float fSpeed)
{
    // ȭ�� �� �÷��̾� ���� ��ġ (��: 40% ����)
    const float fFocusX = WINCX * 0.4f;

    float fOldScrollX = m_fScrollX;

    m_fScrollX = fTargetX - fFocusX;

    Scroll_Lock();

}

void CScrollMgr::Screen_To_World(float _screenX, float _screenY, float* _worldX, float* _worldY)
{
    // ��� -> ���� ��ǥ
    *_worldX = (_screenX + m_fScrollX);
    *_worldY = (_screenY + m_fScrollY);
}

void CScrollMgr::World_To_Screen(float _worldX, float _worldY, float* _screenX, float* _screenY)
{

    // ��ũ�� ��ġ�� ����Ͽ� �׸��� ��ǥ ���
    *_screenX = (_worldX - m_fScrollX);
    *_screenY = (_worldY - m_fScrollY);
}

void CScrollMgr::World_To_Grid(float _worldX, float _worldY, float* _gridX, float* _gridY)
{
    // ���� ��ǥ�� �׸��� �ε����� ��ȯ (���� �ε���)
    int gridIndexX = static_cast<int>(_worldX / 48.0f);
    int gridIndexY = static_cast<int>(_worldY / 48.0f);

    // �׸����� �߽���ǥ ���
    *_gridX = (gridIndexX * 48.0f) + 24.0f;
    *_gridY = (gridIndexY * 48.0f) + 24.0f;
}
