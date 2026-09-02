#pragma once
#include <UiSystem/UILayoutComponent.h>

#include "UIGridLayoutComponent.generated.h"

// OwnerElement의 너비에 따라  높이를  컴포넌트가 자동으로 강제 설정한다.

class REFLECT_CLASS(EngineClass) UIGridLayoutComponent : public UI::UILayoutComponent
{
    GENERATED_BODY(UIGridLayoutComponent)
  public:
    UIGridLayoutComponent();
    virtual ~UIGridLayoutComponent();

    virtual void Update(float deltaTime) override;
    virtual void OnTransformChanged(UI::ETransformChangeType type) override;

    virtual void CalculateLayout() override;

    // void SetCellSize(float width, float height);
    void SetSpacing(float spacing);

    void SetMinHeight(float height);

    void SetItemSize(float w, float h) override;

  private:
    float mCellWidth = 100.0f;
    float mCellHeight = 150.0f;
    float mSpacing = 20.0f;

    float mMinHeight = 300.0f;
};
