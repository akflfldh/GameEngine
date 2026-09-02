#pragma once
#include <UiSystem/IUIComponent.h>

#include "UILayoutComponent.generated.h"

namespace UI
{

class UISYSTEM_API REFLECT_CLASS(EngineClass) UILayoutComponent : public IUIComponent
{
    GENERATED_BODY(UILayoutComponent)
  public:
    UILayoutComponent();
    virtual ~UILayoutComponent() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void CalculateLayout() = 0;

    virtual void SetItemSize(float w, float h) = 0;

  protected:
    // 계산 재귀 방지를 위해사용
    bool mIsCalculating = false;

  private:
};
} // namespace UI
