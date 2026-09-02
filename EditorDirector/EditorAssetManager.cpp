#include "EditorAssetManager.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetPtr.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalFileSystem.h>
#include <LogicalFileSystem/LogicalFolder.h>
Quad::EditorAssetManager *Quad::EditorAssetManager::GetInstance()
{
    static EditorAssetManager instance;
    return &instance;
}

Quad::EditorAssetManager::EditorAssetManager() {}

Quad::EditorAssetManager::~EditorAssetManager() {}

void Quad::EditorAssetManager::Initialize(CoreAsset::AssetManager *assetManager,
                                          QuadLF::LogicalFileSystem *logicalFileSystem,
                                          CoreAsset::AssetMetaDataManager *assetMetaDataManager)
{

    mAssetManager = assetManager;
    mLogicalFileSystem = logicalFileSystem;
    mAssetMetaDataManager = assetMetaDataManager;
}

CoreAsset::AssetPtr Quad::EditorAssetManager::CreateAsset(CoreAsset::EAssetType assetType,
                                                          CoreAsset::IntermediateAsset *intermediateAssetData)
{
    QuadLF::LogicalFolder *currFolder = mLogicalFileSystem->GetCurrentLogicalFolder();

    const std::string &prefixAssetName = currFolder->GetFullPath();

    CoreAsset::AssetPtr pAsset = mAssetManager->CreateAsset(assetType, intermediateAssetData, prefixAssetName.c_str());

    if (pAsset.Get() == nullptr)
        return pAsset;

    CoreAsset::Asset *asset = pAsset.Get();

    // 2 결과물이 asset들에대해서 논리적파일시스템에 등록
    QuadLF::LogicalFileAssetInfo assetFileInfo;
    assetFileInfo.mAssetID = asset->GetID();
    assetFileInfo.mAssetType = asset->GetType();
    assetFileInfo.mName = asset->GetName().c_str();

    QuadLF::LogicalFile *currFile =
        mLogicalFileSystem->MakeFile(assetFileInfo, pAsset.Get()->GetName().c_str(), currFolder);

    // metaData수정
    CoreAsset::AssetMetaDataManager *assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();
    CoreAsset::AssetMetaData *assetMetaData = assetMetaDataManager->GetMetaData(asset->GetID());
    assetMetaData->mFilePath = currFile->GetFullPath(); // 논리적 파일상대경로(물리적 파일경로이기도 하다)
    assetMetaData->mKeepRawDataFlag = true;

    asset->SetEmptyAssetFlag(false);

    return pAsset;
}

CoreAsset::AssetPtr Quad::EditorAssetManager::GetAssetInner(const char *assetClassName, CoreAsset::AssetID id)
{

    return mAssetManager->ResolveAsset(mAssetManager->GetAssetTypeFromClassName(assetClassName), id);

    // CoreAsset::AssetPtr pAsset = mAssetManager->GetAssetFromAssetName(assetClassName, id);

    // if (pAsset.Get() != nullptr)
    //     return pAsset;

    //// 해당 id asset이없다면 디폴트로 한번더 시도

    // pAsset = mAssetManager->GetDefaultAsset(mAssetManager->GetAssetTypeFromClassName(assetClassName));

    // return pAsset;
}
