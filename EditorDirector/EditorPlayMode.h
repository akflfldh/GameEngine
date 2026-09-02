#pragma once
#include "EditorDirector/EditorMode.h"
#include <Core/GameMode.h>

class EditorPlayMode : public EditorMode
{
  public:
    EditorPlayMode();
    virtual ~EditorPlayMode();

    virtual void Start(::Map *map) override;
    virtual void Update(::Map *map, float DeltaTime) override;
    virtual void EndUpdate(::Map *map, float DeltaTime) override;
    virtual void CleanUp(::Map *map) override;

    virtual ObjectController *GetCurrentObjectController(::Map *map) override;
    virtual CameraComponent *GetActiveCameraComponent(::Map *map) override;
    virtual void SetPause() override;
    virtual void ReleasePause() override;

    bool IsPaused() const;

    void SetEditorController(ObjectController *controller);
    void SetEditorCameraIndex(size_t index);
    void SetEditorCameraComponent(CameraComponent *com);

  private:
    void UpdateEditorObjects(float DeltaTime);
    void EndUpdateEditorObjects(float DeltaTime);

  private:
    GameMode mGameMode;

    bool mPaused = false;
    ObjectController *mEditorController = nullptr;
    // size_t mEditorCameraIndex = 0;
    CameraComponent *mEditorCamearComponent = nullptr;

    Map *mEditorMap = nullptr;
};
