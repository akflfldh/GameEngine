#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <Core/ObjectTypeDef.h>
#include <CoreBase/BaseClass.h>
#include <CoreBase/CallbackSystem.h>
#include <ReflectSystem/ReflectionMacro.h>
#include <Utility/UniqueID.h>
#include <memory>
#include <string>
#include <unordered_set>

#include "Object.generated.h"

namespace CoreMath
{
struct Ray;
}

namespace Core
{

struct ObjectHandle;
class ObjectImpl;

struct HitResult;

class IHittable;

} // namespace Core

class World;
class Component;
struct PrefabInstanceContext
{
    CoreAsset::AssetID mPrefab = NoneAssetID;
    std::unordered_set<std::string> mOverriddenProperties;
};

using OnComponentAddedCallbackSystem = Core::MultiCallbackSystem<Component *>;
using OnComponentRemovedCallbackSystem = Core::MultiCallbackSystem<Component *>;

class CORE_API_LIB REFLECT_CLASS(EngineClass) Object : public BaseClass
{
    GENERATED_BODY(Object)
    friend class Map;
    friend class Prefab;

  public:
    Object();
    virtual ~Object();
    // ★ 이동 생성자와 이동 대입 연산자 선언 추가
    Object(Object &&other) noexcept = default;
    Object &operator=(Object &&other) noexcept = default;

    // 복사는 막는 것이 Pimpl에서 일반적 (unique_ptr 때문)
    Object(const Object &) = delete;
    Object &operator=(const Object &) = delete;

    // 에디터/런타임 공통 1회 초기화
    void Start();

    // Play 시작시 초기화 함수 , 1회호출 (play로직이 들어감)
    void BeginPlay();
    void Update(float deltaTime);

    // Play종료 함수 , 1회 호출
    void EndPlay();
    void EndUpdate(float deltaTime);

    const std::string &GetObjectName() const;
    CoreUtility::UniqueID GetUniqueID() const;

    bool GetKillState() const;

    // 가장먼저발견된 컴포넌트 반환
    template <typename T> T *GetComponent();

    const Core::ObjectHandle &GetObjectHandle() const;

    Map *GetMap() const;
    World *GetWorld() const;

    // Native 방식
    template <typename T> Component *CreateDefaultComponent(const char *comInstanceName);
    Component *CreateDefaultComponent(const char *comClassName, const char *comInstanceName);

    // InstanceAdded method 방식
    template <typename T> Component *CreateComponent(const char *comInstanceName);
    Component *CreateComponent(const char *comClassName, const char *comInstanceName);

    // Compomnent 제거 함수 필요

    // 자식까지 모두 제거 Destory
    void Destroy();

    void RequestDestroyComponent(Component *com);

    void DestroyDeadComponents();

    bool RayHit(const CoreMath::Ray &ray, Core::HitResult &oHitResult);

    const std::vector<Component *> &GetComponentList() const;

    void SetActive(bool state);

    // 최종상태
    bool GetActive() const;

    Object *GetParent() const;
    void SetParent(Object *parent);
    const std::vector<Object *> &GetChildObjectList() const;

    bool IsChildOf(Object *targetObject) const;

    // 새로운부모와 연결되었을때(계층구조가 구축되었을때) 호출되는 메서드
    virtual void OnConnectedNewParent(Object *parent);

    // 부모와의 연결이 끊어졌을때 호출되는 메서드
    virtual void OnDisconnectedParent();

    void SetObjectFlag(Core::EObjectFlag flag);
    void AddObjectFlag(Core::EObjectFlag flag);
    void RemoveObjectFlag(Core::EObjectFlag flag);
    Core::EObjectFlag GetObjectFlag() const;
    bool HasObjectFlag(Core::EObjectFlag flag) const;

    virtual void Serialize(Arch &arch) override;
    virtual void FlushPropertyDirty() override;

    // 엔진이 추가한 component들이 직렬화된다.
    void SerializeComponents(Arch &arch);

    void UpdateComponentID(CoreUtility::UniqueID oldID, CoreUtility::UniqueID newID, Component *com);

    void SetPrefabID(CoreAsset::AssetID id);
    CoreAsset::AssetID GetPrefabID() const;

    void OverrideProperty(const std::string &property);
    bool IsOverridenProperty(const std::string &property) const;

    void ClearChildObjectListForLoad();
    void AddChildObjectForLoad(Object *object);

    void MarkComponentTreeDirty();
    bool IsComponentTreeDirty() const;
    void ClearComponentTreeDirty();

    // 씬컴포넌트들의 로드직후 계층구조를 구축하는 함수(각 씬컴포넌트들의 부모의 자식리스트에 자기자신을 넣는다)
    void RebuildSceneComponentHierarchyForLoad();

#pragma region CallbackSystem

    OnComponentAddedCallbackSystem mOnComponentAddedCallbackSystem;
    OnComponentRemovedCallbackSystem mOnComponentRemovedCallbackSystem;

#pragma endregion

  protected:
    Component *GetComponent(const char *comInstanceName);

    // 가장먼저발견된 컴포넌트 반환
    Component *GetComponentFromClassName(const char *componentName);

    // map에 등록될때 호출된다.
    virtual void OnAddedToMap();

    // map에서 제거될때 호출된다.
    virtual void OnRemovedFromMap();

    virtual void OnBegin();
    virtual void OnBeginPlay();
    virtual void Tick(float deltaTime);
    virtual void OnEndPlay();
    virtual void EndTick(float deltaTime);

    // self active state는 유지하면서 조상 상태를 체크하여 최종상태 업데이트
    void UpdateActiveInHierarchy();

    // Prefab 사용
    void RefreshComponentIDTable();

  private:
    void SetObjectName(const char *objectName);
    void SetObjectUniqueID(const CoreUtility::UniqueID &id);
    void SetObjectPoolHandle(const Core::ObjectHandle &handle);
    void SetMap(Map *map);
    void DestroyImmediatelyForLoad();
    void DestroyComponentsImmediatelyForLoad();

    std::string MakeUniqueComponentName(const char *desiredName) const;

    Component *CreateComponentCommon(const char *comClassName, const char *comInstanceName);

  private:
    bool mKillState;

    std::vector<Component *> mComList;
    std::unordered_map<std::string, Component *> mComNameTable;
    std::vector<Component *> mDeadComList;
    std::unordered_map<CoreUtility::UniqueID, Component *> mComIDTable;

    // Ray에 Hit가능한 컴포넌트 캐싱
    std::vector<Core::IHittable *> mHittableComList;

    REFLECT_PROPERTY()
    CoreUtility::UniqueID mObjectUniqueID;

    Core::ObjectHandle mObjectPoolHandle;
    REFLECT_PROPERTY()
    std::string mObjectName;
    // Object가 속한 Map
    Map *mMap;

    Object *mParentObject;
    std::vector<Object *> mChildObjectLIst;

    // 자신의 상태
    bool mActiveStateSelf = true;

    // 자신의 상태 + 조상들의 상태를 반영한 상태 = 최종상태
    bool mActiveStateHierachy = true;

    Core::EObjectFlag mObjectFlag;

    PrefabInstanceContext mPrefabInstanceContext;

    // start 중복 방지 플래그
    bool mStarted = false;

    // 플레이 시작 중복 방지플래그
    bool mPlayBegun = false;

    // 컴포넌트 변화 더티플래그
    // 추가,제거,계층변화 모두 감지
    bool mComponentTreeDirty = false;
};

template <typename T> inline Component *Object::CreateComponent(const char *comInstanceName)
{
    return static_cast<T *>(CreateComponent(T::GetStaticClassName(), comInstanceName));
}

template <typename T> inline Component *Object::CreateDefaultComponent(const char *comInstanceName)
{
    return static_cast<T *>(CreateDefaultComponent(T::GetStaticClassName(), comInstanceName));
}

template <typename T> inline T *Object::GetComponent()
{
    static_assert(std::is_base_of_v<Component, T>);

    return static_cast<T *>(GetComponentFromClassName(T::GetStaticClassName()));
}
