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
	

	// ���� ���� Ÿ�� ��ġ
	//x :448��
	CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, new CPlayer);
	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(TILECY * SCALE_FACTOR * 5, WINCY - (TILECY * SCALE_FACTOR * 5), TILE_EMPTY));
	// �ٴ� ����
	/*CLineMgr::Get_Instance()->Add_Line(
		{ 0.f, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) },
		{ WINCX, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) });*/
	

	// �׽�Ʈ�� ���� ����
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

	CLineMgr::Get_Instance()->Render(hDC);
	CObjectMgr::Get_Instance()->Render(hDC);


}

void CTutorial::Release()
{
	CLineMgr::Get_Instance()->Destroy_Instance();
	CObjectMgr::Get_Instance()->Destroy_Instance();
}
