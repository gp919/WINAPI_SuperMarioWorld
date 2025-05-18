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

	float fBestY = 99999.f;
	bool bHit = false;

	for (auto& pLine : m_Linelist)
	{
		const LINE* tInfo = pLine->Get_Line();
		float fLx = tInfo->tLPoint.fX;
		float fLy = tInfo->tLPoint.fY;
		float fRx = tInfo->tRPoint.fX;
		float fRy = tInfo->tRPoint.fY;

		if (fLx == fRx) continue;

		float slope = (fRy - fLy) / (fRx - fLx);
		float fx_contact = (slope >= 0.f) ? (_info.fX + _info.fCX * 0.5f) : (_info.fX - _info.fCX * 0.5f);

		if (fx_contact >= fLx && fx_contact <= fRx)
		{
			float fLineY = slope * (fx_contact - fLx) + fLy;
			float fBottom = _info.fY + _info.fCY * 0.5f;

			// �Ʒ��ʿ��� �ö���� ��쵵 �����ؼ� �� �����ϰ� ó��
			if ((fBottom >= fLineY && fBottom - fLineY <= 30.f))
			{
				if (fLineY < fBestY)
				{
					fBestY = fLineY;
					bHit = true;
				}
			}
		}
	}

	if (bHit)
	{
		*pY = fBestY;
		return true;
	}

	return false;
}

bool CLineMgr::Collision_Vertical(INFO _info, float* pX)
{
	if (m_Linelist.empty())
		return false;

	for (auto& pLine : m_Linelist)
	{
		const LINE* tInfo = pLine->Get_Line();
		float fLx = tInfo->tLPoint.fX;
		float fLy = tInfo->tLPoint.fY;
		float fRx = tInfo->tRPoint.fX;
		float fRy = tInfo->tRPoint.fY;
		if (fLx == fRx)
		{
			float fx = fLx;
			if ((fx > _info.fX - _info.fCX * 0.5f) && (fx < _info.fX + _info.fCX * 0.5f)) // x �浹
			{
				// y ���� ���� ����
				float fTopY = min(fLy, fRy);	
				float fBottomY = max(fLy, fRy);

				if ((_info.fY >= fTopY) && (_info.fY <= fBottomY))
				{
					*pX = fx;
					return true;
				}
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

void CLineMgr::Sub_Line(float _fx, float _fy)
{
	for (auto iter = m_Linelist.begin(); iter != m_Linelist.end();)
	{
		LINE* pLine = (*iter)->Get_Line();

		if (((pLine->tLPoint.fX >= _fx - 24.f && pLine->tLPoint.fX <= _fx + 24.f) &&
			(pLine->tLPoint.fY >= _fy - 24.f && pLine->tLPoint.fY <= _fy + 24.f)) ||
			((pLine->tRPoint.fX >= _fx - 24.f && pLine->tRPoint.fX <= _fx + 24.f) &&
				(pLine->tRPoint.fY >= _fy - 24.f && pLine->tRPoint.fY <= _fy + 24.f)))
		{
			Safe_Delete<CLine*>(*iter);
			iter = m_Linelist.erase(iter);
		}
		else
		{
			++iter;
		}
	}
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