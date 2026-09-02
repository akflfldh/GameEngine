#pragma once

#include <functional>

#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>
#include <string>

#include "UIReflectFloatPanel.generated.h"

namespace UI
{
class UIText;
class UIEditBox;
} // namespace UI

class REFLECT_CLASS(EngineClass) UIReflectFloatPanel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(UIReflectFloatPanel)
  public:
    using Getter = std::function<float()>;
    using Setter = std::function<void(float)>;
    using CommitNotifier = std::function<void()>;

  public:
    UIReflectFloatPanel();
    virtual ~UIReflectFloatPanel();

    void BindFloat(Getter getter, Setter setter);
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
    void OnBeginEdit();
    void OnEndEdit(const std::string &text);
    void SetFloat(float value);

    bool IsSameValue(float lhs, float rhs) const;

  private:
    UI::UIText *mTagText = nullptr;
    UI::UIEditBox *mEditBox = nullptr;

    float mCurrentValue = 0.0f;
    Getter mGetter;
    Setter mSetter;
    CommitNotifier mCommitNotifier;

    void *mTargetMemory = nullptr;
    Quad::PropertyInfo *mPropertyInfo = nullptr;
    bool mIsEditing = false;

    std::string mTagTextStr;
};
