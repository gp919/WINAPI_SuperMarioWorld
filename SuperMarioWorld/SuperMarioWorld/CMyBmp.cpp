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
	HDC	hDC = GetDC(g_hWnd);	// ȭ�� DC (���� �׸��� ��)

	m_hMemDC = CreateCompatibleDC(hDC);	// �޸� DC (������ũ�� �۾���)

	ReleaseDC(g_hWnd, hDC);

	// �̹��� ���Ͽ��� ��Ʈ�� ��ü ����
	m_hBitmap = (HBITMAP)LoadImage(NULL,		// �ν��Ͻ� �ڵ�(���Ϸκ��� �о�� ���̱� ������ null)
		pFilePath,	// ������ ���
		IMAGE_BITMAP, // � Ÿ���� �о�� ���ΰ�(�̹��� ����)
		0, 0, // �̹��� ����, ���� ������
		LR_LOADFROMFILE | LR_CREATEDIBSECTION); // LR_LOADFROMFILE : ���Ϸκ��� �̹����� �о����, LR_CREATEDIBSECTION : �о�� ������ dib ���·� ��ȯ

	m_hOldBmp = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);

	// ... hMemDC�� �׸� �۾� ���� ...

}

void CMyBmp::Release()
{
	// SelectObject : gdi ������Ʈ�� �����ϱ� ���� ������ ������ �ִ� ������Ʈ�� ��ȯ
	SelectObject(m_hMemDC, m_hOldBmp);
	DeleteObject(m_hBitmap);
	DeleteDC(m_hMemDC);
}
