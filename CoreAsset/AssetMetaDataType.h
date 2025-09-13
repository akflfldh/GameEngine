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
    AssetID mAssetID = NoneAssetID;
    std::string mAssetName = "";
    std::string mFilePath = ""; // 논리적 에셋파일경로
    EAssetType mAssetType = EAssetType::eUnknown;
    bool mKeepRawDataFlag = false;
    FString mRawFileName = "";
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

struct CORE_ASSET_API AssetCommonHeader
{
    // name
    // type
    // asset id
    AssetID mAssetID = NoneAssetID;
    EAssetType mAssetType = EAssetType::eUnknown;
    FName mAssetName = "";

    void Serialize(Arch &arch);
};

struct CORE_ASSET_API TextureCommonHeader
{
    // raw asset file path

    // runtime에도 필요할수도있어
    FString mRawFileName;
    void Serialize(Arch &arch);
};

struct CORE_ASSET_API MaterialCommonHeader
{
    // 일단없다
    int i = 0;

    void Serialize(Arch &arch);
};

} // namespace CoreAsset