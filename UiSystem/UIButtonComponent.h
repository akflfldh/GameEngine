#pragma once

#include <UiSystem/IUIComponent.h>

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UISystemDllMacro.h>

#include "UIButtonComponent.generated.h"

// mouse x,y
using ButtonComponentCallbackSystem = Core::MultiCallbackSystem<float, float>;

namespace UI

{

class UIElement;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIButtonComponent : public IUIComponent
{
    GENERATED_BODY(UIButtonComponent)

  public:
    UIButtonComponent();
    virtual ~UIButtonComponent();

    virtual void Update(float deltaTime) override;

    virtual int IsPointInside(float x, float y) const override;

    virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                       bool &oCaptureActiveRequestFlag, bool &oCaptureReleaseRequestFlag) override;

    // 위험한것은 만약 오브젝트나,ui요소들을 캡처한 람다를 넘겨서 호출할떄, 그
    // 오브젝트,ui요소들이 더이상 유효하지않은상황
    // 그것의 책임은 외부에게
    // void RegisterOnClickCallback(void *data, void (*onClickCallback)(void *));

    ButtonComponentCallbackSystem mButtonClickCallbackSystem;

    // TO DO 컴포넌트들, Input 구현
#pragma region Input
    //   virtual void HandleInput(const Quad::InputData &inputData, bool &bConsume);

    virtual void OnHover(int x, int y);
    virtual void OnReleaseHover();

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;
    // virtual void OnMouseClick(const Quad::InputData &inputData, bool &bConsume) override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;
    //    virtual void OnKeyDown(const Quad::InputData &inputData, bool &bConsume) override;

    virtual bool IsHovered() const override;
    virtual void OnChangeHoverPart(int before, int after) override;

#pragma endregion

  protected:
    void OnClick();
    void SetCaptureAvailable(bool flag);

  private:
  private:
    // 클릭시 캡처가 가능하도록할것인지의 설정
    // 기본은 true
    //  bool mCaptureAvailableFlag;
    // 눌렀는지의 여부
    bool mPressedState;

    //  void *mData;
    // void (*mOnClickCallback)(void *);

    bool mHover;
    bool mPress;
};
} // namespace UI
