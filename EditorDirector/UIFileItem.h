#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIElement.h>

#include "UIFileItem.generated.h"

namespace UI
{
class UIImageComponent;
class UIImage;
class UIText;
class UIEditBox;
class UIDoubleClickComponent;
class UIButtonComponent;
} // namespace UI

namespace QuadLF
{
class LogicalNode;
}

class UIFileItem;

using OnFileOpenedCallbackSystem = Core::MultiCallbackSystem<QuadLF::LogicalNode *>;

class UIDragSourceComponent;
class UISelectableComponent;
class REFLECT_CLASS(EngineClass) UIFileItem : public UI::UIElement
{
    GENERATED_BODY(UIFileItem)
  public:
    UIFileItem();
    virtual ~UIFileItem();
    virtual void OnBegin() override;

    void SetLogicalFileNode(QuadLF::LogicalNode *node);

    void SetFileImageSizeRatio(float w, float h);
    void SetFileTextSizeRatio(float w, float h);
    void SetFileTextPosRatio(float x, float y);

    virtual void OnTransformChanged(UI::ETransformChangeType type) override;
    UISelectableComponent *GetSelectableComponent() const;
    OnFileOpenedCallbackSystem mOnFileOpendCallbackSystem;

    UI::UIButtonComponent *mFileTextButtonComponent;
    UI::UIText *mFileTextElement;

  private:
    void HandleDoubleClick();
    void OnSelected(bool state);

  private:
    UI::UIImageComponent *mBackImageCom;
    UI::UIImageComponent *mSelectOverlayImageCom;
    UI::UIDoubleClickComponent *mDoubleClickCom;
    UI::UIImage *mFileImageElement;
    UIDragSourceComponent *mDragDropCom;
    UISelectableComponent *mSelectableCom;

    QuadLF::LogicalNode *mFileNode;

    // 범위[0  1]
    float mFileImageWidthRatio = 0.6F;
    float mFileImageHeightRatio = 0.6F;

    float mFileTextWidthRatio = 0.7f;
    float mFileTextHeightRatio = 0.4f;

    float mFileTextPosXRatio = 0.0f;
    float mFileTextPosYRatio = 0.7f;
};
