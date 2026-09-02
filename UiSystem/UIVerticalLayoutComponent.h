#pragma once
#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UILayoutComponent.h>
#include <unordered_map>

#include "UIVerticalLayoutComponent.generated.h"
namespace UI
{

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIVerticalLayoutComponent : public UILayoutComponent
{
    GENERATED_BODY(UIVerticalLayoutComponent)

  public:
    UIVerticalLayoutComponent();
    ~UIVerticalLayoutComponent();

    virtual void Update(float deltaTime) override;

    virtual void CalculateLayout() override;
    virtual void OnTransformChanged(ETransformChangeType type) override;

    virtual void OnRemoved() override;

    // 아무것도하지않음.
    virtual void SetItemSize(float w, float h) override;

    void SetItemPaddingX(float x);

  protected:
    virtual void OnBegin() override;

  private:
    Core::CallbackID mOwnerElementOnAddedCallbackID;
    Core::CallbackID mOwnerElementOnRemovedCallbackID;

    std::unordered_map<UI::UIElement *, Core::CallbackID> mSizeCallbackIDTable;
    std::unordered_map<UI::UIElement *, Core::CallbackID> mActiveCallbackIDTable;

    float mPaddingX = 0.0f;
};
} // namespace UI
