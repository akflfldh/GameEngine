#include "SceneComponent.h"

Core::SceneComponent::SceneComponent() {}

Core::SceneComponent::~SceneComponent() {}

void Core::SceneComponent::Start() {}

void Core::SceneComponent::SetParent(SceneComponent *parentCom)
{
    // 기존부모가 존재한다면 관계를 먼저 끊는다.
    if (mParentSceneComponent != nullptr)
    {

        size_t index = mParentSceneComponent->mChildSceneComponentList.Find(this);

        if (index != mParentSceneComponent->mChildSceneComponentList.End())
        {
            mParentSceneComponent->mChildSceneComponentList.Erase(index);
        }
    }

    if (parentCom != nullptr)
    {
        parentCom->mChildSceneComponentList.PushBack(this);
    }
    mParentSceneComponent = parentCom;

    ResetParentTransformVersion();
}

Core::SceneComponent *Core::SceneComponent::GetParent() const
{

    return mParentSceneComponent;
}

void Core::SceneComponent::SetScaleLocal(const CoreMath::Vector3 &scaleLocal)
{
    mTransform.SetScaleLocal(scaleLocal);
}

void Core::SceneComponent::SetQuaternionLocal(const CoreMath::Quaternion &quaternionLocal)
{
    mTransform.SetQuaternionLocal(quaternionLocal);
}

void Core::SceneComponent::SetRotationLocal(const CoreMath::Vector3 &rotataionLocal)
{

    // quternion으로변환 -> mQuaternion에서 설정

    // SetDirtyLocal();
}

void Core::SceneComponent::SetPositionLocal(const CoreMath::Vector3 &positionLocal)
{
    mTransform.SetPositionLocal(positionLocal);
}

CoreMath::Vector3 Core::SceneComponent::GetScaleLocal() const
{

    return mTransform.GetScaleLocal();
}

CoreMath::Quaternion Core::SceneComponent::GetQuaternionLocal() const
{
    return mTransform.GetQuaternionLocal();
}

CoreMath::Vector3 Core::SceneComponent::GetRotationLocal() const
{

    // GetQuaternionLocal(); 을 변환해서 리턴
    return CoreMath::Vector3();
}

CoreMath::Vector3 Core::SceneComponent::GetPositionLocal() const
{
    return mTransform.GetPositionLocal();
}

const CoreMath::Matrix4X4 &Core::SceneComponent::GetTransformLocal() const
{
    // TODO: 여기에 return 문을 삽입합니다.
    UpdateIfDirty();
    return mTransform.GetTransformLocal();
}

CoreMath::Vector3 Core::SceneComponent::GetScaleWorld() const
{
    UpdateIfDirty();
    return mTransform.GetScaleWorld();
}

CoreMath::Quaternion Core::SceneComponent::GetQuaternionWorld() const
{
    UpdateIfDirty();
    return mTransform.GetQuaternionWorld();
}

CoreMath::Vector3 Core::SceneComponent::GetPositionWorld() const
{
    UpdateIfDirty();
    return mTransform.GetPositionWorld();
}

CoreMath::Vector3 Core::SceneComponent::GetRotationWorld() const
{
    UpdateIfDirty();
    // quaternionworld를 변환해서 리턴
    return mTransform.GetPositionWorld();
}

const CoreMath::Matrix4X4 &Core::SceneComponent::GetTransformWorld() const
{
    UpdateIfDirty();
    // TODO: 여기에 return 문을 삽입합니다.
    return mTransform.GetTransformWorld();
}

void Core::SceneComponent::ResetParentTransformVersion()
{

    mParentTransformVersion = 0;
}

void Core::SceneComponent::UpdateIfDirty() const
{
    if (mTransform.IsDirty() == true)
    {
        UpdateTransformLocal();
    }
    // 부모버전과 내가 유지하고있는 부모버전비교 (정확히는 모든조상을 거슬러서 다 비교)
    // 불일치시 update 수행

    if (mTransform.IsDirty() || CheckVersionNum() == false)
    {
        UpdateTransformWorld();
        mTransform.ClearDirtyFlag(); // world는 local하고연관이있기에 로컬더티를 여기서 셋팅
    }
}

void Core::SceneComponent::UpdateTransformLocal() const
{

    mTransform.SetTransformLocal(CoreMath::Matrix4X4::MakeTransform(
        mTransform.GetPositionLocal(), mTransform.GetQuaternionLocal(), mTransform.GetScaleLocal()));
}

bool Core::SceneComponent::CheckVersionNum() const
{

    // 계층을 거슬러올라가면서 불일치가 있는지만 빠르게 확인한다.

    if (mParentSceneComponent == nullptr)
    {
        return true;
    }

    if (mParentSceneComponent->CheckVersionNum() == false)
    {
        return false;
    }
    if (mParentSceneComponent->mTransformVersion != mParentTransformVersion)
    {
        return false;
    }

    return true;
}

void Core::SceneComponent::UpdateTransformWorld() const
{

    // 부모로부터 부모의 world transform을 얻는다. (부모도 갱신될수있다 ,)

    if (mParentSceneComponent)
    {

        // Transform &parentTransform = mParentSceneComponent->mTransform;
        const CoreMath::Matrix4X4 &parentTransformWorld = mParentSceneComponent->GetTransformWorld();
        mTransform.SetTransformWorld(parentTransformWorld * mTransform.GetTransformLocal());
        mParentTransformVersion = mParentSceneComponent->mTransformVersion;
    }
    else
    {
        mTransform.SetTransformWorld(mTransform.GetTransformLocal());
    }

    CoreMath::Vector3 posWorld;
    CoreMath::Vector3 scaleWorld;
    CoreMath::Quaternion quaternionWorld;

    CoreMath::Matrix4X4::MatrixDecompose(posWorld, quaternionWorld, scaleWorld, mTransform.GetTransformWorld());
    mTransform.SetPositionWorld(posWorld);
    mTransform.SetQuaternionWorld(quaternionWorld);
    mTransform.SetScaleWorld(scaleWorld);

    mTransformVersion++;
}
