#pragma once

#include <UiSystem/IUIComponent.h>

#include <UiSystem/UISystemDllMacro.h>

// #include <UiSystem/Generated/UIButtonComponent.generated.h>

namespace UI

{

class UIElement;

class UISYSTEM_API REFLECT_CLASS UIButtonComponent : public IUIComponent
{
    GENERATED_BODY(UIButtonComponent)

  public:
    UIButtonComponent();
    virtual ~UIButtonComponent();

    virtual void Update() override;

    virtual bool IsPointInside(float x, float y) const override;

    virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                       bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag) override;

    // 위험한것은 만약 오브젝트나,ui요소들을 캡처한 람다를 넘겨서 호출할떄, 그
    // 오브젝트,ui요소들이 더이상 유효하지않은상황
    // 그것의 책임은 외부에게
    void RegisterOnClickCallback(void *data, void (*onClickCallback)(void *));

  protected:
    void OnClick();
    void SetCaptureAvailable(bool flag);

  private:
    // 캡처상태가아닌 hover상태에서의 업데이트
    void UpdateOnHover(const UIManagerMouseInputContext &mouseInputContext, bool &oCaptureActiveRequestFlag,
                       bool &oCaptureReleaseRequestFlag);

    // 캡처상태일떄의 업데이트
    void UpdateOnCapture(const UIManagerMouseInputContext &mouseInputContext, bool &oCaptureActiveRequestFlag,
                         bool &oCaptureReleaseRequestFlag);

  private:
    // 클릭시 캡처가 가능하도록할것인지의 설정
    // 기본은 true
    bool mCaptureAvailableFlag;
    // 눌렀는지의 여부
    bool mPressedState;

    void *mData;
    void (*mOnClickCallback)(void *);
};
} // namespace UI
