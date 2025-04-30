#pragma once

#define WINCX	768
#define WINCY	672

// TODO : 마리오 사이즈 16*24로 수정할것
#define TILEX	14
#define TILEY	10

#define TILECX	50
#define TILECY	50

#define PI			3.141592f
#define GRAVITY	0.45f
#define MAX_ACC 7.5f

#define PURE		= 0

#define VK_MAX		0xff


// 임의 키 매핑
#define VK_JUMP	'X'
#define VK_SPIN VK_LSHIFT
#define VK_DASH 'C'


#define			NOEVENT		0
#define			DEAD		1

enum SCENEID { SC_LOGO, SC_MENU, SC_WORLD, SC_STAGE_ONE, SC_STAGE_TWO, SC_STAGE_FINAL, SC_BOSS, SC_END };
enum EOBJECTID { OBJ_PLAYER, OBJ_MONSTER, OBJ_END};
enum OBJECTDIR {
	DIR_LEFT, DIR_RIGHT, DIR_END
};
enum PLAYERSTATE {
	IDLE, WALK, RUN, JUMP, JUMP_ACCEL, STOMP, CROUCH, BRAKE, KICK,
	HOLD_IDLE, HOLD_MOVE, PDEAD, END
};


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

extern int g_iHeight; 
extern int g_iMaxHeight;