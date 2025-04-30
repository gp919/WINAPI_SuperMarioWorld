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
