#pragma once
#include <stdint.h>
enum class EInputState : uint32_t
{
    eNone = 0,
    eKeyDown = 1,
    eKeyUp = 1 << 1,
    eKeyHeld = 1 << 2,
    eKeyChar = 1 << 3,
    eMouseMove = 1 << 4,
    eMouseLButtonDown = 1 << 5,
    eMouseLButtonUp = 1 << 6,
    eMouseLButtonHeld = 1 << 7,
    eMouseRButtonDown = 1 << 8,
    eMouseRButtonUp = 1 << 9,
    eMouseRButtonHeld = 1 << 10,
    eMouseWheel = 1 << 11
};

inline EInputState operator|(EInputState a, EInputState b)
{
    return static_cast<EInputState>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline EInputState &operator|=(EInputState &a, EInputState b)
{
    a = a | b;
    return a;
}

inline bool operator&(EInputState a, EInputState b)
{
    return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
}

inline bool operator==(EInputState a, uint32_t value)
{
    return static_cast<uint32_t>(a) == value;
}

inline bool operator!=(EInputState a, uint32_t value)
{
    return !operator==(a, value);
}

inline bool operator>(EInputState a, uint32_t value)
{
    return static_cast<uint32_t>(a) > value;
}

inline bool operator>=(EInputState a, uint32_t value)
{
    return static_cast<uint32_t>(a) >= value;
}

class IWindowEventHandler
{

  public:
    virtual void OnInput() = 0; // input이 들어왔을때 가장먼저 호출되는 메서드
    virtual void SetMousePos(int sx, int sy, int cx, int cy) = 0;
    virtual void OnMouseMove(int deltaX, int deltaY) = 0;
    virtual void OnMouseWheel(int wheelDelta) = 0;

    virtual void OnWindowMinimized() = 0;
    virtual void OnWindowRestored() = 0;
    virtual void OnWindowResize(unsigned int width, unsigned int height) = 0;
    virtual void OnMouseButtonEvent(EInputState mouseButtonState) = 0;

    virtual void OnKeyEvent(EInputState keyState, uint8_t scanKey) = 0;
    virtual void OnCharEvent(uint32_t ch) = 0;

  private:
};