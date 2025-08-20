#pragma once

#include <string>

// namespace CoreAsset {
//	class TextureManager;
// class Texture;
// class AssetMetaDataManager;
// }

#include <D3DGpuResourceManager/GpuTypes.h>

namespace Import
{

class TextureImporter
{
  public:
    static TextureImporter *GetInstance();
    virtual ~TextureImporter();

    // dds, jpg, png
    bool Import(const std::string &filePath, GRM::TextureDesc &oTextureDesc) const;

  private:
    // 에디터에서 사용할 메타데이터
    // void ReigsterTextureMetaData(CoreAsset::Texture* texture, const std::string & filePath) const;

    TextureImporter();

  private:
    // CoreAsset::TextureManager* mTextureManager;
    // CoreAsset::AssetMetaDataManager* mAssetMetaDataManager;
};

} // namespace Import