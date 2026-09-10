#pragma once

#include <Core/IEngineMode.h>

#include <Core/GameMode.h>

namespace Quad
{

class GameRuntimeMode : public Core::IEngineMode
{

  public:
    GameRuntimeMode();
    virtual ~GameRuntimeMode() override;

    virtual void Start(::Map *map) override;
    virtual void Update(::Map *map, float DeltaTime) override;
    virtual void EndUpdate(::Map *map, float DeltaTime) override;
    virtual void CleanUp(::Map *map) override;

    virtual ObjectController *GetCurrentObjectController(::Map *map) override;
    virtual CameraComponent *GetActiveCameraComponent(::Map *map) override;
    virtual void SetPause() override;
    virtual void ReleasePause() override;

  private:
    GameMode mGameMode;
};

} // namespace Quad