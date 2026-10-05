#include "PhysicsBridgeSystem.h"

#include <Core/CollisionChannelSystem.h>
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

PhysicsBridgeSystem::PhysicsBridgeSystem() : mPhysicsWorld(std::make_unique<PhysicsWorld>())
{

    auto collisionSystem = Core::CollisionChannelSystem::GetInstance();
    collisionSystem->mOnChannelListChanged.Register(
        [this]()
        {
            // 채널 다시구축
            BuildColliisonChannelResponseTable();
        });
}

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

PhysicsBodyHandle PhysicsBridgeSystem::RegisterPhysicsBodyComponent(SceneComponent *transformSyncTarget,
                                                                    SceneComponent *sceneComponent,
                                                                    IPhysicsBodyComponent *bodyComponent,
                                                                    IPhysicsShapeProvider *shapeProvider)
{

    if (transformSyncTarget == nullptr || sceneComponent == nullptr || bodyComponent == nullptr ||
        shapeProvider == nullptr)
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
    physicsBodyDesc.mCollisionChannelResponseID = bodyComponent->GetCollisionChannelID();

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
        binding.mTransformSyncTarget = transformSyncTarget;
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

    ConsumeCollisionEvents(physicsScene);
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

            SceneComponent *rootComponent = bindingIt->mTransformSyncTarget;

            if (sceneComponent == rootComponent)
            {

                sceneComponent->SetPositionWorld(transformResult.mPosition);
                sceneComponent->SetQuaternionWorld(transformResult.mRotation);
                unsigned long long transformVersion = sceneComponent->GetTransformVersion();

                if (bindingIt != it->second.mSceneComponentBindingList.end())
                {
                    bindingIt->mLastSyncedTransformVersion = transformVersion;
                }
            }
            else
            {
                ApplyBodyTransformToSyncTarget(*bindingIt, transformResult);
            }
        }
    }
}

void PhysicsBridgeSystem::ApplyBodyTransformToSyncTarget(const PhysicsSceneComponentBinding &binding,
                                                         const PhysicsTransformResult &result)
{

    SceneComponent *rootComponent = binding.mTransformSyncTarget;
    SceneComponent *sceneComponent = binding.mSceneComponent;

    CoreMath::Vector3 oldRootPosition = rootComponent->GetPositionWorld();
    CoreMath::Quaternion oldRootRotation = rootComponent->GetQuaternionWorld();

    CoreMath::Vector3 oldBodyPosition = sceneComponent->GetPositionWorld();
    CoreMath::Quaternion oldBodyRotation = sceneComponent->GetQuaternionWorld();

    CoreMath::Vector3 newBodyPosition = result.mPosition;
    CoreMath::Quaternion newBodyRotation = result.mRotation;

    CoreMath::Quaternion deltaRotation = newBodyRotation * oldBodyRotation.GetConjugate();
    deltaRotation.Normalize();

    CoreMath::Quaternion newRootRotation = deltaRotation * oldRootRotation;

    CoreMath::Vector3 bodyToRoot = oldRootPosition - oldBodyPosition;

    CoreMath::Vector3 newRootPosition = deltaRotation.RotateVector(bodyToRoot) + newBodyPosition;

    rootComponent->SetPositionWorld(newRootPosition);
    rootComponent->SetQuaternionWorld(newRootRotation);

    //   sceneComponent->SetPositionWorld(transformResult.mPosition);
    //  sceneComponent->SetQuaternionWorld(transformResult.mRotation);
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

void PhysicsBridgeSystem::BuildColliisonChannelResponseTable()
{

    auto collisionChannelSystem = Core::CollisionChannelSystem::GetInstance();

    PhysicsCollisionChannelResponseTable phyiscsTable;

    std::vector<Core::CollisionChannelID> collisionChannelIDList = collisionChannelSystem->GetAllChannelD();

    for (int i = 0; i < collisionChannelIDList.size(); ++i)
    {

        Core::CollisionChannelID id = collisionChannelIDList[i];
        Core::CollisionChannelInfo info;
        bool ret = collisionChannelSystem->GetCollisionChannelInfo(id, info);

        if (ret == false)
        {
        }

        for (auto &entry : info.mResponseTypeTable)
        {

            EPhysicsCollisionChannelResponseType newType = ConvertPhysicsCollisionResponseType(entry.second);
            // 없으면 생성시키기 위해 작은경우 뿐만 아니라 같은경우도 통과
            if (phyiscsTable.GetResponseType(id, entry.first) >= newType)
            {
                phyiscsTable.SetResponseType(id, entry.first, newType);
            }
        }
    }

    mPhysicsWorld->SetCollisionChannelResponseTable(phyiscsTable);
}

EPhysicsCollisionChannelResponseType PhysicsBridgeSystem::ConvertPhysicsCollisionResponseType(
    ECollisionResponseType type) const
{

    switch (type)
    {
    case ECollisionResponseType::eIgnore:
        return EPhysicsCollisionChannelResponseType::eIgnore;

    case ECollisionResponseType::eOverlap:

        return EPhysicsCollisionChannelResponseType::eOverlap;
    case ECollisionResponseType::eBlock:

        return EPhysicsCollisionChannelResponseType::eBlock;
    }
}

void PhysicsBridgeSystem::ConsumeCollisionEvents(PhysicsScene *physicsScene)
{

    if (physicsScene == nullptr)
        return;

    std::vector<PhysicsCollisionResponseData> responseDataList;
    physicsScene->ConsumeCollisionEvents(responseDataList);

    for (const auto &entry : responseDataList)
    {
        CollisionResponseData responseData{};
        // Physics와 Core의 enum 값 배치에 의존하지 않고 이벤트 의미를 변환한다.
        switch (entry.mResponseType)
        {
        case decltype(entry.mResponseType)::eOverlap:
            responseData.responseType = ECollisionResponseType::eOverlap;
            break;
        case decltype(entry.mResponseType)::eBlock:
            responseData.responseType = ECollisionResponseType::eBlock;
            break;
        default:
            continue;
        }

        switch (entry.mEventType)
        {
        case EPhysicsCollisionResponseEventType::eBegin:
            responseData.eventType = ECollisionResponseEventType::eBegin;
            break;
        case EPhysicsCollisionResponseEventType::eEnd:
            responseData.eventType = ECollisionResponseEventType::eEnd;
            break;
        default:
            continue;
        }

        // 이벤트 하나는 Body 쌍 하나다. Shape는 무시하고 양쪽에 자기 기준 상대를 전달한다.
        // 콜백이 등록 해제나 DEAD 처리를 할 수 있으므로, 전달마다 매핑을 새로 찾고
        // 컨테이너 iterator나 binding 참조를 콜백 이후에 재사용하지 않는다.
        auto dispatch = [&](PhysicsBodyHandle bodyHandle, PhysicsBodyHandle otherHandle)
        {
            for (auto &contextEntry : mMapContextTable)
            {
                PhysicsBridgeMapContext &context = contextEntry.second;
                if (context.mPhysicsScene != physicsScene)
                    continue;

                auto findComponent = [&](PhysicsBodyHandle handle) -> SceneComponent *
                {
                    auto componentIt = context.mBodyHandleSceneComponentTable.find(handle);
                    if (componentIt == context.mBodyHandleSceneComponentTable.end())
                        return nullptr;

                    SceneComponent *component = componentIt->second;
                    if (component == nullptr || component->GetDeadState())
                        return nullptr;

                    Object *owner = component->GetOwnerObject();
                    if (owner == nullptr || owner->GetKillState())
                        return nullptr;
                    return component;
                };

                SceneComponent *component = findComponent(bodyHandle);
                if (component == nullptr)
                    return;

                auto bindingIt =
                    std::find_if(context.mSceneComponentBindingList.begin(), context.mSceneComponentBindingList.end(),
                                 [bodyHandle](const PhysicsSceneComponentBinding &binding)
                                 { return binding.mPhysicsBodyHandle == bodyHandle; });
                if (bindingIt == context.mSceneComponentBindingList.end() ||
                    bindingIt->mPhysicsBodyComponent == nullptr)
                    return;

                CollisionResponseData data = responseData;
                data.otherComponent = findComponent(otherHandle);
                // 제거된 상대의 포인터는 전달하지 않는다. End는 상대가 없어도 종료를 알린다.
                if (data.otherComponent == nullptr && data.eventType == ECollisionResponseEventType::eBegin)
                    return;

                IPhysicsBodyComponent *bodyComponent = bindingIt->mPhysicsBodyComponent;
                bodyComponent->OnCollisionResponse(data);
                return;
            }
        };

        dispatch(entry.mBodyHandle, entry.mOtherBodyHandle);
        if (entry.mBodyHandle != entry.mOtherBodyHandle)
            dispatch(entry.mOtherBodyHandle, entry.mBodyHandle);
    }
}
