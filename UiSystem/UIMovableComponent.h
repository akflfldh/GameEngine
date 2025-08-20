#pragma once
#include <ReflectSystem/ReflectionMacro.h>
#include <UiSystem/IUIComponent.h>

#include <UiSystem/Generated/UIMovableComponent.generated.h>

namespace UI
{

class UISYSTEM_API REFLECT_CLASS UIMovableComponent : public IUIComponent
{
    GENERATED_BODY(UIMovableComponent)

  public:
    UIMovableComponent();
    virtual ~UIMovableComponent();

    virtual void Update() override;

    virtual bool IsPointInside(float x, float y) const override;
    virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                       bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag) override;

  private:
    // 캡처상태가아닌 hover상태에서의 업데이트
    void UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext, bool &captureActiveRequestFlag,
                       bool &oCaptureReleaseRequestFlag);

    // 캡처상태일떄의 업데이트
    void UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext, bool &oCaptureActiveRequestFlag,
                         bool &oCaptureReleaseRequestFlag);

    // 얼만큼 마우스가 이동했는지 구하기위한 정보
    float mPreMouseWorldPosX = 0;
    float mPreMouseWorldPosY = 0;
};

} // namespace UI
