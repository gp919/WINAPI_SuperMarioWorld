#pragma once

#define WINCX	960
#define WINCY	540

#define TILEX	60
#define TILEY	33

#define TILECX	16
#define TILECY	16

#define PI			3.141592f

#define PURE		= 0

#define VK_MAX		0xff

#define			NOEVENT		0
#define			DEAD		1

enum SCENEID { SC_LOGO, SC_MENU, SC_WORLD, SC_STAGE_ONE, SC_STAGE_TWO, SC_STAGE_FINAL, SC_BOSS, SC_END };
enum EOBJECTID { OBJ_PLAYER, OBJ_MONSTER, OBJ_END};

template<typename T>
void Safe_Delete(T& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

typedef struct tagInfo
{
	float	fX;
	float	fY;

	float	fCX;
	float	fCY;

}INFO;

typedef struct tagLinePoint
{
	float	fX;
	float	fY;

	tagLinePoint()
	{
		ZeroMemory(this, sizeof(tagLinePoint));
	}
	tagLinePoint(float _fX, float _fY)
		: fX(_fX), fY(_fY) {
	}

}LINEPOINT;

typedef struct tagLine
{
	tagLinePoint	tLPoint;
	tagLinePoint	tRPoint;

	tagLine() { ZeroMemory(this, sizeof(tagLine)); }
	tagLine(tagLinePoint tLeft, tagLinePoint tRight)
		: tLPoint(tLeft), tRPoint(tRight)
	{
	}

}LINE;


typedef struct tagFrame
{
	int		iStart;
	int		iEnd;
	int		iMotion;
	DWORD	dwSpeed;
	DWORD	dwTime;

}FRAME;


struct tagFinder
{
public:
	tagFinder(const TCHAR* pTag) : m_pTag(pTag) {}

	template<typename T>
	bool operator()(T& MyPair)
	{
		return !lstrcmp(MyPair.first, m_pTag);
	}

	const TCHAR* m_pTag;
};

struct tagDeleteMap
{
	template<typename T>
	void operator()(T& MyPair)
	{
		if (MyPair.second)
		{
			delete MyPair.second;
			MyPair.second = nullptr;
		}
	}
};



extern HWND g_hWnd;