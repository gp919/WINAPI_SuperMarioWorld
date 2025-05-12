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
	CScrollMgr::Get_Instance()->Set_Size(5120.f * SCALE_FACTOR, 432.f * SCALE_FACTOR);

	CSceneMgr::Get_Instance()->Load_Data(); // ���� ������ �ҷ�����
	
	// �� �� �÷��̾� ���� (Ÿ�� �ٴ� ��)
	CObject* pPlayer = new CPlayer;
	pPlayer->Initialize();  // ��������� �ʱ�ȭ
	pPlayer->Update();
	pPlayer->Late_Update();
	
	CObjectMgr::Get_Instance()->Add_Object(OBJ_PLAYER, pPlayer);
	//pPlayer->Late_Update();
	//pPlayer->Late_Update();

	// ��ũ�� ��ġ ����
	CScrollMgr::Get_Instance()->Set_ScrollX(pPlayer->Get_Info()->fX - WINCX * 0.42f);
	CScrollMgr::Get_Instance()->Set_ScrollY(pPlayer->Get_Info()->fY - WINCY * 0.5f);
	CScrollMgr::Get_Instance()->Scroll_Lock();
	//CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(200.f, 400.f, TILE_EMPTY));
	//CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(248.f, 400.f, TILE_Q));
	//CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(296.f, 400.f, TILE_E));
	//CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(344.f, 400.f, TILE_ROT));
	//CObjectMgr::Get_Instance()->Add_Object(OBJ_TILE, new CTile(392.f, 400.f, TILE_CLOUD));

	//CObjectMgr::Get_Instance()->Add_Object(OBJ_MONSTER, new CMonster(2000.f, 512.f));

	//// �ٴ� ����
	///*CLineMgr::Get_Instance()->Add_Line(
	//	{ 0.f, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) },
	//	{ WINCX, (WINCY - (16.f * 2.5f * SCALE_FACTOR + SMALLY * 0.5f)) });*/


	//	// �׽�Ʈ�� ���� ����
	//CLineMgr::Get_Instance()->Add_Line({ 0.f, 528.f }, { Get_MapSize().first * SCALE_FACTOR, 528.f});
	//
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
	m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
	m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

	m_fWorldX = m_fScrollX + WINCX * 0.5f;
	m_fWorldY = m_fScrollY + WINCY * 0.5f;

	// ��� �̹��� ��� (��ũ�� ���� or ��� ���� ���� ����)
	HDC hMemDC_back = CBmpMgr::Get_Instance()->Find_Image(L"BackGround01");
	if (hMemDC_back)
	{
		const int BACK_WIDTH = 512;
		const int BACK_HEIGHT = 432;

		int startIdx = (int)(m_fScrollX / (BACK_WIDTH * SCALE_FACTOR));
		int numBacks = WINCX / (BACK_WIDTH * SCALE_FACTOR) + 2;

		for (int i = 0; i < numBacks; ++i)
		{
			int drawX = (startIdx + i) * BACK_WIDTH * SCALE_FACTOR - (int)m_fScrollX;
			BitBlt(hDC, drawX, 0, BACK_WIDTH * SCALE_FACTOR, BACK_HEIGHT * SCALE_FACTOR, hMemDC_back, 0, 0, SRCCOPY);
		}
	}

	// Scene1 �̹��� ���
	HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(L"Scene1");
	int srcX = static_cast<int>(CScrollMgr::Get_Instance()->Get_ScrollX() / SCALE_FACTOR);
	int srcY = static_cast<int>(CScrollMgr::Get_Instance()->Get_ScrollY() / SCALE_FACTOR);

	int srcW = WINCX / SCALE_FACTOR;
	int srcH = WINCY / SCALE_FACTOR;

	GdiTransparentBlt(
		hDC,
		0, 0,              // ȭ�� ���� ��ܿ� ���
		WINCX, WINCY,      // Ȯ��� ��� ũ��
		hMemDC,
		srcX, srcY,        // �̹����� �߶� ���� ��ġ
		srcW, srcH,
		RGB(0, 255, 0));

	// ������Ʈ, ����, UI�� ������ǥ ��� �� ������ �� ��ũ�� ����
	CLineMgr::Get_Instance()->Render(hDC);
	CObjectMgr::Get_Instance()->Render(hDC);
	CUiMgr::Get_Instance()->Render(hDC);
}

void CScene01::Release()
{
	CLineMgr::Get_Instance()->Destroy_Instance();
	CObjectMgr::Get_Instance()->Destroy_Instance();
}
