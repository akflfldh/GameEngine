#pragma once
#include "EditorDirector/EditorMode.h"
#include <Core/GameInstance.h>
#include <Core/GameMode.h>

#include <memory>

namespace UI
{
class UICanvas;
}

class EditorPlayMode : public EditorMode
{
  public:
    EditorPlayMode();
    virtual ~EditorPlayMode();

    virtual void StartPlay() override;
    virtual void BeginMap(::Map *map) override;
    virtual void EndMap(::Map *map) override;
    virtual void EndPlay(::Map *map) override;
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

    Core::MouseMode GetMouseMode(Map *map) const override;
    void SetCanvas(UI::UICanvas *canvas);

    void SetWorld(World *world);

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
    World *mWorld = nullptr;
    std::unique_ptr<GameInstance, ReflectionGameInstanceDeleter> mGameInstance;

    UI::UICanvas *mCanvas;
};
