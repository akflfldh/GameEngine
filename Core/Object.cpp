#include "Object.h"
#include <Core/Component.h>
#include <Core/IHittable.h>
#include <Core/Map.h>
#include <Core/SceneComponent.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <Utility/Utility.h>
Object::Object() : mKillState(false), mMap(nullptr), mParentObject(nullptr), mObjectFlag(Core::EObjectFlag::eNone)
{
    // 향후 수정
    // mImpl = std::make_unique<ObjectImpl>(this);
}

Object::~Object() {}

void Object::BeginPlay()
{
    if (mPlayBegun)
        return;

    mPlayBegun = true;

    OnBeginPlay();

    for (auto com : mComList)
    {
        if (com && !com->GetDeadState())
            com->BeginPlay();
    }
}

void Object::Update(float deltaTime)
{

    Tick(deltaTime);

    for (auto com : mComList)
    {
        if (com && !com->GetDeadState())
            com->Update(deltaTime);
    }
}

void Object::EndPlay()
{
    if (!mPlayBegun)
        return;

    mPlayBegun = false;

    //  일반적으로 종료는 하위 요소를 먼저 정리한 뒤 object가 마무리하는 구조가 더 안전할 때가
    // 많습니다.
    for (auto com : mComList)
    {
        if (com && !com->GetDeadState())
            com->EndPlay();
    }

    OnEndPlay();

    // 단, object의 OnEndPlay()에서 component를 참조해 정리해야 한다면 현재 순서도 가능합니다. 중요한 건 정책을 정해두는
    // 것입니다.
}

void Object::EndUpdate(float deltaTime)
{

    EndTick(deltaTime);

    for (auto com : mComList)
    {
        if (com && !com->GetDeadState())
            com->EndUpdate(deltaTime);
    }
}

void Object::Start()
{

    if (mStarted)
        return;

    mStarted = true;

    OnBegin();

    for (auto com : mComList)
    {
        com->Start();
    }
}

const std::string &Object::GetObjectName() const
{
    return mObjectName;
}

CoreUtility::UniqueID Object::GetUniqueID() const
{
    return mObjectUniqueID;
}

bool Object::GetKillState() const
{
    return mKillState;
}

const Core::ObjectHandle &Object::GetObjectHandle() const
{
    return mObjectPoolHandle;
}

Map *Object::GetMap() const
{

    return mMap;
}

World *Object::GetWorld() const
{
    return mMap->GetWorld();
}

Component *Object::CreateDefaultComponent(const char *comClassName, const char *comInstanceName)
{
    Component *com = CreateComponentCommon(comClassName, comInstanceName);
    if (com)
    {
        com->mComponentCreationMethod = Core::EComponentCreationMethod::eNative;
    }

    return com;
}

Component *Object::CreateComponent(const char *comClassName, const char *comInstanceName)
{
    Component *com = CreateComponentCommon(comClassName, comInstanceName);
    if (com)
    {
        com->mComponentCreationMethod = Core::EComponentCreationMethod::eInstancedAdded;
    }

    return com;
}

void Object::Destroy()
{
    if (mKillState)
        return;

    auto map = GetMap();

    std::queue<Object *> queue;
    queue.push(this);
    while (!queue.empty())
    {
        auto ob = queue.front();
        queue.pop();

        ob->mKillState = true;

        for (auto com : ob->GetComponentList())
        {
            com->Destory();
        }

        for (auto child : ob->GetChildObjectList())
        {
            if (!child->GetKillState())
                queue.push(child);
        }

        map->RequestDestoryEntity(ob);
    }
    // (선택) 부모로부터 나 자신을 떼어내는 로직(SetParent(nullptr))도 시작 전에 필요함.

    // 부모와의 관계를 끊는다.
    SetParent(nullptr);

    mHittableComList.clear();
}

void Object::RequestDestroyComponent(Component *com)
{

    if (com == nullptr)
        return;

    MarkComponentTreeDirty();
    mDeadComList.push_back(com);
}

void Object::DestroyDeadComponents()
{
    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    for (auto com : mDeadComList)
    {
        if (com == nullptr)
            continue;

        mOnComponentRemovedCallbackSystem.ExecuteCallbacks(com);

        auto it = std::find(mComList.begin(), mComList.end(), com);
        if (it != mComList.end())
        {
            mComList.erase(it);
        }

        if (Core::IHittable *hittableCom = dynamic_cast<Core::IHittable *>(com))
        {
            auto it = std::find(mHittableComList.begin(), mHittableComList.end(), hittableCom);
            if (it != mHittableComList.end())
            {
                mHittableComList.erase(it);
            }
        }

        mComNameTable.erase(com->GetInstanceName());
        mComIDTable.erase(com->GetUniqueID());

        reflectionSystem->DestoryClassInstance(com);
    }

    if (auto map = GetMap())
    {
        map->MarkAssetDirty();
    }

    mDeadComList.clear();
}

bool Object::RayHit(const CoreMath::Ray &ray, Core::HitResult &oHitResult)
{

    bool bHit = false;
    oHitResult.mDistance = FLT_MAX;

    for (auto com : mHittableComList)
    {
        Core::HitResult hitResultTemp;

        if (com->Raycast(ray, hitResultTemp))
        {
            if (hitResultTemp.mDistance < oHitResult.mDistance)
            {
                oHitResult = hitResultTemp;
                bHit = true;
            }
        }
    }

    if (bHit)
    {
        oHitResult.mHitObject = this;
        return true;
    }
    return false;
}

Component *Object::GetComponent(const char *comInstanceName)
{
    std::unordered_map<std::string, Component *>::const_iterator it = mComNameTable.find(comInstanceName);
    if (it == mComNameTable.cend())
    {

        // log
        return nullptr;
    }

    return it->second;
}

Component *Object::GetComponentFromClassName(const char *componentName)
{
    for (auto com : mComList)
    {

        if (com != nullptr && std::strcmp(com->GetRunTimeClassName(), componentName) == 0)
        {
            return com;
        }
    }

    return nullptr;
}

void Object::OnAddedToMap()
{

    // 컴포넌트들의 OnAddedOwnerObjectToMap을 호출해준다.
    for (Component *com : mComList)
    {
        com->OnOwnerObjectAddedToMap();
    }
}
void Object::OnRemovedFromMap()
{

    for (Component *com : mComList)
    {
        com->OnOwnerObjectRemovedFromMap();
    }
}

void Object::OnBegin() {}

void Object::OnBeginPlay() {}

void Object::Tick(float deltaTime) {}

void Object::OnEndPlay() {}

void Object::EndTick(float deltaTime) {}

void Object::UpdateActiveInHierarchy()
{

    bool newState = mActiveStateSelf;

    if (GetParent())
        newState &= GetParent()->GetActive();

    if (mActiveStateHierachy != newState)
    {
        mActiveStateHierachy = newState;
        for (auto com : mComList)
        {
            com->OnActiveStateChanged(mActiveStateHierachy);
        }

        // 2. 자식들에게 상태 업데이트 전파 (자식의 로컬 상태는 건드리지 않음!)
        for (auto child : mChildObjectLIst)
            child->UpdateActiveInHierarchy();
    }
}

void Object::SetObjectName(const char *objectName)
{

    mObjectName = objectName;
}

void Object::SetObjectUniqueID(const CoreUtility::UniqueID &id)
{

    mObjectUniqueID = id;
}

void Object::SetObjectPoolHandle(const Core::ObjectHandle &handle)
{

    mObjectPoolHandle = handle;
}

void Object::SetMap(Map *map)
{

    if (mMap == map)
        return;

    if (mMap != nullptr)
    {
        OnRemovedFromMap();
    }

    mMap = map;

    if (mMap)
        OnAddedToMap();
}

const std::vector<Component *> &Object::GetComponentList() const
{
    return mComList;
    // TODO: 여기에 return 문을 삽입합니다.
}
void Object::SetActive(bool state)
{
    if (mActiveStateSelf == state)
        return;

    mActiveStateSelf = state;
    bool newState = mActiveStateSelf;
    if (GetParent())
    {
        newState = mActiveStateSelf && GetParent()->GetActive();
    }

    if (mParentObject && (mParentObject->GetActive() == false))
    {
        // 조상의 상태가 false 이기에 최종상태를 그대로 유지
        return;
    }

    // 조상의상태가 true인데 self상태가 변경되었기에 최종상태업데이트 + 전파
    //  최종상태 업데이트
    UpdateActiveInHierarchy();
}
bool Object::GetActive() const
{

    return mActiveStateHierachy;
}

Object *Object::GetParent() const
{

    return mParentObject;
}

void Object::SetParent(Object *parent)
{
    // 부모동일
    if (parent == mParentObject)
    {
        return;
    }
    // 싸이클 방지
    if (parent)
    {

        if (parent == this || parent->IsChildOf(this))
        {
            return;
        }
    }

    if (mParentObject)
    {
        auto &childList = mParentObject->mChildObjectLIst;
        auto it = std::find(childList.begin(), childList.end(), this);
        if (it != childList.end())
        {
            childList.erase(it);

            // 부모와의 연결이 끊어짐에대해 호출되는 가상메서드
            OnDisconnectedParent();
            // parent->OnDisconnectedChildLink();
        }
    }

    mParentObject = parent;
    if (mParentObject)
    {
        mParentObject->mChildObjectLIst.push_back(this);
        // 새로운 부모와 연결에대해 호출되는 가상메서드
        OnConnectedNewParent(mParentObject);
        // parent->OnConnectedChildLink();
    }

    UpdateActiveInHierarchy();
}

const std::vector<Object *> &Object::GetChildObjectList() const
{
    return mChildObjectLIst;
    // TODO: 여기에 return 문을 삽입합니다.
}

bool Object::IsChildOf(Object *targetObject) const
{

    Object *current = mParentObject;
    while (current)
    {
        if (current == targetObject)
        {
            return true;
        }

        current = current->GetParent();
    }

    return false;
}

void Object::OnConnectedNewParent(Object *parent) {}

void Object::OnDisconnectedParent() {}

void Object::SetObjectFlag(Core::EObjectFlag flag)
{

    mObjectFlag = flag;
}

void Object::AddObjectFlag(Core::EObjectFlag flag)
{
    mObjectFlag = static_cast<Core::EObjectFlag>(static_cast<uint32_t>(mObjectFlag) | static_cast<uint32_t>(flag));
}

void Object::RemoveObjectFlag(Core::EObjectFlag flag)
{
    mObjectFlag = static_cast<Core::EObjectFlag>(static_cast<uint32_t>(mObjectFlag) & ~static_cast<uint32_t>(flag));
}

Core::EObjectFlag Object::GetObjectFlag() const
{
    return mObjectFlag;
}

bool Object::HasObjectFlag(Core::EObjectFlag flag) const
{
    return (uint32_t)flag & (uint32_t)mObjectFlag;
}

void Object::FlushPropertyDirty()
{

    for (auto com : mComList)
    {
        com->FlushPropertyDirty();
    }
}

void Object::Serialize(Arch &arch)
{
    auto reflectSystem = Quad::ReflectionSystem::GetInstance();

    arch << QUAD_SERIALIZEBUFFER(mObjectUniqueID.mUniqueID, 16);

    arch << mObjectName;

    arch << mObjectFlag;

    bool bHasParent = true;
    if (arch.GetLoadingFlag())
    {
        arch << bHasParent;

        if (bHasParent)
        {
            CoreUtility::UniqueID parentID;
            arch << QUAD_SERIALIZEBUFFER(parentID.mUniqueID, 16);

            reflectSystem->RequestObjectPointerFix(parentID, &mParentObject);
        }
    }
    else
    {
        bHasParent = mParentObject ? true : false;
        arch << bHasParent;

        if (mParentObject)
        {
            CoreUtility::UniqueID parentID = mParentObject->GetUniqueID();
            arch << QUAD_SERIALIZEBUFFER(parentID.mUniqueID, 16);
        }
    }
}

void Object::SerializeComponents(Arch &arch)
{
    Quad::ReflectionSystem *reflectSystem = Quad::ReflectionSystem::GetInstance();

    size_t comNum = 0;
    std::string tableStr;
    if (arch.GetLoadingFlag())
    {
        size_t comNum = 0;
        arch << comNum;

        for (size_t i = 0; i < comNum; ++i)
        {
            std::string comClassName;
            std::string comInstanceName;
            Core::EComponentCreationMethod creationMethod;

            arch << comClassName;
            arch << comInstanceName;
            arch << creationMethod;

            arch.StartTable(tableStr);
            Component *com = nullptr;

            if (creationMethod == Core::EComponentCreationMethod::eInstancedAdded)
            {
                com = CreateComponent(comClassName.c_str(), comInstanceName.c_str());
                if (com)
                {
                    if (std::strcmp(com->GetRunTimeClassName(), comClassName.c_str()) != 0)
                    {
                        com = nullptr;
                    }
                }
            }
            else
            {
                com = GetComponent(comInstanceName.c_str());
            }

            if (com)
            {
                BaseClass *b = static_cast<BaseClass *>(com);
                reflectSystem->SerializeBaseClass(arch, b);
                if (com != nullptr && creationMethod == Core::EComponentCreationMethod::eInstancedAdded)
                    com->SetComponentFlag(Core::EComponentFlag::eEngineAdded);

                com->SetInstanceName(comInstanceName.c_str());
            }
            else
            {
                // log
            }

            arch.EndTable();
        }
    }
    else
    {
        std::vector<Component *> comList;

        for (auto com : mComList)
        {
            if (com == nullptr || com->GetDeadState())
                continue;

            comList.push_back(com);
        }

        size_t comNum = comList.size();
        arch << comNum;

        for (auto com : comList)
        {
            if (com == nullptr || com->GetDeadState())
                continue;

            std::string comClassName = com->GetRunTimeClassName();
            arch << comClassName;

            std::string comInstanceName = com->GetInstanceName();
            arch << comInstanceName;

            Core::EComponentCreationMethod creationMethod = com->GetComponentCreationMethod();
            arch << creationMethod;

            arch.StartTable(tableStr);

            BaseClass *b = static_cast<BaseClass *>(com);
            reflectSystem->SerializeBaseClass(arch, b);

            arch.EndTable();
        }
    }
}

void Object::UpdateComponentID(CoreUtility::UniqueID oldID, CoreUtility::UniqueID newID, Component *com)
{

    auto it = mComIDTable.find(oldID);

    if ((it != mComIDTable.end()) && (it->second == com))
    {
        mComIDTable.erase(it);
    }

    mComIDTable[newID] = com;
    com->mComponentUniqueID = newID;
}

void Object::SetPrefabID(CoreAsset::AssetID id)
{

    mPrefabInstanceContext.mPrefab = id;
}
CoreAsset::AssetID Object::GetPrefabID() const
{
    return mPrefabInstanceContext.mPrefab;
}

void Object::OverrideProperty(const std::string &property)
{

    mPrefabInstanceContext.mOverriddenProperties.insert(property);
}

bool Object::IsOverridenProperty(const std::string &property) const
{

    auto it = std::find(mPrefabInstanceContext.mOverriddenProperties.begin(),
                        mPrefabInstanceContext.mOverriddenProperties.end(), property);
    return it != mPrefabInstanceContext.mOverriddenProperties.end() ? true : false;
}

void Object::ClearChildObjectListForLoad()
{

    mChildObjectLIst.clear();
}
void Object::AddChildObjectForLoad(Object *object)
{
    if (object)
        mChildObjectLIst.push_back(object);
}

void Object::DestroyImmediatelyForLoad()
{
    mParentObject = nullptr;
    for (Object *child : mChildObjectLIst)
    {
        if (child)
        {
            child->mParentObject = nullptr;
        }
    }

    mChildObjectLIst.clear();

    DestroyComponentsImmediatelyForLoad();

    mHittableComList.clear();
    mComNameTable.clear();
    mComIDTable.clear();
    mDeadComList.clear();

    mMap = nullptr;
    mKillState = true;
}

void Object::DestroyComponentsImmediatelyForLoad()
{

    auto reflectionSystem = Quad::ReflectionSystem::GetInstance();

    for (auto com : mComList)
    {

        // dead 상태이든 아니든 모두 제거
        reflectionSystem->DestoryClassInstance(com);
    }

    mDeadComList.clear();
    mComList.clear();
    mComIDTable.clear();
    mComNameTable.clear();
}

std::string Object::MakeUniqueComponentName(const char *desiredName) const
{

    if (mComNameTable.find(desiredName) == mComNameTable.end())
        return desiredName;

    std::string originName = desiredName;
    uint32_t index = 1;

    std::string tempName = originName;

    while (1)
    {
        tempName = originName + "_" + std::to_string(index);

        if (mComNameTable.find(tempName) == mComNameTable.end())
        {
            return tempName;
        }
        index++;
    }
}

void Object::RefreshComponentIDTable()
{

    mComIDTable.clear();

    for (auto com : mComList)
    {
        mComIDTable[com->GetUniqueID()] = com;
    }
}

void Object::MarkComponentTreeDirty()
{

    mComponentTreeDirty = true;
}

bool Object::IsComponentTreeDirty() const
{
    return mComponentTreeDirty;
}

void Object::ClearComponentTreeDirty()
{

    mComponentTreeDirty = false;
}

void Object::RebuildSceneComponentHierarchyForLoad()
{
    std::vector<SceneComponent *> sceneComList;
    for (auto com : mComList)
    {
        SceneComponent *sceneCom = dynamic_cast<SceneComponent *>(com);

        if (sceneCom == nullptr)
            continue;

        // 일단 자식 씬컴포넌트 리스트를 비운다.
        sceneCom->ClearChildSceneComponentsForLoad();

        sceneComList.push_back(sceneCom);
    }

    for (auto com : sceneComList)
    {

        SceneComponent *parentCom = com->GetParent();

        if (parentCom == nullptr)
            continue;

        parentCom->AddChildSceneComponentForLoad(com);
    }

    MarkComponentTreeDirty();
}

Component *Object::CreateComponentCommon(const char *comClassName, const char *comInstanceName)
{

    std::string uniqueInstanceName = MakeUniqueComponentName(comInstanceName);
    // reflect system 을 통해서 com 생성

    Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();

    Component *com = (Component *)(reflectionSystem->CreateClassInstance(comClassName));
    if (com == nullptr)
    {
        return nullptr;
    }
    // 이름부여
    com->SetInstanceName(uniqueInstanceName.c_str());
    com->SetOwnerObject(this);
    com->SetComponentUniqueID(CoreUtility::Utility::MakeUniqueID());

    // 테이블 등록
    mComNameTable[uniqueInstanceName] = com;
    mComIDTable[com->GetUniqueID()] = com;
    mComList.push_back(com);

    if (Core::IHittable *hittableCom = dynamic_cast<Core::IHittable *>(com))
    {
        mHittableComList.push_back(hittableCom);
    }

    if (GetMap() != nullptr)
    {
        com->OnOwnerObjectAddedToMap();
        GetMap()->MarkAssetDirty();
    }

    if (mStarted)
        com->Start();

    if (mPlayBegun)
        com->BeginPlay();

    MarkComponentTreeDirty();
    mOnComponentAddedCallbackSystem.ExecuteCallbacks(com);

    return com;
}