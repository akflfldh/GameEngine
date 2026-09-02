#include "AssetIOManager.h"
#include <AssetLoader.h>
#include <BinaryReaderWriter/BinaryReader.h>
#include <CoreAsset/AssetCommon.h>
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/AssetMetaDataManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/AssetStorer.h>
#include <CoreBase/BinaryArch.h>
#include <Logger/Logger.h>

uint32_t CoreAsset::AssetIOManager::mCurrentEngineVerison = 1;

CoreAsset::AssetIOManager *CoreAsset::AssetIOManager::GetInstance()
{

    static AssetIOManager instance;

    return &instance;
};
CoreAsset::AssetIOManager::AssetIOManager() {}

CoreAsset::AssetIOManager::~AssetIOManager() {}

void CoreAsset::AssetIOManager::RegisterAssetLoader(EAssetType assetType, AssetLoader *assetLoader)
{
    mAssetLoaderTable[assetType] = assetLoader;
}

void CoreAsset::AssetIOManager::RegisterAssetStorer(EAssetType assetType, AssetStorer *assetStorer)
{

    mAssetStorerTable[assetType] = assetStorer;
}

CoreAsset::AssetLoadResult CoreAsset::AssetIOManager::LoadAssetFromMetaData(
    const std::filesystem::path &filePath, CoreAsset::AssetFactoryManager *assetFactoryManager, Asset *&oAsset,
    std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr, const AssetLoadExecutionContext &executionContext)
{

    // 모든 에셋들에대해서 공통으로 적용되는 header

    // 에셋타입, name, 실제 데이터 파일명or 이름 (필요없을수도 ==동일한이름에서 확장자만다르게할수있으니)

    // 공통으로 적용되는 header를 읽어서(분리된 구조체  )
    // 직렬화하고 Serialize(Arch arch)

    // 타입을 구분해서 적절한 AssertLoader를 의 LoadAssetFile을 호출

    AssetLoadResult result;
    result.mResultFlag = EAssetLoadResultFlag::eFail;

    BinaryArch binaryArch(true);
    binaryArch.SetFile(filePath.string().data());
    binaryArch.Start();

    AssetCommonHeader assetCommonHeader;
    assetCommonHeader.Serialize(binaryArch);

    if (assetCommonHeader.mMagic != AssetCommonHeader::Magic)
    {
        binaryArch.End();
        return result;
    }

    if (assetCommonHeader.mVersion > mCurrentEngineVerison)
    {
        binaryArch.End();
        return result;
    }

    result.mAssetType = assetCommonHeader.mAssetType;

    if (executionContext.mIgnoredAssetType == assetCommonHeader.mAssetType)
    {
        result.mResultFlag = EAssetLoadResultFlag::eIgnore;
        return result;
    }

    AssetLoader *loader = GetLoader(assetCommonHeader.mAssetType);

    if (loader == nullptr)
    {
        binaryArch.End();
        return result;
    }

    bool ret =
        loader->LoadAssetFile(assetCommonHeader.mAssetType, binaryArch, assetFactoryManager, oAsset, oAssetMetaDataPtr);

    if (ret == false || oAsset == nullptr || oAssetMetaDataPtr == nullptr)
    {
        binaryArch.End();
        result.pAsset = oAsset;
        return result;
    }

    oAssetMetaDataPtr->mAssetID = assetCommonHeader.mAssetID;
    // oAssetMetaDataPtr->mAssetName = assetCommonHeader.mAssetName.GetStr();
    oAssetMetaDataPtr->mAssetType = assetCommonHeader.mAssetType;
    oAssetMetaDataPtr->mAssetName = assetCommonHeader.mAssetName;
    oAssetMetaDataPtr->mRawFileName = assetCommonHeader.mAssetRawName;

    if (oAsset)
    {
        oAsset->SetAssetID(assetCommonHeader.mAssetID);
        oAsset->SetName(assetCommonHeader.mAssetName.c_str());

        result.pAsset = oAsset;
        result.mResultFlag = EAssetLoadResultFlag::eSuccess;
    }

    // 리턴받는것을 출력매개변수로받는게 좋을듯.

    // 여기서 asset이 nullptr이 아니라면 전역공간에 등록? 혹은, 더 상위계층인 assetManager에서 등록
    // AssetIO는 에셋파일을 읽고 빈에셋을 만드는 공장역할, 에셋의 라이프사이클이 시작하는 주정부(테이블)에 등록은
    // 상위계층의 에셋매니저에서 수행 (임포터과정도 결국 에셋을 반환할것이기에 공통의 상위계층인 에셋매니저에서
    // 등록하도록하는것이 좋다)

    binaryArch.End();
    return result;
}

bool CoreAsset::AssetIOManager::LoadAssetRawData(Asset *asset, const std::filesystem::path &path)
{

    if (path.empty())
        return true;

    BinaryArch binaryArch(true);

    binaryArch.SetFile(path);

    binaryArch.Start();

    AssetLoader *loader = GetLoader(asset->GetType());
    if (loader == nullptr)
    {
        binaryArch.End();
        return false;
    }

    bool ret = loader->LoadAssetRawFile(binaryArch, asset);

    binaryArch.End();

    return ret;
}

// std::unique_ptr<CoreAsset::SerializedAssetRawData> CoreAsset::AssetIOManager::LoadAssetRawData(
//     const std::string &filePath, EAssetType assetType)
//{
//     QuadRW::BinaryReader binaryReader;
//
//     std::unique_ptr<CoreAsset::SerializedAssetRawData> serializedRawPtr;
//
//     serializedRawPtr = mAssetLoaderTable[assetType]->LoadRawFile(filePath);
//
//     return serializedRawPtr;
// }

bool CoreAsset::AssetIOManager::StoreAsset(CoreAsset::Asset *asset, const std::filesystem::path &filePath,
                                           AssetMetaData *assetMetaData)
{
    /*asset type을 보고*/

    // 헤더는 여기서 이곳에서?
    //(헤더는 공통이기에, 물론 헤더중에서도 공통헤더)
    // 저장
    {

        if (asset == nullptr || assetMetaData == nullptr)
            return false;

        BinaryArch binaryArch(false);
        binaryArch.SetFile(filePath.c_str());
        binaryArch.Start();

        AssetCommonHeader assetCommonHeader;
        assetCommonHeader.mAssetID = asset->GetID();
        assetCommonHeader.mAssetType = asset->GetType();
        assetCommonHeader.mAssetName = asset->GetName().c_str();
        assetCommonHeader.mAssetRawName = assetMetaData->mRawFileName;
        assetCommonHeader.mVersion = mCurrentEngineVerison;
        assetCommonHeader.Serialize(binaryArch);

        AssetStorer *storer = GetStorer(assetCommonHeader.mAssetType);
        if (storer == nullptr)
        {
            binaryArch.End();
            return false;
        }

        storer->StoreAssetFile(binaryArch, asset, assetMetaData);

        binaryArch.End();
    }
    return true;
}

bool CoreAsset::AssetIOManager::StoreAssetRawData(CoreAsset::Asset *asset, const std::filesystem::path &filePath,
                                                  AssetMetaData *assetMetaData)
{
    if (asset == nullptr)
        return false;

    BinaryArch binaryArch(false);
    binaryArch.SetFile(filePath);
    binaryArch.Start();

    AssetStorer *storer = GetStorer(asset->GetType());
    if (storer == nullptr)
    {
        binaryArch.End();
        return false;
    }

    bool ret = storer->StoreAssetRawDataFile(binaryArch, asset, assetMetaData);

    binaryArch.End();

    return ret;
}

CoreAsset::AssetStorer *CoreAsset::AssetIOManager::GetStorer(EAssetType type) const
{
    auto iter = mAssetStorerTable.find(type);

    if (iter == mAssetStorerTable.end())
        return nullptr;

    return iter->second;
}

CoreAsset::AssetLoader *CoreAsset::AssetIOManager::GetLoader(EAssetType type) const
{

    auto iter = mAssetLoaderTable.find(type);

    if (iter == mAssetLoaderTable.end())
        return nullptr;

    return iter->second;
}
