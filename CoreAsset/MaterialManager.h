#pragma once

#include "CoreAsset/AssetIDGenerator.h"
#include "CoreAsset/AssetIDTable.h"
#include "CoreAsset/AssetPtr.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <RenderSystem/MaterialType.h>
#include <vector>

namespace CoreAsset
{
class Material;
class Texture;
class AssetMetaData;
class TextureManager;

class GlobalAssetRegistrySystem;

class CORE_ASSET_API MaterialManager
{
  public:
    static MaterialManager *GetInstance();
    MaterialManager();
    ~MaterialManager();

    Material *CreateMaterial(Render::MaterialID gpuMaterialID, const std::string &name, const std::string &path = "");
    Material *GetAsset(AssetID id) const;
    Material *GetAsset(const std::string &name) const;

    //// load를 통한 생성
    // Material *CreateMaterial(const CoreAsset::SerializedMaterial &serializedMaterial,
    //                          const CoreAsset::AssetMetaData &metaData, const std::string &path);

  private:
    Material *CreateMaterialInstance(const std::string &name, const std::string &path);
    Material *CreateMaterialInstance(const std::string &name, const std::string &path, AssetID id);

  private:
    GlobalAssetRegistrySystem *mGlobalAssetRegistrySystem;
};

} // namespace CoreAsset