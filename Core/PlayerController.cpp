#include "PlayerController.h"
#include <Core/Character.h>
#include <Core/CharacterMovementComponent.h>
#include <InputSystem/InputSystem.h>
PlayerController::PlayerController()
{

    // 런타임 플레이 컨트롤러는 active 활성화 기본

    mActiveState = false;
}

PlayerController ::~PlayerController() {}

void PlayerController ::Tick(float deltaTime)
{

    ObjectController::Tick(deltaTime);

    if (mPossessedCharacter == nullptr)
        return;

    Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance();
    if (inputSystem == nullptr)
        return;

    if (inputSystem->GetGameInputBlocked())
        return;

    CoreMath::Vector3 forward = mPossessedCharacter->GetForwardWorld();
    CoreMath::Vector3 right = mPossessedCharacter->GetRightWorld();
    CoreMath::Vector3 move = CoreMath::Vector3::Zero;
    if (inputSystem->IsVKeyDown('W'))
        move += forward;
    if (inputSystem->IsVKeyDown('S'))
        move -= forward;
    if (inputSystem->IsVKeyDown('A'))
        move -= right;
    if (inputSystem->IsVKeyDown('D'))
        move += right;

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
