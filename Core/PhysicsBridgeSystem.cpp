#include "PhysicsBridgeSystem.h"

#include <Core/CorePhysicsType.h>
#include <Core/Entity.h>
#include <Core/IPhysicsBodyComponent.h>
#include <Core/IPhysicsShapeProvider.h>
#include <Core/SceneComponent.h>
#include <Physics/PhysicsScene.h>
#include <Physics/PhysicsWorld.h>
#include <algorithm>

PhysicsBridgeSystem *PhysicsBridgeSystem::GetInstance()
{
    static PhysicsBridgeSystem instance;

    return &instance;
}

PhysicsBridgeSystem::PhysicsBridgeSystem() : mPhysicsWorld(std::make_unique<PhysicsWorld>()) {}

PhysicsBridgeSystem::~PhysicsBridgeSystem() {}

bool PhysicsBridgeSystem::RegisterMap(Map *map)
{

    if (map == nullptr)
        return false;

    std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it = mMapContextTable.find(map);

    if (it != mMapContextTable.end())
    {
        return false;
    }

    PhysicsBridgeMapContext mapContext;
    mapContext.mMap = map;

    PhysicsSceneID sceneID = mPhysicsWorld->CreateNewScene();
    mapContext.mPhysicsScene = mPhysicsWorld->GetScene(sceneID);

    mMapContextTable[map] = mapContext;

    return true;
}

bool PhysicsBridgeSystem::UnRegisterMap(Map *map)
{
    if (map == nullptr)
        return false;

    std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it = mMapContextTable.find(map);

    if (it == mMapContextTable.end())
    {
        return false;
    }

    PhysicsScene *scene = it->second.mPhysicsScene;
    mPhysicsWorld->DestroyScene(scene);

    mMapContextTable.erase(it);

    return true;
}

PhysicsBodyHandle PhysicsBridgeSystem::RegisterPhysicsBodyComponent(SceneComponent *sceneComponent,
                                                                    IPhysicsBodyComponent *bodyComponent,
                                                                    IPhysicsShapeProvider *shapeProvider)
{

    if (sceneComponent == nullptr || bodyComponent == nullptr || shapeProvider == nullptr)
        return PhysicsBodyHandleInValid;

    if (!bodyComponent->IsPhysicsEnabled())
        return PhysicsBodyHandleInValid;

    Map *map = sceneComponent->GetMap();

    std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it = mMapContextTable.find(map);
    if (it == mMapContextTable.end())
    {
        return PhysicsBodyHandleInValid;
    }

    // ok 일단 compponent마다 개별적인 body .
    // 향후 entity 와 설정값으로 하나의 바디로 합치는 옵션 기능 제공
    auto bodyHandleIt = it->second.mSceneComponentBodyHandleTable.find(sceneComponent);
    if (bodyHandleIt != it->second.mSceneComponentBodyHandleTable.end())
    {
        return bodyHandleIt->second;
    }

    // body생성
    PhysicsBodyDesc physicsBodyDesc;

    physicsBodyDesc.mBodyType = bodyComponent->GetPhysicsBodyType();
    physicsBodyDesc.mMass = bodyComponent->GetPhysicsMass();
    physicsBodyDesc.mGravity = bodyComponent->IsPhysicsGravityEnabled();
    physicsBodyDesc.mPosition = sceneComponent->GetPositionWorld();
    physicsBodyDesc.mRotation = sceneComponent->GetQuaternionWorld();

    // 즉각적으로생성하는데 만약 물리시스템이 UPDATE중이라면?
    // 1. 생성후 일단 임시리스트로 들어가고 물리시스템 UPDATE시작에서 메인리스트로 옮기기
    PhysicsBodyHandle bodyHandle = it->second.mPhysicsScene->CreatePhysicsBody(physicsBodyDesc);

    if (bodyHandle != PhysicsBodyHandleInValid)
    {

        // 리스트 등록
        PhysicsSceneComponentBinding binding;

        // CreateShape

        CreateShape(it->second.mPhysicsScene, sceneComponent, bodyHandle, shapeProvider, binding);

        // table 에 등록
        it->second.mBodyHandleSceneComponentTable[bodyHandle] = sceneComponent;
        it->second.mSceneComponentBodyHandleTable[sceneComponent] = bodyHandle;

        // 리스트 등록
        binding.mSceneComponent = sceneComponent;
        binding.mPhysicsBodyHandle = bodyHandle;
        binding.mPhysicsBodyComponent = bodyComponent;
        binding.mShapeProvider = shapeProvider;

        it->second.mSceneComponentBindingList.push_back(binding);
    }

    return bodyHandle;
}

void PhysicsBridgeSystem::UnregisterPhysicsBodyComponent(SceneComponent *component)
{

    if (component == nullptr)
        return;

    Map *map = component->GetMap();

    std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it = mMapContextTable.find(map);
    if (it == mMapContextTable.end())
    {
        return;
    }

    auto listIt =
        std::find_if(it->second.mSceneComponentBindingList.begin(), it->second.mSceneComponentBindingList.end(),
                     [component](const PhysicsSceneComponentBinding &binding)
                     {
                         if (binding.mSceneComponent == component)
                             return true;
                         return false;
                     });

    if (listIt == it->second.mSceneComponentBindingList.end())
        return;

    PhysicsBodyHandle bodyHandle = listIt->mPhysicsBodyHandle;

    if (listIt != it->second.mSceneComponentBindingList.end())
    {
        std::iter_swap(listIt, it->second.mSceneComponentBindingList.end() - 1);
        it->second.mSceneComponentBindingList.pop_back();
    }
    it->second.mBodyHandleSceneComponentTable.erase(bodyHandle);
    it->second.mSceneComponentBodyHandleTable.erase(component);

    it->second.mPhysicsScene->DestroyPhysicsBody(bodyHandle);
}

void PhysicsBridgeSystem::Update(Map *map, float deltaTime)
{

    if (map == nullptr)
        return;
    auto it = mMapContextTable.find(map);

    if (it == mMapContextTable.end())
        return;

    if (it->second.mPhysicsScene == nullptr)
        return;

    PrePhysicsUpdate(map, it);
    it->second.mPhysicsScene->Update(deltaTime);
    PostPhysicsUpdate(map, it);
}

void PhysicsBridgeSystem::AddForce(SceneComponent *bodyComponent, const CoreMath::Vector3 &force)
{

    PhysicsScene *physicsScene = GetPhysicsScene(bodyComponent);
    if (physicsScene == nullptr)
        return;

    PhysicsBodyHandle bodyHandle = GetBodyHandleFromBodyCom(bodyComponent);
    if (bodyHandle == PhysicsBodyHandleInValid)
    {
        return;
    }

    PhysicsAddForceCommand command;
    command.mBodyHandle = bodyHandle;
    command.mForce = force;
    physicsScene->EnqueueAddForceCommand(command);
}

void PhysicsBridgeSystem::SetKinematicVelocity(SceneComponent *bodyComponent, const CoreMath::Vector3 &velocity)
{

    PhysicsScene *physicsScene = GetPhysicsScene(bodyComponent);
    if (physicsScene == nullptr)
        return;

    PhysicsBodyHandle bodyHandle = GetBodyHandleFromBodyCom(bodyComponent);
    if (bodyHandle == PhysicsBodyHandleInValid)
    {
        return;
    }
    PhysicsSetVelocityCommmand command;
    command.mBodyHandle = bodyHandle;
    command.mVelocity = velocity;
    physicsScene->EnqueueSetKinematicVelocityCommand(command);
}

CoreMath::Vector3 PhysicsBridgeSystem::GetVelocity(SceneComponent *bodyComponent)
{

    PhysicsScene *physicsScene = GetPhysicsScene(bodyComponent);
    if (physicsScene == nullptr)
        return {0, 0, 0};

    PhysicsBodyHandle bodyHandle = GetBodyHandleFromBodyCom(bodyComponent);
    if (bodyHandle == PhysicsBodyHandleInValid)
    {
        return {0, 0, 0};
    }

    return physicsScene->GetVelocity(bodyHandle);
}

float PhysicsBridgeSystem::GetMass(SceneComponent *bodyComponent)
{
    PhysicsScene *physicsScene = GetPhysicsScene(bodyComponent);
    if (physicsScene == nullptr)
        return 0.0f;

    PhysicsBodyHandle bodyHandle = GetBodyHandleFromBodyCom(bodyComponent);
    if (bodyHandle == PhysicsBodyHandleInValid)
    {
        return 0.0f;
    }

    return physicsScene->GetMass(bodyHandle);
}

// PhysicsBodyHandle PhysicsBridgeSystem::GetPhysicsBodyHandleFromObject(Object *object) const
//{
//
//     if (object == nullptr)
//         return PhysicsBodyHandleInValid;
//
//     Map *map = object->GetMap();
//
//     std::unordered_map<Map *, PhysicsBridgeMapContext>::const_iterator it = mMapContextTable.find(map);
//     if (it == mMapContextTable.end())
//     {
//         return PhysicsBodyHandleInValid;
//     }
//
//     auto bodyHandleIt = it->second.mObjectBodyHandleTable.find(object);
//
//     if (bodyHandleIt == it->second.mObjectBodyHandleTable.end())
//     {
//         return PhysicsBodyHandleInValid;
//     }
//
//     return bodyHandleIt->second;
// }

bool PhysicsBridgeSystem::GetGroundResult(SceneComponent *component, PhysicsGroundResult &outResult) const
{

    const PhysicsSceneComponentBinding *binding = FindBinding(component);

    if (binding == nullptr)
        return false;

    outResult = binding->mGroundResult;
    return true;
}

void PhysicsBridgeSystem::PrePhysicsUpdate(Map *map, std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it)
{
    // com들을 보고 commnad 를 물리씬에 넣는다.

    // static 인 물체들도 업데이트로인해서 위치,방향,크기가 바뀔수있다.
    // transform dirty플래그로 판단하면 좋고

    // phyiscs body
    for (auto &e : it->second.mSceneComponentBindingList)
    {
        if (e.mSceneComponent == nullptr || e.mPhysicsBodyComponent == nullptr)
        {
            continue;
        }

        unsigned long long transformVersion = e.mSceneComponent->GetTransformVersion();
        if (transformVersion == e.mLastSyncedTransformVersion)
            continue;

        /*    if (e.mPhysicsBodyComponent->GetPhysicsBodyType() != EPhysicsBodyType::eDynamic)
          {*/

        PhysicsTransformCommand transformCommand;
        transformCommand.mBodyHandle = e.mPhysicsBodyHandle;
        transformCommand.mWorldPosition = e.mSceneComponent->GetPositionWorld();
        transformCommand.mWorldRotation = e.mSceneComponent->GetQuaternionWorld();
        transformCommand.mWorldScale = e.mSceneComponent->GetScaleWorld();

        it->second.mPhysicsScene->EnqueueTransformCommand(transformCommand);

        // force는 일단없어 .

        e.mLastSyncedTransformVersion = transformVersion;
    }
}

void PhysicsBridgeSystem::PostPhysicsUpdate(Map *map, std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it)
{
    // 물리 frame result를 보고 com에 반영한다.

    PhysicsScene *physicsScene = it->second.mPhysicsScene;

    const PhysicsFrameResult &physicsFrameResult = physicsScene->GetPhysicsFrameResult();

    // SyncTransformToComponent;
    SyncTransformToComponent(physicsFrameResult, it);

    SyncGroundResultToComponent(physicsFrameResult, it);
}

void PhysicsBridgeSystem::SyncTransformToComponent(const PhysicsFrameResult &physicsFrameResult,
                                                   std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it)

{

    for (const auto &transformResult : physicsFrameResult.mTransformResults)
    {
        if (transformResult.mBodyHandle != PhysicsBodyHandleInValid)
        {

            auto bodyHandleIt = it->second.mBodyHandleSceneComponentTable.find(transformResult.mBodyHandle);

            if (bodyHandleIt == it->second.mBodyHandleSceneComponentTable.end())
                continue;

            SceneComponent *sceneComponent = bodyHandleIt->second;

            sceneComponent->SetPositionWorld(transformResult.mPosition);
            sceneComponent->SetQuaternionWorld(transformResult.mRotation);

            unsigned long long transformVersion = sceneComponent->GetTransformVersion();

            auto bindingIt =
                std::find_if(it->second.mSceneComponentBindingList.begin(), it->second.mSceneComponentBindingList.end(),
                             [sceneComponent](const PhysicsSceneComponentBinding &binding)
                             {
                                 if (binding.mSceneComponent == sceneComponent)
                                 {
                                     return true;
                                 }
                                 return false;
                             });

            if (bindingIt != it->second.mSceneComponentBindingList.end())
            {
                bindingIt->mLastSyncedTransformVersion = transformVersion;
            }
        }
    }
}

void PhysicsBridgeSystem::SyncGroundResultToComponent(const PhysicsFrameResult &physicsFrameResult,
                                                      std::unordered_map<Map *, PhysicsBridgeMapContext>::iterator it)
{

    for (PhysicsSceneComponentBinding &binding : it->second.mSceneComponentBindingList)
    {
        binding.mGroundResult.mBodyHandle = binding.mPhysicsBodyHandle;

        binding.mGroundResult.mIsGrounded = false;
        binding.mGroundResult.mGroundBodyHandle = PhysicsBodyHandleInValid;
        binding.mGroundResult.mGroundNormal = {0, 0, 0};
    }

    for (const auto &groundResult : physicsFrameResult.mGroundResults)
    {
        if (groundResult.mBodyHandle != PhysicsBodyHandleInValid)
        {

            auto bodyHandleIt = it->second.mBodyHandleSceneComponentTable.find(groundResult.mBodyHandle);

            if (bodyHandleIt == it->second.mBodyHandleSceneComponentTable.end())
                continue;

            if (groundResult.mIsGrounded == false)
                continue;

            SceneComponent *sceneComponent = bodyHandleIt->second;

            auto bindingIt =
                std::find_if(it->second.mSceneComponentBindingList.begin(), it->second.mSceneComponentBindingList.end(),
                             [sceneComponent](const PhysicsSceneComponentBinding &binding)
                             {
                                 if (binding.mSceneComponent == sceneComponent)
                                 {
                                     return true;
                                 }
                                 return false;
                             });

            if (bindingIt != it->second.mSceneComponentBindingList.end())
            {

                // 기존에 바닥에 있지않은 상태 플래그 이거나 , 더 up벡터에 가까운 노멀일경우 갱신
                if (bindingIt->mGroundResult.mIsGrounded == false ||
                    groundResult.mGroundNormal.Y > bindingIt->mGroundResult.mGroundNormal.Y)
                    bindingIt->mGroundResult = groundResult;
            }
        }
    }
}

void PhysicsBridgeSystem::CreateShape(PhysicsScene *physicsScene, SceneComponent *sceneComponent,
                                      PhysicsBodyHandle bodyHandle, IPhysicsShapeProvider *shapeProvider,
                                      PhysicsSceneComponentBinding &binding)
{
    if (physicsScene == nullptr || shapeProvider == nullptr)
        return;

    const PhysicsCollisionPreset &shapePreset = shapeProvider->GetPhysicsCollisionPreset();

    const CoreMath::Matrix4X4 &sceneWorldMatrix = sceneComponent->GetTransformWorld();
    CoreMath::Quaternion sceneWorldQuaternion = sceneComponent->GetQuaternionWorld();
    CoreMath::Vector3 sceneWorldScale = sceneComponent->GetScaleWorld();
    const std::vector<PhysicsCollisionShapeData> &shapeList = shapePreset.mShapeList;

    for (int i = 0; i < shapeList.size(); ++i)
    {

        const PhysicsCollisionShapeData &shape = shapeList[i];

        PhysicsShapeDesc shapeDesc;
        shapeDesc.mBodyHandle = bodyHandle;
        shapeDesc.mShapeType = shape.mShapeType;

        shapeDesc.mLocalPosition = shape.mLocalPosition;
        shapeDesc.mLocalRotation = shape.mLocalRotation;
        shapeDesc.mLocalScale = shape.mLocalScale;

        shapeDesc.mWorldPosition = sceneWorldMatrix.TransformPoint(shape.mLocalPosition);
        shapeDesc.mWorldRotation = sceneWorldQuaternion * shape.mLocalRotation;

        shapeDesc.mWorldScale = sceneWorldScale * shape.mLocalScale;

        if (shape.mShapeType == EPhysicsCollisionShapeType::eBox)
        {
            shapeDesc.mBoxData.mLocalHalfExtent = shape.mBoxData.mLocalHalfExtents;
        }

        PhysicsShapeHandle shapeHandle = physicsScene->CreatePhysicsShape(shapeDesc);

        PhysicsShapeBinding shapeBinding;
        shapeBinding.mShapeHandle = shapeHandle;
        shapeBinding.mShapeIndex = i;
        binding.mShapeBindingList.push_back(shapeBinding);
    }
}

PhysicsBodyHandle PhysicsBridgeSystem::GetBodyHandleFromBodyCom(SceneComponent *bodyComponent) const
{

    if (bodyComponent == nullptr)
    {
        return PhysicsBodyHandleInValid;
    }

    Map *map = bodyComponent->GetMap();

    if (map == nullptr)
    {
        return PhysicsBodyHandleInValid;
    }

    auto contextIt = mMapContextTable.find(map);
    if (contextIt == mMapContextTable.end())
    {
        return PhysicsBodyHandleInValid;
    }

    auto bodyHandleIt = contextIt->second.mSceneComponentBodyHandleTable.find(bodyComponent);
    if (bodyHandleIt == contextIt->second.mSceneComponentBodyHandleTable.end())
    {
        return PhysicsBodyHandleInValid;
    }

    return bodyHandleIt->second;
}

PhysicsScene *PhysicsBridgeSystem::GetPhysicsScene(SceneComponent *bodyComponent) const
{
    if (bodyComponent == nullptr)
    {
        return nullptr;
    }

    Map *map = bodyComponent->GetMap();

    if (map == nullptr)
    {
        return nullptr;
    }

    auto contextIt = mMapContextTable.find(map);
    if (contextIt == mMapContextTable.end())
    {
        return nullptr;
    }

    return contextIt->second.mPhysicsScene;
}

const PhysicsSceneComponentBinding *PhysicsBridgeSystem::FindBinding(SceneComponent *bodyComponent) const
{
    if (bodyComponent == nullptr)
        return nullptr;

    Map *map = bodyComponent->GetMap();
    if (map == nullptr)
        return nullptr;

    auto mapContextIt = mMapContextTable.find(map);
    if (mapContextIt == mMapContextTable.end())
        return nullptr;

    const auto bindingIt = std::find_if(mapContextIt->second.mSceneComponentBindingList.begin(),
                                        mapContextIt->second.mSceneComponentBindingList.end(),
                                        [bodyComponent](const PhysicsSceneComponentBinding &binding)
                                        {
                                            if (binding.mSceneComponent == bodyComponent)
                                                return true;
                                            return false;
                                        });

    if (bindingIt == mapContextIt->second.mSceneComponentBindingList.end())
        return nullptr;

    return &(*bindingIt);
}
