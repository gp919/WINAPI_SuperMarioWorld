#pragma once
#include "pch.h"

class CLine
{
public:
	CLine();
	CLine(LINE tLine);
	CLine(LINEPOINT& tLeft, LINEPOINT& tRight);
	~CLine();

public:
	void	Render(HDC hDC);

public:
	LINE* Get_Line() { return &m_tLine; };

private:
	LINE			m_tLine;

};

