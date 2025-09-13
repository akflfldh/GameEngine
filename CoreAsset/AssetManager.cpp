#include "AssetManager.h"
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/AssetIOManager.h>
#include <CoreAsset/AssetImporterManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreBase/FVector.h>
#include <Utility/Utility.h>

CoreAsset::AssetManager *CoreAsset::AssetManager::GetInstance()
{
    static AssetManager instance;
    return &instance;
}

CoreAsset::AssetManager::~AssetManager() {}

void CoreAsset::AssetManager::Initialize(AssetFactoryManager *assetFactoryManager, AssetIOManager *assetIOManager,
                                         AssetImporterManager *assetImporterManager)
{
    mAssetFactoryManager = assetFactoryManager;
    mAssetIOManager = assetIOManager;
    mAssetImporterManager = assetImporterManager;

    mGlobalAssetRegistrySystem = GlobalAssetRegistrySystem::GetInstance();
    mAssetMetaDataManager = AssetMetaDataManager::GetInstance();
}

void CoreAsset::AssetManager::SetAssetImporterManager(AssetImporterManager *assetImporterManager)
{
    mAssetImporterManager = assetImporterManager;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::CreateAsset(CoreAsset::EAssetType assetType,
                                                         IntermediateAsset *intermediateAssetData,
                                                         const char *prefixAssetName)
{

    // factory create
    Asset *asset = nullptr;

    asset = mAssetFactoryManager->CreateAssetFromData(*intermediateAssetData);

    // asset들의 unique한 name을 최종 설정 & 전역테이블에 등록 & 메타데이터 등록

    std::string displayName = asset->GetName().c_str();

    std::string uniqueName = std::string(prefixAssetName) + "/" + displayName;

    // uniqueName의 중복을 검사해야한다. , 전역테이블시스템으로부터
    size_t nameCount = 0;
    std::string displayNameTemp;
    while (mGlobalAssetRegistrySystem->GetAsset(uniqueName) != nullptr)
    {
        displayNameTemp = displayName + std::to_string(nameCount);
        uniqueName = std::string(prefixAssetName) + "/" + displayNameTemp;
    }
    asset->SetName(displayName.c_str());
    mGlobalAssetRegistrySystem->RegisterAsset(asset, uniqueName);
    mGlobalAssetRegistrySystem->AddDirtyAsset(asset);

    mAssetMetaDataManager->Register(asset);

    IntermediateAssetFactory *intermediateAssetFactory = IntermediateAssetFactory::GetInstance();
    intermediateAssetFactory->ReleaseIntermediateAsset(assetType, intermediateAssetData);
    return asset;
}

CoreAsset::Asset *CoreAsset::AssetManager::LoadAsset(const char *filePath)
{

    const std::string extension = CoreUtility::Utility::GetExtensionFromPath(filePath);
    AssetMetaData assetMetaData;
    bool ret = false;

    // 해당 asset이 이미 존재하는지확인 // asset파일을 로드하는거라면 filePath의 경로는 논리적경로와 동일한 물리적경로
    // 문자열
    std::string removedExtensionPath = CoreUtility::Utility::RemoveExtension(filePath);
    AssetPtr assetPtr = GetAssetCommon(removedExtensionPath.c_str());
    if (assetPtr.Get() != nullptr)
    {
        return assetPtr.Get();
    }

    Asset *asset = nullptr;
    std::unique_ptr<AssetMetaData> assetMetaDataPtr = nullptr;

    if (extension == "asset" || extension == "map")
    {
        ret = mAssetIOManager->LoadAssetFromMetaData(filePath, mAssetFactoryManager, asset, assetMetaDataPtr);
    }
    else
    {
        return nullptr;
    }

    if (ret == false)
        return nullptr;

    // 메타데이터 등록
    mAssetMetaDataManager->Register(*assetMetaDataPtr.get());

    // 전역테이블에 등록하는 메서드 호출
    RegisterAsset(asset);

    return asset;
}

bool CoreAsset::AssetManager::LoadAssetRawData(CoreAsset::Asset *asset)
{

    if (asset == nullptr)
        return false;

    AssetMetaData *assetMetaData = mAssetMetaDataManager->GetMetaData(asset);

    std::string path = mRawDataPath + "/" + assetMetaData->mRawFileName.c_str();

    return mAssetIOManager->LoadAssetRawData(asset, path);
}

void CoreAsset::AssetManager::SetAssetRawDataPath(const std::string &path)
{
    mRawDataPath = path;
}

FVector<CoreAsset::Asset *> CoreAsset::AssetManager::ImportAsset(const char *filePath, const char *prefixAssetName)
{
    // 임포팅이라면 절대경로
    if (mAssetImporterManager == nullptr)
        return {};

    // import
    FVector<IntermediateAsset *> intermediateAssets = mAssetImporterManager->Import(filePath);
    if (intermediateAssets.Size() == 0)
    {
        return {};
    }

    // factory create
    FVector<Asset *> assetPtrVector(intermediateAssets.Size(), nullptr);
    for (size_t assetIndex = 0; assetIndex < intermediateAssets.Size(); ++assetIndex)
    {
        CreateAsset(assetPtrVector[assetIndex]->GetType(), intermediateAssets[assetIndex], prefixAssetName);
    }

    // for (size_t assetIndex = 0; assetIndex < intermediateAssets.Size(); ++assetIndex)
    //{
    //     assetPtrVector[assetIndex] = mAssetFactoryManager->CreateAssetFromData(*intermediateAssets[assetIndex]);
    // }

    //// asset들의 unique한 name을 최종 설정 & 전역테이블에 등록 & 메타데이터 등록
    // for (size_t assetIndex = 0; assetIndex < intermediateAssets.Size(); ++assetIndex)
    //{
    //     Asset *asset = assetPtrVector[assetIndex];
    //     std::string displayName = asset->GetName().c_str();

    //    std::string uniqueName = std::string(prefixAssetName) + "/" + displayName;

    //    // uniqueName의 중복을 검사해야한다. , 전역테이블시스템으로부터
    //    size_t nameCount = 0;
    //    std::string displayNameTemp;
    //    while (mGlobalAssetRegistrySystem->GetAsset(uniqueName) != nullptr)
    //    {
    //        displayNameTemp = displayName + std::to_string(nameCount);
    //        uniqueName = std::string(prefixAssetName) + "/" + displayNameTemp;
    //    }
    //    asset->SetName(displayName.c_str());
    //    mGlobalAssetRegistrySystem->RegisterAsset(asset, uniqueName);
    //    mGlobalAssetRegistrySystem->AddDirtyAsset(asset);

    //    mAssetMetaDataManager->Register(asset);
    //}

    return assetPtrVector;
}

bool CoreAsset::AssetManager::StoreAsset(Asset *asset, const char *filePath)
{

    // 파일명어디, 논리적 name .

    if (asset == nullptr)
        return false;

    AssetMetaData *assetMetaData = mAssetMetaDataManager->GetMetaData(asset->GetID());

    bool ret = mAssetIOManager->StoreAsset(asset, filePath, assetMetaData);

    return ret;
}

bool CoreAsset::AssetManager::StoreAssetRawData(Asset *asset, const char *filePath)
{

    if (asset == nullptr)
        return false;

    AssetMetaData *assetMetaData = mAssetMetaDataManager->GetMetaData(asset->GetID());

    bool ret = mAssetIOManager->StoreAssetRawData(asset, filePath, assetMetaData);

    return ret;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::CreateAssetInner()
{
    return nullptr;
}

// CoreAsset::AssetPtr CoreAsset::AssetManager::CreateAsset(SerializedAsset *serializedAsset,
//                                                          const AssetMetaData &assetMetaData)
//{
//     AssetPtr assetPtr;
//     // global registry check
//
//     Asset *pAsset = mGlobalAssetRegistrySystem->GetAsset(serializedAsset->mAssetName);
//     if (pAsset != nullptr)
//     {
//         // 이미존재
//         return nullptr;
//     }
//
//     // assetPtr = mAssetFactoryManager->CreateAssetFromData(*(serializedAsset), assetMetaData);
//     ////    mAssetFactory->CreateAsset(serializedAsset);
//     // if (assetPtr.Get() == nullptr)
//     //{
//     //     // 생성 실패
//     //     return nullptr;
//     // }
//
//     // mGlobalAssetRegistrySystem->RegisterAsset(assetPtr.Get(), serializedAsset->mAssetName);
//
//     return assetPtr;
// }

void CoreAsset::AssetManager::RegisterAsset(Asset *asset)
{

    if (asset == nullptr)
        return;

    mGlobalAssetRegistrySystem->RegisterAsset(asset, asset->GetName().c_str());
}

void CoreAsset::AssetManager::InitAssetSetting(Asset *asset)
{

    if (asset == nullptr)
        return;
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetAssetCommon(const char *assetName) const
{
    return mGlobalAssetRegistrySystem->GetAsset(assetName);
}

CoreAsset::AssetPtr CoreAsset::AssetManager::GetAssetCommon(AssetID assetID) const
{
    return mGlobalAssetRegistrySystem->GetAsset(assetID);
}

CoreAsset::AssetManager::AssetManager()
{

    mAssetMetaDataManager = AssetMetaDataManager::GetInstance();
}
