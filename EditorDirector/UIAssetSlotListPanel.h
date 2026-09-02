#pragma once

#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/GlobalOverlayType.h>
#include <UiSystem/UIElement.h>
#include <vector>

using OnClickedAddButtonCallbackSystem = Core::MultiCallbackSystem<>;
// asset id, slot panel index
using OnDroppedAssetIntoAssetSlotPanelCallbackSystem = Core::MultiCallbackSystem<CoreAsset::AssetID, int>;

namespace UI
{
class UIButton;
}

class UIAssetSlotPanel;

class REFLECT_CLASS(EngineClass) UIAssetSlotListPanel : public UI::UIElement
{
    GENERATED_BODY(UIAssetSlotListPanel)
  public:
    UIAssetSlotListPanel();
    virtual ~UIAssetSlotListPanel();

    void ActiveAssetSlotPanels(int num);

    OnClickedAddButtonCallbackSystem mOnClickedAddButtonCallbackSystem;
    OnDroppedAssetIntoAssetSlotPanelCallbackSystem mOnDroppedAssetCallbackSystem;

    void SetAssetSlotImage(CoreAsset::Texture *texture, int index);

    void SetTagText(const std::string &text);
    void SetDragPayloadType(EDragDropType payloadType);

    size_t GetSlotNum() const;

  protected:
    virtual void OnBegin() override;

  private:
    void AddSlotPanel();
    void OnClickedAddButton();
    void OnDroppedAssetFileItem(CoreAsset::AssetID id, UIAssetSlotPanel *panel);

  private:
    std::vector<UIAssetSlotPanel *> mSlotPanels;
    UI::UIButton *mAddButton = nullptr;
    std::string mTagText;
    EDragDropType mPayloadType;
};
