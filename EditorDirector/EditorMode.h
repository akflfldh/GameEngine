#pragma once

#include <Core/IEngineMode.h>
#include <EditorDirector/EditorVisualizerManager.h>
#include <EditorDirector/TransformGizmo.h>

class ObjectController;
class CameraObject;
class Map;

class EditorMode : public Core::IEngineMode
{
  public:
    EditorMode();
    ~EditorMode();

    // virtual void StartPlay() override;
    // virtual void BeginMap(::Map *map) override;
    // virtual void Update(::Map *map, float DeltaTime) override;
    // virtual void EndUpdate(::Map *map, float DeltaTime) override;
    // virtual ObjectController *GetCurrentObjectController(::Map *map) override;
    // virtual CameraComponent *GetActiveCameraComponent(::Map *map) override;
    // virtual void SetPause() override;
    // virtual void ReleasePause() override;

    // Map *GetEditorMap() const;

    Quad::TransformGizmo &GetTransformGizmo();
    // BeginMap 전에 설정하는 워크스페이스 옵션이다. 생성과 컨트롤러의 기즈모 접근에 함께 적용한다.
    void SetUseGizmo(bool state);
    bool GetUseGizmo() const;
    void InitializeGizmo(Map *map);

    void InitializeVisualizerManager(Map *map);
    void BindSourceMapToVisualizerManager(Map *map, bool bUseLightVisualizer = true);

    // void SetShowDebugCollider(bool flag);
    // bool GetShowDebugCollider() const;

    // void SetPlayState(bool state);

    // void SetEditorController(ObjectController *controller);
    // void SetEditorCameraIndex(size_t index);
    // void SetEditorCameraComponent(CameraComponent *com);
    Core ::MouseMode GetMouseMode(Map *map) const override;

  protected:
    void UpdateEditorVisualizerManager(CameraComponent *com);

  private:
    Quad::TransformGizmo mTransformGizmo;
    bool mUseGizmo = true;
    //  CameraComponentVisualizer mCameraComponentVisualizer;
    //   LightComponentVisualizer mLightComponentVisualizer;

    EditorVisualizerManager mEditorVisualizerManager;
};
