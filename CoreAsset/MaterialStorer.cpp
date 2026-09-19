#include "CoreAsset/MaterialStorer.h"

#include "CoreAsset/Material.h"
#include <BinaryReaderWriter/BinaryWriter.h>

CoreAsset::MaterialStorer *CoreAsset::MaterialStorer::GetInstance()
{
    static MaterialStorer instance;
    return &instance;
}

CoreAsset::MaterialStorer::MaterialStorer() {}

CoreAsset::MaterialStorer::~MaterialStorer() {}

void CoreAsset::MaterialStorer::StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *assetMetaData)
{

    // material header serialize 가능

    MaterialMetaData *materialMetaData = static_cast<MaterialMetaData *>(assetMetaData);

    MaterialCommonHeader materialCommonHeader;
    materialCommonHeader.Serialize(arch);

    asset->Serialize(arch);
}

bool CoreAsset::MaterialStorer::Store(CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData,
                                      const std::string &filePath)
{
    return true;
}

bool CoreAsset::MaterialStorer::StoreAssetRawDataFile(Arch & arch ,CoreAsset::Asset *asset, CoreAsset::AssetMetaData *metaData
                                                      )
{
    return false;
}
