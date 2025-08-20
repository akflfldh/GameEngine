#pragma once

namespace Quad
{
enum class EInputState
{
    eKeyDown = 0,
    eKeyUp,
    eKeyHeld
};

struct KeyHeldStruct
{
    int key = 0;
    float mCurrTime = 0.0f;
    float mIntervalTime = 0.2f;
};

enum EMouseState
{
    // 어떠한 상태도아니다.
    eNone = 0,
    // 눌렸는지여부(눌려져있는지)
    eLButtonHeld = 1,
    eRButtonHeld = 1 << 1,

    // 이번프레임에 버튼들이 눌렀는지,띄웠는지를 나타내는 flag
    eLButtonPressed = 1 << 2,
    eLButtonReleased = 1 << 3,
    eRButtonPressed = 1 << 4,
    eRButtonReleased = 1 << 5,

    // 이번프레임에 움직였는지
    eMoved = 1 << 6
};

struct MouseContext
{
    int mMouseState = EMouseState::eNone; // or연산을 통해서 마우스가 어떤상태인지 확인할수있을것
    int mScreenPosX = 0;
    int mScreenPosY = 0;
    int mClientPosX = 0;
    int mClientPosY = 0;
    float mWorldPosX = 0;
    float mWorldPosY = 0;
};

struct MouseInputData
{
    EMouseState mMouseInputType;

    int mClientPositionX;
    int mClientPositionY;
    int mScreenPositionX;
    int mScreenPositionY;

    int mPreClientPositionX;
    int mPreClientPositionY;
    int mPreScreenPositionX;
    int mPreScreenPositionY;
};

} // namespace Quad