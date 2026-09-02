#pragma once

#include <EditorDirector/UIAssetSlotPanel.h>

#include "UIReflectTextureSlotPanel.generated.h"

class REFLECT_CLASS(EngineClass) UIReflectTextureSlotPanel : public UIAssetSlotPanel
{
    GENERATED_BODY(UIReflectTextureSlotPanel)
  public:
    using Getter = std::function<CoreAsset::AssetID()>;
    using Setter = std::function<void(CoreAsset::AssetID)>;

    UIReflectTextureSlotPanel();
    virtual ~UIReflectTextureSlotPanel();

    void BindTexture(Getter getter, Setter setter);
    void RefreshFromSource();

    virtual void OnBegin() override;

  private:
    void OnDroppedTexture(CoreAsset::AssetID textureID);

  private:
    Getter mGetter;
    Setter mSetter;
};
