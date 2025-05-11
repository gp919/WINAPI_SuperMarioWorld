#pragma once



// 화면 사이즈 : 게임 오리지널 사이즈(256 * 224)의 3배
#define WINCX	768
#define WINCY	672

// 원본 타일 사이즈
#define TILECX	16
#define TILECY	16

// 원본 작은마리오 사이즈
#define SMALLX	16
#define SMALLY	24

// 원본 큰마리오 사이즈
#define BIGX	16
#define BIGY	32

// 원본 > 화면 스케일 사이즈
#define SCALE_FACTOR 3


#define SPEED_MAX	10.f
#define MAX_FALL_SPEED 10.f
#define PI			3.141592f
#define GRAVITY		0.45f
#define RUN_ACCEL	0.06f
#define BOUNCE_SPEED 5.f

#define PURE		= 0

#define VK_MAX		0xff


// 임의 키 매핑
#define VK_JUMP	'X'
#define VK_SPIN VK_LSHIFT
#define VK_RUN 'C'


#define			NOEVENT		0
#define			DEAD		1

enum SCENEID { SC_LOGO, SC_MENU, SC_TUTORIAL, SC_WORLD, SC_STAGE_ONE, SC_STAGE_TWO, SC_STAGE_FINAL, SC_BOSS, SC_END };
enum EOBJECTID { OBJ_PLAYER, OBJ_MONSTER, OBJ_TILE, OBJ_END};
enum OBJECTDIR {
	DIR_LEFT, DIR_RIGHT, DIR_END
};
enum COLLISIONDIR{COL_NONE, COL_LEFT, COL_TOP, COL_RIGHT, COL_BOTTOM, COL_END};
enum TILEID { TILE_Q, TILE_E, TILE_ROT, TILE_CLOUD, TILE_EMPTY, TILE_HIDDEN, TILE_END };
enum MONSTERID { MON_KOOPA, MON_GOOMBA, MON_END };
enum PLAYERSTATE {
	IDLE, LOOK_UP, DUCK, WALK, RUN, SKID, PIPE, JUMP, FALL, RUN_JUMP, SPIN_JUMP, SLIDE, KICK, SWIM, VICTORY,
	HOLD_IDLE, HOLD_LOOKUP, HOLD_DUCK, HOLD_WALK, HOLD_RUN, HOLD_JUMP, HOLD_SWIM, DEATH, END
};


enum CHANNELID { SOUND_EFFECT, SOUND_BGM, MAXCHANNEL };

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

	int iType;

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


template <typename T>
const T& clamp(const T& v, const T& lo, const T& hi) {
	return (v < lo) ? lo : (hi < v) ? hi : v;
}

extern HWND g_hWnd;

extern int g_iHeight; 
extern int g_iMaxHeight;

