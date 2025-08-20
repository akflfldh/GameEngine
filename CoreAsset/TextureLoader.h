#pragma once

#include "CoreAsset/CoreAssetDLLMacro.h"
#include "CoreAsset/SerializedAssetType.h"
#include <memory>
#include <string>
namespace CoreAsset
{

struct SerializedTexture;
struct SerializedTextureRawData;
class AssetMetaData;

class CORE_ASSET_API TextureLoader
{
  public:
    static TextureLoader *GetInstance();
    ~TextureLoader();

    std::unique_ptr<SerializedTexture> LoadAssetFile(const std::string &filePath, CoreAsset::AssetMetaData &oMetaData);
    std::unique_ptr<SerializedTextureRawData> LoadRawFile(const std::string &filePath);

  private:
    TextureLoader();
};

} // namespace CoreAsset
