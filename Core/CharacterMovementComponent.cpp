#include "CharacterMovementComponent.h"
#include <Core/PhysicsBridgeSystem.h>

CharacterMovementComponent::CharacterMovementComponent()
    : mMaxVelocity(5.0F), mGroundAcceleration(30.0f), mGroundBrakingDeceleration(40.0f)
{
}

CharacterMovementComponent ::~CharacterMovementComponent() {}

void CharacterMovementComponent::Tick(float deltaTime)
{

    Component::Tick(deltaTime);
    if (mUpdatedPhysicsComponent == nullptr)
        return;

    // 키를 눌렀다면 정해진  베이스 가속도 * deltaTime  발생시키는 힘을 계산한다(max 속도 체크 )

    // owner(캐릭터)의 meshcomponent를 찾아서 그 meshcom과 연결된 phyiscsBody에 계산한 힘을 가한다.
    PhysicsBridgeSystem *bridgeSystem = PhysicsBridgeSystem::GetInstance();

    // 현재 속도 get
    CoreMath::Vector3 currVelocity = bridgeSystem->GetVelocity(mUpdatedPhysicsComponent);

    CoreMath::Vector3 horizontalCurrVelocity = currVelocity;
    horizontalCurrVelocity.Y = 0.0F;

    bool bHasInput = mInputWorldDir.LengthSquared() > 0.001f;
    float acceleration = bHasInput ? mGroundAcceleration : mGroundBrakingDeceleration;
    float velocity = acceleration * deltaTime;

    CoreMath::Vector3 horizontalInputDir = mInputWorldDir;
    horizontalInputDir.Y = 0.0f;
    horizontalInputDir.Normalize();

    CoreMath::Vector3 targetVelocity = horizontalInputDir * mMaxVelocity;

    CoreMath::Vector3 velocityDelta = targetVelocity - horizontalCurrVelocity;

    velocityDelta.Normalize();

    CoreMath::Vector3 newVelocity = currVelocity + velocity * velocityDelta;

    PhysicsGroundResult groundResult;

    bool hasGroundResult = bridgeSystem->GetGroundResult(mUpdatedPhysicsComponent, groundResult);

    if (hasGroundResult && groundResult.mIsGrounded)
    {
        if (newVelocity.Y < 0.0f)
            newVelocity.Y = 0.0f;
    }
    else
    {
        newVelocity.Y += -9.8 * deltaTime;
    }

    if (mUpdatedPhysicsComponent)
        PhysicsBridgeSystem::GetInstance()->SetKinematicVelocity(mUpdatedPhysicsComponent, newVelocity);

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
