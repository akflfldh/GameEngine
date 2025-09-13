#include "CoreAsset/AssetMetaDataType.h"

#include <BinaryReaderWriter/BinaryReader.h>
#include <BinaryReaderWriter/BinaryWriter.h>
#include <CoreBase/Arch.h>
void CoreAsset::AssetCommonHeader::Serialize(Arch &arch)
{
    /*  AssetID mAssetID = NoneAssetID;
      EAssetType mAssetType = EAssetType::eUnknown;
      FName mAssetName = "";*/

    arch << mAssetID;

    if (arch.GetLoadingFlag())
    {

        uint32_t assetType = 0;
        arch << assetType;

        size_t assetNameIndex = mAssetName.GetIndex();
        arch << assetNameIndex;

        mAssetType = (EAssetType)assetType;
        mAssetName = FName(assetNameIndex);
    }
    else
    {
        uint32_t assetType = static_cast<uint32_t>(mAssetType);
        arch << assetType;

        size_t assetNameIndex = mAssetName.GetIndex();
        arch << assetNameIndex;
    }
}

void CoreAsset::TextureCommonHeader::Serialize(Arch &arch)
{

    // raw file path

    arch << mRawFileName;
}

void CoreAsset::MaterialCommonHeader::Serialize(Arch &arch) {}