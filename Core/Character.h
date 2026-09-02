#pragma once

#include <Core/ControllableEntity.h>

#include "ControllableEntity.generated.h"

class CharacterMovementComponent;
class StaticMeshComponent;

class CORE_API_LIB REFLECT_CLASS(EngineClass) Character : public ControllableEntity
{
    GENERATED_BODY(Character)
  public:
    Character();
    virtual ~Character();

    REFLECT_PROPERTY()
    CharacterMovementComponent *mMovementComponent = nullptr;

    REFLECT_PROPERTY()
    StaticMeshComponent *mStaticMeshComponent = nullptr;

  protected:
    virtual void OnBegin() override;

  private:
};
