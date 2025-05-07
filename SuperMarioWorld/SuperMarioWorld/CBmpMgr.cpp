#include "pch.h"
#include "CBmpMgr.h"

CBmpMgr* CBmpMgr::m_pInstance = nullptr;

CBmpMgr::CBmpMgr()
{
}

CBmpMgr::~CBmpMgr()
{
	Release();
}

HDC CBmpMgr::Find_Image(const TCHAR* pImgKey)
{
	auto iter = find_if(m_mapBit.begin(), m_mapBit.end(), tagFinder(pImgKey));

	if (iter == m_mapBit.end())
		return nullptr;

	return iter->second->Get_MemDC();
}

void CBmpMgr::Insert_Bmp(const TCHAR* pFilePath, const TCHAR* pImgKey)
{
	auto iter = find_if(m_mapBit.begin(), m_mapBit.end(), tagFinder(pImgKey));

	if (iter == m_mapBit.end())
	{
		CMyBmp* pMyBmp = new CMyBmp;
		pMyBmp->Load_Bmp(pFilePath);

		//// 확인용 비트맵 사이즈 출력
		//BITMAP bmpInfo;
		//HBITMAP hBmp = (HBITMAP)GetCurrentObject(pMyBmp->Get_MemDC(), OBJ_BITMAP);
		//GetObject(hBmp, sizeof(BITMAP), &bmpInfo);

		//wchar_t szBuf[128];
		//swprintf_s(szBuf, 128, L"%s: %d x %d", pImgKey, bmpInfo.bmWidth, bmpInfo.bmHeight);
		//MessageBox(nullptr, szBuf, L"비트맵 로딩 확인", MB_OK);

		m_mapBit.insert({ pImgKey, pMyBmp });
	}
}

void CBmpMgr::Release()
{
	for_each(m_mapBit.begin(), m_mapBit.end(), tagDeleteMap());
	m_mapBit.clear();

}

void CBmpMgr::PreWarm(HDC hDC)
{
	for (auto& pair : m_mapBit)
	{
		BitBlt(hDC, -100, -100, 1, 1, pair.second->Get_MemDC(), 0, 0, SRCCOPY);
	}
}
