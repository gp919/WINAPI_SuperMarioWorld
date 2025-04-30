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
		WINCX, WINCY,                                  // 출력 크기 (3배)
		hMemDC_back,
		0,                   // 열 인덱스 × 프레임 너비
		0,                  // 행 인덱스 × 프레임 높이
		WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,                                 // 자를 원본 크기
		RGB(0, 255, 0));

	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Tutorial");
	GdiTransparentBlt(
		hDC,
		0, 0,
		WINCX, WINCY,                                  // 출력 크기 (3배)
		hMemDC,
		0,                   // 열 인덱스 × 프레임 너비
		0,                  // 행 인덱스 × 프레임 높이
		WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,                                 // 자를 원본 크기
		RGB(0, 255, 0));


}

void CTutorial::Release()
{
}
