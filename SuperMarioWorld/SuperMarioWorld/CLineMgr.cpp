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

			// 아래쪽에서 올라오는 경우도 포함해서 더 관대하게 처리
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
			if ((fx > _info.fX - _info.fCX * 0.5f) && (fx < _info.fX + _info.fCX * 0.5f)) // x 충돌
			{
				// y 구간 포함 여부
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
	// 추후 맵에디터 고민
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
	HANDLE	hFile = CreateFile(L"../Data/Line.dat",		// 파일 이름을 포함한 경로
		GENERIC_READ,			// 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
		NULL,					// 공유 방식(파일이 개방된 상태에서 다른 프로세스에서 사용하고자 할 때 허가할 것인가, null인 경우 공유하지 않음)
		NULL,					// 보안 속성(null인 경우 기본 값에 해당하는 보안 상태 설정)
		OPEN_EXISTING,			// 생성 방식(CREATE_ALWAYS : 파일이 없으면 생성, 있으면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 불러오기)
		FILE_ATTRIBUTE_NORMAL,  // 파일 속성(일반적인 파일)
		NULL);					// 생성될 파일의 속성을 제공할 템플릿 파일(사용하지 않기 때문에 null)

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
	MessageBox(g_hWnd, L"Load 완료", _T("Success"), MB_OK);
}
*/