#include "Transform.h"
#include <Core/SceneComponent.h>
Core::Transform::Transform()
    : mScaleLocal(1, 1, 1), mQuaternionLocal(0, 0, 0, 1), mPositionLocal(0, 0, 0), mScaleWorld(1, 1, 1),
      mQuaternionWorld(0, 0, 0, 1), mPositionWorld(0, 0, 0), mTransformLocal(CoreMath::Matrix4X4::Identity),
      mTransformWorld(CoreMath::Matrix4X4::Identity), mDirtyLocal(false)
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

const CoreMath::Vector3 &Core::Transform::GetPositionLocal() const
{
    return mPositionLocal;
}

// --- 로컬 프로퍼티 Setters ---

void Core::Transform::SetScaleLocal(const CoreMath::Vector3 &scale)
{
    mScaleLocal = scale;
    // 로컬 데이터가 변경되었으므로 Dirty Flag를 설정하여,
    // 다음 번에 로컬 행렬이 필요할 때 다시 계산하도록 합니다.
    mDirtyLocal = true;
}

void Core::Transform::SetQuaternionLocal(const CoreMath::Quaternion &quaternion)
{
    mQuaternionLocal = quaternion;
    // (선택 사항) 여기서 quaternion.Normalize()를 호출하여 항상 정규화된 상태를 유지할 수도 있습니다.
    mDirtyLocal = true;
}

void Core::Transform::SetPositionLocal(const CoreMath::Vector3 &position)
{
    mPositionLocal = position;
    mDirtyLocal = true;
}

// --- 월드 프로퍼티 Getters (캐시된 데이터 접근) ---

const CoreMath::Vector3 &Core::Transform::GetScaleWorld() const
{
    // 이 함수는 SceneComponent가 계산하여 캐싱해 둔 값을 그냥 반환만 합니다.
    return mScaleWorld;
}

const CoreMath::Quaternion &Core::Transform::GetQuaternionWorld() const
{
    return mQuaternionWorld;
}

const CoreMath::Vector3 &Core::Transform::GetPositionWorld() const
{
    return mPositionWorld;
}

// --- 행렬 Getters (캐시된 데이터 접근) ---

const CoreMath::Matrix4X4 &Core::Transform::GetTransformLocal() const
{
    // 중요: 실제 엔진에서는 GetTransformLocal()이 호출될 때
    // mDirtyLocal을 확인하고, true이면 로컬 행렬을 재계산하는 로직이 여기에 들어갑니다.
    // (이전 SceneComponent에 있던 UpdateTransformLocal() 로직)
    return mTransformLocal;
}

const CoreMath::Matrix4X4 &Core::Transform::GetTransformWorld() const
{
    // 이 함수는 SceneComponent가 계산하여 캐싱해 둔 월드 행렬을 반환합니다.
    return mTransformWorld;
}

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

    mQuaternionWorld = mQuaternionWorld = quaternion;
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
