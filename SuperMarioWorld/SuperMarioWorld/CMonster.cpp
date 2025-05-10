#include "pch.h"
#include "CMonster.h"

CMonster::CMonster() : m_eState(MONSTER_WALK)
{
}


CMonster::~CMonster()
{
}

void CMonster::Initialize()
{
}

int CMonster::Update()
{
	if (m_bDead)
	{
		return DEAD;
	}
	return NOEVENT;
}

void CMonster::Late_Update()
{
}

void CMonster::Render(HDC)
{

}

void CMonster::Release()
{
}
