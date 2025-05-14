#include "pch.h"
#include "CMenu.h"
#include "CSoundMgr.h"

CMenu::CMenu()
{
}

CMenu::~CMenu()
{
	Release();
}

void CMenu::Initialize()
{
	CSoundMgr::Get_Instance()->PlayBGM(L"01. Nintendo Logo.mp3", 0.5f);
}

int CMenu::Update()
{
	return 0;
}

void CMenu::Late_Update()
{
}

void CMenu::Render(HDC hDC)
{
}

void CMenu::Release()
{
}
