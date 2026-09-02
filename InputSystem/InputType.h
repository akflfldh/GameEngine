#pragma once

#include <CoreMath/Geometry.h>
#include <Window/IWindowEventHandler.h>
#include <functional>
namespace Quad
{

// struct KeyHeldStruct
//{
//     int key = 0;
//     float mCurrTime = 0.0f;
//     float mIntervalTime = 0.2f;
// };

enum class EKeyCode : uint32_t
{
    eUnKnown = 0,

    eEnter = 0x0D,

    eSpace = 32,

    eLeft = 37,
    eUp,
    eRight,
    eDown,

    eDel = 46,

    e0 = 48,
    e1,
    e2,
    e3,
    e4,
    e5,
    e6,
    e7,
    e8,
    e9,

    eA = 65,
    eB,
    eC,
    eD,
    eE,
    eF,
    eG,
    eH,
    eI,
    eJ,
    eK,
    eL,
    eM,
    eN,
    eO,
    eP,
    eQ,
    eR,
    eS,
    eT,
    eU,
    eV,
    eW,
    eX,
    eY,
    eZ,

    eMouseLeft = 350,
    eMouseRight,
    eMouseMiddle,
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

    bool bLButtonDown = false; // 현재 눌림 상태
    bool bRButtonDown = false; // 현재 눌림 상태

    bool bLButtonDownThisFrame = false; // 이번프레임 눌림상태
    bool bRButtonDownThisFrame = false;

    bool bLButtonUpThisFrame = false; // 이번 프레임 up상태
    bool bRButtonUpThisFrame = false; // 이번 프레임 up 상태

    int mScreenPosX = 0;
    int mScreenPosY = 0;
    int mClientPosX = 0;
    int mClientPosY = 0;
    float mWorldPosX = 0;
    float mWorldPosY = 0;

    // 매프레임 이동량
    int mDeltaX = 0;
    int mDeltaY = 0;
};

struct keyContext
{
    bool bKeyDown;
    bool bKeyDownThisFrame;
    bool bKeyUpThisFrame;
    uint8_t mVK; // SCAN KEY 에 대응되는 문자(영)
};

struct KeybaordContext
{
    keyContext keyContextArray[128]; // scankeycode;
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

struct RawInputData
{
    EInputState mInputState;

    struct
    {
        uint32_t mCharCode;
    } keyChar;

    struct
    {
        EKeyCode mKeyCode;
        uint8_t mScanKeyCode;

    } keyEvent;

    struct
    {
        int mWheelDelta = 0;
        // 누적델타일수도있고, 순간델타일수도있다. (받는쪽에 따라 다르겠다)
        int mClientX;
        int mClientY;
        int mDeltaX;
        int mDeltaY;
        bool mAccumulateFlag;
    } mouseMoveData;

    struct
    {
        int mWheelDelta = 0;
        int mClientX = 0;
        int mClientY = 0;
    } mouseWheelData;

    bool IsKeyboardEvent() const
    {

        if ((mInputState & EInputState::eKeyDown) || (mInputState & EInputState::eKeyUp) ||
            (mInputState & EInputState::eKeyHeld) || (mInputState & EInputState::eKeyChar))
        {
            return true;
        }
        return false;
    }

    bool IsMouseEvent() const
    {

        if (mInputState >= EInputState::eMouseMove)
        {
            return true;
        }

        return false;
    }
};

using InputHandler = std::function<void(const RawInputData &)>;

} // namespace Quad