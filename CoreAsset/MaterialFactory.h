#pragma once
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreAsset/IAssetFactory.h>
#include <vector>
namespace CoreAsset
{
class Material;

class CORE_ASSET_API MaterialFactory : public IAssetFactory
{
  public:
    static MaterialFactory *GetInstance();
    MaterialFactory();
    virtual ~MaterialFactory();

    virtual Asset *CreateEmptyAsset(EAssetType assetType) override;
    virtual Asset *CreateAssetFromData(const IntermediateAsset &intermediateAsset) override;

  private:
    /* Asset *CreateMaterial(const CoreAsset::SerializedMaterial &serializedMaterial,
                           const CoreAsset::AssetMetaData &metaData);*/

    //  Material *CreateMaterialInstance(const std::string &name, const std::string &path, AssetID id);

    uint32_t GetNextMaterialHandle();
    void ReleaseMaterialHandle(uint32_t handle);

  private:
    std::vector<uint32_t> mMaterialHandleIndexPool;
    uint32_t mNextMaterialHandle = 0;
};
} // namespace CoreAsset
