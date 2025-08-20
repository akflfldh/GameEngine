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

class CORE_ASSET_API MaterialLoader
{
  public:
    static MaterialLoader *GetInstance();
    ~MaterialLoader();

    std::unique_ptr<SerializedMaterial> LoadAssetFile(const std::string &filePath, CoreAsset::AssetMetaData &oMetaData);

  private:
    MaterialLoader();
};

} // namespace CoreAsset
