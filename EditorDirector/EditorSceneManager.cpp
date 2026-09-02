#include "EditorSceneManager.h"
#include <Core/CameraComponent.h>
#include <Core/CameraObject.h>
#include <Core/LogicalWindow.h>
#include <Core/Map.h>
#include <Core/World.h>
#include <CoreAsset/AssetManager.h>
#include <CoreBase/BinaryArch.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/EditorMode.h>
#include <EditorDirector/EditorSceneController.h>
#include <EditorSelectionManager.h>
#include <RenderFrontend/ObjectRenderItemBuilder.h>

Quad::EditorSceneManager *Quad::EditorSceneManager::GetInstance()
{
    static EditorSceneManager instance;
    return &instance;
}

Quad::EditorSceneManager::EditorSceneManager() : mUserPlayWorld(nullptr)
{

    RegisterWorld(mUserWorldName, new World);

    // World  생성
    mUserPlayWorld = new World;
    mUserPlayWorld->SetActiveState(false);
    mUserPlayWorld->SetEngineMode(&mEditorPlayMode);

    RegisterWorld("UserPlayWorld", mUserPlayWorld);
}

Quad::EditorSceneManager::~EditorSceneManager()
{

    for (auto map : mUserPlayMapList)
    {
        delete map;
    }

    delete mWorldTable[mUserWorldName];

    delete mUserPlayWorld;
}

void Quad::EditorSceneManager::RegisterWorld(const std::string &name, World *world)
{

    mWorldTable[name] = world;
}

void Quad::EditorSceneManager::UnRegisterWorld(World *world)
{

    for (auto it = mWorldTable.begin(); it != mWorldTable.end(); ++it)
    {
        if (it->second == world)
        {
            mWorldTable.erase(it);
            break;
        }
    }
}

World *Quad::EditorSceneManager::GetWorld(const std::string &name) const
{
    auto it = mWorldTable.find(name);

    if (it != mWorldTable.cend())
        return it->second;

    return nullptr;
}

void Quad::EditorSceneManager::Update(float deltaTime)
{

    for (const auto &element : mWorldTable)
    {
        World *world = element.second;

        if (world->GetActiveState())
            world->Update(deltaTime);
    }
}

void Quad::EditorSceneManager::EndUpdate(float deltaTime)
{
    for (const auto &element : mWorldTable)
    {
        World *world = element.second;
        if (world->GetActiveState())
            world->EndUpdate(deltaTime);
    }
}

void Quad::EditorSceneManager::CleanUp()
{

    for (const auto &element : mWorldTable)
    {
        World *world = element.second;
        if (world->GetActiveState())
            world->CleanUp();
    }
}

void Quad::EditorSceneManager::AddUserMap(Map *map)
{
    mWorldTable[mUserWorldName]->Register(map);
}

World *Quad::EditorSceneManager::GetUserWorld() const
{
    auto it = mWorldTable.find(mUserWorldName);

    if (it != mWorldTable.cend())
        return it->second;
    return nullptr;
}

void Quad::EditorSceneManager::PlayUserWorld()
{

    if (mPlaySessionState != EPlaySessionState::eStopped)
        return;

    EditorSelectionManager::GetInstance()->ClearSelection();

    // UserWorld 직렬화
    World *userWorld = GetUserWorld();
    Map *map = userWorld->GetCurrentMap();

    // Write
    BinaryArch archWrite(false);

    archWrite.Start();
    map->Serialize(archWrite);
    map->SerilaizeRawData(archWrite);

    uint8_t *pData = archWrite.GetBufferFromMemory();
    size_t bufferSize = archWrite.GetBufferSize();

    // Load
    BinaryArch archLoad(true);
    archLoad.Start(pData, bufferSize);
    // 생성한 world에서 역직렬화

    Map *playMap = new Map;
    mUserPlayWorld->Register(playMap);
    mUserPlayWorld->SetCurrentMap(playMap);
    playMap->SetAssetDirtyActive(false);

    playMap->Serialize(archLoad);
    playMap->SerilaizeRawData(archLoad);

    mUserPlayMapList.push_back(playMap);
    // 생성한월드 play
    archWrite.End();
    archLoad.End();

    mUserMapCameraComponent = userWorld->GetCurrentCameraCom();
    mUserMapObjectController = userWorld->GetCurrentObjectController();

    CameraObject *camObject = static_cast<CameraObject *>(playMap->CreateEngineEntity<CameraObject>("EdtorCamera"));

    camObject->GetCameraComponent()->SetPositionLocal({0, 0, -10});
    camObject->GetCameraComponent()->SetFar(10025.5f);
    auto editorCameraController = playMap->CreateEngineEntity<EditorSceneController>("EditorCameraController");
    editorCameraController->Possess(camObject);
    playMap->SetActiveCameraIndex(0);

    editorCameraController->Intialize(EditorSelectionManager::GetInstance());

    userWorld->SetActiveState(false);
    mUserPlayWorld->SetActiveState(true);
    EditorDirector::GetInstance()->GetMainSceneWindow()->SetWorld(mUserPlayWorld);

    mEditorPlayMode.SetEditorController(editorCameraController);
    mEditorPlayMode.SetEditorCameraComponent(camObject->GetCameraComponent());
    // world begin
    mUserPlayWorld->StartMap();

    mPlaySessionState = EPlaySessionState::ePlaying;
}

void Quad::EditorSceneManager::PauseUserWorld()
{

    if (mPlaySessionState != EPlaySessionState::ePlaying)
        return;

    mPlaySessionState = EPlaySessionState::ePaused;
    mUserPlayWorld->SetPause();
    Map *map = mUserPlayWorld->GetCurrentMap();
}

void Quad::EditorSceneManager::ReleaseUserWorldPause()
{

    if (mPlaySessionState == EPlaySessionState::eStopped)
        return;

    EditorSelectionManager::GetInstance()->ClearSelection();

    mPlaySessionState = EPlaySessionState::ePlaying;
    mUserPlayWorld->ReleasePause();
}

void Quad::EditorSceneManager::EndUserWorld()
{
    if (mPlaySessionState == EPlaySessionState::eStopped)
        return;

    EditorSelectionManager::GetInstance()->ClearSelection();

    if (Map *map = mUserPlayWorld->GetCurrentMap())
    {
        map->EndPlay();
    }

    mEditorPlayMode.ReleasePause();
    mPlaySessionState = EPlaySessionState::eStopped;

    // 생성했던 월드 제거
    World *userWorld = GetUserWorld();
    userWorld->SetActiveState(true);
    // EditorMode *editorMode = static_cast<EditorMode *>(userWorld->GetEngineMode());
    // editorMode->SetPlayState(false);

    //  editorMode->SetEditorController(mUserMapObjectController);
    //  editorMode->SetEditorCameraComponent(mUserMapCameraComponent);
    // 기존에있던 controller로 재설정해야함.

    mUserPlayWorld->SetActiveState(false);
    EditorDirector::GetInstance()->GetMainSceneWindow()->SetWorld(userWorld);
    mUserPlayWorld->UnRegisterMapAll();

    for (auto map : mUserPlayMapList)
    {
        delete map;
    }
    mUserPlayMapList.clear();
}
