#pragma once
#include "pch.h"
#include "CObject.h"

CObject::CObject() : m_bJump(false), m_fJumpTime(0.f)
{
	ZeroMemory(&m_tInfo, sizeof(INFO));
	ZeroMemory(&m_tRect, sizeof(RECT));
	ZeroMemory(&m_tFrame, sizeof(FRAME));
}

CObject::~CObject()
{
}

void CObject::Update_Rect()
{
	m_tRect.left = LONG(m_tInfo.fX - (m_tInfo.fCX * 0.5f));
	m_tRect.top = LONG(m_tInfo.fY - (m_tInfo.fCY * 0.5f));
	m_tRect.right = LONG(m_tInfo.fX + (m_tInfo.fCX * 0.5f));
	m_tRect.bottom = LONG(m_tInfo.fY + (m_tInfo.fCY * 0.5f));
}

// 중심좌표 기준 히트박스 결정하는 함수
// 작은마리오 40*30
void CObject::Update_HitBox()
{
	float fWidth = 48.f;
	float fHeight = 72.f;
	m_tHitBox.left = LONG(m_tInfo.fX - (fWidth * 0.5f));
	m_tHitBox.top = LONG(m_tInfo.fY - (fHeight * 0.5f));
	m_tHitBox.right = LONG(m_tInfo.fX + (fWidth * 0.5f));
	m_tHitBox.bottom = LONG(m_tInfo.fY + (fHeight * 0.5f));
}

void CObject::Move_Frame()
{
	if (m_tFrame.dwSpeed + m_tFrame.dwTime < GetTickCount())
	{
		++m_tFrame.iStart;
		m_tFrame.dwTime = GetTickCount();

		if (m_tFrame.iStart > m_tFrame.iEnd)
			m_tFrame.iStart = 0;
	}

}
