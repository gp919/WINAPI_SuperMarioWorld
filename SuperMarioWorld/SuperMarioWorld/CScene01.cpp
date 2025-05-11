#include "pch.h"
#include "CScene01.h"
#include "fmod_errors.h"

CScene01::CScene01()
{
}

CScene01::~CScene01()
{
	Release();
}

void CScene01::Initialize()
{
	//CSoundMgr::Get_Instance()->PlayBGM(L"overworld.wav", 0.5f);

	CScrollMgr::Get_Instance()->Set_ScrollX(0.f);
	CScrollMgr::Get_Instance()->Set_ScrollY(0.f);
	
	//x :500��
	CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, new CPlayer);

	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(200.f, 400.f, TILE_EMPTY));
	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(248.f, 400.f, TILE_Q));
	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(296.f, 400.f, TILE_E));
	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(344.f, 400.f, TILE_ROT));
	CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(392.f, 400.f, TILE_CLOUD));

	CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, new CMonster(2000.f, 512.f));

	// �ٴ� ����
	/*CLineMgr::Get_Instance()->Add_Line(
		{ 0.f, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) },
		{ WINCX, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) });*/


		// �׽�Ʈ�� ���� ����
	CLineMgr::Get_Instance()->Add_Line({ 0.f, 528.f }, { Get_MapSize().first * SCALE_FACTOR, 528.f});
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

			BitBlt(
				hDC,
				drawX, 0,
				BACK_WIDTH * SCALE_FACTOR, BACK_HEIGHT * SCALE_FACTOR,
				hMemDC_back,
				0, 0,
				SRCCOPY);
		}
	}


	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Scene1");
	GdiTransparentBlt(
		hDC,
		0, 0,
		WINCX, WINCY,                                  // ��� ũ�� (3��)
		hMemDC,
		(int)(fScrollX / SCALE_FACTOR),                   // �� �ε��� �� ������ �ʺ�
		// ������ = ��ü �� ���� - ȭ�� ���� = 432.f - 224.f = 208.f
		(int)(fScrollY / SCALE_FACTOR + 208.f),             // �� �ε��� �� ������ ����
		WINCX / SCALE_FACTOR, WINCY / SCALE_FACTOR,       // �ڸ� ���� ũ��
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
