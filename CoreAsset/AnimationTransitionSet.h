#pragma once

#include <CoreAsset/AnimationTypes.h>
#include <CoreAsset/Asset.h>
#include <CoreAsset/AssetPtr.h>
#include <unordered_map>
#include <vector>

namespace CoreAsset
{

/// Skeleton과 클립 간 공유 전이 설정을 소유한다. 편집 UI의 임시 데이터와
/// Animator별 재생 시간·전이 진행 상태·pose는 소유하지 않는다.
class CORE_ASSET_API AnimationTransitionSet : public Asset
{
  public:
    explicit AnimationTransitionSet(AssetID id = NoneAssetID);
    ~AnimationTransitionSet() override = default;

    static EAssetType GetAssetType();

    void Serialize(Arch &arch) override;
    bool CopyDataFrom(const Asset &source, std::string *failureReason = nullptr) override;

    bool AddTransitionData(const AnimationTransitionData &data, std::string *oMessage = nullptr);

    bool RemoveTransitionData(CoreAsset::AssetID preAnimClip, CoreAsset::AssetID nextAnimClip,
                              std::string *oMessage = nullptr);
    // ui 편집에서 설정할때 사용 // 강제로 덮어쓰기
    bool OverWriteTransitionData(const AnimationTransitionData &data, std::string *oMessage = nullptr);

    void SetAnimationSkeleton(AssetPtr skeleton);
    const AssetPtr &GetAnimationSkeleton() const;

    bool GetTransitionData(AssetID preAnimClip, AssetID nextAnimClip, AnimationTransitionData &oData);

    const std::vector<AnimationTransitionData> &GetAnimationTransitionDataList() const;
    bool SetAnimationTransitionDataList(const std::vector<AnimationTransitionData> &dataList,
                                        std::string *oMessage = nullptr);

  private:
    std::vector<AnimationTransitionData> mTransitionDataList;
    AssetPtr mSkeleton;
};

AssetClassName(AnimationTransitionSet)

} // namespace CoreAsset
