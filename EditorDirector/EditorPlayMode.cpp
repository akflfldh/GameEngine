#include "EditorPlayMode.h"
#include <Core/CameraComponent.h>
#include <Core/Map.h>
#include <Core/ObjectController.h>
#include <Core/PhysicsBridgeSystem.h>
#include <Core/World.h>

EditorPlayMode::EditorPlayMode()
{

    mEditorMap = new Map();
}

EditorPlayMode::~EditorPlayMode()
{

    delete mEditorMap;
}

void EditorPlayMode::Start(::Map *map)
{

    if (map == nullptr)
        return;

    InitializeGizmo(mEditorMap);
    mEditorMap->Start();

    map->Start();
    mGameMode.SetupPlay(map);
    map->BeginPlay();

    // edit map을 생성해서 그곳에서 기즈모를 생성할지, 그러면 물리충돌등이없을거같기도 하고
    //  아니면 동일한맵에 엔진엔티티로 생성할지
    //  InitializeGizmo(map);
}

void EditorPlayMode::Update(::Map *map, float DeltaTime)
{
    if (mPaused)
    {
        UpdateEditorObjects(DeltaTime);
        mEditorMap->Update(DeltaTime);
        return;
    }

    if (map)
    {

        map->Update(DeltaTime);

        // 물리업데이트
        PhysicsBridgeSystem *physicsBridgeSystem = map->GetWorld()->GetPhysicsBridgeSystem();
        if (physicsBridgeSystem)
        {
            physicsBridgeSystem->Update(map, DeltaTime);
        }
    }
}

void EditorPlayMode::EndUpdate(::Map *map, float DeltaTime)
{

    if (mPaused)
    {
        EndUpdateEditorObjects(DeltaTime);
        mEditorMap->EndUpdate(DeltaTime);
        return;
    }

    if (map)
    {
        map->EndUpdate(DeltaTime);
    }
}

void EditorPlayMode::CleanUp(Map *map)
{

    if (map)
        map->CleanUp();

    if (mEditorMap)
        mEditorMap->CleanUp();
}

ObjectController *EditorPlayMode::GetCurrentObjectController(::Map *map)
{

    if (mPaused)
        return mEditorController;

    if (map && map->GetObjectControllerNum() > 0)
        return map->GetCurrentObjectController();

    return mEditorController;
}

CameraComponent *EditorPlayMode::GetActiveCameraComponent(::Map *map)
{
    if (mPaused)
    {
        return mEditorCamearComponent;
    }

    if (map && !map->GetCameraComList().empty())
    {
        return map->GetActiveCameraComponent();
    }

    return mEditorCamearComponent;
}

void EditorPlayMode::SetPause()
{

    mPaused = true;
}

void EditorPlayMode::ReleasePause()
{

    mPaused = false;
}

bool EditorPlayMode::IsPaused() const
{
    return mPaused;
}

void EditorPlayMode::SetEditorController(ObjectController *controller)
{
    mEditorController = controller;
}
void EditorPlayMode::SetEditorCameraIndex(size_t index) {}
void EditorPlayMode::SetEditorCameraComponent(CameraComponent *com)
{

    mEditorCamearComponent = com;
}

void EditorPlayMode::UpdateEditorObjects(float DeltaTime)
{
    if (mEditorController)
    {
        mEditorController->Update(DeltaTime);
    }

    // if (mEditorMap)
    //{
    //     mEditorMap->Update(DeltaTime);
    // }
}

void EditorPlayMode::EndUpdateEditorObjects(float DeltaTime)
{

    if (mEditorController)
    {
        mEditorController->EndUpdate(DeltaTime);
    }
}
