#include "EditorMode.h"
#include <Core/Component.h>
#include <Core/IHittable.h>
#include <Core/Map.h>
#include <Core/Object.h>
#include <Core/ObjectController.h>
#include <Core/World.h>
#include <EditorDirector/EditorSceneManager.h>

// EditorMode::EditorMode() : mShowDebugCollider(false)
//{
//     mEditorMap = new Map();
//     // Quad::EditorSceneManager::GetInstance()->GetUserWorld()->Register(mEditorMap);
// }
//
// EditorMode::~EditorMode() {}
//
// void EditorMode::Start(Map *map)
//{
//     if (mPlayState)
//     {
//         map->Start();
//     }
//     else
//     {
//         mTransformGizmo.Initialize(mEditorMap);
//         mEditorMap->Start();
//     }
//
//     //   map->Start();
// }
//
// void EditorMode::Update(Map *map, float DeltaTime)
//{
//
//     if (mPlayState)
//     {
//
//         for (auto object : map->GetEntityList())
//         {
//             object->Update(DeltaTime);
//         }
//     }
//     else
//     {
//
//         // Engein Object list update
//         //
//         if (mShowDebugCollider)
//         {
//             for (auto object : map->GetEngineObjectList())
//             {
//                 object->Update(DeltaTime);
//             }
//
//             for (auto object : map->GetEntityList())
//             {
//                 for (auto com : object->GetComponentList())
//                 {
//                     if (Core::IHittable *hittable = dynamic_cast<Core::IHittable *>(com))
//                     {
//                         hittable->DrawDebugCollider();
//                     }
//                 }
//             }
//         }
//         else
//         {
//             if (mEditorController)
//             {
//                 mEditorController->Update(DeltaTime);
//             }
//
//             /*for (auto object : map->GetEngineObjectList())
//             {
//                 object->Update(DeltaTime);
//             }*/
//         }
//
//         mEditorMap->Update(DeltaTime);
//     }
// }
//
// void EditorMode::EndUpdate(Map *map, float DeltaTime)
//{
//
//     if (mPlayState)
//     {
//         map->EndUpdate(DeltaTime);
//     }
//     else
//     {
//
//         /* for (auto object : map->GetEngineObjectList())
//          {
//              object->EndUpdate(DeltaTime);
//          }*/
//
//         if (mEditorController)
//         {
//             mEditorController->Update(DeltaTime);
//         }
//
//         mEditorMap->EndUpdate(DeltaTime);
//     }
// }
//
// ObjectController *EditorMode::GetCurrentObjectController(Map *map)
//{
//     if (mPlayState)
//     {
//
//         if (map)
//         {
//             return map->GetCurrentObjectController();
//         }
//     }
//     else
//     {
//         return mEditorController;
//     }
//
//     return nullptr;
// }
//
// CameraComponent *EditorMode::GetActiveCameraComponent(Map *map)
//{
//
//     if (mPlayState)
//     {
//         if (map)
//         {
//             return map->GetCameraComponent();
//         }
//     }
//     else
//     {
//         return mEditorCamearComponent;
//     }
//
//     return nullptr;
// }
//
// void EditorMode::SetPause()
//{
//
//     mPlayState = false;
// }
//
// void EditorMode::ReleasePause()
//{
//
//     mPlayState = true;
// }
//
// Map *EditorMode::GetEditorMap() const
//{
//     return mEditorMap;
// }

EditorMode::EditorMode() {}

EditorMode::~EditorMode() {}

Quad::TransformGizmo &EditorMode::GetTransformGizmo()
{
    return mTransformGizmo;
    // TODO: 여기에 return 문을 삽입합니다.
}
void EditorMode::InitializeGizmo(Map *map)
{

    mTransformGizmo.Initialize(map);
}

void EditorMode::InitializeVisualizerManager(Map *map)
{

    mEditorVisualizerManager.Initialize(map);
}

void EditorMode::BindSourceMapToVisualizerManager(Map *map)
{

    mEditorVisualizerManager.BindMap(map);
}

void EditorMode::UpdateEditorVisualizerManager(CameraComponent *com)
{

    mEditorVisualizerManager.Update(com);
}

//
// void EditorMode::SetShowDebugCollider(bool flag)
//{
//
//    mShowDebugCollider = flag;
//}
//
// bool EditorMode::GetShowDebugCollider() const
//{
//    return mShowDebugCollider;
//}
//
// void EditorMode::SetPlayState(bool state)
//{
//
//    mPlayState = state;
//}
//
// void EditorMode::SetEditorController(ObjectController *controller)
//{
//
//    mEditorController = controller;
//}
//
// void EditorMode::SetEditorCameraIndex(size_t index)
//{
//
//    mEditorCameraIndex = index;
//}
//
// void EditorMode::SetEditorCameraComponent(CameraComponent *com)
//{
//
//    mEditorCamearComponent = com;
//}
