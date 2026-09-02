#pragma once
#include <ReflectSystem/ReflectionMacro.h>
#include <UiSystem/IUIComponent.h>

#include <UIMovableComponent.generated.h>

namespace UI
{

class UIMultiRectComponent;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIMovableComponent : public IUIComponent
{
    GENERATED_BODY(UIMovableComponent)

  public:
    UIMovableComponent();
    virtual ~UIMovableComponent();

    virtual void Update(float deltaTime) override;

    virtual int IsPointInside(float x, float y) const override;
    virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                       bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag) override;

#pragma region Input
    virtual void HandleInput(const Quad::RawInputData &inputData, bool &bConsume) override;

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;
    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;
    virtual void OnMouseClick(const Quad::RawInputData &inputData, bool &bConsume) override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;
#pragma endregion

  private:
    // 캡처상태가아닌 hover상태에서의 업데이트
    void UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext, bool &captureActiveRequestFlag,
                       bool &oCaptureReleaseRequestFlag);

    // 캡처상태일떄의 업데이트
    void UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext, bool &oCaptureActiveRequestFlag,
                         bool &oCaptureReleaseRequestFlag);

    // 얼만큼 마우스가 이동했는지 구하기위한 정보
    float mMouseDownOffsetX = 0;
    float mMouseDownOffsetY = 0;

    bool bHover;
    bool bPress;
};

} // namespace UI
