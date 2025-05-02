#pragma once

#include "pch.h"
#include "CCollisionMgr.h"





// �簢�� �浹(IntersectRect ���)
void CCollisionMgr::Collision_Rect(list<CObject*> _Dst, list<CObject*> _Src)
{
	RECT	rc{};

	for (auto& Dst : _Dst)
	{
		for (auto& Src : _Src)
		{
			if (IntersectRect(&rc, Dst->Get_Rect(), Src->Get_Rect()))
			{
				Dst->Set_Dead();
				Src->Set_Dead();
			}
		}
	}


}

// �� �浹(Check_Circle ���)
void CCollisionMgr::Collision_Circle(list<CObject*> _Dst, list<CObject*> _Src)
{

	for (auto& Dst : _Dst)
	{
		for (auto& Src : _Src)
		{
			if (Check_Circle(Dst, Src))
			{
				Dst->Set_Dead();
				Src->Set_Dead();
			}
		}
	}


}

// �� �浹 ����
bool CCollisionMgr::Check_Circle(CObject* pDst, CObject* pSrc)
{
	float	fWidth = abs(pDst->Get_Info()->fX - pSrc->Get_Info()->fX);
	float	fHeight = abs(pDst->Get_Info()->fY - pSrc->Get_Info()->fY);

	float	fDiagonal = sqrtf(fWidth * fWidth + fHeight * fHeight);

	float	fRadius = (pDst->Get_Info()->fCX + pSrc->Get_Info()->fCX) * 0.5f;

	return fRadius >= fDiagonal;
}


// �簢�� �浹(Check_Rect ���) + ���浹ü ��ġ ����
CObject* CCollisionMgr::Collision_RectEx(list<CObject*>& _Dst, list<CObject*>& _Src)
{
	float	fWidth(0.f), fHeight(0.f);

	for (auto& Dst : _Dst)
	{
		for (auto& Src : _Src)
		{
			if (Check_Rect(Dst, Src, &fWidth, &fHeight))
			{
				// ��, �� �浹
				if (fWidth > fHeight)
				{
					// ��
					if (Dst->Get_Info()->fY < Src->Get_Info()->fY)
					{
						Dst->Set_PosY(-fHeight);
						Dst->Set_Col(COL_BOTTOM);
					}

					// ��
					else
					{
						Dst->Set_PosY(fHeight);
						Dst->Set_Col(COL_TOP);
					}
				}
				// ��, �� �浹

				else
				{
					// ��
					if (Dst->Get_Info()->fX < Src->Get_Info()->fX)
					{
						Dst->Set_PosX(-fWidth);
						Dst->Set_Col(COL_RIGHT);
					}

					// ��
					else
					{
						Dst->Set_PosX(fWidth);
						Dst->Set_Col(COL_LEFT);
					}
				}
				
				return Src;
			}
		}
	}
	return nullptr;
}

bool CCollisionMgr::Check_Rect(CObject* pDst, CObject* pSrc, float* pX, float* pY)
{
	float	fDistanceX = fabsf(pDst->Get_Info()->fX - pSrc->Get_Info()->fX);
	float	fDistanceY = fabsf(pDst->Get_Info()->fY - pSrc->Get_Info()->fY);

	float	fRadiusX = (pDst->Get_Info()->fCX + pSrc->Get_Info()->fCX) * 0.5f;
	float	fRadiusY = (pDst->Get_Info()->fCY + pSrc->Get_Info()->fCY) * 0.5f;

	if (fDistanceX < fRadiusX && fDistanceY < fRadiusY)
	{
		*pX = fRadiusX - fDistanceX;
		*pY = fRadiusY - fDistanceY;

		return true;
	}

	return false;
}
