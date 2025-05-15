#pragma once
#include "CObject.h"
#include "CCollisionMgr.h"
#include "CObjectMgr.h"
#include "CItem.h"
class CBullet : public CObject
{
public:
	CBullet(float, float, OBJECTDIR);
	virtual ~CBullet();

public:
	void Initialize()override;
	int Update()override;
	void Late_Update()override;
	void Render(HDC)override;
	void Release()override;
	void On_Collision(EOBJECTID _id) override;

	bool Out_Of_Screen();

private:
	float m_fSpeed;          // 수평 이동 속도
	float m_fGravity;        // 중력
	float m_fSpeedY;         // 수직 속도
	OBJECTDIR m_eBulletDir;

};

