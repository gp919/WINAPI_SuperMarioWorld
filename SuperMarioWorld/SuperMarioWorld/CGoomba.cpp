#include "pch.h"
#include "CGoomba.h"

CGoomba::CGoomba(float _fx, float _fy)
{
    m_tInfo.fX = _fx;
    m_tInfo.fY = _fy;
}

CGoomba::~CGoomba()
{
    Release();
}

void CGoomba::Initialize()
{
    m_tInfo.fCX = TILECX * SCALE_FACTOR;
    m_tInfo.fCY = TILECY * SCALE_FACTOR;
}

int CGoomba::Update()
{
    CObject::Move_Frame();
    CObject::Update_Rect();
    return NOEVENT;
}

void CGoomba::Late_Update()
{
}

void CGoomba::Render(HDC hDC)
{
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Goomba");

	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	GdiTransparentBlt(hDC,
		(int)(m_tRect.left - fScrollX),
		(int)(m_tRect.top - fScrollY),
		(int)(m_tInfo.fCX),  // ← 몬스터 너비
		(int)(m_tInfo.fCY),  // ← 몬스터 높이
		hMemDC,
		m_tFrame.iStart * TILECX,
		m_tFrame.iMotion * TILECY,
		TILECX,
		TILECY,
		RGB(0, 255, 0));
}

void CGoomba::Release()
{
}

void CGoomba::On_Stomped()
{
    m_eState = MONSTER_STOMPED;
    m_bDead = true;
}

void CGoomba::Update_AI()
{
}
