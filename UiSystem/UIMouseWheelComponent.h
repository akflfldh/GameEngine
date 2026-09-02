#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/IUIComponent.h>

#include "UIMouseWheelComponent.generated.h"

using OnMouseWheelCallbackSystem = Core::MultiCallbackSystem<int>;

namespace UI
{

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIMouseWheelComponent : public IUIComponent
{

    GENERATED_BODY(UIMouseWheelComponent)

  public:
    virtual void Update(float deltaTime) override;

    virtual void OnMouseWheel(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                              bool &bConsume) override;

    OnMouseWheelCallbackSystem mOnMouseWheelCallbackSystem;

  private:
};

} // namespace UI