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

bool CoreAsset::AssetIOManager::LoadAssetFromMetaData(const std::string &filePath,
                                                      CoreAsset::AssetFactoryManager *assetFactoryManager,
                                                      Asset *&oAsset, std::unique_ptr<AssetMetaData> &oAssetMetaDataPtr)
{

    // 모든 에셋들에대해서 공통으로 적용되는 header

    // 에셋타입, name, 실제 데이터 파일명or 이름 (필요없을수도 ==동일한이름에서 확장자만다르게할수있으니)

    // 공통으로 적용되는 header를 읽어서(분리된 구조체  )
    // 직렬화하고 Serialize(Arch arch)

    // 타입을 구분해서 적절한 AssertLoader를 의 LoadAssetFile을 호출

    BinaryArch binaryArch(true);
    binaryArch.SetFile(filePath.c_str());
    binaryArch.Start();

    AssetCommonHeader assetCommonHeader;
    assetCommonHeader.Serialize(binaryArch);

    bool ret = mAssetLoaderTable[assetCommonHeader.mAssetType]->LoadAssetFile(binaryArch, assetFactoryManager, oAsset,
                                                                              oAssetMetaDataPtr);

    oAssetMetaDataPtr->mAssetID = assetCommonHeader.mAssetID;
    // oAssetMetaDataPtr->mAssetName = assetCommonHeader.mAssetName.GetStr();
    oAssetMetaDataPtr->mAssetType = assetCommonHeader.mAssetType;

    // 리턴받는것을 출력매개변수로받는게 좋을듯.

    // 여기서 asset이 nullptr이 아니라면 전역공간에 등록? 혹은, 더 상위계층인 assetManager에서 등록
    // AssetIO는 에셋파일을 읽고 빈에셋을 만드는 공장역할, 에셋의 라이프사이클이 시작하는 주정부(테이블)에 등록은
    // 상위계층의 에셋매니저에서 수행 (임포터과정도 결국 에셋을 반환할것이기에 공통의 상위계층인 에셋매니저에서
    // 등록하도록하는것이 좋다)

    binaryArch.End();
    return ret;
}

bool CoreAsset::AssetIOManager::LoadAssetRawData(Asset *asset, const std::string &path)
{

    if (path == "")
        return true;

    BinaryArch binaryArch(true);

    binaryArch.SetFile(path.c_str());

    binaryArch.Start();

    bool ret = mAssetLoaderTable[asset->GetType()]->LoadAssetRawFile(binaryArch, asset);

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

bool CoreAsset::AssetIOManager::StoreAsset(CoreAsset::Asset *asset, const std::string &filePath,
                                           AssetMetaData *assetMetaData)
{
    /*asset type을 보고*/

    // 헤더는 여기서 이곳에서?
    //(헤더는 공통이기에, 물론 헤더중에서도 공통헤더)
    // 저장
    {

        BinaryArch binaryArch(false);
        binaryArch.SetFile(filePath.c_str());
        binaryArch.Start();

        AssetCommonHeader assetCommonHeader;
        assetCommonHeader.mAssetID = asset->GetID();
        assetCommonHeader.mAssetType = asset->GetType();
        assetCommonHeader.mAssetName = asset->GetName().c_str();
        assetCommonHeader.Serialize(binaryArch);

        mAssetStorerTable[assetCommonHeader.mAssetType]->StoreAssetFile(binaryArch, asset, assetMetaData);

        binaryArch.End();
    }
    return true;
}

bool CoreAsset::AssetIOManager::StoreAssetRawData(CoreAsset::Asset *asset, const std::string &filePath,
                                                  AssetMetaData *assetMetaData)
{
    if (asset == nullptr)
        return false;

    BinaryArch binaryArch(false);
    binaryArch.SetFile(filePath.c_str());
    binaryArch.Start();

    bool ret = mAssetStorerTable[asset->GetType()]->StoreAssetRawDataFile(binaryArch, asset, assetMetaData);

    binaryArch.End();

    return ret;
}
