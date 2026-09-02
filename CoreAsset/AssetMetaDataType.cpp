#include "CoreAsset/AssetMetaDataType.h"

#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>
#include <CoreBase/Arch.h>

void CoreAsset::AssetCommonHeader::Serialize(Arch &arch)
{
    arch << mMagic;
    arch << mVersion;
    arch << mAssetID;
    arch << mAssetType;
    arch << mAssetName;
    arch << mAssetRawName;
}

void CoreAsset::TextureCommonHeader::Serialize(Arch &arch)
{

    // raw file path
}

void CoreAsset::MaterialCommonHeader::Serialize(Arch &arch) {}

void CoreAsset::MeshCommonHeader::Serialize(Arch &arch) {}