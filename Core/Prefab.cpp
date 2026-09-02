#include "Prefab.h"
#include <Core/Character.h>
#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Map.h>
#include <Core/Object.h>
#include <Core/SceneComponent.h>
#include <Core/StaticMeshComponent.h>
#include <CoreBase/BinaryArch.h>
#include <Entity.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <Utility/UniqueID.h>
#include <Utility/Utility.h>
#include <string>
Prefab::Prefab() : CoreAsset::Asset(CoreAsset::EAssetType::ePrefab), mDefaultObject(nullptr) {}

Prefab::~Prefab() {}

void Prefab::Serialize(Arch &arch)
{
    Asset::Serialize(arch);

    Serialize_Object(arch, mDefaultObject);

    if (arch.GetLoadingFlag())
    {
        EnsureDefaultComponentPrefabKey();
        if (mDefaultObject)
        {
            static_cast<Entity *>(mDefaultObject)->SetPositionLocal(0, 0, 0);
        }
    }
}

Object *Prefab::Instantiate(Map *map, const char *instanceName)
{

    if (map == nullptr || mDefaultObject == nullptr)
        return nullptr;

    auto *sourceCharacter = static_cast<Character *>(mDefaultObject);

    sourceCharacter->mStaticMeshComponent->GetStaticMesh();

    BinaryArch arch(false);

    arch.Start();

    // std::string className = mDefaultObject->GetRunTimeClassName();
    // arch << className;
    Serialize_Object(arch, mDefaultObject);
    size_t size = arch.GetBufferSize();
    std::vector<uint8_t> buffer(size);

    arch.GetBufferFromMemory();

    BinaryArch readerArch(true);
    readerArch.Start(arch.GetBufferFromMemory(), size);

    Object *instance = nullptr;

    Serialize_Object(readerArch, instance);

    arch.End();
    readerArch.End();

    if (instance == nullptr)
        return nullptr;

    instance->RebuildSceneComponentHierarchyForLoad();

    if (strcmp(instanceName, "") == 0)
        instance->SetObjectName(GetName().c_str());
    else
        instance->SetObjectName(instanceName);
    // id 새롭게 부여

    // map에 삽입

    instance->SetObjectUniqueID(CoreUtility::Utility::MakeUniqueID());

    for (auto com : instance->mComList)
    {
        com->SetComponentUniqueID(CoreUtility::Utility::MakeUniqueID());
    }

    instance->RefreshComponentIDTable();
    instance->SetPrefabID(GetID());

    map->AddPrefabInstanceObject(instance);

    // Serialize_Object(readerArch, instance) 직후
    auto *instanceCharacter = static_cast<Character *>(instance);

    instanceCharacter->mStaticMeshComponent->GetStaticMesh();

    return instance;
}

Component *Prefab::AddComponent(const std::string &componentClassName)
{

    if (mDefaultObject == nullptr)
        return nullptr;

    Component *component = mDefaultObject->CreateComponent(componentClassName.c_str(), componentClassName.c_str());

    if (component)
    {

        component->SetComponentFlag(Core::EComponentFlag::eEngineAdded);

        Entity *entity = dynamic_cast<Entity *>(mDefaultObject);

        if (entity)
        {

            auto reflectionSystem = Quad::ReflectionSystem::GetInstance();
            auto classInfo = reflectionSystem->FindClassInfo(componentClassName.c_str());
            if (classInfo->IsAncestorClass("SceneComponent"))
            {
                SceneComponent *sceneComponent = static_cast<SceneComponent *>(component);
                sceneComponent->SetParent(entity->GetRootComponent());
            }
        }
        EnsureDefaultComponentPrefabKey();

        return component;
    }
    return nullptr;
}

void Prefab::EnsureDefaultComponentPrefabKey()
{

    if (mDefaultObject == nullptr)
        return;

    for (auto com : mDefaultObject->GetComponentList())
    {

        if (com == nullptr || com->GetDeadState())
            continue;

        if (com->GetPrefabComponenetKey().empty() == false)
            continue;

        // 비어있는 component만 수행한다.
        com->SetPrefabInheritedComponent(GetID(), com->GetInstanceName());
    }
}

void Prefab::SetPositionWorld(const CoreMath::Vector3 &pos)
{

    if (Entity *entity = dynamic_cast<Entity *>(mDefaultObject))
    {
        entity->SetPositionWorld(pos);
    }
}

void Prefab::Serialize_Object(Arch &arch, Object *&object)
{

    //    mDefaultObject->Serialize(arch);
    auto reflectSystem = Quad::ReflectionSystem::GetInstance();

    if (arch.GetLoadingFlag())
    {
        std::string className;
        arch << className;

        // createinstance;
        BaseClass *baseClass = reflectSystem->CreateClassInstance(className.c_str());
        reflectSystem->SerializeBaseClass(arch, baseClass);

        object = (Object *)baseClass;

        if (object)
        {
            object->SerializeComponents(arch);
        }

        Quad::ReflectionSystem::GetInstance()->RegisterComponentGetterCallback(
            [this, object](const CoreUtility::UniqueID &id) -> BaseClass *
            {
                for (auto com : object->GetComponentList())
                {
                    if (com->GetDeadState() == false && com->GetUniqueID() == id)
                        return com;
                }
                return nullptr;
            });
        reflectSystem->ProcessComponentPointerFixup();

        object->RebuildSceneComponentHierarchyForLoad();
    }
    else
    {
        std::string className = object->GetRunTimeClassName();
        arch << className;

        BaseClass *baseClass = object;
        reflectSystem->SerializeBaseClass(arch, baseClass);

        object->SerializeComponents(arch);
    }

    mDefaultObject->DestroyDeadComponents();
}
