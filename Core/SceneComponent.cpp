#include "SceneComponent.h"
#include <Core/Map.h>
#include <Core/Object.h>
#include <ReflectSystem/ReflectionSystem.h>

SceneComponent::SceneComponent() : mParentSceneComponent(nullptr), mTransformVersion(0), mParentTransformVersion(0) {}

SceneComponent::~SceneComponent() {}

void SceneComponent::OnBegin() {}

void SceneComponent::SetParent(SceneComponent *parentCom, bool bKeepWorld)
{

    if ((parentCom == mParentSceneComponent))
    {
        return;
    }

    if (parentCom && parentCom->IsChildOf(this))
    {
        return;
    }

    // 기존부모가 존재한다면 관계를 먼저 끊는다.
    CoreMath::Matrix4X4 oldWorldMat;
    if (bKeepWorld)
    {
        oldWorldMat = GetTransformWorld();
    }

    if (mParentSceneComponent != nullptr)
    {

        std::vector<SceneComponent *> &childSceneComVector = mParentSceneComponent->mChildSceneComponentList;

        std::vector<SceneComponent *>::iterator it =
            std::find(childSceneComVector.begin(), childSceneComVector.end(), this);

        if (it != childSceneComVector.end())
        {
            childSceneComVector.erase(it);
            // 부모캄포넌3트 의 자식  컴포넌트 리스트 목록에서 제거
        }
    }

    if (parentCom != nullptr)
    {
        parentCom->mChildSceneComponentList.push_back(this);
    }
    mParentSceneComponent = parentCom;
    ResetParentTransformVersion();

    if (bKeepWorld)
    {
        CoreMath::Matrix4X4 newLocalTransform;

        if (mParentSceneComponent)
        {
            const CoreMath::Matrix4X4 &parentWorldMat = mParentSceneComponent->GetTransformWorld();

            newLocalTransform = parentWorldMat.GetInversed() * oldWorldMat;
        }
        else
        {
            newLocalTransform = oldWorldMat;
        }

        CoreMath::Vector3 newLocalPos, newLocalScale;
        CoreMath::Quaternion newLocalRot;
        newLocalTransform.Decompose(newLocalPos, newLocalRot, newLocalScale);

        SetPositionLocal(newLocalPos);
        SetQuaternionLocal(newLocalRot);
        SetScaleLocal(newLocalScale);
    }
    else
    {
        mTransform.MarkDirty();
        OnTransformChanged(); // 렌더프록시들의 갱신을 위하여 호출한다.(바로호출하지않으면
    }

    if (Object *owner = GetOwnerObject())
    {
        owner->MarkComponentTreeDirty();
    }
}

SceneComponent *SceneComponent::GetParent() const
{

    return mParentSceneComponent;
}

void SceneComponent::SetScaleLocal(const CoreMath::Vector3 &scaleLocal)
{
    mTransform.SetScaleLocal(scaleLocal);
    NotifyTransformPropertyChanged();
}

void SceneComponent::SetQuaternionLocal(const CoreMath::Quaternion &quaternionLocal)
{
    mTransform.SetQuaternionLocal(quaternionLocal);
    NotifyTransformPropertyChanged();
}

void SceneComponent::SetRotationLocal(const CoreMath::Vector3 &rotationLocal)
{

    // quternion으로변환 -> mQuaternion에서 설정
    mTransform.SetRotationLocal(rotationLocal);
    NotifyTransformPropertyChanged();
    // SetDirtyLocal();
}

void SceneComponent::SetPositionLocal(const CoreMath::Vector3 &positionLocal)
{
    mTransform.SetPositionLocal(positionLocal);
    NotifyTransformPropertyChanged();
}

void SceneComponent::AddQuaternionLocal(const CoreMath::Quaternion &quaternion)
{
    mTransform.AddQuaternionLocal(quaternion);
    NotifyTransformPropertyChanged();
}
void SceneComponent::AddRotationLocal(const CoreMath::Vector3 &rotation)
{

    mTransform.AddRotationLocal(rotation);
    NotifyTransformPropertyChanged();
}

void SceneComponent::TranslationLocal(const CoreMath::Vector3 &shift)
{

    SetPositionLocal(mTransform.GetPositionLocal() + shift);
}

void SceneComponent::SetPositionWorld(const CoreMath::Vector3 &positionWorld)
{

    // 조상 world의 역행렬 곱한다. - > new position Local

    if (mParentSceneComponent)
    {
        const CoreMath::Matrix4X4 &parentTransformWorld = mParentSceneComponent->GetTransformWorld();
        const CoreMath::Matrix4X4 parentTransformInvWorld = parentTransformWorld.GetInversed();

        const CoreMath::Vector3 newPositionLocal = parentTransformInvWorld.TransformPoint(positionWorld);

        SetPositionLocal(newPositionLocal);
    }
    else
    {
        SetPositionLocal(positionWorld);
    }
}

void SceneComponent::SetScaleWorld(const CoreMath::Vector3 &scaleWorld)
{

    if (mParentSceneComponent)
    {
        const CoreMath::Vector3 &parentScale = mParentSceneComponent->GetScaleWorld();

        CoreMath::Vector3 newScaleLocal = scaleWorld / parentScale;

        SetScaleLocal(newScaleLocal);
    }
    else
    {
        SetScaleLocal(scaleWorld);
    }
}

void SceneComponent::SetRotationWorld(const CoreMath::Vector3 &rotationWorld)
{

    SetQuaternionWorld(CoreMath::Quaternion::MakeFromEuler(rotationWorld));
}
void SceneComponent::SetQuaternionWorld(const CoreMath::Quaternion &quaterionWorld)
{

    if (mParentSceneComponent)
    {
        const CoreMath::Quaternion &parentQuaternion = mParentSceneComponent->GetQuaternionWorld();

        CoreMath::Quaternion invParentWorldQuat = parentQuaternion.GetConjugate();

        CoreMath::Quaternion newLocalQuat = invParentWorldQuat * quaterionWorld;

        SetQuaternionLocal(newLocalQuat);
    }
    else
    {
        SetQuaternionLocal(quaterionWorld);
    }
}

void SceneComponent::AddMovementWorld(const CoreMath::Vector3 &movement)
{
    CoreMath::Vector3 positionWorld = GetPositionWorld();
    positionWorld += movement;
    SetPositionWorld(positionWorld);
}

const CoreMath::Vector3 &SceneComponent::GetScaleLocal() const
{

    return mTransform.GetScaleLocal();
}

const CoreMath::Quaternion &SceneComponent::GetQuaternionLocal() const
{
    return mTransform.GetQuaternionLocal();
}

CoreMath::Vector3 SceneComponent::GetRotationLocal() const
{

    // GetQuaternionLocal(); 을 변환해서 리턴

    return mTransform.GetRotationLocal();
}

const CoreMath::Vector3 &SceneComponent::GetPositionLocal() const
{
    return mTransform.GetPositionLocal();
}

const CoreMath::Matrix4X4 &SceneComponent::GetTransformLocal() const
{
    // TODO: 여기에 return 문을 삽입합니다.
    UpdateIfDirty();
    return mTransform.GetTransformLocal();
}

const CoreMath::Vector3 &SceneComponent::GetScaleWorld() const
{
    UpdateIfDirty();
    return mTransform.GetScaleWorld();
}

const CoreMath::Quaternion &SceneComponent::GetQuaternionWorld() const
{
    UpdateIfDirty();
    return mTransform.GetQuaternionWorld();
}

const CoreMath::Vector3 &SceneComponent::GetPositionWorld() const
{
    UpdateIfDirty();
    return mTransform.GetPositionWorld();
}

CoreMath::Vector3 SceneComponent::GetRotationWorld() const
{
    UpdateIfDirty();
    // quaternionworld를 변환해서 리턴
    return mTransform.GetRotationWorld();
}

const CoreMath::Matrix4X4 &SceneComponent::GetTransformWorld() const
{
    UpdateIfDirty();
    // TODO: 여기에 return 문을 삽입합니다.
    return mTransform.GetTransformWorld();
}
const CoreMath::Vector3 SceneComponent::GetForwardWorld() const
{

    UpdateIfDirty();
    return mTransform.GetForwardWorld();
}

const CoreMath::Vector3 SceneComponent::GetUpWorld() const
{

    UpdateIfDirty();
    return mTransform.GetUpWorld();
}
const CoreMath::Vector3 SceneComponent::GetRightWorld() const
{

    UpdateIfDirty();
    return mTransform.GetRightWorld();
}

bool SceneComponent::IsChildOf(SceneComponent *targetCom)
{
    SceneComponent *current = mParentSceneComponent;
    while (current)
    {

        if (current == targetCom)
        {
            return true;
        }
        current = current->GetParent();
    }

    return false;
}

void SceneComponent::OnTransformChanged()
{

    // 자식 컴포넌트들에게 에게 전파

    for (auto child : mChildSceneComponentList)
    {
        child->OnTransformChanged();
    }
}

void SceneComponent::Serialize(Arch &arch)
{

    Component::Serialize(arch);

    auto reflectSystem = Quad::ReflectionSystem::GetInstance();

    if (arch.GetLoadingFlag())
    {
        CoreMath::Vector3 posLocal;
        CoreMath::Vector3 scaleLocal;
        CoreMath::Quaternion quatLocal;
        arch << posLocal;
        arch << scaleLocal;
        arch << quatLocal;

        mTransform.SetPositionLocal(posLocal);
        mTransform.SetScaleLocal(scaleLocal);
        mTransform.SetQuaternionLocal(quatLocal);
    }
    else
    {
        CoreMath::Vector3 posLocal = mTransform.GetPositionLocal();
        CoreMath::Vector3 scaleLocal = mTransform.GetScaleLocal();
        CoreMath::Quaternion quatLocal = mTransform.GetQuaternionLocal();

        arch << posLocal;
        arch << scaleLocal;
        arch << quatLocal;
    }

    bool bHasParentComonent = mParentSceneComponent ? true : false;
    arch << bHasParentComonent;
    if (arch.GetLoadingFlag())
    {
        if (bHasParentComonent)
        {
            CoreUtility::UniqueID parentID;
            arch << QUAD_SERIALIZEBUFFER(parentID.mUniqueID, 16);
            reflectSystem->RequestComponentPointerFix(parentID, &mParentSceneComponent);
        }
    }
    else
    {
        if (bHasParentComonent)
        {
            CoreUtility::UniqueID parentID = mParentSceneComponent->GetUniqueID();
            arch << QUAD_SERIALIZEBUFFER(parentID.mUniqueID, 16);
        }
    }
}

// void Core::SceneComponent::Serialize(Arch &arch)
//{
//
//     // 필요한것 , update version 넘버, flag등은 필요없어
//     // 오직 트랜스폼정보만 기록하면된다.
//     mTransform.Serialize(arch);
// }

void SceneComponent::ResetParentTransformVersion()
{

    mParentTransformVersion = 0;
}
void SceneComponent::ClearChildSceneComponentsForLoad()
{

    mChildSceneComponentList.clear();
}
void SceneComponent::AddChildSceneComponentForLoad(SceneComponent *com)
{

    if (com)
        mChildSceneComponentList.push_back(com);
}

const std::vector<SceneComponent *> &SceneComponent::GetChildSceneComponentList() const
{

    return mChildSceneComponentList;
}

void SceneComponent::SyncPrefabComponentFrom(Component *prefabComponent)
{
    if (prefabComponent == nullptr)
        return;

    SceneComponent *prefabSceneCom = dynamic_cast<SceneComponent *>(prefabComponent);

    if (!prefabSceneCom)
    {
        return;
    }

    SetPositionLocal(prefabSceneCom->GetPositionLocal());
    SetScaleLocal(prefabSceneCom->GetScaleLocal());
    SetRotationLocal(prefabSceneCom->GetRotationLocal());
}

unsigned long long SceneComponent::GetTransformVersion() const
{

    UpdateIfDirty();
    return mTransformVersion;
}

void SceneComponent::UpdateIfDirty() const
{

    bool bIsLocalTransformDirty = mTransform.IsDirty();
    if (bIsLocalTransformDirty == true)
    {
        UpdateTransformLocal();
    }
    // 부모버전과 내가 유지하고있는 부모버전비교 (정확히는 모든조상을 거슬러서 다 비교)
    // 불일치시 update 수행

    // 로컬이 변하면 월드도 업데이트해야하고

    // 로컬이 안변해도 부모,조상이 변하면 월드가 업데이트 되어야하고

    // 마지막으로 자신의 월드만 변하면 로컬도 업데이트 해야하고

    if (bIsLocalTransformDirty || CheckVersionNum() == false)
    {
        UpdateTransformWorld();
        //   mTransform.ClearDirtyFlag(); // world는 local하고연관이있기에 로컬더티를 여기서 셋팅
    }
}

void SceneComponent::UpdateTransformLocal() const
{

    mTransform.GetTransformLocal();
}

bool SceneComponent::CheckVersionNum() const
{

    // 계층을 거슬러올라가면서 불일치가 있는지만 빠르게 확인한다.

    if (mParentSceneComponent == nullptr)
    {
        return true;
    }

    mParentSceneComponent->UpdateIfDirty();

    // if (mParentSceneComponent->CheckVersionNum() == false)
    //{
    //     return false;
    // }
    if (mParentSceneComponent->mTransformVersion != mParentTransformVersion)
    {
        return false;
    }

    return true;
}

void SceneComponent::UpdateTransformWorld() const
{

    // 부모로부터 부모의 world transform을 얻는다. (부모도 갱신될수있다 ,)

    CoreMath::Vector3 posWorld = GetPositionLocal();
    CoreMath::Vector3 scaleWorld = GetScaleLocal();
    CoreMath::Quaternion quaternionWorld = GetQuaternionLocal();

    CoreMath::Matrix4X4 newTransformWorld;

    CoreMath::Matrix4X4 preTransformWorld = mTransform.GetTransformWorld();

    if (mParentSceneComponent)
    {

        mParentTransformVersion = mParentSceneComponent->mTransformVersion;

        CoreMath::Vector3 posParentWorld = mParentSceneComponent->GetPositionWorld();
        CoreMath::Vector3 scaleParentWorld = mParentSceneComponent->GetScaleWorld();
        CoreMath::Quaternion quaternionParentWorld = mParentSceneComponent->GetQuaternionWorld();

        scaleWorld = scaleWorld * scaleParentWorld;
        quaternionWorld = quaternionParentWorld * quaternionWorld;
        posWorld = posParentWorld + quaternionParentWorld.RotateVector(scaleParentWorld * posWorld);
    }

    newTransformWorld = CoreMath::Matrix4X4::MakeTransform(posWorld, quaternionWorld, scaleWorld);

    if (newTransformWorld != preTransformWorld)
    {

        mTransform.SetScaleWorld(scaleWorld);
        mTransform.SetPositionWorld(posWorld);
        mTransform.SetQuaternionWorld(quaternionWorld);
        mTransform.SetTransformWorld(newTransformWorld);
        mTransformVersion++;
    }
}

void SceneComponent::NotifyTransformPropertyChanged()
{
    MarkPropertyDirty();
    OnTransformChanged(); // 차
    MarkOwnerMapAssetDirty();
}

void SceneComponent::MarkOwnerMapAssetDirty()
{

    Object *ownerObject = GetOwnerObject();
    if (ownerObject == nullptr)
        return;

    Map *map = ownerObject->GetMap();
    if (map == nullptr)
        return;

    map->MarkAssetDirty();
}

void SceneComponent::OnDestoryRequested()
{
    Component::OnDestoryRequested();

    for (auto child : mChildSceneComponentList)
    {
        if (child == nullptr || child->GetDeadState())
            continue;

        child->Destory();
    }

    SetParent(nullptr);
}
