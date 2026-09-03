#pragma once

#include <Core/CallbackSystemDef.h>
#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <CoreAsset/Asset.h>
#include <CoreBase/CallbackSystem.h>
#include <IHittable.h>
#include <Physics/PhysicsType.h>
#include <ReflectSystem/ReflectionMacro.h>
#include <Utility/UniqueID.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Quad
{
struct RawInputData;
}

class Object;
class World;

class CameraComponent;
class ObjectController;
class LightComponent;
class PlayerStart;

using ObjectAddedCallbackSystem = Core::MultiCallbackSystem<Object *>;
using ObjectRemovedCallbackSystem = Core::MultiCallbackSystem<Object *>;
using OnCameraComponentChangedCallbackSystem = Core::MultiCallbackSystem<CameraComponent *>;
using OnLightComponentChangedCallbackSystem = Core::MultiCallbackSystem<LightComponent *>;
using OnPlayerStartChangedCallbackSystem = Core::MultiCallbackSystem<PlayerStart *>;

struct MapPhysicsContext
{
    PhysicsSceneID mPhysicsSceneID = PhysicsSceneIDInValid;
    PhysicsScene *mPhysicsScene = nullptr;
};

class CORE_API_LIB Map : public CoreAsset::Asset

{
  public:
    Map();
    virtual ~Map();

    virtual void Start();

    virtual void BeginPlay();
    virtual void Update(float DeltaTime);
    virtual void EndPlay();
    virtual void EndUpdate(float DeltaTime);
    void CleanUp();

    // 오브젝트들의 property갱신
    void FlushPropertyDirty();

    // engine 용
    void AddPrefabObject(Object *object);
    void RemovePrefabObject(Object *object);

    // 생성된 object를 맵에 등록하는 메서드 ( 주로 프리팹 인스턴스 용 )
    void AddPrefabInstanceObject(Object *object);
    Object *InstantiatePrefab(CoreAsset::AssetID prefabID, const char *instanceName);
    Object *DuplicateEntity(Object *sourceObject);

    template <typename T> T *CreateEntity(const char *entityInstanceName);
    virtual Object *CreateEntity(const char *entityClassName, const char *entityInstanceName);

    template <typename T> T *CreateEngineEntity(const char *entityInstanceName);
    Object *CreateEngineEntity(const char *entityClassName, const char *entityInstanceName);

    Object *SpawnObject(const Core::SpawnRequestContext &spawnRequestContext);

    void RequestDestoryEntity(Object *entity);

    virtual Object *GetEntity(CoreUtility::UniqueID id) const;

    virtual Object *GetEntity(const char *entityInstanceName) const;

    // 엔진의 다른시스템 사용위주
    //   std::shared_ptr<Object> GetEntitySharedPtr(CoreUtility::UniqueID id) const;

    // 일단 사용고민
    void RegisterEntityDeleter(const std::function<void(Object *)> &deleter);

    virtual void Serialize(Arch &arch);

    void SerilaizeRawData(Arch &arch);

    void OnInputEvent(const Core::InputData &inputData);

#pragma region Variable
    Core::MultinputCallbackSystem mMultiInputCallbackSystem;

#pragma endregion

    const std::vector<Object *> &GetEntityList() const;

    ObjectAddedCallbackSystem mObjectAddedCallbackSystem;
    ObjectRemovedCallbackSystem mObjectRemovedCallbackSystem;

    OnCameraComponentChangedCallbackSystem mOnCameraComponentAddedCallbackSystem;
    OnCameraComponentChangedCallbackSystem mOnCameraComponentRemovedCallbackSystem;

    OnLightComponentChangedCallbackSystem mOnLightComponentAddeddCallbackSystem;
    OnLightComponentChangedCallbackSystem mOnLightComponentRemovedCallbackSystem;

    OnPlayerStartChangedCallbackSystem mOnPlayerStartAddedCallbackSystem;
    OnPlayerStartChangedCallbackSystem mOnPlayerStartRemovedCallbackSystem;

    void RegisterCameraComponoent(CameraComponent *com);
    void UnRegisterCameraComponent(CameraComponent *com);
    CameraComponent *GetCameraComponent(int index = 0);
    CameraComponent *GetActiveCameraComponent() const;
    const std::vector<CameraComponent *> &GetCameraComList() const;

    bool SetActiveCameraIndex(int index);
    bool SetActiveCamera(CameraComponent *cameraCom);

    void RegisterLightComponent(LightComponent *com);
    void UnRegisterLightComponent(LightComponent *com);
    const std::vector<LightComponent *> &GetLightComponentList() const;

    void RegisterPlayerStart(PlayerStart *playerStart);
    void UnRegisterPlayerStart(PlayerStart *playerStart);
    const std::vector<PlayerStart *> &GetPlayerStartList() const;

    void RegisterObjectController(ObjectController *controller);
    ObjectController *GetCurrentObjectController(int index = 0) const;
    size_t GetObjectControllerNum() const;

    void RegisterEngineEntity(Object *object);

    const std::vector<Object *> &GetEngineObjectList() const;

    // 죽은 엔티티들을 완전히 처리한다.
    void DestroyEntities();

    bool RayHit(const CoreMath::Ray &ray, Core::HitResult &oHitResult) const;

    void SetRenderID(uint32_t id);
    uint32_t GetRenderID() const;

    void SetWorld(World *world);
    World *GetWorld() const;

    void SetAssetDirtyActive(bool flag);
    void MarkAssetDirty();

    void ClearEntitiesForLoad();

    void ClearCallbackSystems();

    const Core::GameModeSetting &GetGameModeSetting() const;
    void SetGameModeSettings(const Core::GameModeSetting &settings);

    void SetDefaultPlayerObjectClass(const std::string &className);
    void SetDefaultPlayerObjectPrefab(CoreAsset::AssetID prefabID);
    void SetDefaultPlayerControllerClass(const std::string &className);
    void ClearDefaultPlayerObject();
    void ClearDefaultPlayerController();

#pragma region ambient settings

    void SetAmbientLightColor(const CoreMath::Vector3 &color);
    void SetAmbientLightIntensity(float intensity);
    void SetAmbientEnableFlag(bool flag);

    const Core::AmbientLightSettings &GetAmbientLightSettings() const;

#pragma endregion

#pragma region skysphere settings

    void SetSkySphereEnableFlag(bool flag);
    void SetSkySphereTintColor(const CoreMath::Vector3 &tintColor);
    void SetSkySphereTextureID(CoreAsset::AssetID id);
    void SetSkySphereIntensity(float intensity);

    const Core::SkySphereSettings &GetSkySphereSettings() const;

#pragma endregion

#pragma region Physics

    // void SetPhysicsScene(PhysicsScene *scene);
    //// void SetPhysicsScene(PhysicsSceneID sceneID);
    // PhysicsSceneID GetPhysicsSceneID() const;
    // PhysicsScene *GetPhysicsScene() const;

#pragma endregion

  protected:
    // virtual Entity *CreateEntity(const char *entityClassName, const char *entityInstanceName);

    // shared_ptr에 부착될 delete callback

    // 일단 좀더 고민
    void EntityDeleter(Object *entity);

    // engine entity 포함
    void DestoryEntitesProgrammatically();

    void DestroyEngineEntitiesImmediately();

    // EndPlay에 호출되어 일시적인 엔티티들을 제거한다.
    void DestroyTemporaryEntitiesForPlay();

  private:
    void *GetMemory(size_t size);
    void ReleaseMemory(void *mem);

    void RegisterEntity(Object *entity);
    void SetObjectProperty(Object *entity, const std::string &objectName);

    Object *CreateEmptyEntityForLoad(const std::string &className);
    void RegisterRestoredEntity(Object *object);

    void RebuildObjectSceneComponentHierarchy();

    void RebuildObjectHierarchy();

    std::string GetAvailableObjectName(const std::string &originName);

    void SerializeGameModeSetting(Arch &arch);
    void SerializeSceneSetting(Arch &arch);

  private:
    std::vector<Object *> mEntityList;
    std::vector<Object *> mEngineEntityList;
    std::unordered_map<std::string, Object *> mEntityNameTable;
    std::unordered_map<CoreUtility::UniqueID, Object *> mEntityIDTable;

    std::vector<Object *> mKilledEntityList;

    Quad::ReflectionSystem *mReflectionSystem;

    // 일단 사용은 후에 결정
    std::function<void(Object *)> mEntityDeleter;

    std::vector<PlayerStart *> mPlayerStartList;

    // 카메라 컴포넌트 getter 메서드 필요
    std::vector<CameraComponent *> mCameraComponentList;

    std::vector<LightComponent *> mLightComponentList;

    /*
    *ObjectControllerList
     -> serialize 대상 아님
     -> runtime session 중 등록되는 controller 목록
     -> GameMode가 play 시작 시 채움
     -> EndPlay/cleanup 때 비워짐
    */
    std::vector<ObjectController *> mObjectControllerList;
    bool mDirty;

    uint32_t mRenderID = 0;

    World *mWorld = nullptr;

    int mAcvieCameraIndex = 0;

    // assetDirty의 활성화여부 ( 에디터에서의 플레이맵같은곳에서는 false로 사용한다)
    bool mAssetDirtyActive = true;

    // 시작 중복 방지 플래그
    bool mStarted = false;

    // 플레이 시작 중복 방지 플래그
    bool mPlayBegun = false;

    // 플레이 게임모드
    // 후에 확장가능한 구조로
    Core::GameModeSetting mGameModeSetting;
    Core::AmbientLightSettings mAmbientLightSettings;
    Core::SkySphereSettings mSkySphereSettings;

    //  MapPhysicsContext mPhysicsContext;
};

template <typename T> T *Map::CreateEntity(const char *entityInstanceName)
{

    T *object = static_cast<T *>(CreateEntity(T::GetStaticClassName(), entityInstanceName));
    return object;
}

template <typename T> inline T *Map::CreateEngineEntity(const char *entityInstanceName)
{

    T *object = static_cast<T *>(CreateEngineEntity(T::GetStaticClassName(), entityInstanceName));

    return object;
}
