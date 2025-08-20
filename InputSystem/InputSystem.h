#pragma once

#include "InputSystem/InputType.h"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "InputSystem/InputSystemDllMacro.h"
#include <RenderSystem/RenderType.h>

namespace Quad
{

class INPUT_SYSTEM_API InputSystem
{
  public:
    static InputSystem *GetInstance();
    ~InputSystem();

    // win proc에서 호출될것이다.
    void OnMouseEvent(EMouseState mouseState, Render::WindowHandle windowHandle);
    void SetMouseScreenPos(int screenPosX, int screenPosY, int clientPosX, int clientPosY,
                           Render::WindowHandle windowHandle);

    // 다음프레임을위한 준비
    void EndFrame();

    const MouseContext &GetMouseContext() const;
    Render::WindowHandle GetCurrentWindowHandle() const;

  private:
    InputSystem();

    MouseContext mMouseContext;
    Render::WindowHandle mCurrWindowHandle; // 현재 프레임에 활성화된 윈도우핸들
};

} // namespace Quad