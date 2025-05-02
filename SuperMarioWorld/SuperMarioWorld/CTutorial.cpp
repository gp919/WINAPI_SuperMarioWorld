#pragma once

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
	

	// 이하 임의 타일 배치
	//x :448쯤
	CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, new CPlayer);
	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(TILECY * SCALE_FACTOR * 5, WINCY - (TILECY * SCALE_FACTOR * 5), TILE_EMPTY));
	// 바닥 라인
	/*CLineMgr::Get_Instance()->Add_Line(
		{ 0.f, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) },
		{ WINCX, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) });*/
	

	// 테스트용 강제 라인
	CLineMgr::Get_Instance()->Add_Line({ 0.f, 600.f }, { WINCX, 600.f });
	CObjectMgr::Get_Instance()->Initialize();


	
}

int CTutorial::Update()
{
	CObjectMgr::Get_Instance()->Update();
	return 0;
}

void CTutorial::Late_Update()
{
	CObjectMgr::Get_Instance()->Late_Update();
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

	CLineMgr::Get_Instance()->Render(hDC);
	CObjectMgr::Get_Instance()->Render(hDC);


}

void CTutorial::Release()
{
	CLineMgr::Get_Instance()->Destroy_Instance();
	CObjectMgr::Get_Instance()->Destroy_Instance();
}
