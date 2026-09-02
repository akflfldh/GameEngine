#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_ROW_MAJOR
#include "CameraComponent.h"
#include <Core/Map.h>
#include <CoreMath/CoreMath.h>
#include <Object.h>

CameraComponent::CameraComponent()
    : mFovY(90), /* mAspect(1.0f),*/ mNearZ(1.0f), mFarZ(10000), mViewMatrix(CoreMath::Matrix4X4::Identity),
      mType(ECameraType::ePerspective)
{
}

CameraComponent::~CameraComponent()
{

    Object *object = GetOwnerObject();

    if (object == nullptr)
        return;

    Map *map = object->GetMap();

    if (map == nullptr)
        return;

    if (!object->HasObjectFlag(Core::EObjectFlag::eEngineEntity))
        map->UnRegisterCameraComponent(this);
}

void CameraComponent::OnBegin()
{
    SceneComponent::OnBegin();
}

const CoreMath::Matrix4X4 &CameraComponent::GetViewMatrix() const
{
    UpdateIfViewDirtyFlag();
    return mViewMatrix;
}
//
// const glm::mat4x4 &Core::CameraComponent::GetProjMatrix() const
//{
//    UpdateIfProjDirtyFlag();
//    return mProjMatrix;
//}
//
// glm::mat4x4 Core::CameraComponent::GetViewProjMatrix() const
//{
//    glm::mat4x4 proj = GetProjMatrix();
//
//    glm ::mat4x4 view = GetViewMatrix();
//
//    return proj * view;
//
//    // TODO: 여기에 return 문을 삽입합니다.
//}
//
// void Core::CameraComponent::OnWindowResize(float aspect)
//{
//    mAspect = aspect;
//    MarkProjMatrixDirtyFlag();
//}

// bool Core::CameraComponent::GetProjMatrixDirtyFlag() const
//{
//     return bProjMatrixDirtyFlag;
// }
//
// void Core::CameraComponent::MarkProjMatrixDirtyFlag()
//{
//
//     bProjMatrixDirtyFlag = true;
// }
//
// void Core::CameraComponent::ClearProjMatrixDirtyFlag() const
//{
//     bProjMatrixDirtyFlag = false;
//}

float CameraComponent::GetFov() const
{
    return mFovY;
}

float CameraComponent::GetNear() const
{
    return mNearZ;
}

float CameraComponent::GetFar() const
{
    return mFarZ;
}
float CameraComponent::GetHeight() const
{
    return mHeight;
}

void CameraComponent::SetNear(float z)
{

    mNearZ = z;
}

void CameraComponent::SetFar(float z)
{
    mFarZ = z;
}

ECameraType CameraComponent::GetType() const
{

    return mType;
}
void CameraComponent::SetType(ECameraType type)
{
    mType = type;
}

void CameraComponent::Serialize(Arch &arch)
{

    SceneComponent::Serialize(arch);

    arch << mType;
    arch << mFovY;
    arch << mNearZ;
    arch << mFarZ;
}

void CameraComponent::SyncPrefabComponentFrom(Component *prefabComponent)
{

    CameraComponent *cameraPrefabCom = dynamic_cast<CameraComponent *>(prefabComponent);
    if (cameraPrefabCom == nullptr)
        return;

    SceneComponent::SyncPrefabComponentFrom(prefabComponent);

    SetType(cameraPrefabCom->GetType());
    SetNear(cameraPrefabCom->GetNear());
    SetFar(cameraPrefabCom->GetFar());
}

void CameraComponent::OnOwnerObjectAddedToMap()
{

    SceneComponent::OnOwnerObjectAddedToMap();

    if (!GetOwnerObject()->HasObjectFlag(Core::EObjectFlag::eEngineEntity))
    {
        GetOwnerObject()->GetMap()->RegisterCameraComponoent(this);
    }
}

void CameraComponent::UpdateIfViewDirtyFlag() const
{

    const CoreMath::Matrix4X4 &currTransformWorld = GetTransformWorld();
    if (mLastWorldMatrix != currTransformWorld)
    {
        CoreMath::Vector3 posW = GetPositionWorld();
        CoreMath::Quaternion quatW = GetQuaternionWorld();

        /*     glm::mat4 rotInverse = glm::transpose(glm::mat4_cast(quatW));
             glm::mat4 posInverse = glm::translate(glm::mat4(1.0f), -posW);*/

        mViewMatrix = currTransformWorld.GetInversed();

        mLastWorldMatrix = currTransformWorld;
    }
}
// void Core::CameraComponent::UpdateIfProjDirtyFlag() const
//{
//
//     if (bProjMatrixDirtyFlag)
//     {
//         mProjMatrix = glm::perspectiveFovLH_ZO(glm::radians(mFovY), mAspect, 1.0f, mNearZ, mFarZ);
//         ClearProjMatrixDirtyFlag();
//     }
// }
