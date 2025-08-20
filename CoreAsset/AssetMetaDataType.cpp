#include "CoreAsset/AssetMetaDataType.h"

#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>

void CoreAsset::AssetMetaData::Serialize(QuadRW::BinaryWriter &writer)
{
    writer.Write(mAssetID);
    writer.Write(mAssetName);
    writer.Write(mFilePath);
    writer.Write((uint32_t)mAssetType);
    writer.Write(mKeepRawDataFlag);
}
void CoreAsset::AssetMetaData::DeSerialize(QuadRW::BinaryReader &reader)
{
    reader.Read(mAssetID);
    reader.Read(mAssetName);
    reader.Read(mFilePath);

    uint32_t assetType;
    reader.Read(assetType);
    mAssetType = (EAssetType)assetType;
    reader.Read(mKeepRawDataFlag);
}

void CoreAsset::TextureMetaData::Serialize(QuadRW::BinaryWriter &writer)
{
    AssetMetaData::Serialize(writer);
}

void CoreAsset::TextureMetaData::DeSerialize(QuadRW::BinaryReader &reader)
{
    AssetMetaData::DeSerialize(reader);
}