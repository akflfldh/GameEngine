#pragma once
#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UILayoutComponent.h>
#include <unordered_map>

#include "UIHorizontalLayoutComponent.generated.h"

namespace UI
{

// 설정 행처럼 여러 자식을 가로로 배치하는 컴포넌트다. 부모의 너비와 자식의 크기·스타일은
// 외부에서 결정하며, 이 컴포넌트는 배치와 행 높이만 관리한다. UI의 소유권은 갖지 않고
// 소유 요소와 자식의 변경 콜백을 구독했다가 OnRemoved에서 해제한다.
class UISYSTEM_API REFLECT_CLASS(EngineClass) UIHorizontalLayoutComponent : public UILayoutComponent
{
    GENERATED_BODY(UIHorizontalLayoutComponent)

  public:
    UIHorizontalLayoutComponent();
    ~UIHorizontalLayoutComponent();

    virtual void Update(float deltaTime) override;
    virtual void CalculateLayout() override;
    virtual void OnTransformChanged(ETransformChangeType type) override;
    virtual void OnRemoved() override;

    // 자식 크기는 생성 시 초기값과 호출부가 결정하므로 일괄 크기 설정은 하지 않는다.
    virtual void SetItemSize(float w, float h) override;

    void SetItemPaddingX(float x);
    void SetItemPaddingTop(float v);

    void SetGlobalPaddingX(float x);

    void SetRightAlign(bool flag);

  protected:
    virtual void OnBegin() override;

  private:
    void RegisterChildCallbacks(UIElement *child);
    void UnRegisterChildCallbacks(UIElement *child);

    Core::CallbackID mOwnerElementOnAddedCallbackID = Core::CallbackIDNone;
    Core::CallbackID mOwnerElementOnRemovedCallbackID = Core::CallbackIDNone;

    std::unordered_map<UIElement *, Core::CallbackID> mSizeCallbackIDTable;
    std::unordered_map<UIElement *, Core::CallbackID> mActiveCallbackIDTable;

    float mGlboalPaddingX = 0.0f;

    // 활성 자식 사이의 가로 간격이며, 양 끝의 여백은 포함하지 않는다.
    float mPaddingX = 0.0f;
    float mPaddingTop = 0.0f;

    // 오른쪽 정렬
    bool mRightAlign = false;
};
} // namespace UI
