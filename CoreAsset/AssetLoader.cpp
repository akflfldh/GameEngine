#include "CoreAsset/AssetLoader.h"
#include <BinaryReaderWriter/BinaryReader.h>

#include "CoreAsset/MaterialLoader.h"
#include "CoreAsset/TextureLoader.h"
#include <Logger/Logger.h>

CoreAsset::AssetLoader *CoreAsset::AssetLoader::GetInstance()
{

    static AssetLoader instance;
    return &instance;
}

CoreAsset::AssetLoader::AssetLoader()
{
    mTextureLoader = TextureLoader::GetInstance();

    mMaterialLoader = MaterialLoader::GetInstance();
}

CoreAsset::AssetLoader::~AssetLoader() {}

std::unique_ptr<CoreAsset::SerializedAsset> CoreAsset::AssetLoader::LoadAsset(const std::string &filePath,
                                                                              AssetMetaData &oAssetMetaData)
{
    QuadRW::BinaryReader binaryReader;

    // header부분 (SerializedAsset부분) 을 먼저 로드한다.
    CoreAsset::SerializedAsset serializedAssetHeader;

    bool ret = binaryReader.StartRead(filePath);
    if (ret == false)
    {
        // log
        LOG_MESSAGE_ERROR("AssetLoader", "에셋로드 실패(잘못된포맷)");
        return nullptr;
    }

    serializedAssetHeader.DeSerialize(binaryReader);

    // 로드한 헤더부분을 보고 타입별로 분기(또는 Dispatcher를통해)하여 적절한 타입의 Loader에게 요청한다.
    std::unique_ptr<CoreAsset::SerializedAsset> serializedAssetPtr;
    binaryReader.SetReadPointer(0);
    // 분기

    switch (serializedAssetHeader.mAssetType)
    {
    case EAssetType::eTexture:

        serializedAssetPtr = mTextureLoader->LoadAssetFile(filePath, oAssetMetaData);
        break;
    case EAssetType::eMaterial:
        serializedAssetPtr = mMaterialLoader->LoadAssetFile(filePath, oAssetMetaData);
        break;
    }

    // 로드된 데이터를 리턴한다.
    if (serializedAssetPtr == nullptr)
    {
        LOG_MESSAGE_ERROR("AssetLoader", "에셋로드 실패");
        // log
        return nullptr;
    }

    return serializedAssetPtr;
}

std::unique_ptr<CoreAsset::SerializedAssetRawData> CoreAsset::AssetLoader::LoadAssetRawData(const std::string &filePath,
                                                                                            EAssetType assetType)
{
    QuadRW::BinaryReader binaryReader;

    std::unique_ptr<CoreAsset::SerializedAssetRawData> serializedRawPtr;
    switch (assetType)
    {
    case EAssetType::eTexture:

        serializedRawPtr = mTextureLoader->LoadRawFile(filePath);

        break;
    }

    if (serializedRawPtr == nullptr)
    {
        // log
    }

    return serializedRawPtr;
}
