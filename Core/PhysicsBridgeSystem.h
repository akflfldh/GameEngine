#pragma once

#include <Core/CoreDllExport.h>
#include <Physics/PhysicsType.h>
#include <memory>
#include <unordered_map>
#include <vector>

class Map;
class PhysicsScene;
class StaticMeshComponent;
class PhysicsWorld;
class SceneComponent;
class IPhysicsBodyComponent;
class IPhysicsShapeProvider;

// Shape는 body를 구성하는 요소
// Shape들이 모여 하나의 body를 구성하고 충돌작용처리는 shape단위로 처리하나 충돌반응은 body 단위로 처리한다.
struct PhysicsShapeBinding
{
    PhysicsShapeHandle mShapeHandle = PhysicsShapeHandleInValid;
    size_t mShapeIndex = 0;
};

// Physics Body와 외부링크 context
struct PhysicsSceneComponentBinding
{
    IPhysicsBodyComponent *mPhysicsBodyComponent = nullptr;
    SceneComponent *mSceneComponent = nullptr;
    IPhysicsShapeProvider *mShapeProvider = nullptr;
    PhysicsBodyHandle mPhysicsBodyHandle = PhysicsBodyHandleInValid;

    unsigned long long mLastSyncedTransformVersion = 0;

    std::vector<PhysicsShapeBinding> mShapeBindingList;
    PhysicsGroundResult mGroundResult;
};

struct PhysicsBridgeMapContext
{
    Map *mMap = nullptr;
    PhysicsScene *mPhysicsScene = nullptr;

    std::vector<PhysicsSceneComponentBinding> mSceneComponentBindingList;
    std::unordered_map<PhysicsBodyHandle, SceneComponent *> mBodyHandleSceneComponentTable;
    std::unordered_map<SceneComponent *, PhysicsBodyHandle> mSceneComponentBodyHandleTable;

    // std::unordered_map<PhysicsBodyHandle, Object *> mBodyHandleObjectTable;
    // std::unordered_map<Object *, PhysicsBodyHandle> mObjectBodyHandleTable;
};

class CORE_API_LIB PhysicsBridgeSystem
{

  public:
    static PhysicsBridgeSystem *GetInstance();

    PhysicsBridgeSystem();
    ~PhysicsBridgeSystem();

    bool RegisterMap(Map *map);
    bool UnRegisterMap(Map *map);

    PhysicsBodyHandle RegisterPhysicsBodyComponent(SceneComponent *rootSceneComponent,
                                                   IPhysicsBodyComponent *bodyComponent,
                                                   IPhysicsShapeProvider *shapeProvider);

    void UnregisterPhysicsBodyComponent(SceneComponent *component);

    void Update(Map *map, float deltaTime);

    void AddForce(SceneComponent *bodyComponent, const CoreMath::Vector3 &force);
    void SetKinematicVelocity(SceneComponent *bodyComponent, const CoreMath::Vector3 &velocity);
    CoreMath::Vector3 GetVelocity(SceneComponent *bodyComponent);
    float GetMass(SceneComponent *bodyComponent);

    //  PhysicsBodyHandle GetPhysicsBodyHandleFromObject(Object *object) const;

    bool GetGroundResult(SceneComponent *component, PhysicsGroundResult &outResult) const;

  private:
    void PrePhysicsUpdate(Map *map, std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it);
    void PostPhysicsUpdate(Map *map, std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it);

    StaticMeshComponent *GetStaticMeshComponentFromBodyHandle(Map *map, PhysicsBodyHandle handle) const;

    void SyncTransformToComponent(const PhysicsFrameResult &physicsFrameResult,
                                  std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it);

    void SyncGroundResultToComponent(const PhysicsFrameResult &physicsFrameResult,
                                     std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it);

    void CreateShape(PhysicsScene *physicsScene, SceneComponent *sceneComponent, PhysicsBodyHandle bodyHandle,
                     IPhysicsShapeProvider *shapeProvider, PhysicsSceneComponentBinding &binding);

    PhysicsBodyHandle GetBodyHandleFromBodyCom(SceneComponent *bodyComponent) const;
    PhysicsScene *GetPhysicsScene(SceneComponent *bodyComponent) const;

    const PhysicsSceneComponentBinding *FindBinding(SceneComponent *bodyComponent) const;

  private:
    std::unique_ptr<PhysicsWorld> mPhysicsWorld;
    std::unordered_map<Map *, PhysicsBridgeMapContext> mMapContextTable;
};
