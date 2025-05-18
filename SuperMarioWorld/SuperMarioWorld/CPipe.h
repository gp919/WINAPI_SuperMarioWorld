#pragma once
#include "CObject.h"
enum PIPE_DIR { PIPE_DOWN, PIPE_UP }; // 관 방향
struct PIPE_INFO {
    POINTF tDestPos;  // 씬 내 목적지 위치
};
class CPipe : public CObject
{
public:
    CPipe(PIPE_INFO tPipeInfo, PIPE_DIR eDir)
        : m_tPipeInfo(tPipeInfo), m_eDir(eDir) {
    }

    virtual void Initialize() override;
    virtual int Update() override;
    virtual void Late_Update() override;
    virtual void Render(HDC hDC) override;
    virtual void Release() override {}

    bool Is_Player_Enterable(CObject* pPlayer);
    const POINTF& Get_DestPos() const { return m_tPipeInfo.tDestPos; }

private:
    PIPE_INFO m_tPipeInfo;
    PIPE_DIR m_eDir;
};

