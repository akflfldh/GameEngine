#pragma once

#include <UiSystem/IUIComponent.h>

#include <CoreBase/CallbackSystem.h>

using OnInputEnterCallbackSystem = Core::MultiCallbackSystem<const std::string &>;
using OnInputStartCallbackSystem = Core::MultiCallbackSystem<>;
using OnInputTextChangedCallbackSystem = Core::MultiCallbackSystem<const std::string &>;

namespace UI
{
class UITextComponent;
class UIMultiRectComponent;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UITextInputComponent : public IUIComponent
{
    GENERATED_BODY(UITextInputComponent)

  public:
    UITextInputComponent();
    virtual ~UITextInputComponent();

    virtual void Update(float deltaTime) override;

    // 외부에서 강제로 포커스 요청 함수
    void RequestKeyboardFocus();
    void ReleaseKeyboardFocus();

    virtual void OnSetMouseFocus(bool isRequester) override;
    // ownerElement가 focus에서 해제되었을때 호출된다.
    virtual void OnLostMouseFocus() override;
    virtual void OnSetKeyboardFocus(bool isRequeste) override;
    virtual void OnLostKeyboardFocus() override;
    virtual int IsPointInside(float x, float y) const override;

#pragma region Input
    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;

    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;

    virtual void OnKeyDown(const Quad::EKeyCode &key, bool &bConsume) override;
    void OnChar(uint32_t ch, bool &bConsume) override;
    virtual bool IsHovered() const override;

#pragma endregion

    // cursor의 index 설정, updateCursorPos수행
    void SetCursorPos(float localX, float localY);
    void SetCursorPos(const glm::vec2 &localPos);

    void ResetCursorBlinkTimer();

    void SetInputType(EUITextInputType inputType);
    EUITextInputType GetTextInputType() const;

    OnInputEnterCallbackSystem mOnEnterInputCallbackSystem;
    OnInputStartCallbackSystem mOnStartInputCallbackSystem;

    // 입력을 통해서만 바뀔때 호출
    OnInputTextChangedCallbackSystem mOnTextChangedCallbackSystem;

  protected:
    virtual void OnBegin() override;

    void UpdateCursorPosition();

    // 현재 cursorIndex에 삽입
    void InsertChar(char ch);

  private:
    void ReleaseSelectedRegion();

    // 선택된 영역을 지우고 나서 커서는 항상 선택했던 영역 맨앞으로 설정
    void RemoveSelectedRegion(std::string &text);

    bool CheckFilter(char ch);

  private:
    UITextComponent *mUITextCom;
    UIMultiRectComponent *mMultiRectCom;
    EUITextInputType mInputType = EUITextInputType::eString;

    bool mIsFocused;
    bool mHover;

    int mCursorIndex;
    int mCursorSelectionStartIndex;

    float mCursorBlinkTime;
    const float mCursorBlinkTimeInterval;

    bool mIsDragging;
    bool mIsSelected; // 드래그로 영역을 선택했는가.
};
} // namespace UI