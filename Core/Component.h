#pragma once
#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <CoreBase/BaseClass.h>
#include <CoreBase/CallbackSystem.h>
#include <ReflectSystem/ReflectionMacro.h>
#include <string >
#include <unordered_set>

#include "Component.generated.h"

class Arch;
class Object;
class Map;

/*
프리팹 object로부터 받은 컴포넌트인지 여부
*/
struct PrefabComponentInstanceContext
{
    CoreAsset::AssetID mPrefabID = NoneAssetID;
    std::string mPrefabComponentKey; // 일단 컴포넌트 이름 ,(이후 정수데이터로)
    bool mIsPrefabInherited = false; // 상속여부
};

class Component;

using ComponentDestroyRequestedCallbackSystem = Core::MultiCallbackSystem<Component *>;

class CORE_API_LIB REFLECT_CLASS(EngineClass) Component : public BaseClass
{
    GENERATED_BODY(Component)
    // friend class ObjectImpl;
    friend class Object;
    friend class Prefab;

  public:
    Component();
    virtual ~Component() = 0;

    void Start();

    void BeginPlay();
    void Update(float deltaTime);
    void EndPlay();

    void EndUpdate(float deltaTime);

    Object *GetOwnerObject() const;
    Map *GetMap() const;

    const char *GetInstanceName() const;

    bool GetDeadState() const;
    void Destory();

    virtual void OnActiveStateChanged(bool state);

    Core::EComponentFlag GetComponentFlag() const;
    void SetComponentFlag(Core::EComponentFlag flag);

    bool HasComponentFlag(Core::EComponentFlag flag) const;

    const CoreUtility::UniqueID &GetUniqueID() const;

    virtual void Serialize(Arch &arch) override;

    // 프리팹
    void OverrideProperty(const std::string &property);
    bool IsOverridenProperty(const std::string &property) const;

    // void DestroyImmediatelyForLoad();

    // 프리팹 컴포넌트 컨텍스트 설정
    void SetPrefabInheritedComponent(CoreAsset::AssetID prefabID, const std::string &componentKey);
    bool IsPrefabInheritedComponent(CoreAsset::AssetID prefabID) const;
    const std::string &GetPrefabComponenetKey() const;

    // 프리팹컴포넌트로부터 동기화
    virtual void SyncPrefabComponentFrom(Component *prefabComponent);

    // Destroy호출될때 콜백호출
    ComponentDestroyRequestedCallbackSystem mOnDestoryRequestedCallbackSystem;

    Core::EComponentCreationMethod GetComponentCreationMethod() const;

  protected:
    // OwnerObject의 하위구성체로 설정될때 호출됨 (SetOwnerObject내부에서 호출될것이다)
    virtual void OnSetUpOwnerObject();
    virtual void OnOwnerObjectAddedToMap();
    virtual void OnOwnerObjectRemovedFromMap();

    virtual void OnBegin();
    virtual void OnBeginPlay();
    virtual void Tick(float deltaTime);
    virtual void OnEndPlay();
    virtual void EndTick(float deltaTime);

    virtual void OnDestoryRequested();

  private:
    void SetOwnerObject(Object *ownerObject);
    void SetInstanceName(const char *name);
    void SetComponentUniqueID(const CoreUtility::UniqueID &id);

  private:
    Object *mOwnerObject;

    REFLECT_PROPERTY()
    std::string mInstanceName;

    bool mDeadState;

    REFLECT_PROPERTY()
    CoreUtility::UniqueID mComponentUniqueID;

    Core::EComponentFlag mComponentFlag;

    Core::EComponentCreationMethod mComponentCreationMethod;

    // 프리팹
    std::unordered_set<std::string> mOverridenProperties;

    // start 중복 방지 플래그
    bool mStarted = false;

    // beingPlay 중복 방지 플래그
    bool mPlayBegun = false;

    // 프리팹 컴포넌트 컨텍스트
    PrefabComponentInstanceContext mPrefabComponentInstanceContext;
};
