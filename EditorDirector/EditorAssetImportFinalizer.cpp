#include "EditorAssetImportFinalizer.h"
#include <CoreAsset/AssetMetaDataManager.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>

Quad::EditorAssetImportFinalizer *Quad::EditorAssetImportFinalizer::GetInstance()
{
    static EditorAssetImportFinalizer instance;
    return &instance;
}

Quad::EditorAssetImportFinalizer::EditorAssetImportFinalizer() {}

Quad::EditorAssetImportFinalizer::~EditorAssetImportFinalizer() {}

void Quad::EditorAssetImportFinalizer::Finalize(const ImportResult &result)
{

    // 이거는 메인스레드가 하도록 옮기자
    //  2 결과물이 asset들에대해서 논리적파일시스템에 등록
    CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();
    auto logicalFileSystem = QuadLF::LogicalFileSystem::GetInstance();

    const std::vector<CoreAsset::Asset *> &assets = result.mAssets;

    for (size_t i = 0; i < assets.size(); ++i)
    {
        CoreAsset::Asset *asset = assets[i];
        if (asset == nullptr)
            continue;

        QuadLF::LogicalFileAssetInfo texturelogicalFileInfo;
        texturelogicalFileInfo.mAssetID = asset->GetID();
        texturelogicalFileInfo.mAssetType = asset->GetType();
        texturelogicalFileInfo.mName = asset->GetName().c_str();

        QuadLF::LogicalFile *LogicalFile = logicalFileSystem->MakeFile(
            texturelogicalFileInfo, asset->GetName().c_str(), logicalFileSystem->GetCurrentLogicalFolder(), false);

        CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset->GetID());

        assetMetaData->mFilePath = LogicalFile->GetFullPath(); // 논리적 파일상대경로(물리적 파일경로이기도 하다)
    }
}
