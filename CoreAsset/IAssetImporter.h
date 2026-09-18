#pragma once

#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreAsset/IntermediateAsset.h>
#include <filesystem>
#include <memory>
#include <vector>
namespace CoreAsset
{

struct IntermediateAsset;
class AssetImporterManager;

struct TextureImportExecutionContext
{
    bool bSRGB = true;
};

struct FBXImportExecutionContext
{
};

struct ImportExecutionContext
{
    bool bEngineAsset = false;
    TextureImportExecutionContext mTextureContext;
    FBXImportExecutionContext mFBXContext;
};

class CORE_ASSET_API IAssetImporter
{
  public:
    IAssetImporter();
    virtual ~IAssetImporter() = 0;

    virtual CoreAsset::ImportPackage Import(const std::filesystem::path &filePath,
                                            AssetImporterManager *importerManager,
                                            const ImportExecutionContext &executionContext) const = 0;

  private:
};
} // namespace CoreAsset
