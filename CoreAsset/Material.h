#pragma once

#include "CoreAsset/Asset.h"

#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/Texture.h"
#include <RenderSystem/MaterialType.h>
#include <vector>
namespace CoreAsset
{
class Texture;

struct AssetMaterialTexResourceContext
{
    Render::TextureShaderResourceInfo mTexShaderResourceInfo;
    AssetPtr<Texture> mTexture = nullptr;
};

class CORE_ASSET_API Material : public Asset
{
    friend class MaterialManager;
    friend class UIMaterialManager;

  public:
    virtual ~Material();
    Render::MaterialID GetGpuMaterialID() const
    {
        return mGpuMaterialID;
    }

    const std::vector<AssetMaterialTexResourceContext> &GetTexResourceContextList() const;
    const std::vector<uint32_t> &GetSamplerResourceContextList() const;

    void SetTextureResource(int index, Texture *tex);

    void SetSamplerResource(int index, uint32_t samplerID);

  private:
    Material(AssetID assetID, const std::string &name);
    Render::MaterialID mGpuMaterialID = MaterialIDNone; // GPU 머터리얼 ID

    // asset material 수준에서 설정되는 텍스처 속성들
    std::vector<AssetMaterialTexResourceContext> mTexResourceContextList;

    // sampler id
    std::vector<uint32_t> mSamplerResourceContextList;
};

} // namespace CoreAsset
