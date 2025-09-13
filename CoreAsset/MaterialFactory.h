#pragma once
#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreAsset/IAssetFactory.h>
namespace CoreAsset
{
class Material;

class CORE_ASSET_API MaterialFactory : public IAssetFactory
{
  public:
    static MaterialFactory *GetInstance();
    MaterialFactory();
    virtual ~MaterialFactory();

    virtual Asset *CreateEmptyAsset() override;
    virtual Asset *CreateAssetFromData(const IntermediateAsset &intermediateAsset) override;

  private:
    /* Asset *CreateMaterial(const CoreAsset::SerializedMaterial &serializedMaterial,
                           const CoreAsset::AssetMetaData &metaData);*/

    //  Material *CreateMaterialInstance(const std::string &name, const std::string &path, AssetID id);

  private:
};
} // namespace CoreAsset
