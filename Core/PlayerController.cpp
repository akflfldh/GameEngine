#include "PlayerController.h"
#include <Core/Character.h>
#include <Core/CharacterMovementComponent.h>
#include <Core/LightComponent.h>
#include <Core/SkeletalMeshComponent.h>
#include <InputSystem/InputSystem.h>

PlayerController::PlayerController() : mMove(0, 0), mJump(0), mLight(false)
{

    // 런타임 플레이 컨트롤러는 active 활성화 기본

    mActiveState = false;
    SetMouseMode(Core::MouseMode::Captured);

    mKeyActionTable[Quad::EKeyCode::eW] = {"Move", {Core::EActionValueType::eVector2, {0, 1, 0, 0}}};
    mKeyActionTable[Quad::EKeyCode::eA] = {"Move", {Core::EActionValueType::eVector2, {-1, 0, 0, 0}}};
    mKeyActionTable[Quad::EKeyCode::eS] = {"Move", {Core::EActionValueType::eVector2, {0, -1, 0, 0}}};
    mKeyActionTable[Quad::EKeyCode::eD] = {"Move", {Core::EActionValueType::eVector2, {1, 0, 0, 0}}};
    mKeyActionTable[Quad::EKeyCode::eSpace] = {"Jump", {Core::EActionValueType::eFloat, {1, 0, 0, 0}}};
    mKeyActionTable[Quad::EKeyCode::eMouseDeltaX] = {"Look", {Core::EActionValueType::eVector2, {1, 0, 0, 0}}};
    mKeyActionTable[Quad::EKeyCode::eMouseDeltaY] = {"Look", {Core::EActionValueType::eVector2, {0, 1, 0, 0}}};
    mKeyActionTable[Quad::EKeyCode::eG] = {"Light", {Core::EActionValueType::eBool, {1, 0, 0, 0}}};

    RegisterInputActionCallbacks();
}

PlayerController ::~PlayerController() {}

void PlayerController ::Tick(float deltaTime)
{

    ObjectController::Tick(deltaTime);

    if (mPossessedCharacter == nullptr)
        return;

    //   mLight = false;

    /* Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();
     if (inputSystem == nullptr)
         return;

     if (inputSystem->GetGameInputBlocked())
         return;*/

    CoreMath::Vector3 forward = mPossessedCharacter->GetForwardWorld();
    CoreMath::Vector3 right = mPossessedCharacter->GetRightWorld();
    CoreMath::Vector3 move = CoreMath::Vector3::Zero;

    // float mouseDeltaX = inputSystem->GetMouseDelta().first;
    // mPossessedCharacter->AddRotationLocal({0, mouseDeltaX, 0});

    auto meshCom = mPossessedCharacter->GetComponent<SkeletalMeshComponent>();

    // bool bMove = false;
    // float movementYaw = 180.0f;
    // if (inputSystem->IsVKeyDown('W'))
    //{
    //     bMove = true;
    //     move += forward;

    //    if (meshCom)
    //        meshCom->SetRotationLocal({0, movementYaw + 0, 0});
    //}
    // if (inputSystem->IsVKeyDown('S'))
    //{
    //    bMove = true;
    //    move -= forward;

    //    if (meshCom)
    //        meshCom->SetRotationLocal({0, movementYaw + 180, 0});
    //}
    // if (inputSystem->IsVKeyDown('A'))
    //{
    //    bMove = true;
    //    move -= right;

    //    if (meshCom)
    //        meshCom->SetRotationLocal({0, movementYaw - 90, 0});
    //}
    // if (inputSystem->IsVKeyDown('D'))
    //{
    //    bMove = true;
    //    move += right;

    //    if (meshCom)
    //        meshCom->SetRotationLocal({0, movementYaw + 90, 0});
    //}
    // if (inputSystem->IsVKeyDown(' '))
    //    move.Y += 1;

    // if (bMove)
    //{
    //     mPossessedCharacter->PlayClip("Asset/Take 0011");
    // }
    // else
    //{

    //    mPossessedCharacter->PlayClip("Asset/Take 0010");
    //}

    // if (inputSystem->IsVKeyDown('G'))
    //{
    //     mPossessedCharacter->PlayClip("Asset/Take 001");
    // }

    move += mMove.X * right;
    move += mMove.Y * forward;
    move.Y += mJump;

    if (move != CoreMath::Vector3::Zero)
    {
        move.Normalize();
        if (mPossessedCharacter->mMovementComponent)
        {

            mPossessedCharacter->mMovementComponent->AddMovementInput(move, 1.0f);
        }
    }
}

void PlayerController::CheckToggleInput(const Core::InputData &inputData)
{

    // 게임용 controller는 editor camera처럼 우클릭 토글이 필요 없다.
    // true가 리턴되면 ui 보다 먼저 입력을 소비하는구조이다. 어떻게 ui와 혼합할지 고민

    mActiveState = false;
}

bool PlayerController::HandleInput(const Core::InputData &inputData)
{

    return false;
}

void PlayerController::OnPossess(ControllableEntity *object)
{

    ObjectController::OnPossess(object);
    mPossessedCharacter = dynamic_cast<Character *>(object);
}
void PlayerController::OnUnPossess()
{

    ObjectController::OnUnPossess();
    mPossessedCharacter = nullptr;
}

void PlayerController::OnMouseCaptureLost() {}

void PlayerController::RegisterInputActionCallbacks()
{

    mInputActionTable["Move"].mExpectedType = Core::EActionValueType::eVector2;
    mInputActionTable["Move"].mCallbacks.Register([this](const InputActionValue &actionValue)
                                                  { OnMove(actionValue.mValue.XY()); });

    mInputActionTable["Look"].mExpectedType = Core::EActionValueType::eVector2;
    mInputActionTable["Look"].mCallbacks.Register([this](const InputActionValue &actionValue)
                                                  { OnLook(actionValue.mValue.XY()); });

    mInputActionTable["Jump"].mExpectedType = Core::EActionValueType::eFloat;
    mInputActionTable["Jump"].mCallbacks.Register([this](const InputActionValue &actionValue)
                                                  { OnJump(actionValue.mValue.X); });

    mInputActionTable["Light"].mExpectedType = Core::EActionValueType::eBool;
    mInputActionTable["Light"].mCallbacks.Register([this](const InputActionValue &actionValue)
                                                   { OnLight(actionValue.mValue.X); });
}

std::unordered_map<std::string, InputActionValue> PlayerController::CollectLocalInputActions()
{

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();
    if (inputSystem == nullptr)
        return {};

    std::unordered_map<std::string, InputActionValue> totalValueTable;

    for (const auto &keyAction : mKeyActionTable)
    {
        Quad::EKeyCode key = keyAction.first;

        const std::string &actionName = keyAction.second.mActionName;
        totalValueTable[actionName].mType = keyAction.second.mActionValue.mType;

        float keyValue = inputSystem->GetInputValue(key);
        totalValueTable[actionName].mValue += keyAction.second.mActionValue.mValue * keyValue;
    }

    return totalValueTable;
}

void PlayerController::PutInputAction(const std::string &actionName, const InputActionValue &actionValue)
{

    auto actionIt = mInputActionTable.find(actionName);

    if (actionIt == mInputActionTable.end())
        return;

    if (actionIt->second.mExpectedType != actionValue.mType)
        return;

    actionIt->second.mCallbacks.ExecuteCallbacks(actionValue);
}

void PlayerController::OnMove(const CoreMath::Vector2 &move)
{
    if (mPossessedCharacter == nullptr)
        return;
    mMove = move;
    if (mMove.NearEqual(CoreMath::Vector2::Zero))
        mPossessedCharacter->PlayClip("Asset/Take 0010");
    else
        mPossessedCharacter->PlayClip("Asset/Take 0011");
}

void PlayerController::OnLook(const CoreMath::Vector2 &look)
{
    if (mPossessedCharacter == nullptr)
        return;
    mPossessedCharacter->AddRotationLocal({0, look.X, 0});
}

void PlayerController::OnJump(float v)
{

    mJump = v;
}

void PlayerController::OnLight(bool v)
{

    if (mPossessedCharacter == nullptr)
        return;

    auto lightCom = mPossessedCharacter->GetComponent<LightComponent>();
    if (lightCom == nullptr)
        return;

    if (v && !mLight)
    {
        lightCom->SetLightEnabled(!lightCom->GetLightEnabled());
    }

    mLight = v;
}
