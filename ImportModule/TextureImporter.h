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
#include <memory>

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
    CoreAsset::ImportPackage Import(const std::filesystem::path &path, CoreAsset::AssetImporterManager *importerManager,
                                    const CoreAsset::ImportExecutionContext &executionContext) const override;

  private:
    // 에디터에서 사용할 메타데이터
    // void ReigsterTextureMetaData(CoreAsset::Texture* texture, const std::string & filePath) const;

    TextureImporter();

  private:
    // CoreAsset::TextureManager* mTextureManager;
    // CoreAsset::AssetMetaDataManager* mAssetMetaDataManager;
};

} // namespace Import