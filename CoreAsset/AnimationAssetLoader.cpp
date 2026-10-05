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
    if (assetType != EAssetType::eSkeleton && assetType != EAssetType::eAnimation)
        return false;

    outAsset = assetFactoryManager->CreateEmptyAsset(assetType);
    if (outAsset == nullptr)
        return false;

    outMetaData = std::make_unique<AssetMetaData>();
    outAsset->Serialize(arch);

    // byte stream을 끝까지 읽은 것만으로는 parent/signature/channel 계약을 보장할 수 없다.
    // 역직렬화 직후 타입별 schema를 다시 검증해 의미상 손상된 에셋이 registry로 넘어가지 않게 한다.
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
