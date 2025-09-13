#pragma once

#include <string>

// namespace CoreAsset {
//	class TextureManager;
// class Texture;
// class AssetMetaDataManager;
// }

#include <CommonHeader/GpuTypes.h>
#include <CoreAsset/IAssetImporter.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreBase/FVector.h>

namespace CoreAsset
{
class AssetImporterManager;
}

namespace Import
{

class TextureImporter : public CoreAsset::IAssetImporter
{
  public:
    static TextureImporter *GetInstance();
    virtual ~TextureImporter();

    // dds, jpg, png
    FVector<CoreAsset::IntermediateAsset *> Import(const char *filePath,
                                                   CoreAsset::AssetImporterManager *importerManager) const override;

  private:
    // 에디터에서 사용할 메타데이터
    // void ReigsterTextureMetaData(CoreAsset::Texture* texture, const std::string & filePath) const;

    TextureImporter();

  private:
    // CoreAsset::TextureManager* mTextureManager;
    // CoreAsset::AssetMetaDataManager* mAssetMetaDataManager;
};

} // namespace Import