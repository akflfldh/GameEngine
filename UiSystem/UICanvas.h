#pragma once

#include "UiSystem/UIType.h"
#include <CoreMath/CoreMath.h>
#include <string>
#include <vector>

namespace UI
{

class UIElement;

class UISYSTEM_API UICanvas
{
    friend class UIManager;

  public:
    UICanvas(UICanvasID id, const std::string &name, ECanvasSizeMode sizeMode);
    ~UICanvas();

    void Update(float deltaTime);

    void AddChild(UIElement *uiElement);

    ECanvasSizeMode GetSizeMode() const;

    void SetSize(CoreMath::Vector2 size);
    bool GetActiveFlag() const;
    const std::vector<UIElement *> &GetChildUIElementAll() const;

    void SetDepthValue(uint32_t value);
    uint32_t GetDepthValue() const;

    uint32_t GetTopUIElementDepthValue() const;

    void SetUIElementTopDepth(UIElement *uiElement);

    template <typename T> T *CreateUIElement(const char *instanceName);

  private:
    void AddChildInternal(UIElement *uiElement);

    UIElement *CreateUIElement(const char *className, const char *instanceName);

  private:
    UICanvasID mID;
    std::string mName;
    bool mActiveFlag;

    ECanvasSizeMode mCanvasSizeMode;

    std::vector<UIElement *> mChildUIElement; // 최상위UIElement들

    CoreMath::Vector2 mSize;

    uint32_t mDepthValue;
    // 현재 가장 위에있는 요소깊이값. 다음번 새로운 요소가 가장위로올라와야한다. 이값 +1 을가지게될것이다.
    uint32_t mTopUIElementDepthValue;
};
template <typename T> T *UI::UICanvas::CreateUIElement(const char *instanceName)
{

    return static_cast<T *>(CreateUIElement(T::GetStaticClassName(), instanceName));


}
} // namespace UI
