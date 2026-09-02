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

    // virtual void Start(::Map *map) override;
    // virtual void Update(::Map *map, float DeltaTime) override;
    // virtual void EndUpdate(::Map *map, float DeltaTime) override;
    // virtual ObjectController *GetCurrentObjectController(::Map *map) override;
    // virtual CameraComponent *GetActiveCameraComponent(::Map *map) override;
    // virtual void SetPause() override;
    // virtual void ReleasePause() override;

    // Map *GetEditorMap() const;

    Quad::TransformGizmo &GetTransformGizmo();
    void InitializeGizmo(Map *map);

    void InitializeVisualizerManager(Map *map);
    void BindSourceMapToVisualizerManager(Map *map);

    // void SetShowDebugCollider(bool flag);
    // bool GetShowDebugCollider() const;

    // void SetPlayState(bool state);

    // void SetEditorController(ObjectController *controller);
    // void SetEditorCameraIndex(size_t index);
    // void SetEditorCameraComponent(CameraComponent *com);

  protected:
    void UpdateEditorVisualizerManager(CameraComponent *com);

  private:
    Quad::TransformGizmo mTransformGizmo;
    //  CameraComponentVisualizer mCameraComponentVisualizer;
    //   LightComponentVisualizer mLightComponentVisualizer;

    EditorVisualizerManager mEditorVisualizerManager;
};
