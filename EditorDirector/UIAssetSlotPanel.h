#pragma once

#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/GlobalOverlayType.h>
#include <UiSystem/UIElement.h>
#include <functional>

namespace UI
{
class UIImage;
class UIText;

} // namespace UI

using OnDroppedAssetCallbackSystem = Core::MultiCallbackSystem<CoreAsset::AssetID>;

class REFLECT_CLASS(EngineClass) UIAssetSlotPanel : public UI::UIElement
{
    GENERATED_BODY(UIAssetSlotPanel)
  public:
    UIAssetSlotPanel();
    virtual ~UIAssetSlotPanel();

    void SetTagText(const std::string &text);
    void SetDragPayloadType(EDragDropType payloadType);

    void SetSlotImage(CoreAsset::Texture *texture);

    virtual void OnTransformChanged(UI::ETransformChangeType type);

    OnDroppedAssetCallbackSystem mOnDroppedAssetCallbackSystem;

    void SetBackgroundColor(float r, float g, float b);

  protected:
    virtual void OnBegin() override;

    void OnDroppedPayload(const DragPayload &payload);

  private:
    UI::UIImage *mBackgroundPanel = nullptr;
    UI::UIText *mTagText = nullptr;
    UI::UIImage *mAssetSlotImagePanel = nullptr;
    EDragDropType mPayloadType;
};
