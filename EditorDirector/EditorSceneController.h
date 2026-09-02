#pragma once
#include <Core/ObjectController.h>
#include <EditorDirector/TransformGizmo.h>

#include "EditorSceneController.generated.h"

class BaseSelectionManager;

namespace Quad
{
class REFLECT_CLASS(EngineClass) EditorSceneController : public ObjectController
{
    GENERATED_BODY(EditorSceneController)
  public:
    EditorSceneController();
    virtual ~EditorSceneController();

    /* EditorCameraController(EditorCameraController &&other) noexcept;
      EditorCameraController &operator=(EditorCameraController &&other) noexcept;*/

    //// 복사는 막는 것이 Pimpl에서 일반적 (unique_ptr 때문)
    // EditorCameraController(const EditorCameraController &) = delete;
    // EditorCameraController &operator=(const EditorCameraController &) = delete;

    void Intialize(BaseSelectionManager *selectionManager);

    virtual void CheckToggleInput(const Core::InputData &inputData) override;

#pragma region InputHandle

    // 포커스 되었을때 직접 rawData를 곧바로 받아 처리하는 입력처리메서드
    virtual bool HandleInput(const Quad::RawInputData &inputData) override;

    // 포커스 되지않을때
    virtual bool HandleInput(const Core::InputData &inputData) override;
    virtual void OnMouseCaptureLost() override;
#pragma endregion

    virtual void OnBegin() override;
    virtual void Tick(float deltaTime) override;

    void SetEditorMap(Map *editorMap);

  protected:
    virtual void OnPossess(ControllableEntity *object) override;
    virtual void OnUnPossess() override;

  private:
    void UpdateGizmoSize();

    // 외부에서 선택했을때
    void OnSelectedObject(Object *object);
    void OnSelectedComponent(Component *com);

    void OnDeleteInput();

  private:
    float mMoveSpeed;
    float mCurrentPitch;
    float mCurrentYaw;

    Object *mSelectedObject = nullptr;
    Core::CallbackID mSelectionCallbackID;
    Core::CallbackID mSelectionComponentCallbackID;

    BaseSelectionManager *mSelectionManager = nullptr;

    Map *mEditorMap = nullptr;
};
} // namespace Quad
