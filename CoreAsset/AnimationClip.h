#pragma once

#include <CoreAsset/AnimationTypes.h>
#include <CoreAsset/Asset.h>

#include <string>
#include <vector>

namespace CoreAsset
{

CORE_ASSET_API Arch &operator<<(Arch &arch, AnimationJointTrack &track);

class Skeleton;

/// 특정 Skeleton 구조를 대상으로 하며 runtime 재생 instance와 분리된 animation 데이터 에셋이다.
/// 대상 identity, seconds 기반 timing과 baked local tracks를 소유하지만 Skeleton 객체의 수명, reference pose,
/// 재생 시간·loop 상태 같은 Animator instance 상태, GPU resource는 소유하지 않는다.
class CORE_ASSET_API AnimationClip : public Asset
{
  public:
    static constexpr uint32_t FormatVersion = 1;

    explicit AnimationClip(AssetID id = NoneAssetID);
    ~AnimationClip() override = default;

    static EAssetType GetAssetType();

    bool Configure(AssetID skeletonAssetID, uint64_t skeletonSignature, float durationSeconds,
                   uint32_t sampleRateNumerator, uint32_t sampleRateDenominator, uint32_t sampleCount,
                   std::vector<AnimationJointTrack> tracks, std::string *failureReason = nullptr);
    bool SampleJoint(const std::string &jointKey, float timeSeconds, bool loop,
                     const AnimationLocalTransform &referencePose, AnimationLocalTransform &outPose,
                     std::string *failureReason = nullptr) const;
    bool IsCompatible(const Skeleton &skeleton) const;
    bool Validate(std::string *failureReason = nullptr) const;

    AssetID GetSkeletonAssetID() const;
    float GetDurationSeconds() const;
    uint32_t GetSampleCount() const;
    const std::vector<AnimationJointTrack> &GetTracks() const;
    void SetTracks(const std::vector<AnimationJointTrack> &tracks);

    void Serialize(Arch &arch) override;
    bool CopyDataFrom(const Asset &source, std::string *failureReason = nullptr) override;

    float GetSampleIntervalSeconds() const;

    bool TrimFrameRange(uint32_t startFrame, uint32_t endFrame, std::string *failureReason = nullptr);

    void SetLoop(bool value);
    bool GetLoop() const;

    const AnimationClipSettings &GetAnimationClipSettings() const;

    bool GetValid() const;

  private:
    uint32_t mFormatVersion = FormatVersion;
    AssetID mSkeletonAssetID = NoneAssetID; ///< 대상 Skeleton 에셋의 비소유 identity다.
    uint64_t mSkeletonSignature = 0;        ///< 대상 Skeleton joint 구조와의 호환성 fingerprint다.
    float mDurationSeconds = 0.0f;
    uint32_t mSampleRateNumerator = 1;        ///< uniform sample rate의 유리수 분자다.
    uint32_t mSampleRateDenominator = 1;      ///< uniform sample rate의 유리수 분모다.
    uint32_t mSampleCount = 1;                ///< clip 시간축의 sample 수이며 non-constant channel 길이의 기준이다.
    std::vector<AnimationJointTrack> mTracks; ///< stable joint key별 baked local channel 소유 데이터다.

    AnimationClipSettings mAnimationClipSettings;
    // 마지막 전체 검증의 결과만 기록한다. 미검증도 false이며, 재생 허용을 위한
    // 캐시로 사용하려면 데이터 변경 시 무효화와 변경 완료 후 검증을 보장해야 한다.
    mutable bool mValid = false;
};
AssetClassName(AnimationClip)

} // namespace CoreAsset
