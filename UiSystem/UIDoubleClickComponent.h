#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/IUIComponent.h>

#include "UIDoubleClickComponent.generated.h"

using OnDoubleClickedCallbackSystem = Core::MultiCallbackSystem<>;

namespace UI
{
class UISYSTEM_API REFLECT_CLASS(EngineClass) UIDoubleClickComponent : public IUIComponent
{
    GENERATED_BODY(UIDoubleClickComponent)
  public:
    UIDoubleClickComponent();
    virtual ~UIDoubleClickComponent();
    virtual void Update(float deltaTime) override;
    virtual int IsPointInside(float x, float y) const override;

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;
    virtual bool IsHovered() const override;

    void SetDoubleClickUpperDeltaTime(float deltaTime);

    OnDoubleClickedCallbackSystem mOnDoubleClickedCallbackSystem;

  private:
    bool mHover;
    float mLastClickTime;
    float mDoubleClickUpperDeltaTime = 0.3f;
}; // namespace UI
} // namespace UI
