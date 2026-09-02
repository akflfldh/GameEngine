#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <CoreBase/CallbackSystem.h>
#include <memory>
#include <string>
#include <unordered_map>

using OnWorldRemovedCallbackSystem = Core::MultiCallbackSystem<>;

class Map;
class Object;
class CameraComponent;
class ObjectController;
class PhysicsBridgeSystem;

using OnMapObjectRemovedCallbackSystem = Core::MultiCallbackSystem<Object *>;

namespace Core
{
class IEngineMode;
}

class CORE_API_LIB World
{
  public:
    World();
    ~World();

    void Begin();
    void StartMap();
    void Update(float DeltaTime);
    void EndUpdate(float DeltaTime);
    void CleanUp();

    bool SetCurrentMap(const std::string &name);
    bool SetCurrentMap(Map *map);
    Map *GetCurrentMap() const;
    ObjectController *GetCurrentObjectController() const;

    // engine 용
    void AddPrefabObject(Object *object);
    void RemovePrefabObject(Object *object);

    bool Register(Map *map);

    template <typename T> T *CreateEntity(const char *entityInstanceName = "");

    // 입력발생시 호출됨
    void OnInputEvent(const Core::InputData &inputData);

    CameraComponent *GetCurrentCameraCom() const;

    void SetEngineMode(Core::IEngineMode *mode);
    Core::IEngineMode *GetEngineMode() const;

    uint32_t GetRenderID() const;
    void SetActiveState(bool flag);
    bool GetActiveState() const;

    OnWorldRemovedCallbackSystem mOnWorldRemovedCallbackSystem;
    OnMapObjectRemovedCallbackSystem mOnMapObjectRemovedCallbackSystem;

    void UnRegisterMapAll();

    void SetPause();
    void ReleasePause();

    CoreMath::Vector3 GetAmbientLight() const;

    PhysicsBridgeSystem *GetPhysicsBridgeSystem() const;

  private:
    virtual Object *CreateEntity(const char *entityClassName, const char *entityInstanceName);

  private:
    std::unordered_map<std::string, Map *> mMapTable;
    Map *mCurrentMap;
    Core::IEngineMode *mCurrentEngineMode;

    uint32_t mRenderID = 0;
    // world 안에  world안에서 사용할 타이머 mangaer가있는게 좋을듯
    bool mActiveState = true;

    PhysicsBridgeSystem *mPhysicsBridgeSystem;
};
template <typename T> inline T *World::CreateEntity(const char *entityInstanceName)
{
    return static_cast<T *>(CreateEntity(T::GetStaticClassName(), entityInstanceName));
}
