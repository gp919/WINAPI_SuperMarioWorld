#pragma once
#include "CObject.h"
#include "CPlayer.h"
#include "CEffect.h"

enum MONSTER_STATE
{
    MONSTER_IDLE,
    MONSTER_WALK,
    MONSTER_RUN,
    MONSTER_STOMPED,
    MONSTER_SHELL_IDLE,
    MONSTER_SHELL_MOVE,
    MONSTER_EJECTED,
    MONSTER_HIDDEN,
    MONSTER_UP,
    MONSTER_DOWN,
    MONSTER_DEAD,
    MONSTER_END
};

class CMonster : public CObject
{
public:
    CMonster();
    CMonster(float, float, MONSTERID);
    CMonster(INFO);
    virtual ~CMonster();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Late_Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
    virtual void On_Collision(EOBJECTID _id) override;

public:
    void Set_State(MONSTER_STATE);
    void Set_MonsterID(MONSTERID _eID) { m_eMonID = _eID; }
    void Set_Move(bool _bMove) { m_bMove = _bMove; }

    MONSTER_STATE Get_State() const { return m_eState; }
    MONSTERID Get_MonsterID() const { return m_eMonID; }
    CObject* Get_Holder() const { return m_pHolder; }

public:
    void On_Stomped();
    void Set_Holder(CObject* pHolder) { m_pHolder = pHolder; }
    bool Is_Held() const { return m_pHolder != nullptr; }
    void Release_From_Holder();
    void On_Kicked(OBJECTDIR);
    bool In_Screen();
    bool Player_In_Range(float range);
    bool Player_Distance_High();

private:
    void Update_AI();
    void Update_ImageKey();
    void Init_Frame();
    void Apply_Gravity();
    bool Is_Koopa() const { return m_eMonID == MON_GREENKOOPA || m_eMonID == MON_REDKOOPA; }
    


private:
    CObject* m_pHolder;  // 들고 있는 플레이어
    MONSTERID     m_eMonID;
    MONSTER_STATE m_eState;
    bool          m_bMove;
    float         m_fFallSpeed;
    DWORD         m_dwDeadTime;
    DWORD         m_dwTime;

    float   m_fDeathFallSpeed;  // 죽을 때 떨어지는 속도
    bool    m_bDeathFalling;    // 죽어서 떨어지는 중인지

    // Static 데이터
    static const map<pair<MONSTERID, MONSTER_STATE>, FRAME> m_mapFrame;
    static const map<pair<MONSTERID, OBJECTDIR>, const TCHAR*> m_mapImage;

    float m_fPipeTopY = 0.f;     // 피라냐가 나오는 최대 높이
    float m_fPipeBottomY = 0.f;  // 피라냐가 숨는 위치
    float m_fUpTargetY = 0.f;    // 두더지가 튀어나올 목표 위치
};