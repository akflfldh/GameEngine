#pragma once

#include "UiSystem/UIRenderableComponent.h"

#include "UiSystem/Generated/UIResizeGizmoRenderableComponent.generated.h"
#include <CoreMath/CoreMath.h>

namespace UI
{

// ui요소에 4개의 모서리를 렌더하는기능이 제공된다 (가장 overlay가 높다.(resize용이기에))
class UISYSTEM_API REFLECT_CLASS UIResizeGizmoRenderableComponent : public UIRenderableComponent
{
    GENERATED_BODY(UIResizeGizmoRenderableComponent)

  public:
    UIResizeGizmoRenderableComponent();
    virtual ~UIResizeGizmoRenderableComponent();

    virtual void Update() override;

    void SetColor(const CoreMath::Vector4 &color);
    const CoreMath::Vector4 &GetColor() const;

    void SetThickness(float thickness);
    float GetThickness() const;

    // 버텍스버퍼의 크기는 16이상
    inline virtual size_t GetVertexNum() const override;
    virtual void GetVertices(UIVertex *oUIVertices) const override;

    inline virtual size_t GetIndexNum() const override;
    virtual void GetIndices(uint32_t *oIndices) const override;

    virtual bool IsPointInside(float x, float y) const override;

    virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                       bool &captureActiveRequestFlag, bool &oCaptureReleaseRequestFlag) override;

  private:
    // 주어진 좌표 ,x,y를 포함하는 활성화할 border 평가
    int EvaluateActiveBorder(float x, float y) const;

    // 캡처상태가아닌 hover상태에서의 업데이트
    void UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext, bool &captureActiveRequestFlag,
                       bool &oCaptureReleaseRequestFlag);

    // 캡처상태일떄의 업데이트
    void UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext, bool &captureActiveRequestFlag,
                         bool &oCaptureReleaseRequestFlag);

  private:
    // 테투리의 두께
    float mThickness;
    CoreMath::Vector4 mColor;
    CoreMath::Vector4 mActiveColor; // 활성화된 모서리의 색
    int mActiveBorder;              // 활성화(클릭,드래그)된 모서리넘버 //  시계방향으로 0 1 2 3     //-1은 없다.
};

} // namespace UI