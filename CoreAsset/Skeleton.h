#pragma once

#include <CoreAsset/AnimationTypes.h>
#include <CoreAsset/Asset.h>

#include <limits>
#include <string>
#include <vector>

namespace CoreAsset
{

CORE_ASSET_API Arch &operator<<(Arch &arch, SkeletonJoint &joint);

/// joint hierarchy와 reference local pose를 소유하는 독립 저장 에셋이다.
/// SkinBinding과 AnimationClip은 이 에셋을 AssetID/signature로 참조하지만 Skeleton은 그 객체나 수명을
/// 소유하지 않는다. runtime Animator 상태, mesh vertex, inverse bind, GPU palette도 포함하지 않는다.
class CORE_ASSET_API Skeleton : public Asset
{
  public:
    static constexpr uint32_t FormatVersion = 1;

    explicit Skeleton(AssetID id = NoneAssetID);
    ~Skeleton() override = default;

    static EAssetType GetAssetType();

    bool SetJoints(std::vector<SkeletonJoint> joints, std::string *failureReason = nullptr);
    bool Validate(std::string *failureReason = nullptr) const;
    const std::vector<SkeletonJoint> &GetJoints() const;
    uint32_t GetJointNum() const;
    uint64_t GetStructureSignature() const;
    uint32_t GetFormatVersion() const;

    void Serialize(Arch &arch) override;
    bool CopyDataFrom(const Asset &source, std::string *failureReason = nullptr) override;

  private:
    static uint64_t CalculateStructureSignature(const std::vector<SkeletonJoint> &joints);

    uint32_t mFormatVersion = FormatVersion;
    /// format version, joint 수, parent index, stable key로 만든 구조 호환성 fingerprint다.
    /// AssetID를 대체하거나 reference pose 전체의 content hash 역할을 하지 않는다.
    uint64_t mStructureSignature = 0;
    std::vector<SkeletonJoint> mJoints; ///< parent-first 순서의 joint와 reference local pose 소유 데이터다.
};
AssetClassName(Skeleton)

} // namespace CoreAsset
