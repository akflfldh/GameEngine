#pragma once

#include <CoreAsset/IAssetImporter.h>

namespace Quad
{
class EditorFontImporter : public CoreAsset::IAssetImporter
{
  public:
    static EditorFontImporter *GetInstance();

    virtual ~EditorFontImporter();

    // filepath는 json data파일, glyphTexture는 json파일 이름 + '_altas' 라는 이름을 가져야함
    virtual CoreAsset::ImportPackage Import(const std::filesystem::path &path,
                                            CoreAsset::AssetImporterManager *importerManager,
                                            const CoreAsset::ImportExecutionContext &executionContext) const override;

  private:
    EditorFontImporter();
};

} // namespace Quad
