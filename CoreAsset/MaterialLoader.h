#pragma once
#include "CoreAsset/CoreAssetDLLMacro.h"

#include <CoreAsset/AssetLoader.h>
#include <memory>
#include <string>
namespace CoreAsset
{

class CORE_ASSET_API MaterialLoader : public AssetLoader
{
  public:
    static MaterialLoader *GetInstance();
    ~MaterialLoader();

    virtual bool LoadAssetFile(Arch &arch, AssetFactoryManager *assetFactoryManage, Asset *&oAsset,
                               std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr) override;

    virtual bool LoadAssetRawFile(Arch &arch, Asset *asset) override;

    // virtual std::unique_ptr<SerializedAsset> LoadAssetFile(const std::string &filePath,
    //                                                        CoreAsset::AssetMetaData &oMetaData) override;
    // virtual std::unique_ptr<AssetMetaData> LoadAssetMetaDataFile(QuadRW::BinaryReader &binaryReader) override;

    //// material은 raw file을 생성하지않는다 따라서 nullptr리턴
    // virtual std::unique_ptr<SerializedAssetRawData> LoadRawFile(const std::string &filePath) override;

  protected:
  private:
    MaterialLoader();
};

} // namespace CoreAsset
