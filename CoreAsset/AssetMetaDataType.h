#pragma once
#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <string>

namespace QuadRW
{
class BinaryWriter;
class BinaryReader;
} // namespace QuadRW

namespace CoreAsset
{

// 정확히 이친구는 어떻게 어디에쓸지 고민해야한다.
class CORE_ASSET_API AssetMetaData
{
  public:
    AssetID mAssetID = NoneAssetID;
    std::string mAssetName = "";
    std::string mFilePath = ""; // 실제
    EAssetType mAssetType = EAssetType::eUnknown;
    bool mKeepRawDataFlag = false;

    // AssetMetaData() = default;
    //~AssetMetaData() = default;

    // AssetMetaData(const AssetMetaData&) = default;
    // AssetMetaData(AssetMetaData&&) = default;

    // AssetMetaData& operator=(const AssetMetaData&) = default;
    // AssetMetaData& operator=(AssetMetaData&&) = default;
    //

    virtual void Serialize(QuadRW::BinaryWriter &writer);
    virtual void DeSerialize(QuadRW::BinaryReader &reader);
};

class CORE_ASSET_API TextureMetaData : public AssetMetaData
{
  public:
    // TextureMetaData() = default;
    //~TextureMetaData() = default;

    // TextureMetaData(const TextureMetaData&) = default;
    // TextureMetaData(TextureMetaData&&) = default;

    // TextureMetaData& operator=(const TextureMetaData&) = default;
    // TextureMetaData& operator=(TextureMetaData&&) = default;

    virtual void Serialize(QuadRW::BinaryWriter &writer) override;
    virtual void DeSerialize(QuadRW::BinaryReader &reader) override;
};

} // namespace CoreAsset