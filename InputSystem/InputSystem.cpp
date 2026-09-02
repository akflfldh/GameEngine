#include "InputSystem/InputSystem.h"
#include "InputSystem/D3DInputSystem.h"
Quad::InputSystem ::InputSystem() {}

Quad::InputSystem::~InputSystem() {}

// void Quad::InputSystem::OnMouseEvent(EMouseState mouseState, Render::WindowHandle windowHandle)
//{
//     int &currMouseState = mMouseContext.mMouseState;
//     mCurrWindowHandle = windowHandle;
//     switch (mouseState)
//     {
//     case EMouseState::eLButtonPressed:
//
//         currMouseState = currMouseState | EMouseState::eLButtonHeld;
//
//         break;
//
//     case EMouseState::eLButtonReleased:
//     {
//         currMouseState = currMouseState & (~EMouseState::eLButtonHeld);
//     }
//     break;
//
//     case EMouseState::eRButtonPressed:
//
//         currMouseState = currMouseState | EMouseState::eRButtonHeld;
//
//         break;
//
//     case EMouseState::eRButtonReleased:
//
//         currMouseState = currMouseState & (~EMouseState::eRButtonHeld);
//
//         break;
//     }
//
//     currMouseState = currMouseState | mouseState;
// }

void Quad::InputSystem::OnMouseButtonEvent(EInputState inputState)
{

    RawInputData inputData;
    inputData.mInputState = inputState;
    inputData.mouseMoveData.mDeltaX = 0;
    inputData.mouseMoveData.mDeltaY = 0;
    inputData.mouseMoveData.mClientX = mMouseContext.mClientPosX;
    inputData.mouseMoveData.mClientY = mMouseContext.mClientPosY;
    inputData.mouseMoveData.mAccumulateFlag = false;

    if (inputState & EInputState::eMouseLButtonDown)
    {
        mMouseContext.bLButtonDown = true;
        mMouseContext.bLButtonDownThisFrame = true;
    }

    if (inputState & EInputState::eMouseLButtonUp)
    {
        mMouseContext.bLButtonDown = false;
        mMouseContext.bLButtonUpThisFrame = true;
    }

    if (inputState & EInputState::eMouseRButtonDown)
    {
        mMouseContext.bRButtonDownThisFrame = true;
        mMouseContext.bRButtonDown = true;
    }

    if (inputState & EInputState::eMouseRButtonUp)
    {
        mMouseContext.bRButtonUpThisFrame = true;
        mMouseContext.bRButtonDown = false;
    }

    mInputDataVector.push_back(inputData);
}

void Quad::InputSystem ::OnMouseMove(int deltaX, int deltaY)
{

    mMouseContext.mDeltaX += deltaX;
    mMouseContext.mDeltaY += deltaY;

    RawInputData inputData;
    inputData.mInputState = EInputState::eMouseMove;
    inputData.mouseMoveData.mDeltaX = deltaX;
    inputData.mouseMoveData.mDeltaY = deltaY;
    inputData.mouseMoveData.mClientX = mMouseContext.mClientPosX;
    inputData.mouseMoveData.mClientY = mMouseContext.mClientPosY;

    inputData.mouseMoveData.mAccumulateFlag = false;

    mInputDataVector.push_back(inputData);
}
void Quad::InputSystem::SetMousePos(int sx, int sy, int cx, int cy)
{

    mMouseContext.mScreenPosX = sx;
    mMouseContext.mScreenPosY = sy;

    mMouseContext.mClientPosX = cx;
    mMouseContext.mClientPosY = cy;
}

void Quad::InputSystem::OnKeyEvent(EInputState keystate, uint8_t scankeyCode)
{

    if (keystate & EInputState::eKeyDown)
    {
        mKeyBoardContext.keyContextArray[scankeyCode].bKeyDown = true;
        mKeyBoardContext.keyContextArray[scankeyCode].bKeyDownThisFrame = true;
    }
    else if (keystate & EInputState::eKeyUp)
    {
        mKeyBoardContext.keyContextArray[scankeyCode].bKeyDown = false;
        mKeyBoardContext.keyContextArray[scankeyCode].bKeyUpThisFrame = true;
    }

    RawInputData inputData;
    inputData.mInputState = keystate;
    inputData.keyEvent.mScanKeyCode = scankeyCode;
    inputData.keyEvent.mKeyCode = static_cast<EKeyCode>(toVirtualKey(scankeyCode));

    mInputDataVector.push_back(inputData);
}
void Quad::InputSystem::OnCharEvent(uint32_t ch)
{

    RawInputData inputData;
    inputData.mInputState = EInputState::eKeyChar;
    inputData.keyChar.mCharCode = ch;

    mInputDataVector.push_back(inputData);
}

void Quad::InputSystem::OnMouseWheel(int wheelDelta)
{
    RawInputData inputData;
    inputData.mInputState = EInputState::eMouseWheel;
    inputData.mouseWheelData.mWheelDelta = wheelDelta;
    inputData.mouseWheelData.mClientX = mMouseContext.mClientPosX;
    inputData.mouseWheelData.mClientY = mMouseContext.mClientPosY;

    mInputDataVector.push_back(inputData);
}

Quad::InputSystem *Quad::InputSystem::GetInstance()
{
#ifdef D3DX

    static D3DInputSystem inputSystem;
    return &inputSystem;
#endif
}

void Quad::InputSystem::EndFrame()
{

    mMouseContext.bLButtonDownThisFrame = false;
    mMouseContext.bLButtonUpThisFrame = false;
    mMouseContext.bRButtonDownThisFrame = false;
    mMouseContext.bRButtonUpThisFrame = false;

    mMouseContext.mDeltaX = 0;
    mMouseContext.mDeltaY = 0;

    for (auto &keyContext : mKeyBoardContext.keyContextArray)
    {
        keyContext.bKeyDownThisFrame = false;
        keyContext.bKeyUpThisFrame = false;
    }

    mInputDataVector.clear();
}

const Quad::MouseContext &Quad::InputSystem::GetMouseContext() const
{
    return mMouseContext;
}

Render::WindowHandle Quad::InputSystem::GetCurrentWindowHandle() const
{
    return mCurrWindowHandle;
}

void Quad::InputSystem::SetCurrentWindowHandle(Render::WindowHandle windowHandle)
{

    mCurrWindowHandle = windowHandle;
}

void Quad::InputSystem::Dispatch(const InputHandler &handler)
{
    // 프레임단위의 누적 moveMove 이벤트도 생성해준다.
    GenerateMouseMoveEventPerFrame();
    // Dispatch 도중 input이들어올수있으니 옮긴다.
    std::vector<RawInputData> currInputDataVector(std::move(mInputDataVector));

    for (auto &inputData : currInputDataVector)
    {
        if (mKeyboardHandler && inputData.IsKeyboardEvent())
        {
            mKeyboardHandler->HandleInput(inputData);
        }
        else if (mMouseHandler && inputData.IsMouseEvent())
        {
            mMouseHandler->HandleInput(inputData);
        }
        else
        {
            handler(inputData);
        }
    }
}
bool Quad::InputSystem::IsRButtonDown() const
{
    return mMouseContext.bRButtonDown;
}

bool Quad::InputSystem::IsLButtonDown() const
{
    return mMouseContext.bLButtonDown;
}

bool Quad::InputSystem::IsScanKeyDown(uint8_t scanKey) const
{

    if (scanKey >= 128)
        return false;

    return mKeyBoardContext.keyContextArray[scanKey].bKeyDown;
}
bool Quad::InputSystem::IsVKeyDown(uint8_t vk) const
{

    uint8_t scanKey = toScanKey(vk);

    return IsScanKeyDown(scanKey);
}

std::pair<int, int> Quad::InputSystem::GetMouseDelta() const
{
    return {mMouseContext.mDeltaX, mMouseContext.mDeltaY};
}

void Quad::InputSystem::GenerateMouseMoveEventPerFrame()
{

    if (mMouseContext.mDeltaX != 0 || mMouseContext.mDeltaY != 0)
    {
        RawInputData inputData;

        inputData.mInputState = EInputState::eMouseMove;
        inputData.mouseMoveData.mDeltaX = mMouseContext.mDeltaX;
        inputData.mouseMoveData.mDeltaY = mMouseContext.mDeltaY;
        inputData.mouseMoveData.mClientX = mMouseContext.mClientPosX;
        inputData.mouseMoveData.mClientY = mMouseContext.mClientPosY;

        inputData.mouseMoveData.mAccumulateFlag = true;
        mInputDataVector.push_back(std::move(inputData));
    }
}

void Quad::InputSystem::SetKeyboardInputFocusHandler(IInputHandler *IInputHandler)
{
    mKeyboardHandler = IInputHandler;
}

void Quad::InputSystem::SetMouseCapture(IInputHandler *IInputHandler)
{

    ReleaseMouseCapture();
    mMouseHandler = IInputHandler;
}

void Quad::InputSystem::ReleaseKeyboardFocus()
{
    mKeyboardHandler = nullptr;
}

void Quad::InputSystem::ReleaseMouseCapture()
{
    if (mMouseHandler)
    {
        mMouseHandler->OnMouseCaptureLost();
    }
    mMouseHandler = nullptr;
}

bool Quad::InputSystem::IsMouseCaptured() const
{
    return mMouseHandler != nullptr;
}

bool Quad::InputSystem::IsMouseCaptured(Quad::IInputHandler *IInputHandler) const
{

    return mMouseHandler == IInputHandler;
}
Quad::IInputHandler *Quad::InputSystem::GetMouseCapturedHandler() const
{

    return mMouseHandler;
}

void Quad::InputSystem::SetGameInputBlocked(bool blocked)
{
    mGameInputBlocked = blocked;
}
bool Quad::InputSystem::GetGameInputBlocked() const
{

    return mGameInputBlocked;
}