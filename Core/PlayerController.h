#pragma once
#include <Core/CoreType.h>
#include <Core/ObjectController.h>
#include <CoreBase/CallbackSystem.h>
#include <InputSystem/InputType.h>
#include <unordered_map>

#include "PlayerController.generated.h"

class Entity;
class ControllableEntity;
class Character;

struct InputActionValue
{
    Core::EActionValueType mType;
    CoreMath::Vector4 mValue;
};
using ActionCallbackSystem = Core::MultiCallbackSystem<const InputActionValue &>;

struct ActionBinding
{
    Core::EActionValueType mExpectedType;
    ActionCallbackSystem mCallbacks;
};

struct KeyBinding
{
    std::string mActionName;
    InputActionValue mActionValue;
};

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

    std::unordered_map<std::string, InputActionValue> CollectLocalInputActions();

    void PutInputAction(const std::string &actionName, const InputActionValue &actionValue);

    // runtime에 action등록하는 메서드을 엔진에서 호출해주자
    void RegisterInputActionCallbacks();

  protected:
    virtual void OnPossess(ControllableEntity *object) override;
    virtual void OnUnPossess() override;

    void OnMove(const CoreMath::Vector2 &move);
    void OnLook(const CoreMath::Vector2 &look);
    void OnJump(float v);
    void OnLight(bool v);

  private:
    CoreMath::Vector2 mMove;
    float mJump;
    bool mLight;

  private:
    REFLECT_PROPERTY()
    float mMoveSpeed = 1.0f;

    Character *mPossessedCharacter = nullptr;

    // inputAction asset
    // temp input action table
    std::unordered_map<std::string, ActionBinding> mInputActionTable;

    std::unordered_map<Quad::EKeyCode, KeyBinding> mKeyActionTable;
};
