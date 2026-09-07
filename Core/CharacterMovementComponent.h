#pragma once
#include <Core/Component.h>

#include "ControllableEntity.generated.h"

class SceneComponent;

class CORE_API_LIB REFLECT_CLASS(EngineClass) CharacterMovementComponent : public Component

{
    GENERATED_BODY(CharacterMovementComponent)
  public:
    CharacterMovementComponent();
    virtual ~CharacterMovementComponent();

    void SetUpdatedPhysicsComponent(SceneComponent *component);
    void AddMovementInput(const CoreMath::Vector3 &worldDir, float scale);

    virtual void Serialize(Arch &arch) override;

  protected:
    virtual void Tick(float deltaTime) override;
    void SyncPrefabComponentFrom(Component *prefabComponent) override;

  private:
    CoreMath::Vector3 mInputWorldDir;
    SceneComponent *mUpdatedPhysicsComponent = nullptr;

    REFLECT_PROPERTY()
    float mMaxVelocity;
    REFLECT_PROPERTY()
    float mGroundAcceleration;
    REFLECT_PROPERTY()
    float mGroundBrakingDeceleration;

    REFLECT_PROPERTY()
    float mJumpVelocity;
};
