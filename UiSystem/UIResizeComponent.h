#pragma once

#include "UiSystem/IUIComponent.h"

namespace UI
{
class UIResizeComponent : public IUIComponent
{
  public:
    UIResizeComponent();
    virtual ~UIResizeComponent();

    virtual void Update() override;

    // 마우스입력
    // hover(색변화), 클릭, 드래그(ui의 사이즈변화)
    //  UIResizeGizmoRenderableComponent와의 연결 당연히 필요(사실상 UIResizeGizmoRenderableComponent 이친구가 종속되어
    //  UIReszieComponent추가시 자동으로 UIElement에게 UIResizeGizmoRenderableComponent도 추가 , 또한UIReszieComponent
    //  가 UIResizeGizmoRenderableComponent를 알고있는상태가 됨 )
    //
    //
    //
    // virtual bool IsPointInside(float x, float y) const override;

    //// 사이즈조절 4개의 막대들의 정점(위치)를 평가한다.
    // void EvaluateResizeStickRegion(UI::UIVertex *oUIVertices) const;
    // virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext) override;

  private:
    // 수치상으로만 범위를 가진다.
    int mBorderThickness;
};

} // namespace UI