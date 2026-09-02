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

  protected:
    virtual void Tick(float deltaTime) override;

  private:
    CoreMath::Vector3 mInputWorldDir;
    SceneComponent *mUpdatedPhysicsComponent = nullptr;

    float mMaxVelocity;
    float mGroundAcceleration;
    float mGroundBrakingDeceleration;
};
