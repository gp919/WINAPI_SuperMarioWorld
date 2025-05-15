#include "pch.h"
#include "CEffect.h"
#include "CBmpMgr.h"
#include "CScrollMgr.h"

// Static 데이터 - 프레임 정보
const map<EFFECT_TYPE, FRAME> CEffect::m_mapFrame = {
    {{EFFECT_DEATH_STAR}, {0, 7, 0, 75, 0}}, // 8프레임, 75ms 간격
};

// Static 데이터 - 이미지 키
const map<EFFECT_TYPE, const TCHAR*> CEffect::m_mapImage = {
    {{EFFECT_DEATH_STAR}, L"Death_Star"},
};

CEffect::CEffect()
    : m_eType(EFFECT_DEATH_STAR)
    , m_dwCreateTime(0)
    , m_dwDuration(2400) // 2.4초간 재생 (8프레임 * 75ms * 4번 반복)
{
    m_eId = OBJ_EFFECT;
}

CEffect::CEffect(float _fX, float _fY, EFFECT_TYPE _eType)
    : CEffect()
{
    m_tInfo.fX = _fX;
    m_tInfo.fY = _fY;
    m_eType = _eType;
}

CEffect::~CEffect()
{
    Release();
}

void CEffect::Initialize()
{
    m_dwCreateTime = GetTickCount();

    switch (m_eType)
    {
    case EFFECT_DEATH_STAR:
        m_tInfo.fCX = 48.f * SCALE_FACTOR;  // 16x16을 3배 확대 후 SCALE_FACTOR 적용
        m_tInfo.fCY = 48.f * SCALE_FACTOR;
        break;
    }

    Init_Frame();
    Update_ImageKey();
}

int CEffect::Update()
{
    // 지속 시간이 끝나면 삭제
    if (GetTickCount() > m_dwCreateTime + m_dwDuration)
        return DEAD;

    Move_Frame();
    return NOEVENT;
}

void CEffect::Late_Update()
{
    Update_Rect();
}

void CEffect::Render(HDC hDC)
{
    m_fScrollX = CScrollMgr::Get_Instance()->Get_ScrollX();
    m_fScrollY = CScrollMgr::Get_Instance()->Get_ScrollY();

    HDC hMemDC = CBmpMgr::Get_Instance()->Find_Image(m_pFrameKey);

    // 4x4 배열에서 현재 프레임의 행과 열 계산
    int iRow = m_tFrame.iStart / 4;  // 행 (0~3)
    int iCol = m_tFrame.iStart % 4;  // 열 (0~3)

    GdiTransparentBlt(hDC,
        (int)(m_tInfo.fX - m_tInfo.fCX * 0.5f - m_fScrollX),
        (int)(m_tInfo.fY - m_tInfo.fCY * 0.5f - m_fScrollY),
        (int)m_tInfo.fCX,
        (int)m_tInfo.fCY,
        hMemDC,
        iCol * 16,  // X 좌표 (열 * 16)
        iRow * 16,  // Y 좌표 (행 * 16)
        16,  // 원본 크기
        16,
        RGB(0, 255, 0));
}

void CEffect::Release()
{
}

void CEffect::Init_Frame()
{
    auto it = m_mapFrame.find(m_eType);
    if (it != m_mapFrame.end())
        m_tFrame = it->second;

    m_tFrame.dwTime = GetTickCount();
}

void CEffect::Update_ImageKey()
{
    auto it = m_mapImage.find(m_eType);
    if (it != m_mapImage.end())
    {
        m_pFrameKey = it->second;
    }
}