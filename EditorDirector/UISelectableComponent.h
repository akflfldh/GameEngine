#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/IUIComponent.h>

#include "UISelectableComponent.generated.h"

using OnSelectedCallbackSystem = Core::MultiCallbackSystem<bool>;

class REFLECT_CLASS(EngineClass) UISelectableComponent : public UI::IUIComponent
{
    GENERATED_BODY(UISelectableComponent)
  public:
    UISelectableComponent();
    virtual ~UISelectableComponent();
    virtual void Update(float deltaTime) override;
    virtual bool IsHovered() const override;
    virtual int IsPointInside(float x, float y) const override;

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;

    OnSelectedCallbackSystem mOnSelectedCallbackSystem;

    void SetSelect(bool bSelected, bool nofity);

  private:
  private:
    bool mHover;
    bool mPress;

    bool mSelect;
};
