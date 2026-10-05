#pragma once

#include <CoreAsset/IAssetFactory.h>

namespace CoreAsset
{

/// Skeleton과 AnimationClip의 빈 저장 객체 생성을 담당하는 CoreAsset 조립 adapter다.
/// FBX SDK 데이터 해석이나 runtime Animator 생성은 담당하지 않는다.
class CORE_ASSET_API AnimationAssetFactory : public IAssetFactory
{
  public:
    static AnimationAssetFactory *GetInstance();
    Asset *CreateEmptyAsset(EAssetType assetType) override;
    Asset *CreateAssetFromData(const IntermediateAsset &intermediateAsset) override;

    void DestoryAsset(Asset *asset);

  private:
};

} // namespace CoreAsset
