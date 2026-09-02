#pragma once

#include "CoreAsset/Asset.h"

#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/Texture.h"
#include <RenderSystem/MaterialType.h>
#include <vector>

namespace CoreAsset
{
class Texture;

struct CORE_ASSET_API AssetMaterialTexResourceContext
{
    Render::TextureShaderResourceInfo mTexShaderResourceInfo;
    AssetPtr mTexture = nullptr;

    void Serialize(Arch &arch);
};

class CORE_ASSET_API Material : public Asset
{
    friend class MaterialManager;
    friend class UIMaterialManager;
    friend class MaterialFactory;

  public:
    Material(AssetID id = NoneAssetID);
    virtual ~Material();
    Render::MaterialID GetGpuMaterialID() const
    {
        return mGpuMaterialID;
    }

    static EAssetType GetAssetType()
    {
        return EAssetType::eMaterial;
    }

    void SetMaterialHandle(uint32_t handle);
    uint32_t GetMaterialHandle() const;

    const std::vector<AssetMaterialTexResourceContext> &GetTexResourceContextList() const;
    const std::vector<uint32_t> &GetSamplerResourceContextList() const;

    void SetTextureResource(int index, Texture *tex);
    void SetTextureResource(int index, CoreAsset::AssetID tex);

    void SetSamplerResource(int index, uint32_t samplerID);

    virtual void Serialize(Arch &arch) override;

    void SetGpuMaterial(Render::MaterialID id);

    std::vector<AssetMaterialTexResourceContext> &GetAlbedoTexResourceList();
    const std::vector<AssetMaterialTexResourceContext> &GetAlbedoTexResourceList() const;
    AssetMaterialTexResourceContext &GetNormalTexResource();
    const AssetMaterialTexResourceContext &GetNormalTexResource() const;

    void SetAlbedoMap(int index, Texture *texture);

    void SetNormalMapTexture(Texture *tex);

    bool HasNormalMap() const;
    void SetHasNormalMap(bool flag);

    void AddAlbedoTexSlot();

    void SetMetallic(float value);
    void SetRoughness(float value);

    float GetMetallic() const;
    float GetRoughness() const;

    void SetUploadDirty();
    void ClearUploadDirty();
    bool GetUploadDirty() const;

    void SetDiffuseColor(const CoreMath::Vector3 &color);
    CoreMath::Vector3 GetDiffuseColor() const;

    void SetDiffuseFactor(float factor);
    void SetSpecular(const CoreMath::Vector3 &specular);
    void SetSpecularFactor(float factor);
    void SetAmbient(const CoreMath::Vector3 &ambient);

    float GetDiffuseFactor() const;
    CoreMath::Vector3 GetSpecular() const;
    float GetSpecularFactor() const;
    CoreMath::Vector3 GetAmbient() const;

    void SetUseExplicitGpuMaterial(bool flag);
    bool GetUseExplicitGpuMaterial() const;

    // Material 속성들을 복사
    void CopyProperty(Material &targetMaterial);

    void SetShadingModel(EShadingModel shadingMode);
    EShadingModel GetShadingMode() const;

  private:
    Render::MaterialID mGpuMaterialID = MaterialIDNone; // GPU 머터리얼 ID

    uint32_t mMaterialHandle = 0;

    // asset material 수준에서 설정되는 텍스처 속성들
    std::vector<AssetMaterialTexResourceContext> mTexResourceContextList;

    std::vector<AssetMaterialTexResourceContext> mAlbedoResourceContextList;
    AssetMaterialTexResourceContext mNormalMapResourceContext;

    bool mHasNormalMap = false;

    // sampler id
    std::vector<uint32_t> mSamplerResourceContextList;

    bool mUploadDirty = true;

    // 0 :비금속 , 1: 금속, 0.5는 반반
    // HLSL에서 (0.4,0.4,0.4) ~ Albedo 값사이에서 보간
    float mMetallic = 0.0f;
    float mRoughness = 0.4f;

    CoreMath::Vector3 mDiffuseColor;
    float mDiffuseFactor = 1.0f;
    CoreMath::Vector3 mSpecular;
    float mSpecularFactor = 1.0f;
    CoreMath::Vector3 mAmbient = {1, 1, 1};

    bool mUseExplicitGpuMaterial = false;

    EShadingModel mShadingModel = EShadingModel::eNone;
};

AssetClassName(Material)
} // namespace CoreAsset

Arch &operator<<(Arch &arch, CoreAsset::AssetMaterialTexResourceContext &assetMaterialTexResourceContext);
