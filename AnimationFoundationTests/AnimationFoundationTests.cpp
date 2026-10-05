#include <CoreAsset/AnimationAssetFactory.h>
#include <CoreAsset/AnimationAssetLoader.h>
#include <CoreAsset/AnimationClip.h>
#include <CoreAsset/AssetFactoryManager.h>
#include <CoreAsset/AssetMetaDataType.h>
#include <CoreAsset/Skeleton.h>
#include <CoreAsset/SkinningMesh.h>
#include <CoreBase/BinaryArch.h>
#include <CoreMath/CoreMath.h>
#include <ImportModule/FBXAxisPolicy.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace
{
constexpr float kEpsilon = 0.0001f;
constexpr float kPi = 3.14159265358979323846f;

class TestHarness
{
  public:
    void Check(bool condition, std::string_view name)
    {
        if (condition)
        {
            std::cout << "[PASS] " << name << '\n';
            ++mPassCount;
            return;
        }

        std::cerr << "[FAIL] " << name << '\n';
        ++mFailureCount;
    }

    void CheckVector(const CoreMath::Vector3 &actual, const CoreMath::Vector3 &expected, std::string_view name,
                     float epsilon = kEpsilon)
    {
        const bool passed = IsNear(actual.X, expected.X, epsilon) && IsNear(actual.Y, expected.Y, epsilon) &&
                            IsNear(actual.Z, expected.Z, epsilon);

        PrintVectorComparison(name, actual, expected, passed);
        passed ? ++mPassCount : ++mFailureCount;
    }

    void CheckQuaternion(const CoreMath::Quaternion &actual, const CoreMath::Quaternion &expected,
                         std::string_view name, float epsilon = kEpsilon)
    {
        const bool passed = IsNear(actual.X, expected.X, epsilon) && IsNear(actual.Y, expected.Y, epsilon) &&
                            IsNear(actual.Z, expected.Z, epsilon) && IsNear(actual.W, expected.W, epsilon);

        std::ostream &stream = passed ? std::cout : std::cerr;
        stream << (passed ? "[PASS] " : "[FAIL] ") << name << " actual=(" << actual.X << ", " << actual.Y << ", "
               << actual.Z << ", " << actual.W << ") expected=(" << expected.X << ", " << expected.Y << ", "
               << expected.Z << ", " << expected.W << ")\n";

        passed ? ++mPassCount : ++mFailureCount;
    }

    void CheckFloat(float actual, float expected, std::string_view name, float epsilon = kEpsilon)
    {
        const bool passed = IsNear(actual, expected, epsilon);
        std::ostream &stream = passed ? std::cout : std::cerr;
        stream << (passed ? "[PASS] " : "[FAIL] ") << name << " actual=" << actual
               << " expected=" << expected << '\n';
        passed ? ++mPassCount : ++mFailureCount;
    }

    int Finish() const
    {
        std::cout << "[SUMMARY] PASS=" << mPassCount << " FAIL=" << mFailureCount << '\n';
        return mFailureCount == 0 ? 0 : 1;
    }

  private:
    static bool IsNear(float lhs, float rhs, float epsilon)
    {
        return std::abs(lhs - rhs) <= epsilon;
    }

    static void PrintVectorComparison(std::string_view name, const CoreMath::Vector3 &actual,
                                      const CoreMath::Vector3 &expected, bool passed)
    {
        std::ostream &stream = passed ? std::cout : std::cerr;
        stream << (passed ? "[PASS] " : "[FAIL] ") << name << " actual=(" << actual.X << ", " << actual.Y << ", "
               << actual.Z << ") expected=(" << expected.X << ", " << expected.Y << ", " << expected.Z << ")\n";
    }

    int mPassCount = 0;
    int mFailureCount = 0;
};

bool IsNear(const CoreMath::Vector3 &lhs, const CoreMath::Vector3 &rhs, float epsilon = kEpsilon)
{
    return std::abs(lhs.X - rhs.X) <= epsilon && std::abs(lhs.Y - rhs.Y) <= epsilon &&
           std::abs(lhs.Z - rhs.Z) <= epsilon;
}

bool IsNear(const CoreMath::Matrix4X4 &lhs, const CoreMath::Matrix4X4 &rhs, float epsilon = kEpsilon)
{
    for (int column = 0; column < 4; ++column)
    {
        for (int row = 0; row < 4; ++row)
        {
            if (std::abs(lhs.mat[column][row] - rhs.mat[column][row]) > epsilon)
                return false;
        }
    }

    return true;
}

void RunMatrixPointDirectionFixture(TestHarness &harness)
{
    // 번역 (10, 20, 30)은 점(w=1)에만 더해지고 방향(w=0)에는 영향을 주지 않아야 한다.
    // expected는 행렬 API를 재사용하지 않고 입력 벡터에 번역을 손으로 더해 얻었다.
    const CoreMath::Matrix4X4 translation = CoreMath::Matrix4X4::MakeTranslation(10.0f, 20.0f, 30.0f);
    const CoreMath::Vector3 input{1.0f, 2.0f, 3.0f};

    harness.CheckVector(translation.TransformPoint(input), {11.0f, 22.0f, 33.0f},
                        "FT-MATRIX-001 TransformPoint applies translation");
    harness.CheckVector(translation.TransformDirection(input), input,
                        "FT-MATRIX-001 TransformDirection ignores translation");
    harness.CheckVector(translation * input, input, "FT-MATRIX-001 Matrix4X4 * Vector3 uses w=0");
}

void RunTrsFixture(TestHarness &harness)
{
    // 회전이 없는 단순 축척과 번역을 사용하면 S -> R -> T 결과를 성분별 산술로 검산할 수 있다.
    // (1, 1, 1) * (2, 3, 4) + (10, 20, 30) = (12, 23, 34)이다.
    const CoreMath::Matrix4X4 transform = CoreMath::Matrix4X4::MakeTransform(
        {10.0f, 20.0f, 30.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {2.0f, 3.0f, 4.0f});

    harness.CheckVector(transform.TransformPoint({1.0f, 1.0f, 1.0f}), {12.0f, 23.0f, 34.0f},
                        "FT-TRS-001 MakeTransform applies Scale then Rotation then Translation");

    CoreMath::Vector3 decomposedPosition;
    CoreMath::Quaternion decomposedRotation;
    CoreMath::Vector3 decomposedScale;
    const bool decomposed = transform.Decompose(decomposedPosition, decomposedRotation, decomposedScale);
    const CoreMath::Matrix4X4 recomposed =
        CoreMath::Matrix4X4::MakeTransform(decomposedPosition, decomposedRotation, decomposedScale);

    harness.Check(decomposed, "FT-TRS-001 regular TRS decomposition succeeds");
    harness.Check(IsNear(transform, recomposed), "FT-TRS-001 regular TRS decomposition round-trip");
}

void RunQuaternionFixture(TestHarness &harness)
{
    // 단위축과 90도를 사용해 Euler 각 이름이 실제로 어느 축을 회전시키는지 기하적으로 검증한다.
    // expected 축 방향은 MakeFromEuler 또는 RotateVector를 재사용해 계산하지 않았다.
    const CoreMath::Quaternion pitch90 = CoreMath::Quaternion::MakeFromEuler({90.0f, 0.0f, 0.0f});
    const CoreMath::Quaternion yaw90 = CoreMath::Quaternion::MakeFromEuler({0.0f, 90.0f, 0.0f});
    const CoreMath::Quaternion roll90 = CoreMath::Quaternion::MakeFromEuler({0.0f, 0.0f, 90.0f});

    harness.CheckVector(pitch90.RotateVector({0.0f, 1.0f, 0.0f}), {0.0f, 0.0f, 1.0f},
                        "FT-QUAT-001 positive pitch rotates +Y to +Z");
    harness.CheckVector(yaw90.RotateVector({0.0f, 0.0f, 1.0f}), {1.0f, 0.0f, 0.0f},
                        "FT-QUAT-001 positive yaw rotates +Z to +X");
    harness.CheckVector(roll90.RotateVector({1.0f, 0.0f, 0.0f}), {0.0f, 1.0f, 0.0f},
                        "FT-QUAT-001 positive roll rotates +X to +Y");

    const CoreMath::Quaternion yaw180 = CoreMath::Quaternion::MakeFromEuler({0.0f, 180.0f, 0.0f});
    harness.CheckVector(yaw180.RotateVector({0.0f, 0.0f, 1.0f}), {0.0f, 0.0f, -1.0f},
                        "FT-QUAT-001 positive 180-degree yaw rotates +Z to -Z");

    const CoreMath::Quaternion negativeYaw90{-yaw90.X, -yaw90.Y, -yaw90.Z, -yaw90.W};
    harness.CheckVector(negativeYaw90.RotateVector({0.0f, 0.0f, 1.0f}), {1.0f, 0.0f, 0.0f},
                        "FT-QUAT-001 q and -q represent the same rotation");

    // q=(1,2,3,4)의 |q|^2는 30이므로 역은 conjugate(q)/30이다.
    // production GetInversed를 expected 계산에 사용하지 않는다.
    const CoreMath::Quaternion arbitrary{1.0f, 2.0f, 3.0f, 4.0f};
    const CoreMath::Quaternion inverse = arbitrary.GetInversed();
    harness.CheckQuaternion(inverse, {-1.0f / 30.0f, -2.0f / 30.0f, -3.0f / 30.0f, 4.0f / 30.0f},
                            "FT-QUAT-001 inverse uses conjugate divided by squared length");
    harness.CheckQuaternion(arbitrary * inverse, {0.0f, 0.0f, 0.0f, 1.0f},
                            "FT-QUAT-001 q multiplied by inverse is identity");

    const CoreMath::Vector3 original{0.25f, -2.0f, 3.0f};
    harness.CheckVector(pitch90.InverseRotateVector(pitch90.RotateVector(original)), original,
                        "FT-QUAT-001 unit quaternion inverse rotation round-trip");

    // 길이가 2인 비단위 quaternion의 conjugate-only 역회전은 길이 제곱만큼 왜곡된다.
    // 이는 실패를 숨기는 테스트가 아니라 InverseRotateVector의 단위 quaternion 전제조건을 검출하는 fixture다.
    const float sqrtTwo = std::sqrt(2.0f);
    const CoreMath::Quaternion nonUnitRoll90{0.0f, 0.0f, sqrtTwo, sqrtTwo};
    harness.CheckVector(nonUnitRoll90.InverseRotateVector({1.0f, 0.0f, 0.0f}), {-3.0f, -4.0f, 0.0f},
                        "FT-QUAT-001 non-unit InverseRotateVector exposes conjugate-only precondition");

    CoreMath::Quaternion zeroQuaternion{0.0f, 0.0f, 0.0f, 0.0f};
    zeroQuaternion.Normalize();
    harness.CheckQuaternion(zeroQuaternion, {0.0f, 0.0f, 0.0f, 0.0f},
                            "FT-QUAT-001 zero quaternion normalization remains invalid");
}

void RunEulerConventionRegression(TestHarness &harness)
{
    // FC-QUAT-001: 동일한 yaw=90도는 Quaternion과 Matrix에서 모두 Y축 회전이어야 한다.
    // +Z가 +X로 이동한다는 expected는 두 production API를 재사용하지 않고 LH 단위축 기하로 정했다.
    const CoreMath::Quaternion quaternionYaw90 = CoreMath::Quaternion::MakeFromEuler({0.0f, 90.0f, 0.0f});
    const CoreMath::Matrix4X4 matrixYaw90 =
        CoreMath::Matrix4X4::MakeRotationYawPitchRollRad(kPi * 0.5f, 0.0f, 0.0f);

    const CoreMath::Vector3 quaternionForward = quaternionYaw90.RotateVector({0.0f, 0.0f, 1.0f});
    const CoreMath::Vector3 matrixForward = matrixYaw90.TransformDirection({0.0f, 0.0f, 1.0f});

    harness.CheckVector(quaternionForward, {1.0f, 0.0f, 0.0f},
                        "RT-EULER-001 Quaternion yaw rotates +Z to +X");
    harness.CheckVector(matrixForward, {1.0f, 0.0f, 0.0f},
                        "RT-EULER-001 Matrix yaw rotates +Z to +X");
    harness.Check(IsNear(quaternionForward, matrixForward),
                  "RT-EULER-001 Quaternion and Matrix yaw conventions match");

    // 합성 expected는 production 회전 생성 API를 쓰지 않고 Z(roll) -> X(pitch) -> Y(yaw) 공식을 직접 적용한다.
    const float pitch = 30.0f * kPi / 180.0f;
    const float yaw = 45.0f * kPi / 180.0f;
    const float roll = 60.0f * kPi / 180.0f;
    const CoreMath::Vector3 input{1.0f, 2.0f, 3.0f};
    const CoreMath::Vector3 afterRoll{std::cos(roll) * input.X - std::sin(roll) * input.Y,
                                      std::sin(roll) * input.X + std::cos(roll) * input.Y, input.Z};
    const CoreMath::Vector3 afterPitch{afterRoll.X,
                                       std::cos(pitch) * afterRoll.Y - std::sin(pitch) * afterRoll.Z,
                                       std::sin(pitch) * afterRoll.Y + std::cos(pitch) * afterRoll.Z};
    const CoreMath::Vector3 expected{std::cos(yaw) * afterPitch.X + std::sin(yaw) * afterPitch.Z,
                                     afterPitch.Y,
                                     -std::sin(yaw) * afterPitch.X + std::cos(yaw) * afterPitch.Z};
    const CoreMath::Vector3 quaternionComposite =
        CoreMath::Quaternion::MakeFromEuler({30.0f, 45.0f, 60.0f}).RotateVector(input);
    const CoreMath::Vector3 matrixComposite =
        CoreMath::Matrix4X4::MakeRotationYawPitchRollRad(yaw, pitch, roll).TransformDirection(input);
    harness.CheckVector(quaternionComposite, expected,
                        "RT-EULER-002 Quaternion composite order is Z then X then Y");
    harness.CheckVector(matrixComposite, expected,
                        "RT-EULER-002 Matrix composite order is Z then X then Y");
}

void RunDefaultMatrixAndShearFixture(TestHarness &harness)
{
    // default matrix가 import 미처리 분기에서 안전한 identity인지, 위험한 zero인지 명시적으로 확인한다.
    const CoreMath::Matrix4X4 defaultMatrix;
    harness.Check(IsNear(defaultMatrix, CoreMath::Matrix4X4::Zero),
                  "IC-IMPORT-001 default Matrix4X4 is zero rather than identity");

    // xy shear 0.5는 순수 TRS가 아니므로 TRS 분해 후 재합성해 원본을 보존할 수 없다.
    CoreMath::Matrix4X4 shear = CoreMath::Matrix4X4::Identity;
    shear.mat[1].X = 0.5f;

    CoreMath::Vector3 position;
    CoreMath::Quaternion rotation;
    CoreMath::Vector3 scale;
    const bool decomposed = shear.Decompose(position, rotation, scale);
    const CoreMath::Matrix4X4 recomposed = CoreMath::Matrix4X4::MakeTransform(position, rotation, scale);

    harness.Check(decomposed, "FT-SCALE-001 Decompose reports success for a shear matrix");
    harness.Check(!IsNear(shear, recomposed),
                  "FT-SCALE-001 TRS recomposition does not preserve the source shear matrix");
}

void RunTwoBoneFixture(TestHarness &harness)
{
    // root=identity, child bind translation=(1,0,0)인 2-bone 계층이다.
    // bind palette G_bind * inverse(G_bind)는 손으로 identity임을 알 수 있다.
    const CoreMath::Matrix4X4 bindRoot = CoreMath::Matrix4X4::Identity;
    const CoreMath::Matrix4X4 bindChild = CoreMath::Matrix4X4::MakeTranslation(1.0f, 0.0f, 0.0f);
    const CoreMath::Matrix4X4 inverseBindRoot = bindRoot.GetInversed();
    const CoreMath::Matrix4X4 inverseBindChild = bindChild.GetInversed();

    const CoreMath::Matrix4X4 bindPaletteRoot = bindRoot * inverseBindRoot;
    const CoreMath::Matrix4X4 bindPaletteChild = bindChild * inverseBindChild;

    harness.Check(IsNear(bindPaletteRoot, CoreMath::Matrix4X4::Identity),
                  "FT-HIERARCHY-001 root bind palette is identity");
    harness.Check(IsNear(bindPaletteChild, CoreMath::Matrix4X4::Identity),
                  "FT-HIERARCHY-001 child bind palette is identity");
    harness.CheckVector(bindPaletteChild.TransformPoint({2.0f, 0.0f, 0.0f}), {2.0f, 0.0f, 0.0f},
                        "FT-HIERARCHY-001 child-bound point remains fixed in bind pose");

    // root만 +Y로 2 이동하면 두 palette 결과 모두 같은 +Y 이동이어야 하며,
    // 50/50 가중합도 동일한 점을 유지해야 한다.
    const CoreMath::Matrix4X4 currentRoot = CoreMath::Matrix4X4::MakeTranslation(0.0f, 2.0f, 0.0f);
    const CoreMath::Matrix4X4 currentChild = currentRoot * bindChild;
    const CoreMath::Matrix4X4 movedPaletteRoot = currentRoot * inverseBindRoot;
    const CoreMath::Matrix4X4 movedPaletteChild = currentChild * inverseBindChild;

    harness.CheckVector(movedPaletteRoot.TransformPoint({2.0f, 0.0f, 0.0f}), {2.0f, 2.0f, 0.0f},
                        "FT-HIERARCHY-002 parent translation moves a root-bound point once");
    harness.CheckVector(movedPaletteChild.TransformPoint({2.0f, 0.0f, 0.0f}), {2.0f, 2.0f, 0.0f},
                        "FT-HIERARCHY-002 parent translation moves a child-bound point once");

    const CoreMath::Vector3 rootResult = movedPaletteRoot.TransformPoint({2.0f, 0.0f, 0.0f});
    const CoreMath::Vector3 childResult = movedPaletteChild.TransformPoint({2.0f, 0.0f, 0.0f});
    harness.CheckVector(rootResult * 0.5f + childResult * 0.5f, {2.0f, 2.0f, 0.0f},
                        "FT-HIERARCHY-002 50/50 linear blend preserves shared parent translation");

    const CoreMath::Matrix4X4 componentWorld = CoreMath::Matrix4X4::MakeTranslation(0.0f, 0.0f, 5.0f);
    harness.CheckVector(componentWorld.TransformPoint(childResult), {2.0f, 2.0f, 5.0f},
                        "FT-HIERARCHY-002 component world transform is applied once after skinning");
}

void RunNonUniformHierarchyFixture(TestHarness &harness)
{
    // parent X scale=2 뒤에 child Z rotation=45도를 합성하면 shear가 생긴다.
    // 정확한 행렬 계층 결과와 SceneComponent 방식의 분리 TRS 결과를 손계산 가능한 sqrt(2) 값으로 비교한다.
    const CoreMath::Vector3 parentScale{2.0f, 1.0f, 1.0f};
    const CoreMath::Quaternion parentRotation{0.0f, 0.0f, 0.0f, 1.0f};
    const CoreMath::Quaternion childRotation = CoreMath::Quaternion::MakeFromEuler({0.0f, 0.0f, 45.0f});

    const CoreMath::Matrix4X4 parentMatrix =
        CoreMath::Matrix4X4::MakeTransform({0.0f, 0.0f, 0.0f}, parentRotation, parentScale);
    const CoreMath::Matrix4X4 childMatrix =
        CoreMath::Matrix4X4::MakeTransform({0.0f, 0.0f, 0.0f}, childRotation, {1.0f, 1.0f, 1.0f});
    const CoreMath::Matrix4X4 exactHierarchy = parentMatrix * childMatrix;

    const CoreMath::Vector3 sceneStyleScale = parentScale * CoreMath::Vector3{1.0f, 1.0f, 1.0f};
    const CoreMath::Quaternion sceneStyleRotation = parentRotation * childRotation;
    const CoreMath::Matrix4X4 sceneStyleHierarchy =
        CoreMath::Matrix4X4::MakeTransform({0.0f, 0.0f, 0.0f}, sceneStyleRotation, sceneStyleScale);

    const CoreMath::Vector3 exactPoint = exactHierarchy.TransformPoint({1.0f, 0.0f, 0.0f});
    const CoreMath::Vector3 sceneStylePoint = sceneStyleHierarchy.TransformPoint({1.0f, 0.0f, 0.0f});

    harness.CheckVector(exactPoint, {std::sqrt(2.0f), std::sqrt(0.5f), 0.0f},
                        "IC-HIERARCHY-001 exact parent-matrix multiplication expected point");
    harness.CheckVector(sceneStylePoint, {std::sqrt(2.0f), std::sqrt(2.0f), 0.0f},
                        "IC-HIERARCHY-001 separated world TRS expected point");
    harness.Check(!IsNear(exactPoint, sceneStylePoint),
                  "IC-HIERARCHY-001 non-uniform parent scale and child rotation diverge");
}

void RunFbxAxisPolicyRegression(TestHarness &harness)
{
    // RH Y-Up의 좌표 (1,2,3)은 handedness 변환으로 Z만 반전되어 (1,2,-3)이 된다.
    // RH Z-Up의 +Y는 engine +Z로 옮겨진다. expected는 정책 행렬을 재사용하지 않고 축 대응을 손으로 정했다.
    const Import::FBXAxisConversion yUp =
        Import::ResolveFBXAxisConversion(Import::EFBXHandedness::eRightHanded, Import::EFBXUpAxis::eY, 1);
    const Import::FBXAxisConversion zUp =
        Import::ResolveFBXAxisConversion(Import::EFBXHandedness::eRightHanded, Import::EFBXUpAxis::eZ, 1);

    harness.Check(yUp.mSupported, "RT-FBX-AXIS-001 RH positive Y-Up is supported");
    harness.CheckVector(yUp.mBakeMatrix.TransformPoint({1.0f, 2.0f, 3.0f}), {1.0f, 2.0f, -3.0f},
                        "RT-FBX-AXIS-001 RH Y-Up conversion is explicit");
    harness.Check(zUp.mSupported, "RT-FBX-AXIS-001 RH positive Z-Up is supported");
    harness.CheckVector(zUp.mBakeMatrix.TransformDirection({0.0f, 1.0f, 0.0f}), {0.0f, 0.0f, 1.0f},
                        "RT-FBX-AXIS-001 RH Z-Up maps source +Y to engine +Z");

    harness.Check(!Import::ResolveFBXAxisConversion(Import::EFBXHandedness::eLeftHanded,
                                                     Import::EFBXUpAxis::eY, 1)
                       .mSupported,
                  "RT-FBX-AXIS-002 left-handed input is rejected");
    harness.Check(!Import::ResolveFBXAxisConversion(Import::EFBXHandedness::eRightHanded,
                                                     Import::EFBXUpAxis::eX, 1)
                       .mSupported,
                  "RT-FBX-AXIS-002 X-Up input is rejected");
    harness.Check(!Import::ResolveFBXAxisConversion(Import::EFBXHandedness::eRightHanded,
                                                     Import::EFBXUpAxis::eY, -1)
                       .mSupported,
                  "RT-FBX-AXIS-002 negative up direction is rejected");
}

void RunJointTransformPolicyRegression(TestHarness &harness)
{
    // V1은 (2,2,2) 같은 양의 uniform scale만 허용한다. 각 거부 fixture는 서로 다른 계약 위반을 한 가지만 가진다.
    CoreAsset::AnimationLocalTransform transform;
    transform.mScale = {2.0f, 2.0f, 2.0f};
    harness.Check(CoreAsset::ValidateAnimationJointTransform(transform).mValid,
                  "RT-TRANSFORM-001 positive uniform scale is accepted");

    transform.mScale = {2.0f, 1.0f, 1.0f};
    harness.Check(CoreAsset::ValidateAnimationJointTransform(transform).mError ==
                      CoreAsset::EAnimationTransformError::eNonUniformScale,
                  "RT-TRANSFORM-002 non-uniform scale is rejected");

    transform.mScale = {-1.0f, -1.0f, -1.0f};
    harness.Check(CoreAsset::ValidateAnimationJointTransform(transform).mError ==
                      CoreAsset::EAnimationTransformError::eNonPositiveScale,
                  "RT-TRANSFORM-003 negative scale is rejected");

    transform.mScale = {0.0f, 0.0f, 0.0f};
    harness.Check(CoreAsset::ValidateAnimationJointTransform(transform).mError ==
                      CoreAsset::EAnimationTransformError::eNonPositiveScale,
                  "RT-TRANSFORM-003 zero scale is rejected");

    transform.mScale = {1.0f, 1.0f, 1.0f};
    transform.mRotation = {0.0f, 0.0f, 0.0f, 2.0f};
    harness.Check(CoreAsset::ValidateAnimationJointTransform(transform).mError ==
                      CoreAsset::EAnimationTransformError::eInvalidRotation,
                  "RT-TRANSFORM-003 non-unit rotation is rejected");
    transform.mRotation = {0.0f, 0.0f, 0.0f, 1.0f};
    CoreMath::Matrix4X4 shear = CoreMath::Matrix4X4::Identity;
    shear.mat[1].X = 0.5f;
    harness.Check(CoreAsset::ValidateAnimationJointTransform(transform, &shear).mError ==
                      CoreAsset::EAnimationTransformError::eShear,
                  "RT-TRANSFORM-004 shear-requiring input is rejected");
}

void BuildTwoJointSkeleton(CoreAsset::Skeleton &skeleton, std::string *failureReason = nullptr)
{
    std::vector<CoreAsset::SkeletonJoint> joints(2);
    joints[0].mDisplayName = "joint";
    joints[0].mStableKey = "/root";
    joints[0].mParentIndex = CoreAsset::SkeletonJoint::NoParent;
    joints[1].mDisplayName = "joint"; // 표시 이름 중복은 허용하지만 stable key는 달라야 한다.
    joints[1].mStableKey = "/root/child";
    joints[1].mParentIndex = 0;
    joints[1].mReferenceLocalPose.mPosition = {100.0f, 0.0f, 0.0f};
    skeleton.SetJoints(std::move(joints), failureReason);
}

void RunAssetOwnershipRegression(TestHarness &harness)
{
    std::string reason;
    CoreAsset::Skeleton skeleton(101);
    BuildTwoJointSkeleton(skeleton, &reason);
    harness.Check(reason.empty() && skeleton.GetJoints().size() == 2,
                  "RT-ASSET-001 parent-first skeleton with stable keys is accepted");
    harness.Check(skeleton.GetStructureSignature() != 0,
                  "RT-ASSET-001 skeleton structure signature is generated");

    CoreAsset::Skeleton sameSkeleton(101);
    BuildTwoJointSkeleton(sameSkeleton);
    harness.Check(skeleton.GetStructureSignature() == sameSkeleton.GetStructureSignature(),
                  "RT-ASSET-001 skeleton signature is deterministic");

    BinaryArch skeletonWriter(false);
    skeletonWriter.Start();
    skeleton.Serialize(skeletonWriter);
    const std::vector<uint8_t> skeletonBytes(
        skeletonWriter.GetBufferFromMemory(),
        skeletonWriter.GetBufferFromMemory() + skeletonWriter.GetBufferSize());
    skeletonWriter.End();
    CoreAsset::Skeleton loadedSkeleton;
    BinaryArch skeletonReader(true);
    skeletonReader.StartRead(skeletonBytes.data(), skeletonBytes.size());
    loadedSkeleton.Serialize(skeletonReader);
    skeletonReader.End();
    harness.Check(loadedSkeleton.GetID() == skeleton.GetID() &&
                      loadedSkeleton.GetStructureSignature() == skeleton.GetStructureSignature() &&
                      loadedSkeleton.GetJoints().size() == 2 &&
                      loadedSkeleton.GetJoints()[1].mStableKey == "/root/child",
                  "RT-ASSET-001 skeleton serialization round-trip");
    harness.Check(loadedSkeleton.Validate(&reason),
                  "RT-ASSET-001 deserialized Skeleton passes schema validation");

    std::vector<CoreAsset::SkeletonJoint> invalidJoints(2);
    invalidJoints[0].mStableKey = "/root";
    invalidJoints[0].mParentIndex = CoreAsset::SkeletonJoint::NoParent;
    invalidJoints[1].mStableKey = "/root";
    invalidJoints[1].mParentIndex = 0;
    CoreAsset::Skeleton invalidSkeleton;
    harness.Check(!invalidSkeleton.SetJoints(std::move(invalidJoints), &reason),
                  "RT-ASSET-002 duplicate stable key is rejected");
    invalidJoints.resize(2);
    invalidJoints[0].mStableKey = "/root";
    invalidJoints[0].mParentIndex = CoreAsset::SkeletonJoint::NoParent;
    invalidJoints[1].mStableKey = "/root/child";
    invalidJoints[1].mParentIndex = 1;
    harness.Check(!invalidSkeleton.SetJoints(std::move(invalidJoints), &reason),
                  "RT-ASSET-002 non-parent-first hierarchy is rejected");

    // 입력 5개 중 큰 4개(0.4,0.3,0.2,0.1)를 선택한다. 합이 이미 1이므로 expected도 그대로다.
    CoreAsset::SkinningVertex vertex{};
    const std::vector<CoreAsset::SkinInfluence> influences = {
        {7, 0.1f}, {3, 0.4f}, {5, 0.2f}, {1, 0.3f}, {9, 0.05f}, {11, 0.0f}};
    harness.Check(CoreAsset::NormalizeSkinInfluences(influences, vertex, &reason),
                  "RT-ASSET-003 valid influences normalize successfully");
    harness.Check(vertex.mJointIndices[0] == 3 && vertex.mJointIndices[1] == 1 &&
                      vertex.mJointIndices[2] == 5 && vertex.mJointIndices[3] == 7,
                  "RT-ASSET-003 deterministic top-four order");
    harness.CheckFloat(vertex.mJointWeights[0] + vertex.mJointWeights[1] + vertex.mJointWeights[2] +
                           vertex.mJointWeights[3],
                       1.0f, "RT-ASSET-003 normalized weight sum is one");
    harness.Check(!CoreAsset::NormalizeSkinInfluences({{0, 0.0f}, {1, -1.0f}}, vertex, &reason),
                  "RT-ASSET-004 vertex without positive weight is rejected");

    CoreAsset::SkinBinding binding;
    binding.mSkeletonAssetID = skeleton.GetID();
    binding.mSkeletonSignature = skeleton.GetStructureSignature();
    binding.mPaletteToSkeletonJoint = {0, 1};
    binding.mInverseBindMatrices = {CoreMath::Matrix4X4::Identity,
                                    CoreMath::Matrix4X4::MakeTranslation(-100.0f, 0.0f, 0.0f)};
    harness.Check(binding.Validate(skeleton, &reason),
                  "RT-ASSET-005 skin binding owns remap and inverse bind data");
    CoreAsset::Skeleton mismatchedSkeleton(102);
    BuildTwoJointSkeleton(mismatchedSkeleton);
    harness.Check(!binding.Validate(mismatchedSkeleton, &reason),
                  "RT-ASSET-005 mismatched Skeleton binding is rejected");

    BinaryArch writer(false);
    writer.Start();
    writer << binding;
    const std::vector<uint8_t> bytes(writer.GetBufferFromMemory(),
                                     writer.GetBufferFromMemory() + writer.GetBufferSize());
    writer.End();

    CoreAsset::SkinBinding loadedBinding;
    BinaryArch reader(true);
    reader.StartRead(bytes.data(), bytes.size());
    reader << loadedBinding;
    reader.End();
    harness.Check(loadedBinding.mSkeletonSignature == binding.mSkeletonSignature &&
                      loadedBinding.mPaletteToSkeletonJoint == binding.mPaletteToSkeletonJoint &&
                      loadedBinding.mInverseBindMatrices.size() == 2,
                  "RT-ASSET-006 skin binding serialization round-trip");

    CoreAsset::AssetFactoryManager *factoryManager = CoreAsset::AssetFactoryManager::GetInstance();
    factoryManager->RegisterAssetFactory(CoreAsset::EAssetType::eSkeleton,
                                         CoreAsset::AnimationAssetFactory::GetInstance());

    CoreAsset::Asset *loadedAsset = nullptr;
    std::unique_ptr<CoreAsset::AssetMetaData> loadedMetaData;
    CoreAsset::AnimationAssetLoader *loader = CoreAsset::AnimationAssetLoader::GetInstance();
    BinaryArch loaderReader(true);
    loaderReader.StartRead(skeletonBytes.data(), skeletonBytes.size());
    const bool validLoad = loader->LoadAssetFile(CoreAsset::EAssetType::eSkeleton, loaderReader, factoryManager,
                                                 loadedAsset, loadedMetaData);
    loaderReader.End();
    harness.Check(validLoad && loadedAsset != nullptr && loadedMetaData != nullptr,
                  "RT-ASSET-007 loader accepts a valid serialized Skeleton");
    delete loadedAsset;

    // joint가 없는 Skeleton payload는 byte stream 자체가 읽혀도 schema validation에서 거부해야 한다.
    CoreAsset::Skeleton emptySkeleton(303);
    BinaryArch invalidWriter(false);
    invalidWriter.Start();
    emptySkeleton.Serialize(invalidWriter);
    const std::vector<uint8_t> invalidSkeletonBytes(
        invalidWriter.GetBufferFromMemory(),
        invalidWriter.GetBufferFromMemory() + invalidWriter.GetBufferSize());
    invalidWriter.End();

    loadedAsset = nullptr;
    loadedMetaData.reset();
    BinaryArch invalidReader(true);
    invalidReader.StartRead(invalidSkeletonBytes.data(), invalidSkeletonBytes.size());
    const bool invalidLoad = loader->LoadAssetFile(CoreAsset::EAssetType::eSkeleton, invalidReader, factoryManager,
                                                   loadedAsset, loadedMetaData);
    invalidReader.End();
    harness.Check(!invalidLoad && loadedAsset == nullptr && loadedMetaData == nullptr,
                  "RT-ASSET-007 loader rejects an invalid serialized Skeleton schema");
}

void RunAnimationClipRegression(TestHarness &harness)
{
    CoreAsset::Skeleton skeleton(101);
    BuildTwoJointSkeleton(skeleton);
    CoreAsset::AnimationJointTrack rootTrack;
    rootTrack.mJointKey = "/root";
    rootTrack.mPositions = {{0.0f, 0.0f, 0.0f}, {2.0f, 0.0f, 0.0f}, {4.0f, 0.0f, 0.0f}};
    rootTrack.mRotations = {{0.0f, 0.0f, 0.0f, 1.0f},
                            {0.0f, 0.0f, 0.0f, -1.0f},
                            {0.0f, 0.0f, 0.0f, 1.0f}};
    // scale channel은 비워 reference pose fallback을 검증한다.

    CoreAsset::AnimationClip clip(202);
    std::string reason;
    harness.Check(clip.Configure(skeleton.GetID(), skeleton.GetStructureSignature(), 1.0f, 2, 1, 3,
                                 {rootTrack}, &reason),
                  "RT-CLIP-001 seconds-based rational uniform track is accepted");
    harness.Check(clip.IsCompatible(skeleton), "RT-CLIP-001 matching skeleton signature is compatible");
    CoreAsset::Skeleton differentSkeleton(102);
    BuildTwoJointSkeleton(differentSkeleton);
    harness.Check(!clip.IsCompatible(differentSkeleton),
                  "RT-CLIP-001 different Skeleton AssetID is incompatible");

    CoreAsset::AnimationLocalTransform referencePose;
    referencePose.mScale = {2.0f, 2.0f, 2.0f};
    CoreAsset::AnimationLocalTransform sampled;

    // 2 samples/second에서 t=0.25는 sample 0과 1의 정확한 중간이므로 X=1이다.
    harness.Check(clip.SampleJoint("/root", 0.25f, false, referencePose, sampled, &reason),
                  "RT-CLIP-002 non-loop sample succeeds");
    harness.CheckVector(sampled.mPosition, {1.0f, 0.0f, 0.0f},
                        "RT-CLIP-002 uniform translation interpolation");
    harness.CheckQuaternion(sampled.mRotation, {0.0f, 0.0f, 0.0f, 1.0f},
                            "RT-CLIP-003 q/-q interpolation uses shortest path");
    harness.CheckVector(sampled.mScale, {2.0f, 2.0f, 2.0f},
                        "RT-CLIP-004 missing scale channel uses reference pose");

    clip.SampleJoint("/root", 1.0f, true, referencePose, sampled, &reason);
    harness.CheckVector(sampled.mPosition, {0.0f, 0.0f, 0.0f},
                        "RT-CLIP-005 loop exact duration wraps to zero");
    clip.SampleJoint("/root", 1.0f, false, referencePose, sampled, &reason);
    harness.CheckVector(sampled.mPosition, {4.0f, 0.0f, 0.0f},
                        "RT-CLIP-006 non-loop exact duration clamps to last sample");
    clip.SampleJoint("/root", 1.25f, true, referencePose, sampled, &reason);
    harness.CheckVector(sampled.mPosition, {1.0f, 0.0f, 0.0f},
                        "RT-CLIP-006 loop time above duration wraps into half frame");
    clip.SampleJoint("/root", -0.25f, true, referencePose, sampled, &reason);
    harness.CheckVector(sampled.mPosition, {3.0f, 0.0f, 0.0f},
                        "RT-CLIP-006 negative loop time wraps into range");
    clip.SampleJoint("/root/child", 0.5f, false, skeleton.GetJoints()[1].mReferenceLocalPose, sampled, &reason);
    harness.CheckVector(sampled.mPosition, {100.0f, 0.0f, 0.0f},
                        "RT-CLIP-007 missing joint track uses reference pose");

    BinaryArch writer(false);
    writer.Start();
    clip.Serialize(writer);
    const std::vector<uint8_t> bytes(writer.GetBufferFromMemory(),
                                     writer.GetBufferFromMemory() + writer.GetBufferSize());
    writer.End();

    CoreAsset::AnimationClip loadedClip;
    BinaryArch reader(true);
    reader.StartRead(bytes.data(), bytes.size());
    loadedClip.Serialize(reader);
    reader.End();
    harness.Check(loadedClip.GetDurationSeconds() == 1.0f && loadedClip.GetSampleCount() == 3 &&
                      loadedClip.GetTracks().size() == 1 && loadedClip.IsCompatible(skeleton),
                  "RT-CLIP-008 animation clip serialization round-trip");
    harness.Check(loadedClip.Validate(&reason),
                  "RT-CLIP-008 deserialized AnimationClip passes schema validation");

    // 유효한 clip에 잘못된 sample 수로 재설정을 시도해도 기존 clip의 duration/track은 유지되어야 한다.
    harness.Check(!clip.Configure(skeleton.GetID(), skeleton.GetStructureSignature(), 1.0f, 2, 1, 2,
                                  {rootTrack}, &reason) &&
                      clip.GetSampleCount() == 3 && clip.GetTracks().size() == 1,
                  "RT-CLIP-008 rejected reconfiguration preserves the previous valid clip");

    // sampleCount=1, duration=0인 clip은 modulo 없이 정지 pose를 반환한다.
    CoreAsset::AnimationJointTrack staticTrack;
    staticTrack.mJointKey = "/root";
    staticTrack.mPositions = {{7.0f, 0.0f, 0.0f}};
    CoreAsset::AnimationClip staticClip(203);
    harness.Check(staticClip.Configure(skeleton.GetID(), skeleton.GetStructureSignature(), 0.0f, 30, 1, 1,
                                       {staticTrack}, &reason),
                  "RT-CLIP-009 zero-duration constant clip is accepted");
    staticClip.SampleJoint("/root", 100.0f, true, referencePose, sampled, &reason);
    harness.CheckVector(sampled.mPosition, {7.0f, 0.0f, 0.0f},
                        "RT-CLIP-009 zero-duration clip remains a static pose");
}
} // namespace

int main()
{
    std::cout << std::fixed << std::setprecision(6);

    TestHarness harness;
    RunMatrixPointDirectionFixture(harness);
    RunTrsFixture(harness);
    RunQuaternionFixture(harness);
    RunEulerConventionRegression(harness);
    RunDefaultMatrixAndShearFixture(harness);
    RunTwoBoneFixture(harness);
    RunNonUniformHierarchyFixture(harness);
    RunFbxAxisPolicyRegression(harness);
    RunJointTransformPolicyRegression(harness);
    RunAssetOwnershipRegression(harness);
    RunAnimationClipRegression(harness);

    return harness.Finish();
}
