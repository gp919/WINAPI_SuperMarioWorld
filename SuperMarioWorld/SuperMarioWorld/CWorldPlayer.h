#pragma once

class CWorld;  // 

class CWorldPlayer
{
public:
	void Initialize(CWorld* pOwnerWorld);
	void Set_Target(int iWorld);
	void Update();
	void Render(HDC);

private:
	void Move_Towards_Target();
	void Switch_To_World2();

private:
	float m_fX = 0, m_fY = 0;
	float m_fSpeed = 2.f;
	int m_iCurWorld = 0;
	bool m_bMoving = false;
	bool m_bChangeToWorld2 = false;
	

	POINT m_tTarget = {};
	std::queue<POINT> m_quePath;
	std::vector<POINT> m_vecWorldPos;

	CWorld* m_pOwnerWorld = nullptr;  //

	DWORD m_dwFrameTime = 0;
	int m_iFrame = 0;
	int m_iMaxFrame = 4;
	DWORD m_dwFrameSpeed = 150;  // 밀리초 단위
};
