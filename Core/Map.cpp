#include "Map.h"
#include <Core/CameraComponent.h>
#include <Core/Component.h>
#include <Core/Entity.h>
#include <Core/Object.h>
#include <Core/ObjectController.h>
#include <Core/ObjectManager.h>
#include <Core/Prefab.h>
#include <Core/SceneComponent.h>
#include <CoreAsset/AssetManager.h>
#include <CoreBase/BinaryArch.h>
#include <Physics/PhysicsScene.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <Utility/Utility.h>
#include <algorithm>
Map::Map() : Asset(CoreAsset::EAssetType::eMap), mDirty(false)
{

    mReflectionSystem = Quad::ReflectionSystem::GetInstance();
}

Map::~Map()
{
    DestoryEntitesProgrammatically();
}

void Map::Start()
{
    if (mStarted)
        return;

    mStarted = true;

    for (auto element : mEntityList)
    {
        element->Start();
    }

    for (auto element : mEngineEntityList)
    {
        element->Start();
    }
}

void Map::BeginPlay()
{

    if (mPlayBegun)
        return;

    mPlayBegun = true;

    for (auto element : mEntityList)
    {
        if (element && !element->GetKillState())
            element->BeginPlay();
    }
}

void Map::Update(float DeltaTime)
{

    for (auto element : mEntityList)
    {
        if (element && !element->GetKillState())
            element->Update(DeltaTime);
    }
}

void Map::EndPlay()
{

    if (!mPlayBegun)
        return;

    mPlayBegun = false;

    for (auto element : mEntityList)
    {
        if (element && !element->GetKillState())
            element->EndPlay();
    }

    DestroyTemporaryEntitiesForPlay();
}

void Map::EndUpdate(float DeltaTime)
{
    for (auto element : mEntityList)
    {
        element->EndUpdate(DeltaTime);
    }

    for (auto element : mEngineEntityList)
    {
        element->EndUpdate(DeltaTime);
    }
}

void Map::CleanUp()
{

    for (auto object : mEntityList)
    {
        if (object && !object->GetKillState())
        {
            object->DestroyDeadComponents();
        }
    }

    for (auto object : mEngineEntityList)
    {
        if (object && !object->GetKillState())
        {
            object->DestroyDeadComponents();
        }
    }

    DestroyEntities();
}

void Map::FlushPropertyDirty()
{
    for (auto object : mEntityList)
    {
        if (object && object->GetKillState() == false)
            object->FlushPropertyDirty();
    }

    for (auto object : mEngineEntityList)
    {
        if (object && object->GetKillState() == false)
            object->FlushPropertyDirty();
    }
}

void Map::AddPrefabObject(Object *object)
{
    // 순서중요
    // map에 추가될때 active상태이면 render proxy가  등록되니 false상태에서 먼저 map에추가
    object->SetMap(this);
    object->SetActive(true); // true로 전환해도 등록됨

    mEntityList.push_back(object);
}

void Map::RemovePrefabObject(Object *object)
{
    // 순서중요
    object->SetActive(false);
    object->SetMap(nullptr);
    auto it = std::find(mEntityList.begin(), mEntityList.end(), object);

    if (it == mEntityList.end())
        return;

    std::iter_swap(mEntityList.end() - 1, it);
    mEntityList.pop_back();

    std::vector<CameraComponent *> cameraComList;

    for (auto com : object->GetComponentList())
    {
        if (CameraComponent *cameraCom = dynamic_cast<CameraComponent *>(com))
        {
            UnRegisterCameraComponent(cameraCom);
        }
    }
}

void Map::AddPrefabInstanceObject(Object *object)
{

    std::string instanceName = object->GetObjectName();

    instanceName = GetAvailableObjectName(instanceName);

    SetObjectProperty(object, instanceName);
    RegisterEntity(object);

    mObjectAddedCallbackSystem.ExecuteCallbacks(object);

    MarkAssetDirty();
}

Object *Map::InstantiatePrefab(CoreAsset::AssetID prefabID, const char *instanceName)
{

    if (prefabID == NoneAssetID)
        return nullptr;

    auto prefabPtr = CoreAsset::AssetManager::GetInstance()->GetAsset<Prefab>(prefabID);
    Prefab *prefab = static_cast<Prefab *>(prefabPtr.Get());

    if (prefab == nullptr)
        return nullptr;

    Object *object = prefab->Instantiate(this);

    return object;
}

Object *Map::DuplicateEntity(Object *sourceObject)
{
    if (sourceObject == nullptr || sourceObject->GetMap() != this || sourceObject->GetKillState())
        return nullptr;

    BinaryArch writerArch(false);
    writerArch.Start();

    std::string className = sourceObject->GetRunTimeClassName();
    writerArch << className;

    BaseClass *sourceBaseClass = sourceObject;

    // 원본오브젝트에대해서 직렬화 수행
    mReflectionSystem->SerializeBaseClass(writerArch, sourceBaseClass);
    sourceObject->SerializeComponents(writerArch);

    uint8_t *buffer = writerArch.GetBufferFromMemory();
    const size_t bufferSize = writerArch.GetBufferSize();

    BinaryArch readerArch(true);
    readerArch.Start(buffer, bufferSize);

    // 복사본 역직렬화 수행
    std::string duplicatedClassName;
    readerArch << duplicatedClassName;

    BaseClass *duplicatedBaseClass = mReflectionSystem->CreateClassInstance(duplicatedClassName.c_str());
    Object *duplicatedObject = static_cast<Object *>(duplicatedBaseClass);
    if (duplicatedObject == nullptr)
    {
        writerArch.End();
        readerArch.End();
        return nullptr;
    }

    mReflectionSystem->SerializeBaseClass(readerArch, duplicatedBaseClass);
    duplicatedObject->SerializeComponents(readerArch);

    writerArch.End();
    readerArch.End();

    // 여기까지 복사본의 objectID가 원본과 동일하기에 이상태로 Map에 등록해서는 안된다 .

    mReflectionSystem->RegisterComponentGetterCallback(
        [this, duplicatedObject](const CoreUtility::UniqueID &id) -> BaseClass *
        {
            for (Component *component : duplicatedObject->GetComponentList())
            {
                if (component && !component->GetDeadState() && component->GetUniqueID() == id)
                    return component;
            }

            for (Object *object : mEntityList)
            {
                for (Component *component : object->GetComponentList())
                {
                    if (component && !component->GetDeadState() && component->GetUniqueID() == id)
                        return component;
                }
            }
            return nullptr;
        });

    mReflectionSystem->ProcessComponentPointerFixup();

    // 동일한 부모 유지
    duplicatedObject->SetParent(sourceObject->GetParent());

    const CoreUtility::UniqueID sourceObjectID = sourceObject->GetUniqueID();

    // 아직까지 com ,object사이의 포인터 fix up을 위해서 id는 변경하지않은상태
    mReflectionSystem->RegisterObjectGetterCallback(
        [this, sourceObjectID, duplicatedObject](const CoreUtility::UniqueID &id) -> BaseClass *
        {
            if (id == sourceObjectID)
                return duplicatedObject;
            return GetEntity(id);
        });

    mReflectionSystem->ProcessObjectPointerFixup();

    duplicatedObject->RebuildSceneComponentHierarchyForLoad();

    // 완전히 재구축이 끝났으니 복사본 오브젝트의 id를 변경한다.
    duplicatedObject->SetObjectUniqueID(CoreUtility::Utility::MakeUniqueID());

    // com의 id도 변경한다.
    for (Component *component : duplicatedObject->GetComponentList())
    {
        if (component == nullptr || component->GetDeadState())
            continue;

        duplicatedObject->UpdateComponentID(component->GetUniqueID(), CoreUtility::Utility::MakeUniqueID(), component);
    }

    duplicatedObject->RefreshComponentIDTable();
    duplicatedObject->SetPrefabID(sourceObject->GetPrefabID());

    // 원래버전의 콜백으로 설정
    mReflectionSystem->RegisterComponentGetterCallback(
        [this](const CoreUtility::UniqueID &id) -> BaseClass *
        {
            for (Object *object : mEntityList)
            {
                for (Component *component : object->GetComponentList())
                {
                    if (component && !component->GetDeadState() && component->GetUniqueID() == id)
                        return component;
                }
            }
            return nullptr;
        });

    mReflectionSystem->RegisterObjectGetterCallback([this](const CoreUtility::UniqueID &id) -> BaseClass *
                                                    { return GetEntity(id); });

    // 맵에 등록한다.
    AddPrefabInstanceObject(duplicatedObject);
    duplicatedObject->FlushPropertyDirty();
    return duplicatedObject;
}

void Map::RequestDestoryEntity(Object *entity)
{

    if (entity == nullptr)
        return;

    mKilledEntityList.push_back(entity);

    mObjectRemovedCallbackSystem.ExecuteCallbacks(entity);
}

const std::vector<Object *> &Map::GetEntityList() const
{
    return mEntityList;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Map::RegisterCameraComponoent(CameraComponent *com)
{
    // 방어코드

    if (com == nullptr)
        return;

    auto it = std::find(mCameraComponentList.begin(), mCameraComponentList.end(), com);
    if (it != mCameraComponentList.end())
        return;

    mCameraComponentList.push_back(com);
    mOnCameraComponentAddedCallbackSystem.ExecuteCallbacks(com);
}

void Map::UnRegisterCameraComponent(CameraComponent *com)
{

    if (com == nullptr)
        return;

    if (mCameraComponentList.empty())
        return;

    auto it = std::find(mCameraComponentList.begin(), mCameraComponentList.end(), com);

    if (it == mCameraComponentList.end())
        return;

    //   size_t index = it - mCameraComponentList.begin();
    mCameraComponentList.erase(it);

    if (mCameraComponentList.empty())
        mAcvieCameraIndex = 0;

    if (mAcvieCameraIndex >= mCameraComponentList.size())
    {
        mAcvieCameraIndex = mCameraComponentList.size() - 1;
    }

    mOnCameraComponentRemovedCallbackSystem.ExecuteCallbacks(com);
}

CameraComponent *Map::GetCameraComponent(int index)
{
    if (index >= mCameraComponentList.size())
        return nullptr;

    return mCameraComponentList[index];
}

CameraComponent *Map::GetActiveCameraComponent() const
{

    if (mCameraComponentList.empty())
        return nullptr;

    if (mAcvieCameraIndex >= mCameraComponentList.size())
        return nullptr;

    return mCameraComponentList[mAcvieCameraIndex];
}

const std::vector<CameraComponent *> &Map::GetCameraComList() const
{
    // TODO: 여기에 return 문을 삽입합니다.
    return mCameraComponentList;
}

bool Map::SetActiveCameraIndex(int index)
{

    if (index >= mCameraComponentList.size())
        return false;

    mAcvieCameraIndex = index;

    return true;
}

bool Map::SetActiveCamera(CameraComponent *cameraCom)
{

    for (size_t i = 0; i < mCameraComponentList.size(); ++i)
    {
        if (mCameraComponentList[i] == cameraCom)
        {

            mAcvieCameraIndex = i;
            return true;
        }
    }

    return false;
}

void Map::RegisterLightComponent(LightComponent *com)
{

    if (com == nullptr)
        return;

    auto it = std::find(mLightComponentList.begin(), mLightComponentList.end(), com);

    if (it != mLightComponentList.end())
        return;

    mLightComponentList.push_back(com);

    mOnLightComponentAddeddCallbackSystem.ExecuteCallbacks(com);
}

void Map::UnRegisterLightComponent(LightComponent *com)
{

    if (com == nullptr)
        return;

    auto it = std::find(mLightComponentList.begin(), mLightComponentList.end(), com);

    if (it == mLightComponentList.end())
        return;

    mLightComponentList.erase(it);

    mOnLightComponentRemovedCallbackSystem.ExecuteCallbacks(com);
}

const std::vector<LightComponent *> &Map::GetLightComponentList() const
{
    return mLightComponentList;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Map::RegisterPlayerStart(PlayerStart *playerStart)
{

    if (playerStart == nullptr)
        return;

    if (std::find(mPlayerStartList.begin(), mPlayerStartList.end(), playerStart) != mPlayerStartList.end())
    {
        return;
    }

    mPlayerStartList.push_back(playerStart);

    mOnPlayerStartAddedCallbackSystem.ExecuteCallbacks(playerStart);
}

void Map::UnRegisterPlayerStart(PlayerStart *playerStart)
{

    if (playerStart == nullptr)
        return;

    auto it = std::find(mPlayerStartList.begin(), mPlayerStartList.end(), playerStart);
    if (it == mPlayerStartList.end())
        return;

    std::iter_swap(it, mPlayerStartList.end() - 1);
    mPlayerStartList.pop_back();

    mOnPlayerStartRemovedCallbackSystem.ExecuteCallbacks(playerStart);
}

const std::vector<PlayerStart *> &Map::GetPlayerStartList() const
{
    return mPlayerStartList;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Map::RegisterObjectController(ObjectController *controller)
{
    // 방어 코드

    if (controller == nullptr)
        return;

    if (controller->HasObjectFlag(Core::EObjectFlag::eEngineEntity))
        return;

    auto it = std::find(mObjectControllerList.begin(), mObjectControllerList.end(), controller);
    if (it != mObjectControllerList.end())
        return;

    mObjectControllerList.push_back(controller);
}

ObjectController *Map::GetCurrentObjectController(int index) const
{

    if (index >= mObjectControllerList.size())
        return nullptr;

    return mObjectControllerList[index];
}

size_t Map::GetObjectControllerNum() const
{
    return mObjectControllerList.size();
}

void Map::RegisterEngineEntity(Object *object)
{

    mEngineEntityList.push_back(object);
}

const std::vector<Object *> &Map::GetEngineObjectList() const
{
    return mEngineEntityList;
    // TODO: 여기에 return 문을 삽입합니다.
}

// void Core::Map::DestoryEntity(Object *entity)
//{
//     // shared_ptr<Entity>가 제거될떄 호출된다.
//
//     // entity에 killflag를 설정
//     // 이미 kill플래그가 설정되어있다면 바로 리턴
//     if (entity->GetKillState())
//         return;
//
//     mObjectRemovedCallbackSystem.ExecuteCallbacks(entity);
//
//     // 그리고 따로 리스트에 유지한다.
//     mKilledEntityList.push_back(entity);
//
//     Object *object = static_cast<Object *>(entity);
//     object->mKillState = true;
//
//     std::vector<Object *>::iterator it = std::find(mEntityList.begin(), mEntityList.end(), entity);
//     if (it != mEntityList.end())
//     {
//         mEntityList.erase(it);
//     }
//
//     std::unordered_map<std::string, Object *>::iterator it2 = mEntityNameTable.find(entity->GetObjectName());
//
//     if (it2 != mEntityNameTable.end())
//     {
//         mEntityNameTable.erase(it2);
//     }
// }

Object *Map::GetEntity(CoreUtility::UniqueID id) const
{

    auto it = mEntityIDTable.find(id);
    if (it != mEntityIDTable.end())
        return it->second;
    return nullptr;
}

Object *Map::GetEntity(const char *entityInstanceName) const
{

    std::unordered_map<std::string, Object *>::const_iterator it = mEntityNameTable.find(entityInstanceName);
    if (it == mEntityNameTable.cend())
    {
        return nullptr;
    }

    return it->second;
}

// std::shared_ptr<Core::Object> Core::Map::GetEntitySharedPtr(CoreUtility::UniqueID id) const
//{
//
//     std::unordered_map<CoreUtility::UniqueID, std::shared_ptr<Object>>::const_iterator it = mEntityIDTable.find(id);
//     if (it == mEntityIDTable.cend())
//     {
//
//         return nullptr;
//     }
//
//     return it->second;
// }

void Map::RegisterEntityDeleter(const std::function<void(Object *)> &deleter) {}

void Map::Serialize(Arch &arch)
{
    Asset::Serialize(arch);
}

void Map::SerilaizeRawData(Arch &arch)
{
    Core::ObjectManager *objectManager = Core::ObjectManager::GetInstance();

    SerializeGameModeSetting(arch);
    SerializeSceneSetting(arch);

    uint32_t objectNum = 0;
    std::string tableStr;

    if (arch.GetLoadingFlag())
    {

        mReflectionSystem->RegisterComponentGetterCallback(
            [this](const CoreUtility::UniqueID &id) -> BaseClass *
            {
                for (auto ob : mEntityList)
                {

                    for (auto com : ob->GetComponentList())
                    {
                        if (!com->GetDeadState() && com->GetUniqueID() == id)
                        {
                            return com;
                        }
                    }
                }
                return nullptr;
            });

        mReflectionSystem->RegisterObjectGetterCallback(
            [this](const CoreUtility::UniqueID &id) -> BaseClass *
            {
                for (auto ob : mEntityList)
                {
                    if (ob->GetUniqueID() == id)
                        return ob;
                }
                return nullptr;
            });

        // 로드시에
        // 먼저 리스트 개수
        arch << objectNum;
        // ob들의 타입 이름  -> 빈 인스턴스 생성

        ClearEntitiesForLoad();
        mEntityList.reserve(objectNum);

        for (size_t i = 0; i < objectNum; ++i)
        {

            std::string className;
            arch << className;

            // TODO arch로 시작과끝을 여기서도 잡긴잡아야한다.
            arch.StartTable(tableStr);

            Object *object = CreateEmptyEntityForLoad(className);
            if (object != nullptr)
            {

                BaseClass *baseClassInstance = object;

                // object가 nullptr이면 스킵됨
                mReflectionSystem->SerializeBaseClass(arch, baseClassInstance);

                // TODO 여기서 에디터가 생성한 COM들을 선별해서 직렬화를 또 하자
                // object nullptr일떄 스킵해야함.

                object->SerializeComponents(arch);

                if (object)
                {
                    RegisterRestoredEntity(object);
                }
            }
            else
            {
                // TODO Log;
            }

            arch.EndTable();

            // TODO arch로 끝을잡자.
        }

        mReflectionSystem->ProcessObjectPointerFixup();
        RebuildObjectHierarchy();

        mReflectionSystem->ProcessComponentPointerFixup();
        RebuildObjectSceneComponentHierarchy();
        for (auto object : mEntityList)
        {
            object->FlushPropertyDirty();
        }

        ClearDirty();
    }
    else
    {
        // 저장시
        // ob 리스트 개수
        //
        //
        objectNum = mEntityList.size();
        for (auto ob : mEntityList)
        {
            if (ob && ob->HasObjectFlag(Core::EObjectFlag::eTemporary))
                objectNum--;
        }

        arch << objectNum;

        for (auto ob : mEntityList)
        {
            if (ob != nullptr && (ob->HasObjectFlag(Core::EObjectFlag::eTemporary) != true))
            {

                Quad::ClassInfo *classInfo = mReflectionSystem->FindClassInfo(ob->GetRunTimeClassName());

                // 항상 해당 object intance의 클래스 이름을 맨위에 저장
                // 로드시객체 생성에 필요
                std::string className = ob->GetRunTimeClassName();
                arch << className;

                arch.StartTable(tableStr);

                BaseClass *pBaseClass = static_cast<BaseClass *>(ob);
                mReflectionSystem->SerializeBaseClass(arch, pBaseClass);

                ob->SerializeComponents(arch);

                arch.EndTable();
            }
        }
    }
}

void Map::OnInputEvent(const Core::InputData &inputData)
{

    mMultiInputCallbackSystem.ExecuteCallbacks(inputData);
    // 등록된 오브젝트들의 입력 콜백들을 호출한다.
}

Object *Map::CreateEntity(const char *entityClassName, const char *entityInstanceName)
{

    std::string instanceName = entityInstanceName;

    if (instanceName == "" || (mEntityNameTable.find(instanceName) != mEntityNameTable.end()))
    {
        instanceName = GetAvailableObjectName(entityClassName);
    }

    Object *object = static_cast<Object *>(mReflectionSystem->CreateClassInstance(entityClassName));
    if (object == nullptr)
        return nullptr;

    Core::ObjectManager *objectManager = Core::ObjectManager::GetInstance();

    //   object->SetObjectName(instanceName.c_str());
    object->SetObjectUniqueID(CoreUtility::Utility::MakeUniqueID());
    //    object->SetObjectPoolHandle(objectManager->Register(object));
    SetObjectProperty(object, instanceName);
    RegisterEntity(object);

    mObjectAddedCallbackSystem.ExecuteCallbacks(object);

    MarkAssetDirty();

    return object;
}

Object *Map::CreateEngineEntity(const char *entityClassName, const char *entityInstanceName)
{
    std::string instanceName = entityInstanceName;

    if (instanceName.empty())
    {
        instanceName = GetAvailableObjectName(entityClassName);
    }

    Object *object = static_cast<Object *>(mReflectionSystem->CreateClassInstance(entityClassName));
    if (object == nullptr)
        return nullptr;

    object->SetObjectUniqueID(CoreUtility::Utility::MakeUniqueID());
    SetObjectProperty(object, instanceName);
    object->SetObjectFlag(Core::EObjectFlag::eTemporary | Core::EObjectFlag::eHidden |
                          Core::EObjectFlag::eEngineEntity);
    object->SetMap(this);

    mEngineEntityList.push_back(object);
    return object;
}

Object *Map::SpawnObject(const Core::SpawnRequestContext &spawnRequestContext)
{
    Object *object = nullptr;
    switch (spawnRequestContext.mSpawnSourceType)
    {
    case Core::ESpawnObjectSourceType::eClass:
    {
        if (spawnRequestContext.mClassName.empty())
            return nullptr;

        object = CreateEntity(spawnRequestContext.mClassName.c_str(), spawnRequestContext.mInstanceName.c_str());
    }
    break;
    case Core::ESpawnObjectSourceType::ePrefab:
    {
        if (spawnRequestContext.mPrefabID == NoneAssetID)
            return nullptr;

        object = InstantiatePrefab(spawnRequestContext.mPrefabID, spawnRequestContext.mInstanceName.c_str());
    }
    break;
    default:
        return nullptr;
    }

    if (object == nullptr)
        return nullptr;

    if (Entity *entity = dynamic_cast<Entity *>(object))
    {
        entity->SetPositionLocal(spawnRequestContext.mPosition);
        entity->SetRotationLocal(spawnRequestContext.mRotation);
        entity->SetScaleLocal(spawnRequestContext.mScale);
    }

    return object;
}

void Map::DestroyEntities()
{
    if (mKilledEntityList.empty() == false)
    {
        MarkAssetDirty();
    }

    for (auto object : mKilledEntityList)
    {

        auto it = std::find(mEntityList.begin(), mEntityList.end(), object);
        if (it != mEntityList.end())
        {
            mEntityList.erase(it);
        }

        //    std::vector<Object *> mEngineEntityList;

        mEntityNameTable.erase(object->GetObjectName());
        mEntityIDTable.erase(object->GetUniqueID());

        object->SetMap(nullptr);
        object->DestroyDeadComponents();
        Core::ObjectManager::GetInstance()->UnRegister(object->GetObjectHandle().mObjectPoolIndex);
        mReflectionSystem->DestoryClassInstance(object);
    }

    mKilledEntityList.clear();
}

bool Map::RayHit(const CoreMath::Ray &ray, Core::HitResult &oHitResult) const
{
    bool bHitAnything = false;
    oHitResult.mDistance = FLT_MAX;
    Core::HitResult hitResultTemp;

    for (auto object : mEntityList)
    {
        if (object == nullptr)
            continue;

        if (object->GetKillState() || !object->GetActive())
            continue;

        if (object->RayHit(ray, hitResultTemp))
        {
            if (oHitResult.mDistance > hitResultTemp.mDistance)
            {
                oHitResult = hitResultTemp;
                bHitAnything = true;
            }
        }
    }

    return bHitAnything;
}

void Map::SetRenderID(uint32_t id)
{
    mRenderID = id;
}

uint32_t Map::GetRenderID() const
{
    return mRenderID;
}
void Map::SetWorld(World *world)
{

    mWorld = world;
}
World *Map::GetWorld() const
{

    return mWorld;
}

void Map::SetAssetDirtyActive(bool flag)
{

    mAssetDirtyActive = flag;
}

void Map::MarkAssetDirty()
{

    if (mAssetDirtyActive)
    {
        SetDirty();
        SetRawDataDirty(true);
    }
}

void Map::ClearEntitiesForLoad()
{
    if (mPlayBegun)
    {
        EndPlay();
    }

    mStarted = false;
    mPlayBegun = false;

    for (auto object : mEntityList)
    {
        if (object == nullptr)
            continue;

        // dead상태이든 아니든 제거
        object->DestroyImmediatelyForLoad();
        Core::ObjectManager::GetInstance()->UnRegister(object->GetObjectHandle().mObjectPoolIndex);
        mReflectionSystem->DestoryClassInstance(object);
    }

    DestroyEngineEntitiesImmediately();

    mKilledEntityList.clear();
    mEntityIDTable.clear();
    mEntityNameTable.clear();
    mEntityList.clear();
    mCameraComponentList.clear();
    mObjectControllerList.clear();
    mAcvieCameraIndex = 0;
}

void Map::ClearCallbackSystems()
{

    mObjectAddedCallbackSystem.CleanUp();
    mObjectRemovedCallbackSystem.CleanUp();
    mMultiInputCallbackSystem.CleanUp();
}

const Core::GameModeSetting &Map::GetGameModeSetting() const
{
    return mGameModeSetting;

    // TODO: 여기에 return 문을 삽입합니다.
}

void Map::SetGameModeSettings(const Core::GameModeSetting &settings)
{

    mGameModeSetting = settings;
    MarkAssetDirty();
}
void Map::SetDefaultPlayerObjectClass(const std::string &className)
{

    mGameModeSetting.mDefaultPlayerObjectSource = Core::ESpawnObjectSourceType::eClass;
    mGameModeSetting.mDefaultPlayerObjectClassName = className;
    mGameModeSetting.mDefaultPlayerObjectPrefabID = NoneAssetID;
    MarkAssetDirty();
}
void Map::SetDefaultPlayerObjectPrefab(CoreAsset::AssetID prefabID)
{

    mGameModeSetting.mDefaultPlayerObjectSource = Core::ESpawnObjectSourceType::ePrefab;
    mGameModeSetting.mDefaultPlayerObjectPrefabID = prefabID;
    mGameModeSetting.mDefaultPlayerObjectClassName.clear();

    MarkAssetDirty();
}
void Map::SetDefaultPlayerControllerClass(const std::string &className)
{

    mGameModeSetting.mDefaultPlayerControllerClassName = className;
    MarkAssetDirty();
}
void Map::ClearDefaultPlayerObject()
{
    mGameModeSetting.mDefaultPlayerObjectSource = Core::ESpawnObjectSourceType::eNone;
    mGameModeSetting.mDefaultPlayerObjectClassName.clear();
    mGameModeSetting.mDefaultPlayerObjectPrefabID = NoneAssetID;

    MarkAssetDirty();
}

void Map::ClearDefaultPlayerController()
{

    mGameModeSetting.mDefaultPlayerControllerClassName.clear();
    MarkAssetDirty();
}

void Map::SetAmbientLightColor(const CoreMath::Vector3 &color)
{

    mAmbientLightSettings.mColor = color;
}

void Map::SetAmbientLightIntensity(float intensity)
{

    mAmbientLightSettings.mIntensity = intensity;
}

void Map::SetAmbientEnableFlag(bool flag)
{

    mAmbientLightSettings.mEnable = flag;
}

const Core::AmbientLightSettings &Map::GetAmbientLightSettings() const
{
    return mAmbientLightSettings;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Map::SetSkySphereEnableFlag(bool flag)
{

    mSkySphereSettings.mEnable = flag;
}

void Map::SetSkySphereTintColor(const CoreMath::Vector3 &tintColor)
{

    mSkySphereSettings.mTintColor = tintColor;
}

void Map::SetSkySphereTextureID(CoreAsset::AssetID id)
{
    mSkySphereSettings.mTexID = id;
}

void Map::SetSkySphereIntensity(float intensity)
{
    mSkySphereSettings.mIntensity = intensity;
}

const Core::SkySphereSettings &Map::GetSkySphereSettings() const
{
    return mSkySphereSettings;
    // TODO: 여기에 return 문을 삽입합니다.
}

// void Map::SetPhysicsScene(PhysicsScene *scene)
//{
//
//     mPhysicsContext.mPhysicsScene = scene;
//     if (scene)
//         mPhysicsContext.mPhysicsSceneID = scene->GetID();
//     else
//         mPhysicsContext.mPhysicsSceneID = PhysicsSceneIDInValid;
// }
//
// PhysicsSceneID Map::GetPhysicsSceneID() const
//{
//     return mPhysicsContext.mPhysicsSceneID;
// }
//
// PhysicsScene *Map::GetPhysicsScene() const
//{
//     return mPhysicsContext.mPhysicsScene;
// }

void Map::EntityDeleter(Object *entity)
{
    mEntityDeleter(entity);
}

void Map::DestoryEntitesProgrammatically()
{
    if (mPlayBegun)
    {
        EndPlay();
    }

    mStarted = false;
    mPlayBegun = false;

    for (auto ob : mEntityList)
    {
        if (ob->GetKillState())
            continue;

        ob->Destroy();
    }
    DestroyEngineEntitiesImmediately();

    for (auto ob : mKilledEntityList)
    {
        ob->DestroyDeadComponents();
        Core::ObjectManager::GetInstance()->UnRegister(ob->GetObjectHandle().mObjectPoolIndex);
        mReflectionSystem->DestoryClassInstance(ob);
    }

    mKilledEntityList.clear();
    mEntityIDTable.clear();
    mEntityNameTable.clear();
    mEntityList.clear();

    mCameraComponentList.clear();
    mObjectControllerList.clear();
}

void Map::DestroyEngineEntitiesImmediately()
{
    for (auto object : mEngineEntityList)
    {
        if (object == nullptr)
            continue;

        // dead상태이든 아니든 제거
        object->DestroyImmediatelyForLoad();
        Core::ObjectManager::GetInstance()->UnRegister(object->GetObjectHandle().mObjectPoolIndex);
        mReflectionSystem->DestoryClassInstance(object);
    }
    mEngineEntityList.clear();
}

void Map::DestroyTemporaryEntitiesForPlay()
{

    std::vector<Object *> temporaryObjects;
    for (auto object : mEntityList)
    {
        if (object && !object->GetKillState() && object->HasObjectFlag(Core::EObjectFlag::eTemporary))
            temporaryObjects.push_back(object);
    }

    // 컨트롤러도 EntityList에 들어가있으나, 따로 리스트를 추가로 유지하기에 추가적인 리스트또한 정리한다.
    std::vector<Object *> temporaryControllers;
    for (auto controller : mObjectControllerList)
    {
        if (controller && controller->HasObjectFlag(Core::EObjectFlag::eTemporary))
        {
            temporaryControllers.push_back(controller);
        }
    }

    for (auto controller : temporaryControllers)
    {
        auto it = std::find(mObjectControllerList.begin(), mObjectControllerList.end(), controller);
        if (it != mObjectControllerList.end())
        {
            std::iter_swap(mObjectControllerList.end() - 1, it);
            mObjectControllerList.pop_back();
        }
    }

    for (auto object : temporaryObjects)
    {
        object->Destroy();
    }

    DestroyEntities();
}

void *Map::GetMemory(size_t size)
{

    void *ptr = new char[size];
    return ptr;
}
void Map::ReleaseMemory(void *mem)
{

    delete[] mem;
    return;
}

void Map::RegisterEntity(Object *entity)
{

    mEntityList.push_back(entity);
    mEntityNameTable[entity->GetObjectName()] = entity;
    mEntityIDTable[entity->GetUniqueID()] = entity;
    entity->SetMap(this);

    if (mStarted)
    {
        entity->Start();
    }

    if (mPlayBegun)
    {
        entity->BeginPlay();
    }
}
void Map::SetObjectProperty(Object *object, const std::string &objectName)
{

    Core::ObjectManager *objectManager = Core::ObjectManager::GetInstance();

    object->SetObjectName(objectName.c_str());
    //  object->SetObjectUniqueID(CoreUtility::Utility::MakeUniqueID());
    object->SetObjectPoolHandle(objectManager->Register(object));
}

Object *Map::CreateEmptyEntityForLoad(const std::string &className)
{
    return static_cast<Object *>(mReflectionSystem->CreateClassInstance(className.c_str()));
}

void Map::RegisterRestoredEntity(Object *object)
{
    if (object == nullptr)
        return;

    Core::ObjectManager *objectManager = Core::ObjectManager::GetInstance();

    object->SetObjectPoolHandle(objectManager->Register(object));
    RegisterEntity(object);

    mObjectAddedCallbackSystem.ExecuteCallbacks(object);
}

void Map::RebuildObjectSceneComponentHierarchy()
{
    for (auto ob : mEntityList)
    {
        ob->RebuildSceneComponentHierarchyForLoad();
    }

    /*  for (auto ob : mEntityList)
      {
          for (auto com : ob->GetComponentList())
          {
              if (SceneComponent *sceneCom = dynamic_cast<SceneComponent *>(com))
              {
                  sceneCom->ClearChildSceneComponentsForLoad();
              }
          }
      }

      for (auto ob : mEntityList)
      {
          for (auto com : ob->GetComponentList())
          {

              if (SceneComponent *sceneCom = dynamic_cast<SceneComponent *>(com))
              {

                  auto parentCom = sceneCom->GetParent();

                  if (parentCom)
                  {
                      parentCom->AddChildSceneComponentForLoad(sceneCom);
                  }
              }
          }
      }*/
}

void Map::RebuildObjectHierarchy()
{
    for (auto ob : mEntityList)
    {
        ob->ClearChildObjectListForLoad();
    }

    for (auto ob : mEntityList)
    {
        Object *parent = ob->GetParent();
        if (parent)
        {
            parent->AddChildObjectForLoad(ob);
        }
    }
}

std::string Map::GetAvailableObjectName(const std::string &originName)
{
    std::string name = originName;
    std::unordered_map<std::string, Object *>::iterator it = mEntityNameTable.find(originName);
    if (it != mEntityNameTable.end())
    {
        std::string tempInstanceName;
        int count = 1;
        do
        {

            tempInstanceName = originName;
            tempInstanceName += std::to_string(count++);
        } while (mEntityNameTable.find(tempInstanceName) != mEntityNameTable.end());

        name = tempInstanceName;
    }

    return name;
}

void Map::SerializeGameModeSetting(Arch &arch)
{

    arch << mGameModeSetting.mDefaultPlayerObjectSource;
    arch << mGameModeSetting.mDefaultPlayerObjectClassName;
    arch << mGameModeSetting.mDefaultPlayerObjectPrefabID;
    arch << mGameModeSetting.mDefaultPlayerControllerClassName;
}

void Map::SerializeSceneSetting(Arch &arch)
{

    arch << mAmbientLightSettings.mEnable;
    arch << mAmbientLightSettings.mColor;
    arch << mAmbientLightSettings.mIntensity;

    arch << mSkySphereSettings.mEnable;
    arch << mSkySphereSettings.mIntensity;
    arch << mSkySphereSettings.mRadius;
    arch << mSkySphereSettings.mTintColor;
    arch << mSkySphereSettings.mTexID;
}
