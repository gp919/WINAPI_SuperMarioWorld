#pragma once
#include "CObject.h"

enum EFFECT_TYPE
{
    EFFECT_DEATH_STAR,
    EFFECT_END
};

class CEffect : public CObject
{
public:
    CEffect();
    CEffect(float _fX, float _fY, EFFECT_TYPE _eType);
    virtual ~CEffect();

public:
    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Late_Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override;
    virtual void On_Collision(EOBJECTID _id) override {}

private:
    void Init_Frame();
    void Update_ImageKey();

private:
    EFFECT_TYPE m_eType;
    DWORD m_dwCreateTime;
    DWORD m_dwDuration;

    // Static µ•¿Ã≈Õ
    static const map<EFFECT_TYPE, FRAME> m_mapFrame;
    static const map<EFFECT_TYPE, const TCHAR*> m_mapImage;
};