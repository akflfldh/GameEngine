#pragma once

#include <EditorDirector/IPropertyBindable.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIType.h>
#include <string>

#include "UIReflectSinglePrimitivePanel.generated.h"

namespace UI
{
class UIText;
class UIEditBox;
} // namespace UI

class REFLECT_CLASS(EngineClass) UIReflectSinglePrimitivePanel : public UI::UIImage, public IPropertyBindable
{
    GENERATED_BODY(UIReflectSinglePrimitivePanel)
  public:
    UIReflectSinglePrimitivePanel();
    virtual ~UIReflectSinglePrimitivePanel();

    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    void SetTagText(const std::string &tag);
    void SetTextFilterMode(UI::EUITextInputType type);

    virtual void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    virtual void Release() override;

    UI::UIEditBox *mEditBox;

  private:
    UI::UIText *mTagText;

    void *mTargetMemory;
    Quad::PropertyInfo *mPropertyInfo;
    std::function<void(const std::string &)> mTypeSpecificSetter;
};
