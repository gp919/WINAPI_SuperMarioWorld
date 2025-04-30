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
	// MoveToEx(hDC, (int)m_tInfo.tLPoint.fX + iScrollX, (int)m_tInfo.tLPoint.fY, nullptr);
	// LineTo(hDC, (int)m_tInfo.tRPoint.fX + iScrollX, (int)m_tInfo.tRPoint.fY);
}
