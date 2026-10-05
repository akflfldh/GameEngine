#pragma once

#include <Core/IEngineMode.h>

#include <Core/GameInstance.h>
#include <Core/GameMode.h>
#include <memory>

namespace UI
{
class UICanvas;
}

namespace Quad
{

// 독립 실행의 플레이 생명주기를 조립한다. GameInstance는 세션 동안 소유하고 GameMode는 맵 시작에 사용한다.
// Canvas는 GameDirector가 UIManager를 통해 준비하여 전달하며, 이 모드는 이를 삭제하지 않는다.
class GameRuntimeMode : public Core::IEngineMode
{

  public:
    GameRuntimeMode();
    virtual ~GameRuntimeMode() override;

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
    virtual Core::MouseMode GetMouseMode(Map *map) const override;

    void SetCanvas(UI::UICanvas *canvas);
    void SetWorld(World *world);

  private:
    GameMode mGameMode;
    std::unique_ptr<GameInstance, ReflectionGameInstanceDeleter> mGameInstance;
    // GameInstance의 Shutdown이 끝날 때까지 유효해야 하는 비소유 참조다.
    UI::UICanvas *mCanvas = nullptr;
    World *mWorld = nullptr;
};

} // namespace Quad
