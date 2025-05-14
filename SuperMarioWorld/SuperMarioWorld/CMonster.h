#pragma once
#include "CObject.h"

enum MONSTER_STATE
{
    MONSTER_IDLE,
    MONSTER_WALK,
    MONSTER_RUN,
    MONSTER_STOMPED,
    MONSTER_SHELL_IDLE,
    MONSTER_SHELL_MOVE,
    MONSTER_EJECTED,
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
    void Set_State(MONSTER_STATE _eState);
    void Set_MonsterID(MONSTERID _eID) { m_eMonID = _eID; }
    void Set_Move(bool _bMove) { m_bMove = _bMove; }

    MONSTER_STATE Get_State() const { return m_eState; }
    MONSTERID Get_MonsterID() const { return m_eMonID; }

public:
    void On_Stomped();

private:
    void Update_AI();
    void Init_Frame();
    void Apply_Gravity();
    bool Is_Koopa() const { return m_eMonID == MON_GREENKOOPA || m_eMonID == MON_REDKOOPA; }

private:
    MONSTERID     m_eMonID;
    MONSTER_STATE m_eState;
    bool          m_bMove;
    float         m_fFallSpeed;
    DWORD         m_dwDeadTime;
    DWORD         m_dwTime;

    // Static µ•¿Ã≈Õ
    static const map<pair<MONSTERID, MONSTER_STATE>, FRAME> m_mapFrame;
    static const map<MONSTERID, LPCTSTR> m_mapImage;
};