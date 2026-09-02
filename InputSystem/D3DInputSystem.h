#pragma once

#include "InputSystem/InputSystem.h"

#ifdef D3DX
#include <Windows.h>
namespace Quad
{

class D3DInputSystem : public InputSystem
{
  public:
    virtual void Update() override;

    void OnMouseAction(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual void EndFrame() override;
    virtual uint8_t toScanKey(uint8_t vk) const override;
    virtual uint8_t toVirtualKey(uint8_t scanKey) const override;

  private:
};

} // namespace Quad

#endif