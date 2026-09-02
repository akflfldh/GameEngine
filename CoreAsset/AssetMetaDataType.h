#pragma once
#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <string>

#include <CoreBase/FName.h>
#include <CoreBase/FString.h>

class Arch;

namespace QuadRW
{
class BinaryWriter;
class BinaryReader;
} // namespace QuadRW

namespace CoreAsset
{

// 정확히 이친구는 어떻게 어디에쓸지 고민해야한다.
struct CORE_ASSET_API AssetMetaData
{

  public:
    AssetMetaData() = default;
    AssetMetaData(AssetID id, const std::string &assetName, EAssetType type)
        : mAssetID(id), mAssetName(assetName), mAssetType(type)
    {
    }

    AssetID mAssetID = NoneAssetID;
    std::string mAssetName = "";
    std::string mFilePath = ""; // 논리적 에셋파일경로
    EAssetType mAssetType = EAssetType::eUnknown;
    bool mKeepRawDataFlag = false;
    std::string mRawFileName = "";
};

struct CORE_ASSET_API TextureMetaData : public AssetMetaData
{
  public:
    //  FString mRawFilePath;
};

struct CORE_ASSET_API MaterialMetaData : public AssetMetaData
{

    // 현재 아무런정보도 없다.
    int i = 0;
};

struct CORE_ASSET_API MeshMetaData : public AssetMetaData
{
    int i = 0;
};

struct CORE_ASSET_API FontMetaData : public AssetMetaData
{
};

struct CORE_ASSET_API MapMetaData : public AssetMetaData
{
};

struct CORE_ASSET_API PrefabMetaData : public AssetMetaData
{
};

struct CORE_ASSET_API AssetCommonHeader
{
    // name
    // type
    // asset id
    static const uint32_t Magic = 0x04232152;

    uint32_t mMagic = 0x04232152;
    uint32_t mVersion = 1;
    AssetID mAssetID = NoneAssetID;
    EAssetType mAssetType = EAssetType::eUnknown;
    std::string mAssetName = "";
    std::string mAssetRawName = "";

    void Serialize(Arch &arch);
};

struct CORE_ASSET_API TextureCommonHeader
{
    // raw asset file path

    // runtime에도 필요할수도있어

    void Serialize(Arch &arch);
};

struct CORE_ASSET_API MaterialCommonHeader
{
    // 일단없다
    int i = 0;

    void Serialize(Arch &arch);
};

struct CORE_ASSET_API MeshCommonHeader
{

    void Serialize(Arch &arch);
};

} // namespace CoreAsset