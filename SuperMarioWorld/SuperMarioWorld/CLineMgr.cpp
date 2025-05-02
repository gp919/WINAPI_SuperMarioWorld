#pragma once

#include "pch.h"
#include "CLineMgr.h"

CLineMgr* CLineMgr::m_pInstance = nullptr;

CLineMgr::CLineMgr()
{
}

CLineMgr::~CLineMgr()
{
	Release();
}

bool CLineMgr::Collision_Line(INFO _info, float* pY)
{
	if (m_Linelist.empty())
		return false;
	
	float fLineY;
	
	for (auto& pLine : m_Linelist)
	{
		const LINE& tInfo = pLine->Get_Line();
		float fLx = tInfo.tLPoint.fX;
		float fLy = tInfo.tLPoint.fY;
		float fRx = tInfo.tRPoint.fX;
		float fRy = tInfo.tRPoint.fY;

		float slope = (fRy - fLy) / (fRx - fLx);
		float fx_contact = (slope >= 0.f) ? (_info.fX + _info.fCX * 0.5f) : (_info.fX - _info.fCX * 0.5f);

		

		// X ���� �� Ȯ��
		if (fx_contact >= fLx && fx_contact <= fRx)
		{
			float fLineY = slope * (fx_contact - fLx) + fLy;
			float fBottom = _info.fY + _info.fCY * 0.5f;

			wchar_t szDbg[256];
			swprintf_s(szDbg, L"fBottom = %.2f, fLineY = %.2f, diff = %.2f\n", fBottom, fLineY, fabsf(fBottom - fLineY));
			OutputDebugString(szDbg);

			if (fabsf(fBottom - fLineY) < 20.f)
			{
				*pY = fLineY;
				return true;
			}
				
		}
	}
	return false;
}
	
void CLineMgr::Add_Line(LINEPOINT _src, LINEPOINT _dst)
{
	
	if (_src.fX > _dst.fX)
	{
		swap(_src.fX, _dst.fX);
		swap(_src.fY, _dst.fY);
	}
	m_Linelist.push_back(new CLine(_src, _dst));
}

void CLineMgr::Initialize()
{
	// ���� �ʿ����� ���
	//Load_Data(); 
}

void CLineMgr::Render(HDC hDC)
{
	for (auto& pLine : m_Linelist)
		pLine->Render(hDC);
}

void CLineMgr::Release()
{
	for_each(m_Linelist.begin(), m_Linelist.end(), Safe_Delete<CLine*>);
		m_Linelist.clear();
}


/*
void CLineMgr::Load_Data()
{
	HANDLE	hFile = CreateFile(L"../Data/Line.dat",		// ���� �̸��� ������ ���
		GENERIC_READ,			// ���� ���� ���(GENERIC_WRITE : ���� ����, GENERIC_READ : �б� ����)
		NULL,					// ���� ���(������ ����� ���¿��� �ٸ� ���μ������� ����ϰ��� �� �� �㰡�� ���ΰ�, null�� ��� �������� ����)
		NULL,					// ���� �Ӽ�(null�� ��� �⺻ ���� �ش��ϴ� ���� ���� ����)
		OPEN_EXISTING,			// ���� ���(CREATE_ALWAYS : ������ ������ ����, ������ �����, OPEN_EXISTING : ������ ���� ��쿡�� �ҷ�����)
		FILE_ATTRIBUTE_NORMAL,  // ���� �Ӽ�(�Ϲ����� ����)
		NULL);					// ������ ������ �Ӽ��� ������ ���ø� ����(������� �ʱ� ������ null)

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Load Error", _T("Error"), MB_OK);
		return;
	}

	DWORD	dwByte(0);
	LINE	tLine{};

	Release();

	while (true)
	{
		ReadFile(hFile, &tLine, sizeof(LINE), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		m_Linelist.push_back(new CLine(tLine));
	}

	CloseHandle(hFile);
	MessageBox(g_hWnd, L"Load �Ϸ�", _T("Success"), MB_OK);
}
*/