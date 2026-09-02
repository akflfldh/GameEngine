#include "EditorEditMode.h"
#include <Core/Component.h>
#include <Core/IHittable.h>
#include <Core/Map.h>
#include <Core/Object.h>
#include <Core/ObjectController.h>
#include <Core/World.h>
#include <EditorDirector/EditorSceneController.h>
#include <EditorDirector/EditorSceneManager.h>

EditorEditMode::EditorEditMode() : mShowDebugCollider(false)
{
    mEditorMap = new Map();
    // Quad::EditorSceneManager::GetInstance()->GetUserWorld()->Register(mEditorMap);
}

EditorEditMode::~EditorEditMode() {}

void EditorEditMode::Start(Map *map)
{
    if (map == nullptr)
        return;

    InitializeGizmo(mEditorMap);
    InitializeVisualizerManager(mEditorMap);
    mEditorMap->Start();
    map->Start();
}

void EditorEditMode::Update(Map *map, float DeltaTime)
{

    // Engein Object list update
    //
    for (auto object : map->GetEngineObjectList())
    {
        object->Update(DeltaTime);
    }

    if (mShowDebugCollider)
    {

        for (auto object : map->GetEntityList())
        {
            for (auto com : object->GetComponentList())
            {
                if (Core::IHittable *hittable = dynamic_cast<Core::IHittable *>(com))
                {
                    hittable->DrawDebugCollider();
                }
            }
        }
    }
    // else
    //{
    //     /*if (mEditorController)
    //     {
    //         mEditorController->Update(DeltaTime);
    //     }*/

    //    /*for (auto object : map->GetEngineObjectList())
    //    {
    //        object->Update(DeltaTime);
    //    }*/
    //}

    mEditorMap->Update(DeltaTime);
    if (mEditorCamearComponent)
        UpdateEditorVisualizerManager(mEditorCamearComponent);
}

void EditorEditMode::EndUpdate(Map *map, float DeltaTime)
{

    mEditorMap->EndUpdate(DeltaTime);

    if (map)
    {
        map->FlushPropertyDirty();
    }
}

void EditorEditMode::CleanUp(Map *map)
{
    if (map)
        map->CleanUp();

    if (mEditorMap)
        mEditorMap->CleanUp();
}

ObjectController *EditorEditMode::GetCurrentObjectController(Map *map)
{

    return mEditorController;

    return nullptr;
}

CameraComponent *EditorEditMode::GetActiveCameraComponent(Map *map)
{

    return mEditorCamearComponent;

    return nullptr;
}

void EditorEditMode::SetPause()
{

    mPlayState = false;
}

void EditorEditMode::ReleasePause()
{

    mPlayState = true;
}

Map *EditorEditMode::GetEditorMap() const
{
    return mEditorMap;
}

void EditorEditMode::SetShowDebugCollider(bool flag)
{

    mShowDebugCollider = flag;
}

bool EditorEditMode::GetShowDebugCollider() const
{
    return mShowDebugCollider;
}

void EditorEditMode::SetPlayState(bool state)
{

    mPlayState = state;
}

void EditorEditMode::SetEditorController(ObjectController *controller)
{

    mEditorController = controller;
    if (Quad::EditorSceneController *editorSceneController =
            dynamic_cast<Quad::EditorSceneController *>(mEditorController))
    {
        editorSceneController->SetEditorMap(mEditorMap);
    }
}

void EditorEditMode::SetEditorCameraIndex(size_t index)
{

    mEditorCameraIndex = index;
}

void EditorEditMode::SetEditorCameraComponent(CameraComponent *com)
{

    mEditorCamearComponent = com;
}
