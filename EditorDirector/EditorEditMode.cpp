#include "EditorEditMode.h"
#include <Core/Component.h>
#include <Core/IHittable.h>
#include <Core/Map.h>
#include <Core/Object.h>
#include <Core/ObjectController.h>
#include <Core/World.h>
#include <EditorDirector/EditorSceneController.h>
#include <EditorDirector/EditorSceneManager.h>

EditorEditMode::EditorEditMode() : mShowDebugCollider(false), mIsBegunEditorMap(false)
{
    mEditorMap = new Map();
    // Quad::EditorSceneManager::GetInstance()->GetUserWorld()->Register(mEditorMap);
}

EditorEditMode::~EditorEditMode() {}

void EditorEditMode::StartPlay()
{
    // 편집 모드에는 GameInstance를 소유하는 플레이 세션이 없다.
}

void EditorEditMode::BeginMap(Map *map)
{
    if (map == nullptr)
        return;

    // 맵 전환은 렌더링 목록을 새 맵으로 교체하므로, 기즈모/visualizer용 맵을 명시적으로 함께 렌더링한다.
    if (World *world = map->GetWorld())
        world->AddRenderingMap(mEditorMap);

    if (mIsBegunEditorMap == false)
    {
        InitializeGizmo(mEditorMap);
        InitializeVisualizerManager(mEditorMap);
        mEditorMap->Start();
        mIsBegunEditorMap = true;
    }
    map->Start();
}

void EditorEditMode::EndMap(Map *)
{
    // 편집 맵은 플레이를 시작하지 않았으므로 EndPlay로 원본 엔티티를 정리하지 않는다.
    // 이전 맵의 카메라/컨트롤러 참조만 해제하고, 보조 맵과 기즈모는 다음 편집 맵에서도 유지한다.
    mEditorController = nullptr;
    mEditorCamearComponent = nullptr;
    if (mIsBegunEditorMap)
    {
        GetTransformGizmo().SetActive(false);
    }
}

void EditorEditMode::EndPlay(Map *)
{
    // 편집 모드는 플레이 세션을 시작하지 않으므로 편집 중인 맵과 UI를 유지한다.
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
