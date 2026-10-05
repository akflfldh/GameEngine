#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <CoreBase/CallbackSystem.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using OnWorldRemovedCallbackSystem = Core::MultiCallbackSystem<>;

class Map;
class Object;
class CameraComponent;
class ObjectController;
class PhysicsBridgeSystem;

using OnMapObjectRemovedCallbackSystem = Core::MultiCallbackSystem<Object *>;
using OnMapChangedCallbackSystem = Core::MultiCallbackSystem<Map *>;

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
    // 맵 유무와 무관하게 플레이 세션을 한 번 시작한다. EndPlay 뒤에는 새 세션을 시작할 수 있다.
    void StartPlay();
    void BeginMap();
    // 실행 모드에 플레이 세션 종료를 전달한다. 현재 맵의 교체만으로는 호출하지 않는다.

    // 현재 맵이 종료되었을때
    void EndMap();

    void EndPlay();
    void Update(float DeltaTime);
    void EndUpdate(float DeltaTime);
    void CleanUp();

    void EndFrame();

    bool SetCurrentMap(const std::string &name);
    bool SetCurrentMap(Map *map);
    Map *GetCurrentMap() const;
    // 입력/카메라의 기준 맵과 렌더링 참여 맵은 구분한다. 맵 전환 시 목록은 새 현재 맵으로 교체된다.
    // 추가 맵은 이 World에 등록되어 있어야 하며, 포함 여부는 오브젝트의 active 상태를 변경하지 않는다.
    bool AddRenderingMap(Map *map);
    void RemoveRenderingMap(Map *map);
    const std::vector<Map *> &GetRenderingMaps() const;
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

    // World 자체의 런타임 식별자다. 프록시 조회에는 GetRenderingMaps()에 포함된 각 Map의 ID를 사용한다.
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

    Core::MouseMode GetMouseMode() const;

    std::vector<Map *> GetMapList() const;
    std::vector<std::string> GetMapNameList() const;

    // 맵전환 요청 받기
    // world는 프레임에 마지막에 안전한게 기존의 map end와 새로운map을 준비한다
    // 새로운 map을 먼저준비하고 나서 기존맵의 end map도가능

    void RequestChangeMap(const std::string &mapName);
    OnMapChangedCallbackSystem mOnMapChangedCallbackSystem;

  private:
    virtual Object *CreateEntity(const char *entityClassName, const char *entityInstanceName);

    void ChangeMapIfRequested();

  private:
    std::unordered_map<std::string, Map *> mMapTable;
    // 등록된 맵 중 렌더링에 참여하는 비소유 참조 목록이다. 현재 맵과 에디터 오버레이 맵을 함께 담을 수 있다.
    // 맵의 수명은 기존 소유자가 책임지며, World 등록 전체 해제 시 이 목록도 비운다.
    std::vector<Map *> mRenderingMaps;
    Map *mCurrentMap;
    Core::IEngineMode *mCurrentEngineMode;
    // 맵 교체로는 초기화하지 않는다. GameInstance와 같은 세션 상태의 중복 생성을 방지한다.
    bool mPlayStarted = false;

    uint32_t mRenderID = 0;
    // world 안에  world안에서 사용할 타이머 mangaer가있는게 좋을듯
    bool mActiveState = true;

    PhysicsBridgeSystem *mPhysicsBridgeSystem;

    bool mRequestMapChange = false;
    std::string mRequestedMapName;
};
template <typename T> inline T *World::CreateEntity(const char *entityInstanceName)
{
    return static_cast<T *>(CreateEntity(T::GetStaticClassName(), entityInstanceName));
}
