#include "GameRuntimeMode.h"
#include <Core/Map.h>
#include <Core/PhysicsBridgeSystem.h>
#include <Core/World.h>

Quad::GameRuntimeMode::GameRuntimeMode() {}

Quad::GameRuntimeMode::~GameRuntimeMode() {}

void Quad::GameRuntimeMode::Start(::Map *map)
{

    if (map == nullptr)
        return;

    map->Start();
    mGameMode.SetupPlay(map);
    map->BeginPlay();
}

void Quad::GameRuntimeMode::Update(::Map *map, float DeltaTime)
{

    if (!map)
        return;

    map->Update(DeltaTime);

    // 물리업데이트
    PhysicsBridgeSystem *physicsBridgeSystem = map->GetWorld()->GetPhysicsBridgeSystem();
    if (physicsBridgeSystem)
    {
        physicsBridgeSystem->Update(map, DeltaTime);
    }
}

void Quad::GameRuntimeMode::EndUpdate(::Map *map, float DeltaTime)
{

    if (map)
    {
        map->EndUpdate(DeltaTime);
    }
}

void Quad::GameRuntimeMode::CleanUp(::Map *map)
{

    if (map)
        map->CleanUp();
}

ObjectController *Quad::GameRuntimeMode::GetCurrentObjectController(::Map *map)
{
    // if (mPaused)
    //  return mEditorController;

    if (map && map->GetObjectControllerNum() > 0)
        return map->GetCurrentObjectController();

    return nullptr;
    //  return mEditorController;
}

CameraComponent *Quad::GameRuntimeMode::GetActiveCameraComponent(::Map *map)
{

    if (map && !map->GetCameraComList().empty())
    {
        return map->GetActiveCameraComponent();
    }

    return nullptr;
}

void Quad::GameRuntimeMode::SetPause() {}

void Quad::GameRuntimeMode::ReleasePause() {}
