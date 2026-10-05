#include "AnimationTypes.h"

#include <algorithm>
#include <cmath>

namespace CoreAsset
{
namespace
{
bool IsFinite(const CoreMath::Vector3 &value)
{
    return std::isfinite(value.X) && std::isfinite(value.Y) && std::isfinite(value.Z);
}

bool IsFinite(const CoreMath::Quaternion &value)
{
    return std::isfinite(value.X) && std::isfinite(value.Y) && std::isfinite(value.Z) && std::isfinite(value.W);
}

float Length(const CoreMath::Vector3 &value)
{
    return std::sqrt(value.X * value.X + value.Y * value.Y + value.Z * value.Z);
}

bool HasShear(const CoreMath::Matrix4X4 &matrix, float tolerance)
{
    // Decompose의 성공 여부만으로는 shear 보존을 판정할 수 없다. 각 basis의 scale을 제거한 뒤
    // 서로 직교하는지 직접 검사하고, 정규화할 수 없는 축도 V1 transform으로 표현할 수 없으므로 거부한다.
    const CoreMath::Vector3 axes[3] = {
        {matrix.mat[0].X, matrix.mat[0].Y, matrix.mat[0].Z},
        {matrix.mat[1].X, matrix.mat[1].Y, matrix.mat[1].Z},
        {matrix.mat[2].X, matrix.mat[2].Y, matrix.mat[2].Z}};

    CoreMath::Vector3 normalized[3];
    for (int i = 0; i < 3; ++i)
    {
        const float length = Length(axes[i]);
        if (!std::isfinite(length) || length <= tolerance)
            return true;
        normalized[i] = axes[i] / length;
    }

    return std::abs(normalized[0].Dot(normalized[1])) > tolerance ||
           std::abs(normalized[0].Dot(normalized[2])) > tolerance ||
           std::abs(normalized[1].Dot(normalized[2])) > tolerance;
}
} // namespace

AnimationTransformValidationResult ValidateAnimationJointTransform(const AnimationLocalTransform &transform,
                                                                   const CoreMath::Matrix4X4 *sourceMatrix,
                                                                   float tolerance)
{
    // V1 Animation은 안정적으로 누적 가능한 finite TRS, unit quaternion, positive uniform scale만 허용한다.
    // importer와 저장 에셋이 같은 함수를 사용해 입력 경계마다 동일한 계약을 적용한다.
    if (!IsFinite(transform.mPosition) || !IsFinite(transform.mRotation) || !IsFinite(transform.mScale))
        return {false, EAnimationTransformError::eNonFinite};

    const float rotationLengthSquared = transform.mRotation.X * transform.mRotation.X +
                                        transform.mRotation.Y * transform.mRotation.Y +
                                        transform.mRotation.Z * transform.mRotation.Z +
                                        transform.mRotation.W * transform.mRotation.W;
    if (rotationLengthSquared <= tolerance * tolerance || std::abs(rotationLengthSquared - 1.0f) > tolerance)
        return {false, EAnimationTransformError::eInvalidRotation};

    if (transform.mScale.X <= 0.0f || transform.mScale.Y <= 0.0f || transform.mScale.Z <= 0.0f)
        return {false, EAnimationTransformError::eNonPositiveScale};

    // 큰 scale에서도 고정 절대 오차만 사용해 정상 입력을 거부하지 않도록 상대 tolerance로 비교한다.
    const float largestScale = std::max({transform.mScale.X, transform.mScale.Y, transform.mScale.Z, 1.0f});
    const float scaleTolerance = tolerance * largestScale;
    if (std::abs(transform.mScale.X - transform.mScale.Y) > scaleTolerance ||
        std::abs(transform.mScale.X - transform.mScale.Z) > scaleTolerance)
        return {false, EAnimationTransformError::eNonUniformScale};

    // 분해된 TRS에는 원본 shear 정보가 남지 않으므로, importer가 원본 matrix를 제공한 경우에만 별도로 검사한다.
    if (sourceMatrix != nullptr && HasShear(*sourceMatrix, tolerance))
        return {false, EAnimationTransformError::eShear};

    return {true, EAnimationTransformError::eNone};
}

bool NormalizeSkinInfluences(const std::vector<SkinInfluence> &source, SkinningVertex &outVertex,
                             std::string *failureReason)
{
    // V1의 정점당 최대 4개 제한을 적용하기 전에 invalid/0 weight를 제거한다. 동률은 joint index로
    // 정렬해 import 실행이나 입력 순서가 달라도 같은 palette 결과를 만들고, 선택된 weight만 다시 정규화한다.
    std::vector<SkinInfluence> valid;
    valid.reserve(source.size());
    for (const SkinInfluence &influence : source)
    {
        if (std::isfinite(influence.mWeight) && influence.mWeight > 0.0f)
            valid.push_back(influence);
    }

    std::sort(valid.begin(), valid.end(), [](const SkinInfluence &lhs, const SkinInfluence &rhs) {
        if (lhs.mWeight != rhs.mWeight)
            return lhs.mWeight > rhs.mWeight;
        return lhs.mJointIndex < rhs.mJointIndex;
    });

    if (valid.size() > 4)
        valid.resize(4);

    float totalWeight = 0.0f;
    for (const SkinInfluence &influence : valid)
        totalWeight += influence.mWeight;

    if (!std::isfinite(totalWeight) || totalWeight <= 0.0f)
    {
        if (failureReason != nullptr)
            *failureReason = "정점에 유효한 positive skin influence가 없습니다.";
        return false;
    }

    outVertex.mJointIndices = {};
    outVertex.mJointWeights = {};
    for (size_t i = 0; i < valid.size(); ++i)
    {
        outVertex.mJointIndices[i] = valid[i].mJointIndex;
        outVertex.mJointWeights[i] = valid[i].mWeight / totalWeight;
    }

    if (failureReason != nullptr)
        failureReason->clear();
    return true;
}

Arch &operator<<(Arch &arch, AnimationLocalTransform &transform)
{
    // compiler padding이나 CoreMath 타입의 내부 배치에 의존하지 않도록 V1 필드 순서를 성분 단위로 고정한다.
    arch << transform.mPosition.X << transform.mPosition.Y << transform.mPosition.Z;
    arch << transform.mRotation.X << transform.mRotation.Y << transform.mRotation.Z << transform.mRotation.W;
    arch << transform.mScale.X << transform.mScale.Y << transform.mScale.Z;
    return arch;
}

} // namespace CoreAsset
