#include "World.h"
#include <Core/IEngineMode.h>
#include <Core/Map.h>
#include <Core/PhysicsBridgeSystem.h>
#include <Core/RenderIDManager.h>

World::World()
    : mCurrentMap(nullptr), mCurrentEngineMode(nullptr), mRenderID(Core::RenderIDManager::GetInstance()->AllocID()),
      mPhysicsBridgeSystem(PhysicsBridgeSystem::GetInstance())
{
}

World::~World()
{
    UnRegisterMapAll();

    mOnWorldRemovedCallbackSystem.ExecuteCallbacks();
}

void World::Begin() {}
void World::StartMap()
{

    if (mCurrentMap != nullptr && mCurrentEngineMode != nullptr)
    {
        mCurrentEngineMode->Start(mCurrentMap);
    }
}
void World::Update(float DeltaTime)
{
    if (mCurrentMap != nullptr && mCurrentEngineMode != nullptr)
    {
        mCurrentEngineMode->Update(mCurrentMap, DeltaTime);
        // mCurrentMap->Update(DeltaTime);
    }
}
void World::EndUpdate(float DeltaTime)
{
    if (mCurrentMap != nullptr && mCurrentEngineMode != nullptr)
    {
        mCurrentEngineMode->EndUpdate(mCurrentMap, DeltaTime);
    }
}

void World::CleanUp()
{
    if (mCurrentEngineMode)
        mCurrentEngineMode->CleanUp(mCurrentMap);
}

bool World::SetCurrentMap(const std::string &name)
{

    if (mCurrentMap)
    {
        mCurrentMap->ClearCallbackSystems();
    }

    std::unordered_map<std::string, Map *>::iterator it = mMapTable.find(name);
    if (it == mMapTable.end())
        return false;

    if (mCurrentMap != it->second)
    {
        mCurrentMap = it->second;

        mCurrentMap->mObjectRemovedCallbackSystem.Register(
            [this](Object *object) { mOnMapObjectRemovedCallbackSystem.ExecuteCallbacks(object); });
    }

    return true;
}

bool World::SetCurrentMap(Map *map)
{

    if (map == nullptr)
        return false;

    return SetCurrentMap(map->GetName().c_str());
}

Map *World::GetCurrentMap() const
{
    return mCurrentMap;
}

ObjectController *World::GetCurrentObjectController() const
{

    if (mCurrentEngineMode == nullptr)
        return nullptr;

    return mCurrentEngineMode->GetCurrentObjectController(mCurrentMap);
}

void World::AddPrefabObject(Object *object)
{

    if (mCurrentMap)
    {
        mCurrentMap->AddPrefabObject(object);
    }
}

void World::RemovePrefabObject(Object *object)
{
    if (mCurrentMap)
    {
        mCurrentMap->RemovePrefabObject(object);
    }
}

bool World::Register(Map *map)
{

    if (map == nullptr)
        return false;

    std::pair<std::unordered_map<std::string, Map *>::iterator, bool> ret =
        mMapTable.insert({map->GetName().c_str(), map});

    if (ret.second == false)
        return false;

    map->SetWorld(this);
    map->SetRenderID(mRenderID);

    mPhysicsBridgeSystem->RegisterMap(map);

    return ret.second;
}

void World::OnInputEvent(const Core::InputData &inputData)
{

    // 현재 맵에 전달
    if (mCurrentMap)
    {
        mCurrentMap->OnInputEvent(inputData);
    }
}

CameraComponent *World::GetCurrentCameraCom() const
{
    if (mCurrentEngineMode)
    {
        return mCurrentEngineMode->GetActiveCameraComponent(mCurrentMap);
    }

    return nullptr;
}

void World::SetEngineMode(Core::IEngineMode *mode)
{

    mCurrentEngineMode = mode;
}

Core::IEngineMode *World::GetEngineMode() const
{
    return mCurrentEngineMode;
}
uint32_t World::GetRenderID() const
{
    return mRenderID;
}
void World::SetActiveState(bool flag)
{

    mActiveState = flag;
}
bool World::GetActiveState() const
{

    return mActiveState;
}

void World::UnRegisterMapAll()
{

    for (auto e : mMapTable)
    {
        Map *map = e.second;

        if (map)
        {
            mPhysicsBridgeSystem->UnRegisterMap(map);
        }
    }

    mMapTable.clear();
    mCurrentMap = nullptr;
}

void World::SetPause()
{

    if (mCurrentEngineMode)
    {
        mCurrentEngineMode->SetPause();
    }
}

void World::ReleasePause()
{

    if (mCurrentEngineMode)
    {
        mCurrentEngineMode->ReleasePause();
    }
}

CoreMath::Vector3 World::GetAmbientLight() const
{

    if (mCurrentMap)
    {
        const auto &ambientLightSettings = mCurrentMap->GetAmbientLightSettings();
        if (ambientLightSettings.mEnable == false)
            return {0, 0, 0};
        else
        {
            return ambientLightSettings.mColor * ambientLightSettings.mIntensity;
        }
    }

    return {0, 0, 0};
}

PhysicsBridgeSystem *World::GetPhysicsBridgeSystem() const
{
    return mPhysicsBridgeSystem;
}

Object *World::CreateEntity(const char *entityClassName, const char *entityInstanceName)
{

    if (mCurrentMap == nullptr)
        return nullptr;

    return mCurrentMap->CreateEntity(entityClassName, entityInstanceName);
}
