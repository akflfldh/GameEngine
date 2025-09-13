#include "EditorAssetImporterModule.h"
#include <CoreAsset/AssetImporterManager.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreBase/FVector.h>
#include <ImportModule/TextureImporter.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
Quad::EditorAssetImporterModule *Quad::EditorAssetImporterModule::GetInstance()
{

    static EditorAssetImporterModule instance;
    return &instance;
}

Quad::EditorAssetImporterModule::EditorAssetImporterModule() {}

Quad::EditorAssetImporterModule::~EditorAssetImporterModule() {}

void Quad::EditorAssetImporterModule::Initialize()
{
    CoreAsset::AssetImporterManager *assetImporterManager = CoreAsset::AssetImporterManager::GetInstance();

    Import::TextureImporter *textureImporter = Import::TextureImporter::GetInstance();

    assetImporterManager->RegisterAssetImporter("png", textureImporter);
    assetImporterManager->RegisterAssetImporter("jpg", textureImporter);
    assetImporterManager->RegisterAssetImporter("bmp", textureImporter);
    assetImporterManager->RegisterAssetImporter("dds", textureImporter);
}

void Quad::EditorAssetImporterModule::Import(const char *file)
{

    // 1. assetManager에게 import요청
    CoreAsset::AssetManager *assetManager = CoreAsset::AssetManager::GetInstance();

    QuadLF::LogicalFileSystem *logicalFileSystem = QuadLF ::LogicalFileSystem::GetInstance();
    // 현재 논리적파일시스템의 현재 폴더경로도 같이 넘긴다.
    const std::string currLogicalFolderPath = logicalFileSystem->GetCurrentLogicalFolderPath();

    FVector<CoreAsset::Asset *> importedAssets = assetManager->ImportAsset(file, currLogicalFolderPath.c_str());

    if (importedAssets.Size() == 0)
        return;

    // 2 결과물이 asset들에대해서 논리적파일시스템에 등록
    CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

    for (size_t i = 0; i < importedAssets.Size(); ++i)
    {
        CoreAsset::Asset *asset = importedAssets[i];

        QuadLF::LogicalFileAssetInfo texturelogicalFileInfo;
        texturelogicalFileInfo.mAssetID = asset->GetID();
        texturelogicalFileInfo.mAssetType = asset->GetType();
        texturelogicalFileInfo.mName = asset->GetName().c_str();

        QuadLF::LogicalFile *LogicalFile = logicalFileSystem->MakeFile(texturelogicalFileInfo, asset->GetName().c_str(),
                                                                       logicalFileSystem->GetRootFolder(), false);

        CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset->GetID());

        assetMetaData->mFilePath = LogicalFile->GetFullPath(); // 논리적 파일상대경로(물리적 파일경로이기도 하다)
    }

    // 3. 에셋 브라우저에게 알림전송
}
