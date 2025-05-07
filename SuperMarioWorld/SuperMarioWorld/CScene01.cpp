#include "pch.h"
#include "CScene01.h"

CScene01::CScene01()
{
}

CScene01::~CScene01()
{
}

void CScene01::Initialize()
{

	CScrollMgr::Get_Instance()->Set_Size(Get_MapSize().first, Get_MapSize().second);
	CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
	CScrollMgr::Get_Instance()->Set_ScrollY((Get_MapSize().second - WINCY / SCALE_FACTOR) * SCALE_FACTOR);
	
	// ���� ���� Ÿ�� ��ġ
	//x :448��
	CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, new CPlayer);

	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(400.f, 720.f, TILE_EMPTY));
	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(500.f, 720.f, TILE_Q));

	// �ٴ� ����
	/*CLineMgr::Get_Instance()->Add_Line(
		{ 0.f, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) },
		{ WINCX, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) });*/


		// �׽�Ʈ�� ���� ����
	CLineMgr::Get_Instance()->Add_Line({ 0.f, 576.f }, { Get_MapSize().first, 576.f});
	CObjectMgr::Get_Instance()->Initialize();
}

int CScene01::Update()
{
	CObjectMgr::Get_Instance()->Update();
	return 0;
}

void CScene01::Late_Update()
{
	CObjectMgr::Get_Instance()->Late_Update();
}

void CScene01::Render(HDC hDC)
{
	float fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	float fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	HDC hMemDC_back = CBmpMgr::Get_Instance()->Find_Image(L"BackGround01");
	if (hMemDC_back)
	{
		const int BACK_WIDTH = 512;
		const int BACK_HEIGHT = 432;

		// ���� ������ ��� ���� ���� �ε���
		int startIdx = (int)(fScrollX / (BACK_WIDTH * SCALE_FACTOR));

		// �ʿ��� ��� ��: ȭ�� �ʺ� Ŀ���� ��ŭ
		int numBacks = WINCX / (BACK_WIDTH * SCALE_FACTOR) + 2;

		for (int i = 0; i < numBacks; ++i)
		{
			int drawX = (startIdx + i) * BACK_WIDTH * SCALE_FACTOR - (int)fScrollX;

			GdiTransparentBlt(
				hDC,
				drawX, 0,
				BACK_WIDTH * SCALE_FACTOR, BACK_HEIGHT * SCALE_FACTOR,
				hMemDC_back,
				0, 0,
				BACK_WIDTH, BACK_HEIGHT,
				RGB(0, 255, 0));
		}
	}


	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Scene1");
	GdiTransparentBlt(
		hDC,
		0, 0,
		WINCX, WINCY,                                  // ��� ũ�� (3��)
		hMemDC,
		(int)(fScrollX / SCALE_FACTOR),                   // �� �ε��� �� ������ �ʺ�
		(int)(fScrollY / SCALE_FACTOR),                  // �� �ε��� �� ������ ����
		WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,                                 // �ڸ� ���� ũ��
		RGB(0, 255, 0));

	CLineMgr::Get_Instance()->Render(hDC);
	CObjectMgr::Get_Instance()->Render(hDC);

	CUiMgr::Get_Instance()->Render(hDC);

}

void CScene01::Release()
{
	CLineMgr::Get_Instance()->Destroy_Instance();
	CObjectMgr::Get_Instance()->Destroy_Instance();
}
