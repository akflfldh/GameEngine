// #include "RigidBodyComponent.h"
// #include <Core/Entity.h>
// #include <Core/IPhysicsShapeProvider.h>
// #include <Core/Object.h>
// #include <Core/PhysicsBridgeSystem.h>
// #include <Core/SceneComponent.h>
//
// RigidBodyComponent::RigidBodyComponent() {}
//
// RigidBodyComponent::~RigidBodyComponent() {}
//
// void RigidBodyComponent::SetPhysicsEnabled(bool flag)
//{
//
//     mPhysicsEnabled = flag;
// }
// void RigidBodyComponent::SetBodyType(EPhysicsBodyType type)
//{
//     mPhysicsBodyType = type;
// }
// void RigidBodyComponent::SetUseGravity(bool flag)
//{
//
//     mUseGravity = flag;
// }
// void RigidBodyComponent::SetMass(float mass)
//{
//
//     mMass = mass;
// }
//
// bool RigidBodyComponent::IsPhysicsEnabled() const
//{
//
//     return mPhysicsEnabled;
// }
// EPhysicsBodyType RigidBodyComponent::GetPhysicsBodyType() const
//{
//
//     return mPhysicsBodyType;
// }
// bool RigidBodyComponent::IsPhysicsGravityEnabled() const
//{
//
//     return mUseGravity;
// }
// float RigidBodyComponent::GetPhysicsMass() const
//{
//
//     return mMass;
// }
// void RigidBodyComponent::OnOwnerObjectAddedToMap()
//{
//
//     Component::OnOwnerObjectAddedToMap();
//
//     Object *ownerObject = GetOwnerObject();
//     if (ownerObject == nullptr)
//         return;
//
//     Entity *ownerEntity = dynamic_cast<Entity *>(ownerObject);
//     if (ownerEntity == nullptr)
//         return;
//
//     SceneComponent *rootComponent = ownerEntity->GetRootComponent();
//
//     // 시스템에 등록
//     auto physicsBridgeSystem = PhysicsBridgeSystem::GetInstance();
//
//     std::vector<PhysicsShapeProviderEntry> shapeProviderList;
//
//     for (auto com : ownerEntity->GetComponentList())
//     {
//         IPhysicsShapeProvider *provider = dynamic_cast<IPhysicsShapeProvider *>(com);
//         SceneComponent *sceneComponent = dynamic_cast<SceneComponent *>(com);
//         if (provider != nullptr && sceneComponent != nullptr)
//         {
//             shapeProviderList.push_back({provider, sceneComponent});
//         }
//     }
//
//     physicsBridgeSystem->RegisterPhysicsBodyComponent(rootComponent, this, shapeProviderList);
// }
