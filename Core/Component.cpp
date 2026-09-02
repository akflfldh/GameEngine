#include "Component.h"
#include <Core/Object.h>
#include <ReflectSystem/ReflectionClassInfo.h>
#include <ReflectSystem/ReflectionSystem.h>

Component::Component() : mOwnerObject(nullptr), mDeadState(false), mComponentFlag(Core::EComponentFlag::eNone) {}

Component::~Component() {}

Object *Component::GetOwnerObject() const
{
    return mOwnerObject;
}

Map *Component::GetMap() const
{

    Object *ownerObject = GetOwnerObject();
    if (ownerObject)
    {

        return ownerObject->GetMap();
    }
    return nullptr;
}

void Component::Start()
{
    if (mStarted)
        return;

    mStarted = true;

    OnBegin();
}

void Component::BeginPlay()
{
    if (mPlayBegun)
        return;

    mPlayBegun = true;

    OnBeginPlay();
}

void Component::Update(float deltaTime)
{

    Tick(deltaTime);
}

void Component::EndPlay()
{
    if (!mPlayBegun)
        return;

    mPlayBegun = false;

    OnEndPlay();
}

void Component::EndUpdate(float deltaTime)
{

    EndTick(deltaTime);
    ClearPropertyDirty();
}

void Component::OnSetUpOwnerObject() {}

void Component::OnOwnerObjectAddedToMap() {}

void Component::OnOwnerObjectRemovedFromMap() {}

void Component::SetOwnerObject(Object *ownerObject)
{
    mOwnerObject = ownerObject;

    OnSetUpOwnerObject();
}
void Component::OnBegin() {}

void Component::OnBeginPlay() {}

void Component::Tick(float deltaTime) {}

void Component::OnEndPlay() {}

void Component::EndTick(float deltaTime) {}
void Component::OnDestoryRequested() {}

const char *Component::GetInstanceName() const
{
    return mInstanceName.c_str();
}

void Component::SetInstanceName(const char *name)
{

    mInstanceName = name;
}

void Component::SetComponentUniqueID(const CoreUtility::UniqueID &id)
{

    mComponentUniqueID = id;
}

bool Component::GetDeadState() const
{

    return mDeadState;
}

void Component::Destory()
{

    if (mDeadState)
        return;

    mDeadState = true;
    mOnDestoryRequestedCallbackSystem.ExecuteCallbacks(this);
    OnDestoryRequested();

    if (mOwnerObject)
        mOwnerObject->RequestDestroyComponent(this);
}

void Component::OnActiveStateChanged(bool state) {}

Core::EComponentFlag Component::GetComponentFlag() const
{
    return mComponentFlag;
}

void Component::SetComponentFlag(Core ::EComponentFlag flag)
{

    mComponentFlag = flag;
}

bool Component::HasComponentFlag(Core::EComponentFlag flag) const
{
    return (uint32_t)mComponentFlag & (uint32_t)flag;
}

const CoreUtility::UniqueID &Component::GetUniqueID() const
{
    return mComponentUniqueID;
}

void Component::Serialize(Arch &arch)
{

    // arch << mInstanceName;
    CoreUtility::UniqueID id = mComponentUniqueID;

    arch << QUAD_SERIALIZEBUFFER(id.mUniqueID, 16);
    arch << mComponentFlag;

    arch << mPrefabComponentInstanceContext.mPrefabID;
    arch << mPrefabComponentInstanceContext.mPrefabComponentKey;
    arch << mPrefabComponentInstanceContext.mIsPrefabInherited;

    if (arch.GetLoadingFlag())
        GetOwnerObject()->UpdateComponentID(mComponentUniqueID, id, this);
}

void Component::OverrideProperty(const std::string &property)
{
    mOverridenProperties.insert(property);
}
bool Component::IsOverridenProperty(const std::string &property) const
{

    auto it = std::find(mOverridenProperties.begin(), mOverridenProperties.end(), property);
    return it != mOverridenProperties.end() ? true : false;
}

// void Core::Component::MarkPropertyDirty()
//{
//
//     mPropertyDirty = true;
// }
//
// bool Core::Component::GetPropertyDirty() const
//{
//
//     return mPropertyDirty;
// }
//
// void Core::Component::ClearPropertyDirty()
//{
//
//     mPropertyDirty = false;
// }

void Component::SetPrefabInheritedComponent(CoreAsset::AssetID prefabID, const std::string &componentKey)
{

    mPrefabComponentInstanceContext.mPrefabID = prefabID;
    mPrefabComponentInstanceContext.mIsPrefabInherited = true;
    mPrefabComponentInstanceContext.mPrefabComponentKey = componentKey;
}
bool Component::IsPrefabInheritedComponent(CoreAsset::AssetID prefabID) const
{

    if (prefabID == NoneAssetID)
        return false;

    if (prefabID != mPrefabComponentInstanceContext.mPrefabID)
        return false;

    return true;
}

const std::string &Component::GetPrefabComponenetKey() const
{

    return mPrefabComponentInstanceContext.mPrefabComponentKey;
    // TODO: 여기에 return 문을 삽입합니다.
}

void Component::SyncPrefabComponentFrom(Component *prefabComponent) {}

Core::EComponentCreationMethod Component::GetComponentCreationMethod() const
{

    return mComponentCreationMethod;
}