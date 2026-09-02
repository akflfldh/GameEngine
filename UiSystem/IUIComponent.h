#pragma once

#include <ReflectSystem/ReflectionMacro.h>

#include "UiSystem/UIType.h"
#include <CoreBase/BaseClass.h>
#include <UiSystem/UIElementTyprDef.h>

#include "IUIComponent.generated.h"

namespace UI
{
class UIElement;

class UISYSTEM_API REFLECT_CLASS(EngineClass) IUIComponent : public BaseClass
{
    friend class UIComponentFactory;

    GENERATED_BODY(IUIComponent)

  public:
    IUIComponent();
    virtual ~IUIComponent() = 0;

    void Begin();
    virtual void Update(float deltaTime) = 0;

    const char *GetComponentName() const;

    UIElement *GetOwnerUIElement() const;

    // 기본은 단순히 false 리턴
    // (오버라이드할시 IUIComponent 버전은 호출할필요가없음)
    // 0 : NoHit,  1~ : Hit(각번호는 컴포넌트내에서 구분하는 세부 영역번호)
    virtual int IsPointInside(float x, float y) const;
    // 빈메서드 (오버라이드할시 IUIComponent 버전은 호출할필요가없음)
    virtual void UpdateMouseInputEvent(const UIManagerMouseInputContext &mouseInputContext,
                                       bool &captureActiveRequestFlag, bool &oCaptureReleaseRequestFlag);
    virtual void Serialize(Arch &arch) override;

    void SetDepthValue(uint32_t depth);
    uint32_t GetDepthValue() const;

    const UIComponentHandle &GetHandle() const;

    // 해당 owner ui에 canvas가 설정되면 호출
    virtual void OnOwnerAddedToCavas();

#pragma region Input
    virtual void HandleInput(const Quad::RawInputData &inputData, bool &bConsume);

    void SetInputEnable(bool flag);
    bool GetInputEnable() const;

    virtual void OnHover(int x, int y);
    virtual void OnReleaseHover();

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY);
    virtual void OnMouseClick(const Quad::RawInputData &inputData, bool &bConsume);
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY, bool &bConsume);
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY, bool &bConsume);
    virtual void OnMouseWheel(const Quad::RawInputData &inputData, float worldPosX, float worldPosY, bool &bConsume);

    virtual void OnKeyDown(const Quad::EKeyCode &key, bool &bConsume);
    virtual void OnKeyUp(const Quad::EKeyCode &key, bool &bConsume);
    virtual void OnChar(uint32_t ch, bool &bConsume);

    virtual void OnPreviewMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                    bool &bSteal);
    virtual void OnPreviewMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY, bool &bSteal);

    // 기본 false 리턴
    virtual bool IsHovered() const;

    virtual void OnChangeHoverPart(int before, int after);

    // 기본 0(No Hit)
    int GetHoverPart() const;
    void SetHoverPart(int num);

    void RequestMouseCaptureInput();
    void ReleaseMouseCaptureInput();

    void RequestKeyboardCaptureInput();
    void ReleaseKeyboardCaptureInput();

    // ownerElement가 focus됬을때 모든 com에대하여 호출된다.
    virtual void OnSetMouseFocus(bool isRequester);
    // ownerElement가 focus에서 해제되었을때 호출된다.
    virtual void OnLostMouseFocus();
    // 동일
    virtual void OnSetKeyboardFocus(bool isRequeste);
    virtual void OnLostKeyboardFocus();

    virtual void OnRemoved() {}

    void SetPreviewInputListener(bool flag);
    bool GetPreviewInputListener() const;
#pragma endregion

    virtual void OnTransformChanged(ETransformChangeType type) {};

  protected:
    virtual void OnBegin() {};

    // 기본 uiElement영역에대한 메서드
    bool IsPointInsideDefault(float x, float y) const;

  private:
    // 이 컴포넌트를 소유한 uielement, factory가 호출할것이다.
    void SetOwnerUIElement(UIElement *owner);

    void SetUIComponentHandle(const UIComponentHandle &handle);

  private:
    const char *mName;
    UIElement *mOwnerUIElement;
    // com사이에서 유효한 깊이값
    uint32_t mDepthValue;

    UIComponentHandle mUIComponentHandle;

    // 마우스가 위치한 세부영역번호
    int mMouseHitSectionNum;

    bool mOnwerHasMouseCapture = false;
    bool mOwnerHasKeyboardCapture = false;

    bool mInputEnable = true;

    bool mPreviewInputListener = false;
};

} // namespace UI
