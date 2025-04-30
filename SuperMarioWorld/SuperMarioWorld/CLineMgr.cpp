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

bool CLineMgr::Collision_Line(float _fX, float* pY)
{

	if (m_Linelist.empty())
		return false;

	CLine* pTargetLine = nullptr;

	for (auto& pLine : m_Linelist)
	{
		if (_fX >= pLine->Get_Line().tLPoint.fX &&
			_fX <= pLine->Get_Line().tRPoint.fX)
		{
			pTargetLine = pLine;
		}
	}

	if (!pTargetLine)
		return false;



	float x1 = pTargetLine->Get_Line().tLPoint.fX;
	float y1 = pTargetLine->Get_Line().tLPoint.fY;

	float x2 = pTargetLine->Get_Line().tRPoint.fX;
	float y2 = pTargetLine->Get_Line().tRPoint.fY;

	*pY = ((y2 - y1) / (x2 - x1)) * (_fX - x1) + y1;

	return true;
}

void CLineMgr::Initialize()
{
	/*
	LINEPOINT	tPoint[4] =
	{
		{ 100.f, 450.f},
		{ 300.f, 450.f},
		{ 500.f, 250.f},
		{ 700.f, 250.f},
	};

	m_Linelist.push_back(new CLine(tPoint[0], tPoint[1]));
	m_Linelist.push_back(new CLine(tPoint[1], tPoint[2]));
	m_Linelist.push_back(new CLine(tPoint[2], tPoint[3]));
	*/

	Load_Data();
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
