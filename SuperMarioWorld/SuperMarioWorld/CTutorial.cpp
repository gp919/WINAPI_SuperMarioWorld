#include "pch.h"
#include "CTutorial.h"

CTutorial::CTutorial()
{
}

CTutorial::~CTutorial()
{
}

void CTutorial::Initialize()
{
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/Tutorial/Yoshi.bmp", L"Tutorial");
	CBmpMgr::Get_Instance()->Insert_Bmp(L"../Resource/BackGround01.bmp", L"BackGround01");
}

int CTutorial::Update()
{
	return 0;
}

void CTutorial::Late_Update()
{
}

void CTutorial::Render(HDC hDC)
{
	HDC hMemDC_back = CBmpMgr::Get_Instance()->Find_Image(L"BackGround01");
	GdiTransparentBlt(
		hDC,
		0, 0,
		WINCX, WINCY,                                  // ��� ũ�� (3��)
		hMemDC_back,
		0,                   // �� �ε��� �� ������ �ʺ�
		0,                  // �� �ε��� �� ������ ����
		WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,                                 // �ڸ� ���� ũ��
		RGB(0, 255, 0));

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Tutorial");
	GdiTransparentBlt(
		hDC,
		0, 0,
		WINCX, WINCY,                                  // ��� ũ�� (3��)
		hMemDC,
		0,                   // �� �ε��� �� ������ �ʺ�
		0,                  // �� �ε��� �� ������ ����
		WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,                                 // �ڸ� ���� ũ��
		RGB(0, 255, 0));


}

void CTutorial::Release()
{
}
