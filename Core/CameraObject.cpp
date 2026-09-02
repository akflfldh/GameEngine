#include "CameraObject.h"
#include <Core/CameraComponent.h>
#include <Core/Map.h>
#include <Core/ObjectPtr.h>
#include <InputSystem/InputSystem.h>
#include <Logger/Logger.h>
#include <sstream>

CameraObject::CameraObject()
{

    mCameraComponent = static_cast<CameraComponent *>(CreateDefaultComponent<CameraComponent>("CamCom"));
    mRootSceneComponent = mCameraComponent;
}

CameraObject::~CameraObject() {}

void CameraObject::OnBegin()
{

    ControllableEntity::OnBegin();

    // Map *map = GetMap();
    //   map->mMultiInputCallbackSystem.Register(this, &CameraObject::OnInput);
}

void CameraObject::Update(float DeltaTime)
{
    ControllableEntity::Update(DeltaTime);

    // if (Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance())
    //{
    //     if (inputSystem->IsRButtonDown() && )
    //     {
    //         std::pair<int, int> mouseMovementDelta = inputSystem->GetMouseDelta();

    //        if (mouseMovementDelta.first != 0 || mouseMovementDelta.second != 0)
    //            GetCameraComponent()->AddRotationLocal({mouseMovementDelta.second, mouseMovementDelta.first, 0});

    //        // left
    //        glm::vec3 moveVector = glm::zero<glm::vec3>();
    //        if (inputSystem->IsScanKeyDown(30))
    //        {
    //            moveVector -= GetCameraComponent()->GetRightWorld();
    //        }

    //        // right
    //        if (inputSystem->IsScanKeyDown(32))
    //        {
    //            moveVector += GetCameraComponent()->GetRightWorld();
    //        }

    //        // up
    //        if (inputSystem->IsScanKeyDown(17))
    //        {
    //            moveVector += GetCameraComponent()->GetForwardWorld();
    //        }

    //        // down
    //        if (inputSystem->IsScanKeyDown(31))
    //        {
    //            moveVector -= GetCameraComponent()->GetForwardWorld();
    //        }

    //        if (moveVector != glm::zero<glm::vec3>())
    //        {

    //            moveVector = glm::normalize(moveVector);
    //            moveVector *= mMoveSpeed * DeltaTime;

    //            GetCameraComponent()->AddMovementWorld(moveVector);
    //        }
    //    }
    //}
}
void CameraObject::OnInput(const Core::InputData &inputData)
{

    // if (inputData.mInputState & EInputState::eMouseMove)
    //{
    //     if (bMouseRDown && inputData.mouseMoveData.mAccumulateFlag)
    //     {
    //         GetCameraComponent()->AddRotationLocal({0, inputData.mouseMoveData.mDeltaX, 0});
    //         // deltaX를  회전 값에 사용
    //     }
    // }
}

CameraComponent *CameraObject::GetCameraComponent() const
{
    return mCameraComponent;
}

void CameraObject::SetTestValue(float v) {}

void CameraObject::Serialize(Arch &arch)
{
    ControllableEntity::Serialize(arch);

    if (mCameraComponent)
    {
        mCameraComponent->Serialize(arch);
    }
}
