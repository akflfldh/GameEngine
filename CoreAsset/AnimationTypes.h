#pragma once

#include <CoreAsset/AssetType.h>
#include <CoreMath/CoreMath.h>

#include <limits>
#include <stdint.h>
#include <string>
#include <vector>

namespace CoreAsset
{
/// Skeleton reference pose와 AnimationClip sample이 공유하는 joint-local TRS 표현이다.
/// position은 cm, scale은 무차원이며 부모 누적 결과나 component world transform은 소유하지 않는다.
struct AnimationLocalTransform
{
    CoreMath::Vector3 mPosition{0.0f, 0.0f, 0.0f};
    CoreMath::Quaternion mRotation{0.0f, 0.0f, 0.0f, 1.0f};
    CoreMath::Vector3 mScale{1.0f, 1.0f, 1.0f};
};

/// Skeleton의 parent-first 배열에 저장되는 단일 joint 정의다.
/// 표시 이름은 중복될 수 있고 mStableKey가 clip track과 구조 호환성 검사의 identity 역할을 한다.
struct SkeletonJoint
{
    static constexpr uint32_t NoParent = (std::numeric_limits<uint32_t>::max)();

    std::string mDisplayName;                    ///< UI 표시용 이름이며 identity로 사용하지 않는다.
    std::string mStableKey;                      ///< Skeleton 내부에서 유일한 정규화 hierarchy key다.
    uint32_t mParentIndex = NoParent;            ///< NoParent 또는 현재 joint보다 앞선 parent index다.
    AnimationLocalTransform mReferenceLocalPose; ///< parent 기준 reference pose이며 world pose가 아니다.
};

/// 하나의 stable joint key에 대응하는 uniform baked local TRS channel 집합이다.
/// 비어 있는 channel은 reference pose fallback을 뜻하며 Skeleton이나 reference pose 자체는 소유하지 않는다.
struct AnimationJointTrack
{
    std::string mJointKey; ///< SkeletonJoint::mStableKey와 대응하는 joint identity다.
    std::vector<CoreMath::Vector3> mPositions;
    std::vector<CoreMath::Quaternion> mRotations;
    std::vector<CoreMath::Vector3> mScales;
};

enum class EAnimationTransformError : uint8_t
{
    eNone,
    eNonFinite,
    eInvalidRotation,
    eNonPositiveScale,
    eNonUniformScale,
    eShear
};

/// V1 joint transform 계약 검증 결과이며, importer와 저장 에셋 검증 경계에서 공통으로 사용한다.
struct AnimationTransformValidationResult
{
    bool mValid = false;
    EAnimationTransformError mError = EAnimationTransformError::eNone;
};

CORE_ASSET_API AnimationTransformValidationResult
ValidateAnimationJointTransform(const AnimationLocalTransform &transform,
                                const CoreMath::Matrix4X4 *sourceMatrix = nullptr, float tolerance = 0.0001f);

/// import 원본의 joint-weight 한 쌍이다.
/// 영구 정점 형식이 아니라 SkinningVertex의 최대 4개 influence를 만들기 위한 입력 데이터다.
struct SkinInfluence
{
    uint32_t mJointIndex = 0;
    float mWeight = 0.0f;
};

CORE_ASSET_API bool NormalizeSkinInfluences(const std::vector<SkinInfluence> &source, SkinningVertex &outVertex,
                                            std::string *failureReason = nullptr);

CORE_ASSET_API Arch &operator<<(Arch &arch, AnimationLocalTransform &transform);

struct AnimationClipSettings
{

    bool bLoop = false;
};

} // namespace CoreAsset
