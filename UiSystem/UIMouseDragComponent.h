#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/IUIComponent.h>

#include "UIMouseDragComponent.generated.h"

namespace UI
{
struct UIMouseDragContext
{
    float mDeltaX = 0.0f;
    float mDeltaY = 0.0f;
};

using OnDragStartedCallbackSystem = Core::MultiCallbackSystem<>;
using OnDragEndedCallbackSystem = Core::MultiCallbackSystem<>;
using OnDraggedCallbackSystem = Core::MultiCallbackSystem<const UIMouseDragContext &>;
using OnHoverCallbackSystem = Core::MultiCallbackSystem<>;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIMouseDragComponent : public IUIComponent
{
    GENERATED_BODY(UIMouseDragComponent)

  public:
    UIMouseDragComponent();
    virtual ~UIMouseDragComponent();

    virtual void Update(float deltaTime) override;
    virtual int IsPointInside(float x, float y) const override;
    bool IsHovered() const;

    virtual void OnHover(int x, int y);
    virtual void OnReleaseHover();

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;
    // virtual void OnMouseClick(const Quad::InputData &inputData, bool &bConsume) override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;

    OnHoverCallbackSystem mOnHoverCallbackSystem;

    // press, drag상태에서는 호출되지않는다.
    OnHoverCallbackSystem mOnReleaseHoverCallbackSystem;
    OnDragStartedCallbackSystem mOnDragStartedCallbackSystem;
    OnDragEndedCallbackSystem mOnDragEndededCallbackSystem;
    OnDraggedCallbackSystem mOnDraggedCallbackSystem;

  private:
    bool mHover = false;
    bool mDragging = false;
    bool mPressed = false;
    CoreMath::Vector2 mStartPosition;
    CoreMath::Vector2 mPrevPosition;
}; // namespace UI
} // namespace UI