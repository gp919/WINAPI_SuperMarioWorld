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
	float m_fSpeed;          // ���� �̵� �ӵ�
	float m_fGravity;        // �߷�
	float m_fSpeedY;         // ���� �ӵ�
	OBJECTDIR m_eBulletDir;

};

