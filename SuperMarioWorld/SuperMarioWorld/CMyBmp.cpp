#include "pch.h"
#include "CMyBmp.h"

CMyBmp::CMyBmp()
{
}

CMyBmp::~CMyBmp()
{
	Release();
}

void CMyBmp::Load_Bmp(const TCHAR* pFilePath)
{
	HDC	hDC = GetDC(g_hWnd);	// 화면 DC (직접 그리는 것)

	m_hMemDC = CreateCompatibleDC(hDC);	// 메모리 DC (오프스크린 작업용)

	ReleaseDC(g_hWnd, hDC);

	// 이미지 파일에서 비트맵 객체 생성
	m_hBitmap = (HBITMAP)LoadImage(NULL,		// 인스턴스 핸들(파일로부터 읽어올 것이기 때문에 null)
		pFilePath,	// 파일의 경로
		IMAGE_BITMAP, // 어떤 타입을 읽어올 것인가(이미지 파일)
		0, 0, // 이미지 가로, 세로 사이즈
		LR_LOADFROMFILE | LR_CREATEDIBSECTION); // LR_LOADFROMFILE : 파일로부터 이미지를 읽어들임, LR_CREATEDIBSECTION : 읽어온 파일을 dib 형태로 변환

	m_hOldBmp = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

	// ... hMemDC로 그림 작업 수행 ...

}

void CMyBmp::Release()
{
	// SelectObject : gdi 오브젝트를 선택하기 전에 기존에 가지고 있던 오브젝트를 반환
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hBitmap);
	DeleteDC(m_hMemDC);
}
