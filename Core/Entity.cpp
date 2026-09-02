#include "Entity.h"
#include <Core/CameraComponent.h>
#include <CoreBase/Arch.h>

Entity::Entity() : mRootSceneComponent(nullptr)
{

    mRootSceneComponent = static_cast<SceneComponent *>(CreateDefaultComponent<SceneComponent>("RootSceneCom"));
    mDefaultRootSceneComponent = mRootSceneComponent;
}

Entity::~Entity() {}

void Entity::OnBegin()
{
    Object::OnBegin();

    CleanUpOrphanSceneComponents();
}

void Entity::SetPositionLocal(const CoreMath::Vector3 &pos)
{

    if (mRootSceneComponent)
    {
        mRootSceneComponent->SetPositionLocal(pos);
    }
}
void Entity::SetPositionLocal(float x, float y, float z)
{
    SetPositionLocal({x, y, z});
}

void Entity::SetScaleLocal(const CoreMath::Vector3 &scale)
{

    if (mRootSceneComponent)
    {
        mRootSceneComponent->SetScaleLocal(scale);
    }

    /*  for (auto &com : GetComponentList())
      {
          if (SceneComponent *sceneComp = dynamic_cast<SceneComponent *>(com))
          {
              sceneComp->OnTransformChanged();
          }
      }*/
}

void Entity::SetScaleLocal(float x, float y, float z)
{

    SetScaleLocal({x, y, z});
}

void Entity::SetRotationLocal(const CoreMath::Vector3 &rotation)
{

    if (mRootSceneComponent)
    {
        mRootSceneComponent->SetRotationLocal(rotation);
    }

    // mRootSceneComponent->OnTransformChanged(); // 차
}

void Entity::SetRotationLocal(float x, float y, float z)
{

    SetRotationLocal({x, y, z});
}

void Entity::AddRotationLocal(float x, float y, float z)
{
    AddRotationLocal({x, y, z});
}

void Entity::AddRotationLocal(const CoreMath::Vector3 &rotation)
{
    if (mRootSceneComponent)
    {
        mRootSceneComponent->AddRotationLocal(rotation);

        // mRootSceneComponent->OnTransformChanged(); // 차
    }
}

void Entity::SetPositionWorld(const CoreMath::Vector3 &pos)
{

    if (mRootSceneComponent)
    {
        mRootSceneComponent->SetPositionWorld(pos);
    }

    /*  for (auto &com : GetComponentList())
      {
          if (SceneComponent *sceneComp = dynamic_cast<SceneComponent *>(com))
          {
              sceneComp->OnTransformChanged();
          }
      }*/
}

void Entity::SetPositionWorld(float x, float y, float z)
{

    SetPositionWorld({x, y, z});
}

void Entity::SetScaleWorld(const CoreMath::Vector3 &scale)
{

    if (mRootSceneComponent)
    {
        mRootSceneComponent->SetScaleWorld(scale);
    }

    // for (auto &com : GetComponentList())
    //{
    //     if (SceneComponent *sceneComp = dynamic_cast<SceneComponent *>(com))
    //     {
    //         sceneComp->OnTransformChanged();
    //     }
    // }
}

void Entity::SetScaleWorld(float x, float y, float z)
{

    SetScaleWorld({x, y, z});
}

void Entity::SetRotationWorld(const CoreMath::Vector3 &rotation)
{

    if (mRootSceneComponent)
    {
        mRootSceneComponent->SetRotationWorld(rotation);
    }
    /*for (auto &com : GetComponentList())
    {
        if (SceneComponent *sceneComp = dynamic_cast<SceneComponent *>(com))
        {
            sceneComp->OnTransformChanged();
        }
    }*/
}

void Entity::SetRotationWorld(float x, float y, float z)
{
    SetRotationWorld({x, y, z});
}

void Entity::TranslationWorld(float x, float y, float z)
{
    TranslationWorld({x, y, z});
}

void Entity::TranslationWorld(const CoreMath::Vector3 &shift)
{

    CoreMath::Vector3 worldPos = GetPositionWorld();

    SetPositionWorld(worldPos + shift);
}

CoreMath::Vector3 Entity::GetPositionLocal() const
{

    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetPositionLocal();
    }

    return {0, 0, 0};
}
CoreMath::Vector3 Entity::GetScaleLocal() const
{
    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetScaleLocal();
    }
    return {0, 0, 0};
}
CoreMath::Vector3 Entity::GetRotationLocal() const
{
    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetRotationLocal();
    }
    return {0, 0, 0};
}
CoreMath::Vector3 Entity::GetPositionWorld() const
{
    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetPositionWorld();
    }
    return {0, 0, 0};
}
CoreMath::Vector3 Entity::GetScaleWorld() const
{
    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetScaleWorld();
    }
    return {0, 0, 0};
}
CoreMath::Vector3 Entity::GetRotationWorld() const
{
    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetRotationWorld();
    }
    return {0, 0, 0};
}

CoreMath::Vector3 Entity::GetForwardWorld() const
{
    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetForwardWorld();
    }

    return {0, 0, 0};
}

CoreMath::Vector3 Entity::GetRightWorld() const
{

    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetRightWorld();
    }

    return {0, 0, 0};
}

CoreMath::Vector3 Entity::GetUpWorld() const
{
    if (mRootSceneComponent)
    {
        return mRootSceneComponent->GetUpWorld();
    }

    return {0, 0, 0};
}

void Entity::SetRootComponent(SceneComponent *com)
{

    mRootSceneComponent = com;
}

SceneComponent *Entity::GetRootComponent() const
{
    return mRootSceneComponent;
}

void Entity::OnConnectedNewParent(Object *parent)
{

    Entity *parentE = dynamic_cast<Entity *>(parent);
    if (parentE && parentE->GetRootComponent())
    {
        mRootSceneComponent->SetParent(parentE->GetRootComponent(), true);
    }
}

void Entity::OnDisconnectedParent()
{

    mRootSceneComponent->SetParent(nullptr, true);
}

void Entity::Serialize(Arch &arch)
{
    Object::Serialize(arch);

    bool bHasDefaultRootSceneComponent = true;
    if (arch.GetLoadingFlag())
    {
        arch << bHasDefaultRootSceneComponent;

        // 저장당시에는 디폴트루트씬컴포넌트가존재했다.
        if (bHasDefaultRootSceneComponent)
        {
            mDefaultRootSceneComponent->Serialize(arch);

            // 유저가 코드를수정해서 자신이생성한 컴포넌트를 root로 설정했다면
            if (mRootSceneComponent != mDefaultRootSceneComponent)
            {
                // 현재 프로젝트 코드가 실행되었을떄 디폴트씬컴포넌트가 사라졌다면
                // 제거 필요
                mDefaultRootSceneComponent->Destory();
                mDefaultRootSceneComponent = nullptr;
            }
        }
        else
        {
            CoreUtility::UniqueID rootID;
            arch << QUAD_SERIALIZEBUFFER(rootID.mUniqueID, 16);
            Quad::ReflectionSystem::GetInstance()->RequestComponentPointerFix(
                rootID, reinterpret_cast<void **>(&mRootSceneComponent));
        }
    }
    else
    {

        bHasDefaultRootSceneComponent = (mRootSceneComponent == mDefaultRootSceneComponent) ? true : false;
        arch << bHasDefaultRootSceneComponent;

        if (bHasDefaultRootSceneComponent)
        {
            // 사용자에의해서 루트컴포넌트가 변경되지않았다.
            // 직렬화대상.
            mRootSceneComponent->Serialize(arch);
        }
        else
        {
            // 유저가 정의한 멤버변수의컴포넌트를 코드상에서 rootComponent설정했다던가 .아니면 에디터에서 설정했다던가
            // 했을거다. 그 컴포넌트들은 다른곳에서 직렬화 된다 다만 코드상에서 rootComponent설정하더라도 설정 코드를
            // 지우고 다시 빌드시에도 유지되도록하기위해서 , 더욱이 에디터에서 설정했을경우들에 대해서 RootComponent의
            // ID를 저장한다.

            CoreUtility::UniqueID id = mRootSceneComponent->GetUniqueID();
            arch << QUAD_SERIALIZEBUFFER(id.mUniqueID, 16);
        }
    }
}

void Entity::CleanUpOrphanSceneComponents()
{

    std::vector<SceneComponent *> orphanComList;
    for (auto com : GetComponentList())
    {
        if (auto sceneCom = dynamic_cast<SceneComponent *>(com))
        {
            if (sceneCom != mRootSceneComponent && (sceneCom->GetParent() == nullptr))
            {
                orphanComList.push_back(sceneCom);
            }
        }
    }

    for (auto com : orphanComList)
    {
        com->Destory();
    }
}
