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

class UIDropTargetComponent;

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
    // style일변화 , hover,등 상태변화 에서 호출
    virtual void ApplyVisualStyle(const UI::UIControlStyle &style, UI::EUIVisualState visualState) override;

  private:
    UI::UIImage *mBackgroundPanel = nullptr;
    UI::UIText *mTagText = nullptr;
    UI::UIImage *mAssetSlotImagePanel = nullptr;
    EDragDropType mPayloadType = EDragDropType::eNone;
    UIDropTargetComponent *mDropTargetCom = nullptr;
};
