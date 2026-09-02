#pragma once
#include <InputSystem/InputSystemDllMacro.h>
#include <InputSystem/InputType.h>
namespace Quad
{
class INPUT_SYSTEM_API IInputHandler
{

  public:
    IInputHandler();
    virtual ~IInputHandler() = 0;

    virtual bool HandleInput(const Quad::RawInputData &inputData) = 0;

    // 마우스캡처를 잃게되었을때 InputSystem이 호출해준다( 명시적으로 캡처를 해제해도 호출)
    virtual void OnMouseCaptureLost() = 0;

  private:
};

} // namespace Quad