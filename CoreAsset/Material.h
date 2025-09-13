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
    virtual ~Material();
    Render::MaterialID GetGpuMaterialID() const
    {
        return mGpuMaterialID;
    }

    static EAssetType GetAssetType()
    {
        return EAssetType::eMaterial;
    }

    const std::vector<AssetMaterialTexResourceContext> &GetTexResourceContextList() const;
    const std::vector<uint32_t> &GetSamplerResourceContextList() const;

    void SetTextureResource(int index, Texture *tex);
    void SetTextureResource(int index, CoreAsset::AssetID tex);

    void SetSamplerResource(int index, uint32_t samplerID);

    virtual void Serialize(Arch &arch) override;

    void SetGpuMaterial(Render::MaterialID id);

  private:
    Material(AssetID id = NoneAssetID);
    Render::MaterialID mGpuMaterialID = MaterialIDNone; // GPU 머터리얼 ID

    // asset material 수준에서 설정되는 텍스처 속성들
    std::vector<AssetMaterialTexResourceContext> mTexResourceContextList;

    // sampler id
    std::vector<uint32_t> mSamplerResourceContextList;
};

} // namespace CoreAsset
