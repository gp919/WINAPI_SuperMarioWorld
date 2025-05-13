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

void CObject::On_Collision(EOBJECTID _id)
{
}

void CObject::Update_Rect()
{
	m_tRect.left = LONG(m_tInfo.fX - (m_tInfo.fCX * 0.5f));
	m_tRect.top = LONG(m_tInfo.fY - (m_tInfo.fCY * 0.5f));
	m_tRect.right = LONG(m_tInfo.fX + (m_tInfo.fCX * 0.5f));
	m_tRect.bottom = LONG(m_tInfo.fY + (m_tInfo.fCY * 0.5f));

}

void CObject::Move_Frame()
{
	if (m_tFrame.iEnd==0)
		return;

	if (m_tFrame.dwSpeed + m_tFrame.dwTime < GetTickCount())
	{
		++m_tFrame.iStart;
		m_tFrame.dwTime = GetTickCount();

		if (m_tFrame.iStart > m_tFrame.iEnd)
			m_tFrame.iStart = 0;
	}

}

void CObject::Allign_Bottom(float _gridx, float _gridy)
{
	// 그리드 좌하단 좌표 계산
	float gridLeftX = _gridx - TILECX * SCALE_FACTOR * 0.5f;
	float gridBottomY = _gridy + TILECY * SCALE_FACTOR * 0.5f;
	// 하단 중심 기준으로 오브젝트 위치 보정
	m_tInfo.fX = gridLeftX + m_tInfo.fCX * 0.5f;
	m_tInfo.fY = gridBottomY - m_tInfo.fCY * 0.5f;
}
