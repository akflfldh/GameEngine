#pragma once

#include <functional>

#include <CoreMath/CoreMath.h>
#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>
#include <string>

#include "UIReflectVector3Panel.generated.h"

namespace UI
{
class UIText;
class UIEditBox;
} // namespace UI

class REFLECT_CLASS(EngineClass) UIReflectVector3Panel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(UIReflectVector3Panel)
  public:
    using Getter = std::function<CoreMath::Vector3()>;
    using Setter = std::function<void(const CoreMath::Vector3 &)>;
    using CommitNotifier = std::function<void()>;

  public:
    UIReflectVector3Panel();
    virtual ~UIReflectVector3Panel();

    void BindVector3(Getter getter, Setter setter);
    void SetCommitNotifier(CommitNotifier notifier);

    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    void RefreshFromSource();

    void SetTagText(const std::string &tag);
    virtual void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    virtual void Release() override;

    void Unbind();
    void ClearDisplay();

    // X, Y, Z 중 하나라도 바뀌면 외부로 쏴줄 통합 콜백 시스템
    //  Core::MultiCallbackSystem<const CoreMath::Vector3 &> mOnValueChanged;

  private:
    enum class EAxis : uint8_t
    {
        eX = 0,
        eY,
        eZ
    };

    void OnBeginEdit();
    void OnEndEdit(const std::string &text, EAxis axis);
    void ApplyAxisValue(EAxis axis, float value);
    void SetVector3(const CoreMath::Vector3 &val);

    bool IsSameValue(const CoreMath::Vector3 &lhs, const CoreMath::Vector3 &rhs) const;

  private:
    UI::UIText *mTagText;
    UI::UIEditBox *mEditBoxX;
    UI::UIEditBox *mEditBoxY;
    UI::UIEditBox *mEditBoxZ;

    CoreMath::Vector3 mCurrentValue;
    Getter mGetter;
    Setter mSetter;
    CommitNotifier mCommitNotifier;

    void *mTargetMemory = nullptr;
    Quad::PropertyInfo *mPropertyInfo = nullptr;
    bool mIsEditing = false;

    std::string mTagTextStr;
};
