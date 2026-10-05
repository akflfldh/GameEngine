#include "AnimationClip.h"

#include <CoreAsset/Skeleton.h>

#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace CoreAsset
{
namespace
{
void SerializeVector3(Arch &arch, CoreMath::Vector3 &value)
{
    arch << value.X << value.Y << value.Z;
}

void SerializeQuaternion(Arch &arch, CoreMath::Quaternion &value)
{
    arch << value.X << value.Y << value.Z << value.W;
}

template <typename T, typename Serializer>
void SerializeValues(Arch &arch, std::vector<T> &values, Serializer serializer)
{
    uint64_t count = static_cast<uint64_t>(values.size());
    arch << count;
    if (arch.GetLoadingFlag())
        values.resize(count);
    for (T &value : values)
        serializer(arch, value);
}

bool IsFinite(const CoreMath::Vector3 &value)
{
    return std::isfinite(value.X) && std::isfinite(value.Y) && std::isfinite(value.Z);
}

bool IsUnit(const CoreMath::Quaternion &value)
{
    const float lengthSquared = value.X * value.X + value.Y * value.Y + value.Z * value.Z + value.W * value.W;
    return std::isfinite(lengthSquared) && std::abs(lengthSquared - 1.0f) <= 0.0001f;
}

CoreMath::Vector3 Lerp(const CoreMath::Vector3 &lhs, const CoreMath::Vector3 &rhs, float alpha)
{
    return lhs * (1.0f - alpha) + rhs * alpha;
}

CoreMath::Quaternion NlerpShortest(CoreMath::Quaternion lhs, CoreMath::Quaternion rhs, float alpha)
{
    // q와 -q는 같은 회전을 나타낸다. dot이 음수면 한쪽 부호를 뒤집어 긴 호를 통과하지 않게 하고,
    // 선형 보간 뒤 정규화하여 이후 회전 연산의 unit quaternion 전제를 유지한다.
    const float dot = lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W;
    if (dot < 0.0f)
        rhs = {-rhs.X, -rhs.Y, -rhs.Z, -rhs.W};

    CoreMath::Quaternion result{lhs.X * (1.0f - alpha) + rhs.X * alpha, lhs.Y * (1.0f - alpha) + rhs.Y * alpha,
                                lhs.Z * (1.0f - alpha) + rhs.Z * alpha, lhs.W * (1.0f - alpha) + rhs.W * alpha};
    result.Normalize();
    return result;
}

bool HasValidChannelSize(size_t size, uint32_t sampleCount)
{
    return size == 0 || size == 1 || size == sampleCount;
}
} // namespace

Arch &operator<<(Arch &arch, AnimationJointTrack &track)
{
    /// 수정생각
    arch << track.mJointKey;
    SerializeValues(arch, track.mPositions, SerializeVector3);
    SerializeValues(arch, track.mRotations, SerializeQuaternion);
    SerializeValues(arch, track.mScales, SerializeVector3);
    return arch;
}

Arch &operator<<(Arch &arch, AnimationClipSettings &settings)
{

    arch << settings.bLoop;
    return arch;
}

AnimationClip::AnimationClip(AssetID id) : Asset(EAssetType::eAnimation, id), mValid(false) {}

bool AnimationClip::CopyDataFrom(const Asset &source, std::string *failureReason)
{
    const AnimationClip *sourceClip = dynamic_cast<const AnimationClip *>(&source);
    if (!sourceClip)
    {
        if (failureReason)
            *failureReason = "AnimationClip 에셋이 필요합니다.";
        return false;
    }

    // 재평가나 TRS 재분해 없이 baked sample을 그대로 복사한다.
    // missing/constant channel, sample rate 및 끝 sample의 의미도 바뀌지 않는다.
    mFormatVersion = sourceClip->mFormatVersion;
    mSkeletonAssetID = sourceClip->mSkeletonAssetID;
    mSkeletonSignature = sourceClip->mSkeletonSignature;
    mDurationSeconds = sourceClip->mDurationSeconds;
    mSampleRateNumerator = sourceClip->mSampleRateNumerator;
    mSampleRateDenominator = sourceClip->mSampleRateDenominator;
    mSampleCount = sourceClip->mSampleCount;
    mTracks = sourceClip->mTracks;
    mValid = sourceClip->mValid;
    return Asset::CopyDataFrom(source, failureReason);
}

EAssetType AnimationClip::GetAssetType()
{
    return EAssetType::eAnimation;
}

bool AnimationClip::Configure(AssetID skeletonAssetID, uint64_t skeletonSignature, float durationSeconds,
                              uint32_t sampleRateNumerator, uint32_t sampleRateDenominator, uint32_t sampleCount,
                              std::vector<AnimationJointTrack> tracks, std::string *failureReason)
{
    // 후보를 먼저 완전히 검증해 실패한 재설정이 이미 사용 중인 유효 clip 상태를 부분적으로 덮어쓰지 않게 한다.
    AnimationClip candidate;
    candidate.mSkeletonAssetID = skeletonAssetID;
    candidate.mSkeletonSignature = skeletonSignature;
    candidate.mDurationSeconds = durationSeconds;
    candidate.mSampleRateNumerator = sampleRateNumerator;
    candidate.mSampleRateDenominator = sampleRateDenominator;
    candidate.mSampleCount = sampleCount;
    candidate.mTracks = std::move(tracks);

    if (!candidate.Validate(failureReason))
        return false;

    mSkeletonAssetID = candidate.mSkeletonAssetID;
    mSkeletonSignature = candidate.mSkeletonSignature;
    mDurationSeconds = candidate.mDurationSeconds;
    mSampleRateNumerator = candidate.mSampleRateNumerator;
    mSampleRateDenominator = candidate.mSampleRateDenominator;
    mSampleCount = candidate.mSampleCount;
    mTracks = std::move(candidate.mTracks);

    SetEmptyAssetFlag(false);
    if (failureReason != nullptr)
        failureReason->clear();

    mValid = true;

    return true;
}

bool AnimationClip::SampleJoint(const std::string &jointKey, float timeSeconds, bool loop,
                                const AnimationLocalTransform &referencePose, AnimationLocalTransform &outPose,
                                std::string *failureReason) const
{
    if (!std::isfinite(timeSeconds) /*|| !Validate(failureReason)*/)
        return false;

    const AnimationJointTrack *track = nullptr;
    for (const AnimationJointTrack &candidate : mTracks)
    {
        if (candidate.mJointKey == jointKey)
        {
            track = &candidate;
            break;
        }
    }

    if (track == nullptr)
    {
        // track 자체가 없는 joint도 missing channel과 동일하게 Skeleton reference local pose를 사용한다.
        outPose = referencePose;
        if (failureReason != nullptr)
            failureReason->clear();
        return true;
    }

    float normalizedTime = 0.0f;
    if (mDurationSeconds > 0.0f)
    {
        if (loop)
        {
            // loop 계약은 [0,duration)이므로 정확한 duration은 0으로 돌아간다. fmod의 음수 결과도 같은
            // 반열린 구간으로 옮겨 음수 preview/playback 시간이 일관되게 wrap되도록 한다.
            normalizedTime = std::fmod(timeSeconds, mDurationSeconds);
            if (normalizedTime < 0.0f)
                normalizedTime += mDurationSeconds;
        }
        else
        {
            // non-loop는 마지막 sample을 표시해야 하므로 닫힌 구간 [0,duration]으로 clamp한다.
            normalizedTime = std::clamp(timeSeconds, 0.0f, mDurationSeconds);
        }
    }

    const float frame =
        normalizedTime * static_cast<float>(mSampleRateNumerator) / static_cast<float>(mSampleRateDenominator);
    const uint32_t lower = std::min(static_cast<uint32_t>(std::floor(frame)), mSampleCount - 1);
    const uint32_t upper = std::min(lower + 1, mSampleCount - 1);
    const float alpha = std::clamp(frame - static_cast<float>(lower), 0.0f, 1.0f);

    // reference pose로 시작한 뒤 존재하는 channel만 덮어써 missing channel fallback을 한 경로에서 보장한다.
    outPose = referencePose;
    if (!track->mPositions.empty())
    {
        outPose.mPosition = track->mPositions.size() == 1
                                ? track->mPositions[0]
                                : Lerp(track->mPositions[lower], track->mPositions[upper], alpha);
    }
    if (!track->mRotations.empty())
    {
        outPose.mRotation = track->mRotations.size() == 1
                                ? track->mRotations[0]
                                : NlerpShortest(track->mRotations[lower], track->mRotations[upper], alpha);
    }
    if (!track->mScales.empty())
    {
        outPose.mScale =
            track->mScales.size() == 1 ? track->mScales[0] : Lerp(track->mScales[lower], track->mScales[upper], alpha);
    }

    if (failureReason != nullptr)
        failureReason->clear();
    return true;
}

bool AnimationClip::IsCompatible(const Skeleton &skeleton) const
{
    return mSkeletonAssetID == skeleton.GetID() && mSkeletonSignature != 0 &&
           mSkeletonSignature == skeleton.GetStructureSignature();
}

AssetID AnimationClip::GetSkeletonAssetID() const
{
    return mSkeletonAssetID;
}

float AnimationClip::GetDurationSeconds() const
{
    return mDurationSeconds;
}

uint32_t AnimationClip::GetSampleCount() const
{
    return mSampleCount;
}

const std::vector<AnimationJointTrack> &AnimationClip::GetTracks() const
{
    return mTracks;
}

void AnimationClip::SetTracks(const std::vector<AnimationJointTrack> &tracks)
{

    mTracks = tracks;
    Validate();
}

void AnimationClip::Serialize(Arch &arch)
{
    Asset::Serialize(arch);
    arch << mFormatVersion << mSkeletonAssetID << mSkeletonSignature << mDurationSeconds;
    arch << mSampleRateNumerator << mSampleRateDenominator << mSampleCount << mAnimationClipSettings << mTracks;
}

float AnimationClip::GetSampleIntervalSeconds() const
{

    return float(mSampleRateDenominator) / mSampleRateNumerator;
}

bool AnimationClip::TrimFrameRange(uint32_t startFrame, uint32_t endFrame, std::string *failureReason)
{

    if (mSampleCount < endFrame || startFrame >= endFrame)
        return false;

    mSampleCount = endFrame - startFrame;

    for (int i = 0; i < mTracks.size(); ++i)
    {
        std::vector<CoreMath::Vector3> trimPositionsChannel(mTracks[i].mPositions.begin() + startFrame,
                                                            mTracks[i].mPositions.begin() + endFrame);
        std::vector<CoreMath::Vector3> trimScalesChannel(mTracks[i].mScales.begin() + startFrame,
                                                         mTracks[i].mScales.begin() + endFrame);
        std::vector<CoreMath::Quaternion> trimRotaionsChannel(mTracks[i].mRotations.begin() + startFrame,
                                                              mTracks[i].mRotations.begin() + endFrame);

        std::swap(mTracks[i].mPositions, trimPositionsChannel);
        std::swap(mTracks[i].mScales, trimScalesChannel);
        std::swap(mTracks[i].mRotations, trimRotaionsChannel);
    }

    mDurationSeconds = (mSampleCount - 1) * GetSampleIntervalSeconds();

    Validate(failureReason);

    return true;
}

void AnimationClip::SetLoop(bool value)
{

    mAnimationClipSettings.bLoop = value;
}

bool AnimationClip::GetLoop() const
{

    return mAnimationClipSettings.bLoop;
}

const AnimationClipSettings &AnimationClip::GetAnimationClipSettings() const
{

    return mAnimationClipSettings;
}

bool AnimationClip::GetValid() const
{
    return mValid;
}

bool AnimationClip::Validate(std::string *failureReason) const
{
    // 모든 실패 경로는 false를 유지하고, 전체 검사를 통과한 경우에만 true로 기록한다.
    // const 검증 API를 유지하며 에셋 데이터가 아닌 검증 결과만 갱신한다.
    mValid = false;
    auto fail = [failureReason](const char *message)
    {
        if (failureReason != nullptr)
            *failureReason = message;
        return false;
    };

    if (mFormatVersion != FormatVersion)
        return fail("지원하지 않는 AnimationClip format version입니다.");
    if (mSkeletonAssetID == NoneAssetID || mSkeletonSignature == 0)
        return fail("AnimationClip의 Skeleton identity가 유효하지 않습니다.");
    if (!std::isfinite(mDurationSeconds) || mDurationSeconds < 0.0f || mSampleRateNumerator == 0 ||
        mSampleRateDenominator == 0 || mSampleCount == 0)
        return fail("AnimationClip timing 값이 유효하지 않습니다.");

    // uniform baked track은 양 끝 sample을 포함하므로 duration=(sampleCount-1)/sampleRate여야 한다.
    // sample 하나인 clip은 zero-duration 정지 pose로 취급한다.
    const float expectedDuration = mSampleCount <= 1 ? 0.0f
                                                     : static_cast<float>(mSampleCount - 1) * mSampleRateDenominator /
                                                           static_cast<float>(mSampleRateNumerator);
    if (std::abs(mDurationSeconds - expectedDuration) > 0.0001f)
        return fail("durationSeconds와 rational sample rate/sample count가 일치하지 않습니다.");

    std::unordered_set<std::string> jointKeys;
    for (const AnimationJointTrack &track : mTracks)
    {
        if (track.mJointKey.empty() || !jointKeys.insert(track.mJointKey).second)
            return fail("Animation track joint key가 비어 있거나 중복되었습니다.");
        // channel은 reference fallback(0), constant(1), uniform baked(sampleCount) 세 형태만 허용한다.
        if (!HasValidChannelSize(track.mPositions.size(), mSampleCount) ||
            !HasValidChannelSize(track.mRotations.size(), mSampleCount) ||
            !HasValidChannelSize(track.mScales.size(), mSampleCount))
            return fail("Animation channel은 missing, constant 또는 uniform sample count여야 합니다.");
        for (const CoreMath::Vector3 &position : track.mPositions)
        {
            if (!IsFinite(position))
                return fail("Animation position channel에 finite가 아닌 값이 있습니다.");
        }
        for (const CoreMath::Quaternion &rotation : track.mRotations)
        {
            if (!IsUnit(rotation))
                return fail("Animation rotation channel은 unit quaternion이어야 합니다.");
        }
        for (const CoreMath::Vector3 &scale : track.mScales)
        {
            AnimationLocalTransform transform;
            transform.mScale = scale;
            if (!ValidateAnimationJointTransform(transform).mValid)
                return fail("Animation scale channel이 V1 transform 계약을 위반합니다.");
        }
    }

    mValid = true;
    return true;
}

} // namespace CoreAsset
