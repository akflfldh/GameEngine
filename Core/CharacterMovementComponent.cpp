#include "CharacterMovementComponent.h"
#include <Core/IPhysicsBodyComponent.h>
#include <Core/Object.h>
#include <Core/PhysicsBridgeSystem.h>
#include <Core/SceneComponent.h>
#include <NetWork/NetWorkSystem.h>

CharacterMovementComponent::CharacterMovementComponent()
    : mMaxVelocity(5.0F), mGroundAcceleration(30.0f), mGroundBrakingDeceleration(40.0f)
{
}

CharacterMovementComponent ::~CharacterMovementComponent() {}

void CharacterMovementComponent::Tick(float deltaTime)
{

    Component::Tick(deltaTime);

    // 예측 없는 최소 네트워크 버전에서는 클라이언트가 호스트의 위치를 덮어쓰는 이동 명령을 만들지 않는다.
    // Host와 Offline은 기존 이동 및 중력 계산을 유지한다.
    if (NetWork::NetWorkSystem::GetInstance()->GetNetWorkRole() == NetWork::ENetWorkRole::eClient)
        return;

    auto updatedPhysicsComponent = GetPhysicsComponent();

    if (updatedPhysicsComponent == nullptr)
        return;

    // 키를 눌렀다면 정해진  베이스 가속도 * deltaTime  발생시키는 힘을 계산한다(max 속도 체크 )

    // owner(캐릭터)의 meshcomponent를 찾아서 그 meshcom과 연결된 phyiscsBody에 계산한 힘을 가한다.
    PhysicsBridgeSystem *bridgeSystem = PhysicsBridgeSystem::GetInstance();

    // 현재 속도 get
    CoreMath::Vector3 currVelocity = bridgeSystem->GetVelocity(updatedPhysicsComponent);

    CoreMath::Vector3 horizontalCurrVelocity = currVelocity;
    horizontalCurrVelocity.Y = 0.0F;

    bool bHasInput = mInputWorldDir.LengthSquared() > 0.001f;
    float acceleration = bHasInput ? mGroundAcceleration : mGroundBrakingDeceleration;

    CoreMath::Vector3 horizontalInputDir = mInputWorldDir;
    horizontalInputDir.Y = 0.0f;
    horizontalInputDir.Normalize();

    CoreMath::Vector3 targetVelocity = horizontalInputDir * mMaxVelocity;

    CoreMath::Vector3 velocityDelta = targetVelocity - horizontalCurrVelocity;

    float velocity = std::min(acceleration * deltaTime, velocityDelta.Length());
    velocityDelta.Normalize();

    // float velocity = acceleration * deltaTime;

    CoreMath::Vector3 newVelocity = currVelocity + velocity * velocityDelta;

    PhysicsGroundResult groundResult;

    bool hasGroundResult = bridgeSystem->GetGroundResult(updatedPhysicsComponent, groundResult);

    if (hasGroundResult && groundResult.mIsGrounded)
    {

        if (newVelocity.Y < 0.0f)
            newVelocity.Y = 0.0f;

        // 땅에 있을때만 , 공중에있을땐 점프 불가능 .
        if (mInputWorldDir.Y > 0.1f)
        { // 점프키를 눌렀다.
            newVelocity.Y = mJumpVelocity;
        }
    }
    else
    {
        newVelocity.Y += -980 * deltaTime;
    }

    newVelocity += mAddVelocity;
    mAddVelocity = CoreMath::Vector3::Zero;

    if (updatedPhysicsComponent)
        PhysicsBridgeSystem::GetInstance()->SetKinematicVelocity(updatedPhysicsComponent, newVelocity);

    mInputWorldDir = {0, 0, 0};
}

void CharacterMovementComponent::AddMovementInput(const CoreMath::Vector3 &worldDir, float scale)
{

    mInputWorldDir = worldDir * scale;
}

void CharacterMovementComponent::SetUpdatedPhysicsComponent(SceneComponent *component)
{

    mUpdatedPhysicsComponent = component;
}

void CharacterMovementComponent::SyncPrefabComponentFrom(Component *prefabComponent)
{

    CharacterMovementComponent *prefabMovementCom = static_cast<CharacterMovementComponent *>(prefabComponent);

    mMaxVelocity = prefabMovementCom->mMaxVelocity;
    mGroundAcceleration = prefabMovementCom->mGroundAcceleration;
    mGroundBrakingDeceleration = prefabMovementCom->mGroundBrakingDeceleration;
    mJumpVelocity = prefabMovementCom->mJumpVelocity;
}

void CharacterMovementComponent::Serialize(Arch &arch)
{
    Component::Serialize(arch);

    arch << mMaxVelocity;
    arch << mGroundAcceleration;
    arch << mGroundBrakingDeceleration;
    arch << mJumpVelocity;
}

SceneComponent *CharacterMovementComponent::GetPhysicsComponent() const
{

    auto object = GetOwnerObject();

    if (!object)
    {
        return nullptr;
    }

    SceneComponent *result = nullptr;

    for (Component *com : object->GetComponentList())
    {
        if (!com || com->GetDeadState())
            continue;

        auto physicsCom = dynamic_cast<IPhysicsBodyComponent *>(com);
        auto sceneCom = dynamic_cast<SceneComponent *>(com);

        if (!physicsCom || !sceneCom)
            continue;

        if (!physicsCom->IsPhysicsEnabled() || physicsCom->GetPhysicsBodyType() != EPhysicsBodyType::eKinematic)
            continue;

        // 후보가 여러 개면 임의의 바디를 이동시키지 않는다.
        if (result)
            return nullptr;

        result = sceneCom;
    }

    return result;
}

void CharacterMovementComponent::ApplyVelocity(const CoreMath::Vector3 v)
{

    mAddVelocity += v;
}
