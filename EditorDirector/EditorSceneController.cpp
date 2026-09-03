#include "EditorDirector/EditorSceneController.h"

#include <Core/CameraComponent.h>
#include <Core/CameraObject.h>
#include <Core/Map.h>
#include <Core/World.h>
#include <EditorDeleteCommand.h>
#include <EditorDirector/EditorEditMode.h>
#include <EditorDirector/EditorMode.h>
#include <EditorDirector/EditorSelectionManager.h>
#include <GlobalAppHelper.h>
#include <IEditorVisualizerObject.h>
#include <InputSystem/InputSystem.h>

Quad::EditorSceneController::EditorSceneController() : mMoveSpeed(30), mCurrentPitch(0.0f), mCurrentYaw(0.0f)
{

    // auto selectionManager = EditorSelectionManager::GetInstance();
}
Quad::EditorSceneController::~EditorSceneController()
{
    //  auto selectionManager = EditorSelectionManager::GetInstance();
    mSelectionManager->mOnSelectedObjectCallbackSystem.UnRegister(mSelectionCallbackID);
    mSelectionManager->mOnSelectedComponentCallbackSystem.UnRegister(mSelectionComponentCallbackID);
    if (mSelectedObject)
    {
        mSelectionManager->SetSelectedObject(nullptr);
    }
}
void Quad::EditorSceneController::Intialize(BaseSelectionManager *selectionManager)
{

    mSelectionManager = selectionManager;
    mSelectionCallbackID = mSelectionManager->mOnSelectedObjectCallbackSystem.Register([this](Object *object)
                                                                                       { OnSelectedObject(object); });

    mSelectionComponentCallbackID = mSelectionManager->mOnSelectedComponentCallbackSystem.Register(
        [this](Component *com) { OnSelectedComponent(com); });
}

void Quad::EditorSceneController::CheckToggleInput(const Core::InputData &inputData)
{

    // 오른쪽 마우스 버튼 down - > 활성화 ,
    if (inputData.mRawInputData.mInputState & EInputState::eMouseRButtonDown)
    {
        InputSystem *inputSystem = InputSystem::GetInstance();

        inputSystem->SetMouseCapture(this);

        mActiveState = true;
    }
}
bool Quad::EditorSceneController::HandleInput(const Quad::RawInputData &inputData)
{
    InputSystem *inputSystem = InputSystem::GetInstance();

    // CheckToggleInput(inputData);

    if (inputData.mInputState & EInputState::eMouseRButtonUp)
    {
        mActiveState = false;
        inputSystem->ReleaseMouseCapture();
        return true;
    }

    if (inputSystem->IsMouseCaptured(this))
    {
        CameraObject *cameraObject = static_cast<CameraObject *>(GetPossessObject());

        if (cameraObject == nullptr)
            return false;
        CameraComponent *cameraCom = cameraObject->GetCameraComponent();

        if (inputData.mInputState & EInputState::eMouseMove)
        {
            if (inputData.mouseMoveData.mAccumulateFlag)
            {
                float deltaX = inputData.mouseMoveData.mDeltaX;
                float deltaY = inputData.mouseMoveData.mDeltaY;

                mCurrentPitch += deltaY;
                mCurrentYaw += deltaX;

                if (mCurrentPitch > 89.0f)
                    mCurrentPitch = 89.0f;
                else if (mCurrentPitch < -89.0f)
                    mCurrentPitch = -89.0f;

                cameraCom->SetQuaternionLocal(CoreMath::Quaternion::MakeFromEuler({mCurrentPitch, mCurrentYaw, 0.0f}));
            }
        }

        return true;
    }

    return false;
}

bool Quad::EditorSceneController::HandleInput(const Core::InputData &inputData)
{
    auto &transformGizmo = static_cast<EditorMode *>(GetWorld()->GetEngineMode())->GetTransformGizmo();
    if (inputData.mRawInputData.mInputState & EInputState::eMouseLButtonDown)
    {

        // Map 에서 Ray hit
        Map *map = GetMap();
        if (map == nullptr)
        {
            return true;
        }

        // 기즈모 먼저 Ray hit

        // 기즈모가 활성화되어있다면
        if (transformGizmo.GetActiveState())
        {
            Core::HitResult gizmoHitResult;
            if (transformGizmo.RayHit(inputData.mWorldRay, gizmoHitResult))
            {

                constexpr uint8_t controlVirtualKey = 0x11;
                InputSystem *inputSystem = InputSystem::GetInstance();
                const bool duplicateRequested = inputSystem->IsVKeyDown(controlVirtualKey);

                // 오브젝트선택 + ctrl 키눌름상태에서 조작이라면 복사본생성후, 그 복사본을 기즈모의 타킷으로한다.
                if (duplicateRequested && !transformGizmo.GetComponentControlState())
                {

                    if (dynamic_cast<EditorEditMode *>(GetWorld()->GetEngineMode()) == nullptr)
                        return true;

                    Object *sourceObject = mSelectionManager->GetSelectedObject();
                    if (sourceObject == nullptr || sourceObject->HasObjectFlag(Core::EObjectFlag::eEngineEntity))
                        return true;

                    Object *duplicatedObject = map->DuplicateEntity(sourceObject);
                    if (duplicatedObject == nullptr)
                        return true;

                    // 복사본을 선택되게한다.
                    mSelectionManager->SetSelectedObject(duplicatedObject);
                }

                //       기즈모 선택플래그 킴
                transformGizmo.SetSelectState(true, inputData.mWorldRay);
                return true;
            }
        }

        // editorSceneController이니
        // editormap을 알고있어도 문제없고
        // 1.editorMap rayhit

        if (mEditorMap)
        {
            Core::HitResult hitResult;
            bool bHit = mEditorMap->RayHit(inputData.mWorldRay, hitResult);
            bool editorHitHandle = false;
            if (bHit)
            {
                IEditorVisualizerObject *visualObject = dynamic_cast<IEditorVisualizerObject *>(hitResult.mHitObject);

                if (visualObject)
                {
                    visualObject->SelectSource(mSelectionManager);

                    editorHitHandle = true;
                }
            }

            // editor visualizerobject hit 실패시 일반 사용자 map에서 레이판정을 수행을 이어간다.
            if (!editorHitHandle)
            {

                // 일반 object Ray hit
                Core::HitResult hitResult;
                bool bHit = map->RayHit(inputData.mWorldRay, hitResult);

                if (bHit)
                {

                    if (transformGizmo.GetComponentControlState())
                    {
                        mSelectionManager->SetSelectedComponent(hitResult.mHitComponent);
                    }
                    else
                    {
                        mSelectionManager->SetSelectedObject(hitResult.mHitObject);
                    }

                    //// 기즈모 활성화 (보여지고 ,피킹가능해짐)
                    // transformGizmo.SetActive(true);
                    // transformGizmo.SetTargetObject(static_cast<Core::Entity *>(hitResult.mHitObject));
                    // transformGizmo.SetSelectState(false, inputData.mWorldRay);
                    // UpdateGizmoSize();
                }
                else
                {
                    // 아무것도 피킹되지않았다.?
                    // 만약 gizmo가 활성화된상태여서보인다면
                    // 비활성화한다.
                    if (transformGizmo.GetActiveState())
                    {
                        transformGizmo.SetActive(false);
                        transformGizmo.SetTargetObject(nullptr);
                    }
                }
            }
        }
    }
    else if (inputData.mRawInputData.mInputState & EInputState::eMouseMove)
    {

        if (transformGizmo.GetSelectState())
        {

            // Ctrl키 눌린상태인지?
            // 오브젝트 복사요청,기즈모의 타켓을 해당복사본으로 설정
            // 복사는 어디서 수행하는지? Map인지?    아니면 어디에서 복사를 해주는가?

            transformGizmo.OnMouseMove(inputData, inputData.mWorldRay);

            // 기즈모에게 입력 정보전달
            // 기즈모는 내부적으로 처리
        }
    }
    else if (inputData.mRawInputData.mInputState & EInputState::eMouseLButtonUp)
    {
        // 만약 기즈모가 선택된 상태였다면 기즈모 선택해제

        if (transformGizmo.GetSelectState())
        {
            transformGizmo.SetSelectState(false, inputData.mWorldRay);
        }
    }
    else if (inputData.mRawInputData.mInputState & EInputState::eKeyDown)
    {
        Quad::EKeyCode keycode = inputData.mRawInputData.keyEvent.mKeyCode;
        switch (keycode)
        {
        case Quad::EKeyCode::eT:
        {
            transformGizmo.SetMode(EGizmoMode::eTranslation);
        }
        break;
        case Quad::EKeyCode::eR:
        {
            transformGizmo.SetMode(EGizmoMode::eRotation);
        }
        break;
        case Quad::EKeyCode::eE:
        {
            transformGizmo.SetMode(EGizmoMode::eScale);
        }
        break;
        case Quad::EKeyCode::eDel:
        {
            OnDeleteInput();
        }
        }
    }

    return true;
}

void Quad::EditorSceneController::OnMouseCaptureLost() {}

void Quad::EditorSceneController::OnBegin()
{

    ObjectController::OnBegin();
}

void Quad::EditorSceneController::Tick(float deltaTime)
{

    InputSystem *inputSystem = InputSystem::GetInstance();
    if (inputSystem && inputSystem->IsMouseCaptured(this))
    {

        CameraObject *cameraObject = static_cast<CameraObject *>(GetPossessObject());

        if (cameraObject == nullptr)
            return;

        CameraComponent *cameraCom = cameraObject->GetCameraComponent();

        if (Quad::InputSystem *inputSystem = Quad::InputSystem::GetInstance())
        {
            if (inputSystem->IsRButtonDown())
            {

                // left
                CoreMath::Vector3 moveVector = CoreMath::Vector3::Zero;
                if (inputSystem->IsVKeyDown('A'))
                {
                    moveVector -= cameraCom->GetRightWorld();
                }

                // right
                if (inputSystem->IsVKeyDown('D'))
                {
                    moveVector += cameraCom->GetRightWorld();
                }

                // up
                if (inputSystem->IsVKeyDown('W'))
                {
                    moveVector += cameraCom->GetForwardWorld();
                }

                // down
                if (inputSystem->IsVKeyDown('S'))
                {
                    moveVector -= cameraCom->GetForwardWorld();
                }

                if (moveVector != CoreMath::Vector3::Zero)
                {

                    moveVector.Normalize();
                    moveVector *= mMoveSpeed * deltaTime;

                    cameraCom->AddMovementWorld(moveVector);

                    UpdateGizmoSize();
                }
            }
        }
    }
}

void Quad::EditorSceneController::OnPossess(ControllableEntity *object)
{
    ObjectController::OnPossess(object);
}

void Quad::EditorSceneController::OnUnPossess()
{
    ObjectController::OnUnPossess();
}

void Quad::EditorSceneController::UpdateGizmoSize()
{
    CameraObject *cameraObject = static_cast<CameraObject *>(GetPossessObject());

    if (cameraObject == nullptr)
        return;

    CameraComponent *cameraCom = cameraObject->GetCameraComponent();
    if (cameraCom)
    {
        // Editor Mode를 중간단계로 기즈모 공통으로  기존 EditorMode를 EditorEditMode, EditorPlayMode로
        auto &transformGizmo = static_cast<EditorMode *>(GetWorld()->GetEngineMode())->GetTransformGizmo();

        // 카메라와 기즈모사이의 길이
        transformGizmo.UpdateGizmoSize(cameraCom->GetPositionWorld());
    }
}
void Quad::EditorSceneController::OnSelectedObject(Object *object)
{
    auto &transformGizmo = static_cast<EditorMode *>(GetWorld()->GetEngineMode())->GetTransformGizmo();

    if (object)
    {

        // 기즈모 활성화 (보여지고 ,피킹가능해짐)
        transformGizmo.SetActive(true);
        transformGizmo.SetTargetObject(static_cast<Entity *>(object));
        transformGizmo.SetSelectStateFalse();
        UpdateGizmoSize();
    }
    else
    {
        transformGizmo.SetActive(false);
        transformGizmo.SetTargetObject(nullptr);
    }

    mSelectedObject = object;
}

void Quad::EditorSceneController::OnSelectedComponent(Component *com)
{

    auto &transformGizmo = static_cast<EditorMode *>(GetWorld()->GetEngineMode())->GetTransformGizmo();
    transformGizmo.SetTargetComponent(com);
}

void Quad::EditorSceneController::SetEditorMap(Map *editorMap)
{

    mEditorMap = editorMap;
}

void Quad::EditorSceneController::OnDeleteInput()
{

    if (EditorDeleteCommand::Execute(mSelectionManager))
    {
        /*   auto &transformGizmo = static_cast<EditorMode *>(GetWorld()->GetEngineMode())->GetTransformGizmo();
           transformGizmo.SetTargetObject(nullptr);
           transformGizmo.SetActive(false);
           transformGizmo.SetSelectStateFalse();*/
    }

    // if (mSelectedObject)
    //{
    //     mSelectedObject->Destroy();
    //     mSelectedObject = nullptr;
    // }
}
