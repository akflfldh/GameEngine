#pragma once

#include <CoreAsset/AssetStorer.h>

namespace CoreAsset
{

/// Skeleton/AnimationClip/AnimationTransitionSet의 명시적 필드를 asset payload로 저장하는 storer다.
/// raw FBX source나 runtime playback 상태는 저장하지 않는다.
class CORE_ASSET_API AnimationAssetStorer : public AssetStorer
{
  public:
    static AnimationAssetStorer *GetInstance();
    void StoreAssetFile(Arch &arch, Asset *asset, AssetMetaData *assetMetaData) override;
    bool Store(Asset *asset, AssetMetaData *metaData, const std::string &filePath) override;
    bool StoreAssetRawDataFile(Arch &arch, Asset *asset, AssetMetaData *metaData) override;
};

} // namespace CoreAsset
