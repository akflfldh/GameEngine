#include "Transform.h"

// #include <Core/SceneComponent.h> // 이 파일에서는 SceneComponent가 직접 필요하지 않으므로 제거 가능
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::scale 등 사용
#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr 사용 (필요 시)

Core::Transform::Transform()
    : mScaleLocal(1.0f, 1.0f, 1.0f),
      // quat(x, y,z, w) - 단위 쿼터니언은 w가 1
      mQuaternionLocal(0.0f, 0.0f, 0.0f, 1.0f), mPositionLocal(0.0f, 0.0f, 0.0f), mScaleWorld(1.0f, 1.0f, 1.0f),
      mQuaternionWorld(0.0f, 0.0f, 0.0f, 1.0f), mPositionWorld(0.0f, 0.0f, 0.0f),
      // mat4(1.0f)는 단위 행렬을 생성
      mTransformLocal(CoreMath::Matrix4X4::Identity), mTransformWorld(CoreMath::Matrix4X4::Identity),
      mDirtyLocal(true) // 최초에는 행렬 계산을 위해 true로 시작하는 것이 좋습니다.
{
}

Core::Transform::~Transform() {}

const CoreMath::Vector3 &Core::Transform::GetScaleLocal() const
{
    return mScaleLocal;
}

const CoreMath::Quaternion &Core::Transform::GetQuaternionLocal() const
{
    return mQuaternionLocal;
}

const CoreMath::Vector3 Core::Transform::GetRotationLocal() const
{
    // 쿼터니언을 오일러 각(degree)으로 변환해서 반환합니다.

    return mQuaternionLocal.ToEulerAngles();
}

const CoreMath::Vector3 &Core::Transform::GetPositionLocal() const
{
    return mPositionLocal;
}

// --- 로컬 프로퍼티 Setters ---

void Core::Transform::SetScaleLocal(const CoreMath::Vector3 &scale)
{
    mScaleLocal = scale;
    MarkDirty();
}

void Core::Transform::SetQuaternionLocal(const CoreMath::Quaternion &quaternion)
{
    // 항상 정규화된 쿼터니언을 보장하기 위해 normalize를 호출하는 것이 안전합니다.

    mQuaternionLocal = quaternion;
    mQuaternionLocal.Normalize();
    MarkDirty();
}

void Core::Transform::SetRotationLocal(const CoreMath::Vector3 &rotation)
{
    // 오일러 각(degree)을 라디안으로 변환한 뒤 쿼터니언으로 생성합니다.
    //  mQuaternionLocal = glm::quat(glm::radians(rotation));
    mQuaternionLocal = CoreMath::Quaternion::MakeFromEuler(rotation);
    MarkDirty();
}

void Core::Transform::SetPositionLocal(const CoreMath::Vector3 &position)
{
    mPositionLocal = position;
    MarkDirty();
}

void Core::Transform::AddQuaternionLocal(const CoreMath::Quaternion &quaternion)
{
    SetQuaternionLocal(quaternion * mQuaternionLocal);
}

void Core::Transform::AddRotationLocal(const CoreMath::Vector3 &rotation)
{
    // AddQuaternionLocal(glm::quat(glm::radians(rotation)));

    AddQuaternionLocal(CoreMath::Quaternion::MakeFromEuler(rotation));
}

// --- 월드 프로퍼티 Getters (캐시된 데이터 접근) ---

const CoreMath::Vector3 &Core::Transform::GetScaleWorld() const
{
    return mScaleWorld;
}

const CoreMath::Quaternion &Core::Transform::GetQuaternionWorld() const
{
    return mQuaternionWorld;
}

const CoreMath::Vector3 Core::Transform::GetRotationWorld() const
{
    // 캐시된 월드 쿼터니언을 오일러 각으로 변환
    //  return glm::degrees(glm::eulerAngles(mQuaternionWorld));

    return mQuaternionWorld.ToEulerAngles();
}

const CoreMath::Vector3 &Core::Transform::GetPositionWorld() const
{
    return mPositionWorld;
}

// --- 행렬 Getters (캐시된 데이터 접근) ---

const CoreMath::Matrix4X4 &Core::Transform::GetTransformLocal() const
{
    // Dirty Flag가 true일 경우에만 로컬 행렬을 재계산합니다.
    if (mDirtyLocal)
    {
        // glm::mat4 transMat = glm::translate(glm::mat4(1.0f), mPositionLocal);
        // glm::mat4 rotMat = glm::mat4_cast(mQuaternionLocal);
        // glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), mScaleLocal);

        mTransformLocal = CoreMath::Matrix4X4::MakeTransform(mPositionLocal, mQuaternionLocal, mScaleLocal);

        // SRT 순서로 행렬을 결합합니다. (Scale -> Rotate -> Translate)
        // mTransformLocal = transMat * rotMat * scaleMat;

        // 재계산이 끝났으므로 Dirty Flag를 false로 설정합니다.
        ClearDirtyFlag();
    }
    return mTransformLocal;
}

const CoreMath::Matrix4X4 &Core::Transform::GetTransformWorld() const
{
    return mTransformWorld;
}

// --- 캐시 데이터 Setters ---

void Core::Transform::SetTransformLocal(const CoreMath::Matrix4X4 &matrix) const
{
    mTransformLocal = matrix;
}

void Core::Transform::SetTransformWorld(const CoreMath::Matrix4X4 &matrix) const
{
    mTransformWorld = matrix;
}

void Core::Transform::SetScaleWorld(const CoreMath::Vector3 &scale) const
{
    mScaleWorld = scale;
}

void Core::Transform::SetQuaternionWorld(const CoreMath::Quaternion &quaternion) const
{
    mQuaternionWorld = quaternion;
}

void Core::Transform::SetRotationWorld(const CoreMath::Vector3 &rotation) const
{
    // 월드 프로퍼티는 캐시용이므로, Dirty Flag를 설정하지 않습니다.
    // mQuaternionWorld = glm::quat(glm::radians(rotation));

    mQuaternionWorld = CoreMath::Quaternion::MakeFromEuler(rotation);
}

void Core::Transform::SetPositionWorld(const CoreMath::Vector3 &position) const
{
    mPositionWorld = position;
}

bool Core::Transform::IsDirty() const
{
    return mDirtyLocal;
}

void Core::Transform::ClearDirtyFlag() const
{
    mDirtyLocal = false;
}

void Core::Transform::Serialize(Arch &arch)
{
    // TODO: 리플렉션 시스템과 연동하여 멤버 변수들을 직렬화/역직렬화하는 코드를 구현합니다.
}

void Core::Transform::MarkDirty()
{

    mDirtyLocal = true;
}

CoreMath::Vector3 Core::Transform::GetForwardWorld() const
{

    return mQuaternionWorld.RotateVector(CoreMath::Vector3({0, 0, 1}));

    /*  const CoreMath::Matrix4X4 &mat = GetTransformWorld();

      return (mat.GetColumn(2).XYZ()).GetNormalize();*/
}

CoreMath::Vector3 Core::Transform::GetUpWorld() const
{

    return mQuaternionWorld.RotateVector(CoreMath::Vector3({0, 1, 0}));

    // const CoreMath::Matrix4X4 &mat = GetTransformWorld();

    // return (mat.GetColumn(1).XYZ()).GetNormalize();
}

CoreMath::Vector3 Core::Transform::GetRightWorld() const
{

    return mQuaternionWorld.RotateVector(CoreMath::Vector3({1, 0, 0}));
    /*   const CoreMath::Matrix4X4 &mat = GetTransformWorld();

       return (mat.GetColumn(0).XYZ()).GetNormalize();*/
}
