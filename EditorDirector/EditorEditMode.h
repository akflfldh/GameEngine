#pragma once

#include <EditorDirector/EditorMode.h>

class EditorEditMode : public EditorMode
{
  public:
    EditorEditMode();
    ~EditorEditMode();

    virtual void Start(::Map *map) override;
    virtual void Update(::Map *map, float DeltaTime) override;
    virtual void EndUpdate(::Map *map, float DeltaTime) override;
    virtual void CleanUp(::Map *map) override;
    virtual ObjectController *GetCurrentObjectController(::Map *map) override;
    virtual CameraComponent *GetActiveCameraComponent(::Map *map) override;
    virtual void SetPause() override;
    virtual void ReleasePause() override;

    Map *GetEditorMap() const;

    void SetShowDebugCollider(bool flag);
    bool GetShowDebugCollider() const;

    void SetPlayState(bool state);

    void SetEditorController(ObjectController *controller);
    void SetEditorCameraIndex(size_t index);
    void SetEditorCameraComponent(CameraComponent *com);

  private:
    Map *mEditorMap;
    bool mShowDebugCollider;
    bool mPlayState = false;

    ObjectController *mEditorController = nullptr;
    size_t mEditorCameraIndex = 0;
    CameraComponent *mEditorCamearComponent = nullptr;
};
