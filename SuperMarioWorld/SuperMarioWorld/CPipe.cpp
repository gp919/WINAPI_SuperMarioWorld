#include "pch.h"
#include "CPipe.h"


// CPipe.cpp - �ʼ� �Լ��� ����
void CPipe::Initialize()
{
    m_tInfo.fCX = TILECX * SCALE_FACTOR;
    m_tInfo.fCY = TILECY * SCALE_FACTOR;
}

bool CPipe::Is_Player_Enterable(CObject* pPlayer)
{
    const float fDistX = fabs(m_tInfo.fX - pPlayer->Get_Info()->fX);
    const float fDistY = fabs(m_tInfo.fY - pPlayer->Get_Info()->fY);

    // ���� �Ÿ� && �Ʒ�Ű ������ ����
    if (fDistX < 10.f && fDistY < 10.f)
    {
        if (m_eDir == PIPE_DOWN && GetAsyncKeyState(VK_DOWN) & 0x8000)
            return true;
        if (m_eDir == PIPE_UP && GetAsyncKeyState(VK_UP) & 0x8000)
            return true;
    }
    return false;
}