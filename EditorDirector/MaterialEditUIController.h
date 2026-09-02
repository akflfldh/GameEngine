#pragma once

#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/AssetType.h>

namespace CoreAsset
{
class Material;
}
namespace UI
{
class UICanvas;
}

class UIScrollBox;
class UIAssetSlotListPanel;
class UIReflectFloatPanel;
class UIReflectVector3Panel;

struct MaterialEditData
{
    CoreMath::Vector3 mDiffuseColor;
    float mDiffuseFactor = 1.0f;

    CoreMath::Vector3 mSpecular;
    float mSpecularFactor = 1.0f;

    float mMetallic = 0.0f;
    float mRoughness = 0.4f;

    bool mUseExplicitGpuMaterial = false;

    std::vector<CoreAsset::AssetPtr> mAlbedoTextures;
    CoreAsset::AssetPtr mNormalTexture;
    bool mHasNormalMap = false;
    // std::vector<uint32_t> mSamplerResources;
};

class MaterialEditUIController
{
  public:
    MaterialEditUIController();
    ~MaterialEditUIController();

    void Initialize(UI::UICanvas *canvas, CoreAsset::Material *previewMaterial);
    void BeginUI(float panelPosY);

    void SetTargetMaterial(CoreAsset::Material *material);

    void ReBuild();

  private:
    void Clear();

    void OnClickedDiffuseMapAddButtotn();
    void OnDiffuseMapDropped(CoreAsset::AssetID id, int index);

    //    void CopyTargetMaterial();

    void BuildMaterialEdtiData(const CoreAsset::Material &material, MaterialEditData &oEditData);
    void ApplyEditDataToMaterial(const MaterialEditData &editData, CoreAsset::Material &material);

    // EditData를 previewMaterial 에 적용
    void OnClickedApplyEditDataButton();

    CoreMath::Vector3 GetDiffuseColor() const;
    float GetDiffuseFactor() const;
    float GetRoughness() const;

    void SetDiffuseColor(const CoreMath::Vector3 &value);
    void SetDiffuseFactor(float value);
    void SetRoughness(float value);

    UIReflectVector3Panel *CreateVector3ReflectPanel(const std::string &tagText,
                                                     std::function<CoreMath::Vector3()> getter,
                                                     std::function<void(const CoreMath::Vector3 &)> setter);

    UIReflectFloatPanel *CreateFloatReflectPanel(const std::string &tagText, std::function<float()> getter,
                                                 std::function<void(float)> setter);

  private:
    bool mBegun = false;
    UI::UICanvas *mCanvas = nullptr;
    CoreAsset::Material *mTargetMaterial = nullptr;
    CoreAsset::Material *mPreviewMaterial = nullptr;
    UIScrollBox *mScrollBox = nullptr;
    UIAssetSlotListPanel *mDiffuseMapListSlotPanel = nullptr;

    MaterialEditData mMaterialEditData;

    UIReflectVector3Panel *mDiffuseColorReflectPanel = nullptr;
    UIReflectFloatPanel *mDiffuseFactorReflectPanel = nullptr;
    UIReflectFloatPanel *mRoughnessReflectPanel = nullptr;
};
