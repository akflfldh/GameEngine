#pragma once

#include <CoreAsset/AssetLoader.h>

namespace CoreAsset
{

/// 애니메이션 에셋 payload를 역직렬화하고 Skeleton/AnimationClip의 V1 schema를 검증하는 loader다.
/// AnimationTransitionSet은 현재 stream 상태만 확인하며 전이 설정의 의미 검증은 수행하지 않는다.
/// Skeleton 의존성 resolve나 runtime/GPU 객체 생성은 수행하지 않는다.
class CORE_ASSET_API AnimationAssetLoader : public AssetLoader
{
  public:
    static AnimationAssetLoader *GetInstance();
    bool LoadAssetFile(EAssetType assetType, Arch &arch, AssetFactoryManager *assetFactoryManager,
                       Asset *&outAsset, std::unique_ptr<AssetMetaData> &outMetaData) override;
    bool LoadAssetRawFile(Arch &arch, Asset *asset) override;
};

} // namespace CoreAsset
