#include "AnimationAssetLoader.h"

#include <CoreAsset/AnimationClip.h>
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/Skeleton.h>

namespace CoreAsset
{

AnimationAssetLoader *AnimationAssetLoader::GetInstance()
{
    static AnimationAssetLoader instance;
    return &instance;
}

bool AnimationAssetLoader::LoadAssetFile(EAssetType assetType, Arch &arch,
                                         AssetFactoryManager *assetFactoryManager, Asset *&outAsset,
                                         std::unique_ptr<AssetMetaData> &outMetaData)
{
    if (assetType != EAssetType::eSkeleton && assetType != EAssetType::eAnimation &&
        assetType != EAssetType::eAnimationTransitionSet)
        return false;

    outAsset = assetFactoryManager->CreateEmptyAsset(assetType);
    if (outAsset == nullptr)
        return false;

    outMetaData = std::make_unique<AssetMetaData>();
    outAsset->Serialize(arch);

    // Skeleton/Clip은 byte stream을 읽은 뒤에도 parent/signature/channel 계약을 검증한다.
    // 전이 세트는 현재 전용 Validate API가 없으므로 stream 상태만 확인한다.
    // 참조 에셋은 아직 모두 등록되지 않았을 수 있어 여기서 Skeleton/Clip을 resolve하지 않는다.
    bool isValid = arch.IsGood();
    std::string failureReason;
    if (isValid && assetType == EAssetType::eSkeleton)
        isValid = static_cast<Skeleton *>(outAsset)->Validate(&failureReason);
    else if (isValid && assetType == EAssetType::eAnimation)
        isValid = static_cast<AnimationClip *>(outAsset)->Validate(&failureReason);

    if (!isValid)
    {
        // 실패 반환에서는 소유권이 호출자에게 전달되지 않도록 생성한 객체와 metadata를 함께 정리한다.
        delete outAsset;
        outAsset = nullptr;
        outMetaData.reset();
    }
    return isValid;
}

bool AnimationAssetLoader::LoadAssetRawFile(Arch &, Asset *)
{
    return true;
}

} // namespace CoreAsset
