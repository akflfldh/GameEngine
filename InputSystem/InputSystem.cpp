#include "InputSystem/InputSystem.h"

Quad::InputSystem ::InputSystem() {}

Quad::InputSystem::~InputSystem() {}

void Quad::InputSystem::OnMouseEvent(EMouseState mouseState, Render::WindowHandle windowHandle)
{
    int &currMouseState = mMouseContext.mMouseState;
    mCurrWindowHandle = windowHandle;
    switch (mouseState)
    {
    case EMouseState::eLButtonPressed:

        currMouseState = currMouseState | EMouseState::eLButtonHeld;

        break;

    case EMouseState::eLButtonReleased:
    {
        currMouseState = currMouseState & (~EMouseState::eLButtonHeld);
    }
    break;

    case EMouseState::eRButtonPressed:

        currMouseState = currMouseState | EMouseState::eRButtonHeld;

        break;

    case EMouseState::eRButtonReleased:

        currMouseState = currMouseState & (~EMouseState::eRButtonHeld);

        break;
    }

    currMouseState = currMouseState | mouseState;
}

void Quad::InputSystem::SetMouseScreenPos(int screenPosX, int screenPosY, int clientPosX, int clientPosY,
                                          Render::WindowHandle windowHandle)
{
    mCurrWindowHandle = windowHandle;

    if (clientPosX != mMouseContext.mClientPosX || clientPosY != mMouseContext.mClientPosY)
        mMouseContext.mMouseState = mMouseContext.mMouseState | (EMouseState::eMoved);

    mMouseContext.mScreenPosX = screenPosX;
    mMouseContext.mScreenPosY = screenPosY;
    mMouseContext.mClientPosX = clientPosX;
    mMouseContext.mClientPosY = clientPosY;
}

Quad::InputSystem *Quad::InputSystem::GetInstance()
{
    static InputSystem inputSystem;

    return &inputSystem;
}

void Quad::InputSystem::EndFrame()
{

    int &currMouseState = mMouseContext.mMouseState;

    currMouseState = currMouseState & (~EMouseState::eLButtonPressed);
    currMouseState = currMouseState & (~EMouseState::eLButtonReleased);
    currMouseState = currMouseState & (~EMouseState::eRButtonPressed);
    currMouseState = currMouseState & (~EMouseState::eRButtonPressed);
    currMouseState = currMouseState & (~EMouseState::eMoved);
}

const Quad::MouseContext &Quad::InputSystem::GetMouseContext() const
{
    return mMouseContext;
}

Render::WindowHandle Quad::InputSystem::GetCurrentWindowHandle() const
{
    return mCurrWindowHandle;
}