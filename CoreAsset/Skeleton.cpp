#include "Skeleton.h"

#include <unordered_set>

namespace CoreAsset
{
namespace
{
void SetFailure(std::string *failureReason, const std::string &message)
{
    if (failureReason != nullptr)
        *failureReason = message;
}

void HashByte(uint64_t &hash, uint8_t value)
{
    hash ^= value;
    hash *= 1099511628211ull;
}

void HashUint32(uint64_t &hash, uint32_t value)
{
    for (uint32_t shift = 0; shift < 32; shift += 8)
        HashByte(hash, static_cast<uint8_t>((value >> shift) & 0xff));
}

bool ValidateJoints(const std::vector<SkeletonJoint> &joints, std::string *failureReason)
{
    // parent-first와 단일 logical root 계약은 runtime이 재귀 탐색 없이 앞에서부터 global pose를 누적하기 위한
    // 저장 규칙이다. 여러 scene root의 synthetic root 생성은 importer가 담당하며 여기서는 완성된 결과만 받는다.
    if (joints.empty())
    {
        SetFailure(failureReason, "Skeleton에는 최소 한 개의 joint가 필요합니다.");
        return false;
    }

    size_t rootCount = 0;
    std::unordered_set<std::string> stableKeys;
    for (size_t i = 0; i < joints.size(); ++i)
    {
        const SkeletonJoint &joint = joints[i];
        if (joint.mStableKey.empty() || !stableKeys.insert(joint.mStableKey).second)
        {
            SetFailure(failureReason, "Skeleton joint stable key가 비어 있거나 중복되었습니다.");
            return false;
        }

        if (joint.mParentIndex == SkeletonJoint::NoParent)
        {
            ++rootCount;
        }
        else if (joint.mParentIndex >= i)
        {
            SetFailure(failureReason, "Skeleton joint 배열은 parent-first 순서여야 합니다.");
            return false;
        }

        if (!ValidateAnimationJointTransform(joint.mReferenceLocalPose).mValid)
        {
            SetFailure(failureReason, "Skeleton reference pose가 V1 joint transform 계약을 위반합니다.");
            return false;
        }
    }

    if (rootCount != 1)
    {
        SetFailure(failureReason, "Skeleton에는 정확히 하나의 logical root가 필요합니다.");
        return false;
    }

    if (failureReason != nullptr)
        failureReason->clear();
    return true;
}
} // namespace

Arch &operator<<(Arch &arch, SkeletonJoint &joint)
{
    arch << joint.mDisplayName << joint.mStableKey << joint.mParentIndex << joint.mReferenceLocalPose;
    return arch;
}

Skeleton::Skeleton(AssetID id) : Asset(EAssetType::eSkeleton, id) {}

bool Skeleton::CopyDataFrom(const Asset &source, std::string *failureReason)
{
    const Skeleton *sourceSkeleton = dynamic_cast<const Skeleton *>(&source);
    if (!sourceSkeleton)
    {
        if (failureReason)
            *failureReason = "Skeleton 에셋이 필요합니다.";
        return false;
    }

    // signature는 AssetID가 아닌 joint 구조의 fingerprint이므로 동일 구조 복사에서는 유지한다.
    // 이 Skeleton을 참조하던 다른 에셋의 ID를 새 Skeleton으로 바꾸는 작업은 하지 않는다.
    mFormatVersion = sourceSkeleton->mFormatVersion;
    mStructureSignature = sourceSkeleton->mStructureSignature;
    mJoints = sourceSkeleton->mJoints;
    return Asset::CopyDataFrom(source, failureReason);
}

EAssetType Skeleton::GetAssetType()
{
    return EAssetType::eSkeleton;
}

bool Skeleton::SetJoints(std::vector<SkeletonJoint> joints, std::string *failureReason)
{
    // 검증이 끝난 뒤에만 소유 데이터를 교체하여 실패한 설정이 기존 유효 Skeleton을 훼손하지 않게 한다.
    if (!ValidateJoints(joints, failureReason))
        return false;

    mJoints = std::move(joints);
    mStructureSignature = CalculateStructureSignature(mJoints);
    SetEmptyAssetFlag(false);
    if (failureReason != nullptr)
        failureReason->clear();
    return true;
}

bool Skeleton::Validate(std::string *failureReason) const
{
    if (mFormatVersion != FormatVersion)
    {
        SetFailure(failureReason, "지원하지 않는 Skeleton format version입니다.");
        return false;
    }
    if (!ValidateJoints(mJoints, failureReason))
        return false;
    // 역직렬화된 signature를 현재 joint 구조에서 다시 계산해 손상되거나 다른 구조의 payload를 거부한다.
    if (mStructureSignature == 0 || mStructureSignature != CalculateStructureSignature(mJoints))
    {
        SetFailure(failureReason, "Skeleton structure signature가 저장된 joint 구조와 일치하지 않습니다.");
        return false;
    }

    if (failureReason != nullptr)
        failureReason->clear();
    return true;
}

const std::vector<SkeletonJoint> &Skeleton::GetJoints() const
{
    return mJoints;
}

uint32_t Skeleton::GetJointNum() const
{
    return mJoints.size();
}

uint64_t Skeleton::GetStructureSignature() const
{
    return mStructureSignature;
}

uint32_t Skeleton::GetFormatVersion() const
{
    return mFormatVersion;
}

void Skeleton::Serialize(Arch &arch)
{
    Asset::Serialize(arch);
    arch << mFormatVersion << mStructureSignature << mJoints;
}

uint64_t Skeleton::CalculateStructureSignature(const std::vector<SkeletonJoint> &joints)
{
    // 구조 호환성만 판정하도록 parent와 stable key를 FNV-1a 순서로 hash한다. 표시 이름과 reference pose는
    // 의도적으로 제외하여 pose 값이 바뀌어도 같은 joint topology/key를 대상으로 한 clip 호환성은 유지한다.
    uint64_t hash = 14695981039346656037ull;
    HashUint32(hash, FormatVersion);
    HashUint32(hash, static_cast<uint32_t>(joints.size()));
    for (const SkeletonJoint &joint : joints)
    {
        HashUint32(hash, joint.mParentIndex);
        for (const char character : joint.mStableKey)
            HashByte(hash, static_cast<uint8_t>(character));
        HashByte(hash, 0);
    }
    return hash == 0 ? 1 : hash;
}

} // namespace CoreAsset
