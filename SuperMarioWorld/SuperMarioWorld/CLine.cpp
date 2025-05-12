#pragma once

#include "pch.h"
#include "CLine.h"

CLine::CLine()
{
}

CLine::CLine(LINE tLine) : m_tLine(tLine)
{
}

CLine::CLine(LINEPOINT& tLeft, LINEPOINT& tRight) : m_tLine(tLeft, tRight)
{
}

CLine::~CLine()
{
}

void CLine::Render(HDC hDC)
{
    float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    float fX1 = m_tLine.tLPoint.fX - fScrollX;
    float fY1 = m_tLine.tLPoint.fY - fScrollY;
    float fX2 = m_tLine.tRPoint.fX - fScrollX;
    float fY2 = m_tLine.tRPoint.fY - fScrollY;

    MoveToEx(hDC, int(fX1), int(fY1), nullptr);
    LineTo(hDC, int(fX2), int(fY2));
    
}
