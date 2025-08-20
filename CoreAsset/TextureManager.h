#pragma once

#include "CoreAsset/AssetIDGenerator.h"
#include "CoreAsset/AssetIDTable.h"
#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <D3DGpuResourceManager/GpuTypes.h>
#include <string>

namespace GRM
{
class IGpuResourceManager;
}

namespace CoreAsset
{
class Texture;
class AssetIDTable;
class AssetMetaData;

struct SerializedTexture;
struct SerializedTextureRawData;

class GlobalAssetRegistrySystem;

constexpr AssetID DefaultTextureID = 1;

class CORE_ASSET_API TextureManager
{
  public:
    static TextureManager *GetInstance();
    TextureManager(GRM::IGpuResourceManager *gpuResourceManager);
    ~TextureManager();

    Texture *GetAsset(const std::string &path, const std::string &name) const;
    Texture *GetAsset(AssetID id) const;

    Texture *CreateTexture(const GRM::TextureDesc &textureDesc, const std::string &name, const std::string &path = "",
                           bool keepRawDataFlag = false); // 동적으로 생성

    // loader같은 외부시스템이 호출한다.
    Texture *CreateTexture(const SerializedTexture &serializedTexture,
                           const SerializedTextureRawData &serializedTextureRawData,
                           const CoreAsset::AssetMetaData &metaData, const std::string &path);

    Texture *CreateTexture(const SerializedTexture &serializedTexture,
                           SerializedTextureRawData &&serializedTextureRawData,
                           const CoreAsset::AssetMetaData &metaData, const std::string &path);

  private:
    Texture *CreateTextureInstance(const std::string &name, const std::string &path);
    Texture *CreateTextureInstance(const std::string &name, const std::string &path, AssetID id);

    void SetTextureProperties(Texture *texture, const GRM::TextureDesc &textureDesc);

  private:
    static TextureManager *mInstance;

    GRM::IGpuResourceManager *mGpuResourceManager;
    // AssetIDTable mIDTable;
    // AssetIDGenerator mIDGenerator;

    GlobalAssetRegistrySystem *mGlobalAssetRegistrySystem;
};

} // namespace CoreAsset
