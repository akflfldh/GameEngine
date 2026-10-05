#include "AnimationAssetFactory.h"

#include <CoreAsset/AnimationClip.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreAsset/Skeleton.h>

namespace CoreAsset
{

AnimationAssetFactory *AnimationAssetFactory::GetInstance()
{
    static AnimationAssetFactory instance;
    return &instance;
}

Asset *AnimationAssetFactory::CreateEmptyAsset(EAssetType assetType)
{
    if (assetType == EAssetType::eSkeleton)
        return new Skeleton;
    if (assetType == EAssetType::eAnimation)
        return new AnimationClip;

    return nullptr;
}

Asset *AnimationAssetFactory::CreateAssetFromData(const IntermediateAsset &intermeidateAsset)
{
    // FBX skeleton/clip intermediate 생성은 Golden FBX가 준비되는 후속 importer 단계에서 연결한다.

    if (intermeidateAsset.mAssetType == EAssetType::eSkeleton)
    {

        const IntermediateSkeleton &intermediateSkeleton = static_cast<const IntermediateSkeleton &>(intermeidateAsset);

        Skeleton *skeleton = static_cast<Skeleton *>(CreateEmptyAsset(EAssetType::eSkeleton));
        if (!skeleton)
        {
            return nullptr;
        }

        std::string failReason;
        bool ret = skeleton->SetJoints(intermediateSkeleton.mSkeletonJoints, &failReason);

        if (ret == false)
        {
            DestoryAsset(skeleton);
            return nullptr;
        }

        return skeleton;
    }
    else if (intermeidateAsset.mAssetType == EAssetType::eAnimation)
    {

        const IntermediateAnimationClip &intermediateAnimationClip =
            static_cast<const IntermediateAnimationClip &>(intermeidateAsset);

        AnimationClip *animationClip = static_cast<AnimationClip *>(CreateEmptyAsset(EAssetType::eAnimation));
        if (!animationClip)
        {
            return nullptr;
        }

        // animationClip->SetTracks(intermediateAnimationClip.mTracks);

        return animationClip;
    }

    return nullptr;
}

void AnimationAssetFactory::DestoryAsset(Asset *asset)
{

    if (asset == nullptr)
        return;

    delete asset;
}

} // namespace CoreAsset
