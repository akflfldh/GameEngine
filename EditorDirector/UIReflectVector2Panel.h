#pragma once

#include <functional>

#include <CoreMath/CoreMath.h>
#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>
#include <string>

#include "UIReflectVector2Panel.generated.h"

namespace UI
{
class UIText;
class UIEditBox;
} // namespace UI

// Vector2 속성을 X/Y 입력으로 편집하는 에디터 패널이다. 원본 데이터는 소유하지 않고
// getter/setter 또는 reflection으로 연결하며, 풀 반환 시 이전 대상과 commit 콜백을 해제한다.
// 자식 입력 UI의 수명은 기존 Canvas 관리 경로를 따른다.
class REFLECT_CLASS(EngineClass) UIReflectVector2Panel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(UIReflectVector2Panel)
  public:
    using Getter = std::function<CoreMath::Vector2()>;
    using Setter = std::function<void(const CoreMath::Vector2 &)>;
    using CommitNotifier = std::function<void()>;

    UIReflectVector2Panel();
    virtual ~UIReflectVector2Panel();

    void BindVector2(Getter getter, Setter setter);
    void SetCommitNotifier(CommitNotifier notifier);

    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    void RefreshFromSource();
    void SetTagText(const std::string &tag);
    virtual void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    virtual void Release() override;

    void Unbind();
    void ClearDisplay();

  private:
    enum class EAxis : uint8_t
    {
        eX = 0,
        eY
    };

    void OnBeginEdit();
    void OnEndEdit(const std::string &text, EAxis axis);
    void ApplyAxisValue(EAxis axis, float value);
    void SetVector2(const CoreMath::Vector2 &value);
    bool IsSameValue(const CoreMath::Vector2 &lhs, const CoreMath::Vector2 &rhs) const;

    UI::UIText *mTagText = nullptr;
    UI::UIEditBox *mEditBoxX = nullptr;
    UI::UIEditBox *mEditBoxY = nullptr;

    CoreMath::Vector2 mCurrentValue = {0.0f, 0.0f};
    Getter mGetter;
    Setter mSetter;
    CommitNotifier mCommitNotifier;

    void *mTargetMemory = nullptr;
    Quad::PropertyInfo *mPropertyInfo = nullptr;
    bool mIsEditing = false;
    // 초기값이 0이거나 Begin 전에 바인딩되어도 최초 표시 갱신을 생략하지 않는다.
    bool mHasValue = false;
    std::string mTagTextStr;
};
