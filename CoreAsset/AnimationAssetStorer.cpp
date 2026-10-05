#include "AnimationAssetStorer.h"

#include <CoreAsset/Asset.h>

namespace CoreAsset
{

AnimationAssetStorer *AnimationAssetStorer::GetInstance()
{
    static AnimationAssetStorer instance;
    return &instance;
}

void AnimationAssetStorer::StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *)
{
    asset->Serialize(arch);
}

bool AnimationAssetStorer::Store(Asset *, AssetMetaData *, const std::string &)
{
    return false;
}

bool AnimationAssetStorer::StoreAssetRawDataFile(Arch &, Asset *, AssetMetaData *)
{
    return true;
}

} // namespace CoreAsset
