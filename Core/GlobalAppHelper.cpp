#include "Core/GlobalAppHelper.h"

HINSTANCE Quad::GlobalAppHelper::mHinstance;

void Quad::GlobalAppHelper::Initialize(HINSTANCE hInstance)
{
    mHinstance = hInstance;
}

Quad::GlobalAppHelper *Quad::GlobalAppHelper::GetInstance()
{
    static GlobalAppHelper helper;

    return &helper;
}

HINSTANCE Quad::GlobalAppHelper::GetHinstance()
{
    return mHinstance;
}
