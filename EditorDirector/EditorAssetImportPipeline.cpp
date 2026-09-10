#include "EditorAssetImportPipeline.h"
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetType.h>
#include <EditorAssetImporterManager.h>
#include <coreasset/AssetCommon.h>

EditorAssetImportPipeline *EditorAssetImportPipeline::GetInstance()
{

    static EditorAssetImportPipeline instance;

    return &instance;
}

AssetImportStoreResult EditorAssetImportPipeline::ImportAndStore(const AssetImportStoreRequest &request)
{

    // 이렇게먼저 해당 asset의 .asset , raw파일이 있는지 확인?

    // 있으면 리턴

    // import

    // 정해진 asset id 설정

    // 그리고 정해진 uniquename

    // 그리고 .asset, raw 파일도 생성 (즉 저장)

    auto importManager = Quad::EditorAssetImporterManager::GetInstance();

    Quad::EngineAssetImportRequest importReqeust;
    importReqeust.mSourcePath = request.mSourcePath;
    importReqeust.mRequestedAssetID = request.mRequestedAssetID;
    importReqeust.mRequestedAssetName = request.mRequestedAssetName;
    importReqeust.mTargetRegistryPath = request.mTargetRegistryPath;
    importReqeust.mSerializedOutputDirectory = request.mSerializedOutputPath;
    importReqeust.mRawOutputDirectory = request.mRawOutputPath;
    importReqeust.mTargetImportAssetKey = request.mSourcePath.stem().string();

    ImportResult result = importManager->RequestImportSync(importReqeust);

    // 저장 asset . raw 데이터

    //// 1. 전체 에셋의 저장 경로와 Metadata를 먼저 확정
    // for (CoreAsset::Asset *asset : result.mAssets)
    //{
    //     // LogicalFile 생성
    //     // AssetMetaData::mFilePath 설정
    // }

    // 2. 실제 파일 저장
    auto assetMetaDataManager = CoreAsset::AssetMetaDataManager::GetInstance();

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    AssetImportStoreResult assetImportStoreResult;
    std::filesystem::create_directories(request.mSerializedOutputPath);
    std::filesystem::create_directories(request.mRawOutputPath);

    for (CoreAsset::Asset *asset : result.mAssets)
    {

        CoreAsset::AssetMetaData *metadata = assetMetaDataManager->GetMetaData(asset->GetID());

        std::filesystem::path serializedAssetPath =
            request.mSerializedOutputPath /
            (std::string(asset->GetName().c_str()) + CoreAsset::GetAssetDotFileExtension());

        assetManager->StoreAsset(asset, serializedAssetPath, metadata);

        if (metadata->mHasRawData)
        {
            std::filesystem::path rawDataPath = request.mRawOutputPath / metadata->mRawFileName;

            assetManager->StoreAssetRawData(asset, rawDataPath);
        }
    }
    assetImportStoreResult.mSuccess = result.mSuccess;
    return assetImportStoreResult;
}
