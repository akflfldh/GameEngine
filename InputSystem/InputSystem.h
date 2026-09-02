#pragma once

#include "InputSystem/InputSystemDllMacro.h"
#include "InputSystem/InputType.h"
#include <InputSystem/IInputHandler.h>
#include <RenderSystem/RenderType.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Quad
{

class INPUT_SYSTEM_API InputSystem
{
  public:
    static InputSystem *GetInstance();
    ~InputSystem();

    virtual void Update() = 0;
    void OnMouseMove(int deltaX, int deltaY);

    void SetMousePos(int sx, int sy, int cx, int cy);
    // win proc에서 호출될것이다.
    void OnMouseEvent(EMouseState mouseState, Render::WindowHandle windowHandle);

    void OnMouseButtonEvent(EInputState inputState);

    void OnKeyEvent(EInputState keystate, uint8_t scankeyCode);
    void OnCharEvent(uint32_t cha);

    void OnMouseWheel(int wheelDelta);

    // 다음프레임을위한 준비
    virtual void EndFrame();

    const MouseContext &GetMouseContext() const;
    Render::WindowHandle GetCurrentWindowHandle() const;

    void SetCurrentWindowHandle(Render::WindowHandle windowHandle);

    // InputEvent를 처리할 핸들러 , 매프레임호출
    void Dispatch(const InputHandler &handler);

    bool IsRButtonDown() const;
    bool IsLButtonDown() const;

    bool IsScanKeyDown(uint8_t scanKey) const;
    bool IsVKeyDown(uint8_t vk) const;

    // 프레임에 마우스의 이동량
    std::pair<int, int> GetMouseDelta() const;

    virtual uint8_t toScanKey(uint8_t vk) const = 0;
    virtual uint8_t toVirtualKey(uint8_t scanKey) const = 0;

    // input focus api

    void SetKeyboardInputFocusHandler(IInputHandler *IInputHandler);
    void SetMouseCapture(IInputHandler *IInputHandler);
    void ReleaseKeyboardFocus();
    void ReleaseMouseCapture();

    bool IsMouseCaptured() const;
    bool IsMouseCaptured(IInputHandler *IInputHandler) const;
    IInputHandler *GetMouseCapturedHandler() const;

    void SetGameInputBlocked(bool blocked);
    bool GetGameInputBlocked() const;

  protected:
    InputSystem();
    // 최종 프레임단위의 마우스이동 이벤트생성(단 하나)(매 프레임Dispatch시 호출)
    void GenerateMouseMoveEventPerFrame();

    MouseContext mMouseContext;
    KeybaordContext mKeyBoardContext;
    Render::WindowHandle mCurrWindowHandle; // 현재 프레임에 활성화된 윈도우핸들

    std::vector<RawInputData> mInputDataVector;

    // 입력을 독점하는 핸들러
    // 설정되었다면 Dispatch시 해당 핸들러로만 전달
    //(키보드 입력만 독점)
    IInputHandler *mKeyboardHandler;
    //(마우스 입력만 독점)(캡처한다)
    IInputHandler *mMouseHandler;

    // 입력을 시스템들에서 막을떄 사용(즉 독점하고싶을떄 시스템단위로)
    bool mGameInputBlocked = false;
};

} // namespace Quad