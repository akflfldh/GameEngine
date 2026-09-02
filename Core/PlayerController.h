#pragma once
#include <Core/ObjectController.h>

#include "PlayerController.generated.h"

class Entity;
class ControllableEntity;
class Character;

class CORE_API_LIB REFLECT_CLASS(EngineClass) PlayerController : public ObjectController
{
    GENERATED_BODY(PlayerController)

  public:
    PlayerController();
    virtual ~PlayerController();

    virtual void Tick(float deltaTime) override;

    virtual void CheckToggleInput(const Core::InputData &inputData) override;

    virtual bool HandleInput(const Core::InputData &inputData) override;
    virtual void OnMouseCaptureLost() override;

  protected:
    virtual void OnPossess(ControllableEntity *object) override;
    virtual void OnUnPossess() override;

  private:
    REFLECT_PROPERTY()
    float mMoveSpeed = 1.0f;

    Character *mPossessedCharacter = nullptr;
};
