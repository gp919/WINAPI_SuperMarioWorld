#pragma once
#include "pch.h"
#include "CObject.h"

class CCollisionMgr
{
public:
	static void Collision_Rect(list<CObject*> _Dst, list<CObject*> _Src);
	static void Collision_Circle(list<CObject*> _Dst, list<CObject*> _Src);
	static bool Check_Circle(CObject* pDst, CObject* pSrc);
	static CObject* Collision_RectEx(list<CObject*>& _Dst, list<CObject*>& _Src);
	static bool Check_Rect(CObject* pDst, CObject* pSrc, float* pX, float* pY);

};
