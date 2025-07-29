#include "AssetMetaDataType.h"

#include<BinaryWriter.h>
#include<BinaryReader.h>

void CoreAsset::AssetMetaData::Serialize(QuadRW::BinaryWriter& writer)
{

	writer.Write(mKeepRawDataFlag);

}
void CoreAsset::AssetMetaData::DeSerialize(QuadRW::BinaryReader& reader)
{

	reader.Read(mKeepRawDataFlag);
}


void CoreAsset::TextureMetaData::Serialize(QuadRW::BinaryWriter& writer)
{
	AssetMetaData::Serialize(writer);

}

void CoreAsset::TextureMetaData::DeSerialize(QuadRW::BinaryReader& reader)
{
	AssetMetaData::DeSerialize(reader);

}