#pragma once

#include "pch.h"
#include "CCollisionMgr.h"





// 사각형 충돌(IntersectRect 사용)
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

// 원 충돌(Check_Circle 사용)
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

// 원 충돌 감지
bool CCollisionMgr::Check_Circle(CObject* pDst, CObject* pSrc)
{
	float	fWidth = abs(pDst->Get_Info()->fX - pSrc->Get_Info()->fX);
	float	fHeight = abs(pDst->Get_Info()->fY - pSrc->Get_Info()->fY);

	float	fDiagonal = sqrtf(fWidth * fWidth + fHeight * fHeight);

	float	fRadius = (pDst->Get_Info()->fCX + pSrc->Get_Info()->fCX) * 0.5f;

	return fRadius >= fDiagonal;
}


// 사각형 충돌(Check_Rect 사용) + 피충돌체 위치 보정
CObject* CCollisionMgr::Collision_RectEx(list<CObject*>& _Dst, list<CObject*>& _Src)
{
	float	fWidth(0.f), fHeight(0.f);

	for (auto& Dst : _Dst)
	{
		for (auto& Src : _Src)
		{
			if (Check_Rect(Dst, Src, &fWidth, &fHeight))
			{
				// 상, 하 충돌
				if (fWidth > fHeight)
				{
					// 상
					if (Dst->Get_Info()->fY < Src->Get_Info()->fY)
					{
						Dst->Set_PosY(-fHeight);
						Dst->Set_Col(COL_BOTTOM);
					}

					// 하
					else
					{
						Dst->Set_PosY(fHeight);
						Dst->Set_Col(COL_TOP);
					}
				}
				// 좌, 우 충돌

				else
				{
					// 좌
					if (Dst->Get_Info()->fX < Src->Get_Info()->fX)
					{
						Dst->Set_PosX(-fWidth);
						Dst->Set_Col(COL_RIGHT);
					}

					// 우
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
