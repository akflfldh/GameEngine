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

bool Prefab::CopyDataFrom(const CoreAsset::Asset &source, std::string *failureReason)
{
    const Prefab *sourcePrefab = dynamic_cast<const Prefab *>(&source);
    if (!sourcePrefab)
    {
        if (failureReason)
            *failureReason = "Prefab 에셋이 필요합니다.";
        return false;
    }
    if (this == sourcePrefab)
        return Asset::CopyDataFrom(source, failureReason);
    if (mDefaultObject || !sourcePrefab->mDefaultObject)
    {
        if (failureReason)
            *failureReason = "Prefab 복사는 default object가 있는 원본과 비어 있는 대상이 필요합니다.";
        return false;
    }

    // default object 포인터를 공유하지 않는다. 기존 reflection/component 직렬화 형식만 사용해
    // 새 object graph를 만들고, Asset 자체의 ID/이름은 직렬화 대상에서 제외한다.
    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();
    Object *sourceObject = sourcePrefab->mDefaultObject;
    const std::string className = sourceObject->GetRunTimeClassName();
    Object *object = static_cast<Object *>(reflectionSystem->CreateClassInstance(className.c_str()));
    if (!object)
    {
        if (failureReason)
            *failureReason = "Prefab default object의 클래스를 생성할 수 없습니다.";
        return false;
    }
    BinaryArch writer(false);
    writer.Start();
    BaseClass *sourceBase = sourceObject;
    reflectionSystem->SerializeBaseClass(writer, sourceBase);
    sourceObject->SerializeComponents(writer);
    BinaryArch reader(true);
    reader.StartRead(writer.GetBufferFromMemory(), writer.GetBufferSize());
    BaseClass *destinationBase = object;
    reflectionSystem->SerializeBaseClass(reader, destinationBase);
    object->SerializeComponents(reader);
    reader.End();
    writer.End();

    reflectionSystem->RegisterObjectGetterCallback(
        [object](const CoreUtility::UniqueID &id) -> BaseClass *
        { return object->GetUniqueID() == id ? object : nullptr; });
    reflectionSystem->RegisterComponentGetterCallback(
        [object](const CoreUtility::UniqueID &id) -> BaseClass *
        {
            for (Component *component : object->GetComponentList())
            {
                if (component && !component->GetDeadState() && component->GetUniqueID() == id)
                    return component;
            }
            return nullptr;
        });
    reflectionSystem->ProcessObjectPointerFixup();
    reflectionSystem->ProcessComponentPointerFixup();
    object->RebuildSceneComponentHierarchyForLoad();
    object->SetObjectUniqueID(CoreUtility::Utility::MakeUniqueID());
    object->SetPrefabID(NoneAssetID);
    for (Component *component : object->GetComponentList())
    {
        if (!component || component->GetDeadState())
            continue;
        object->UpdateComponentID(component->GetUniqueID(), CoreUtility::Utility::MakeUniqueID(), component);
        // 아직 새 Prefab의 AssetID가 없다. 등록 후 저장/Instantiate 시 새 ID로 key를 설정한다.
        component->SetPrefabInheritedComponent(NoneAssetID, "");
    }
    object->RefreshComponentIDTable();
    mDefaultObject = object;
    return Asset::CopyDataFrom(source, failureReason);
}

void Prefab::Serialize(Arch &arch)
{
    Asset::Serialize(arch);

    if (!arch.GetLoadingFlag())
        EnsureDefaultComponentPrefabKey();

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

    EnsureDefaultComponentPrefabKey();

    BinaryArch arch(false);

    arch.Start();

    // std::string className = mDefaultObject->GetRunTimeClassName();
    // arch << className;
    Serialize_Object(arch, mDefaultObject);
    size_t size = arch.GetBufferSize();
    std::vector<uint8_t> buffer(size);

    arch.GetBufferFromMemory();

    BinaryArch readerArch(true);
    readerArch.StartRead(arch.GetBufferFromMemory(), size);

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
