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
    mEditorPlayMode.SetWorld(mUserPlayWorld);
    mUserPlayWorld->mOnMapChangedCallbackSystem.Register([this](Map *map) { OnUserPlayMapChanged(map); });

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

void Quad::EditorSceneManager::EndFrame()
{
    for (const auto &element : mWorldTable)
    {
        World *world = element.second;
        if (world->GetActiveState())
            world->EndFrame();
    }
}

void Quad::EditorSceneManager::AddUserMap(Map *map)
{
    mWorldTable[mUserWorldName]->Register(map);

    mOnUserMapAddedCallbackSystem.ExecuteCallbacks(map);
}

World *Quad::EditorSceneManager::GetUserWorld() const
{
    auto it = mWorldTable.find(mUserWorldName);

    if (it != mWorldTable.cend())
        return it->second;
    return nullptr;
}

void Quad::EditorSceneManager::SetEditorMap(Map *map)
{
    mEditorMap = map;
}

std::vector<Map *> Quad::EditorSceneManager::GetUserMapList() const
{
    World *userWorld = GetUserWorld();
    if (userWorld == nullptr)
        return {};

    std::vector<Map *> userMaps;
    const auto registeredMaps = userWorld->GetMapList();
    userMaps.reserve(registeredMaps.size());

    // EditorMap은 기즈모 등의 동작을 위해 월드에 유지하되, 사용자에게 선택할 프로젝트 맵으로 노출하지 않는다.
    // 이름이나 AssetID 대신 등록된 보조 맵의 객체 identity로 구분한다.
    for (Map *map : registeredMaps)
    {
        if (map != nullptr && map != mEditorMap)
            userMaps.push_back(map);
    }

    return userMaps;
}

std::vector<std::string> Quad::EditorSceneManager::GetUserMapNameList() const
{
    const auto userMaps = GetUserMapList();
    std::vector<std::string> mapNames;
    mapNames.reserve(userMaps.size());

    for (Map *map : userMaps)
        mapNames.push_back(map->GetName().c_str());

    return mapNames;
}

void Quad::EditorSceneManager::PlayUserWorld()
{

    if (mPlaySessionState != EPlaySessionState::eStopped)
        return;

    EditorSelectionManager::GetInstance()->ClearSelection();

    // UserWorld 직렬화
    World *userWorld = GetUserWorld();

    auto mapList = GetUserMapList();
    Map *currentMap = userWorld->GetCurrentMap();
    Map *mCurrentPlayMap = nullptr;
    for (int mapIndex = 0; mapIndex < mapList.size(); ++mapIndex)
    {
        Map *map = mapList[mapIndex];
        BinaryArch archWrite(false);

        archWrite.Start();
        map->Serialize(archWrite);
        if (currentMap == map)
            map->SerilaizeRawData(archWrite);

        uint8_t *pData = archWrite.GetBufferFromMemory();
        size_t bufferSize = archWrite.GetBufferSize();

        BinaryArch archLoad(true);
        archLoad.StartRead(pData, bufferSize);

        Map *playMap = new Map;

        playMap->SetAssetDirtyActive(false);

        playMap->Serialize(archLoad);
        if (currentMap == map)
            playMap->SerilaizeRawData(archLoad);

        mUserPlayWorld->Register(playMap);
        if (currentMap == map)
            mUserPlayWorld->SetCurrentMap(playMap);

        mUserPlayMapList.push_back(playMap);
        // 생성한월드 play
        archWrite.End();
        archLoad.End();
    }

    EditorDirector::GetInstance()->GetMainSceneWindow()->SetDebugGridRender(false);
    // Map *map = userWorld->GetCurrentMap();

    //// Write
    // BinaryArch archWrite(false);

    // archWrite.Start();
    // map->Serialize(archWrite);
    // map->SerilaizeRawData(archWrite);

    // uint8_t *pData = archWrite.GetBufferFromMemory();
    // size_t bufferSize = archWrite.GetBufferSize();

    //// Load
    // BinaryArch archLoad(true);
    // archLoad.StartRead(pData, bufferSize);
    //// 생성한 world에서 역직렬화

    // Map *playMap = new Map;
    // mUserPlayWorld->Register(playMap);
    // mUserPlayWorld->SetCurrentMap(playMap);
    // playMap->SetAssetDirtyActive(false);

    // playMap->Serialize(archLoad);
    // playMap->SerilaizeRawData(archLoad);

    // mUserPlayMapList.push_back(playMap);
    //// 생성한월드 play
    // archWrite.End();
    // archLoad.End();

    // mUserMapCameraComponent = userWorld->GetCurrentCameraCom();
    // mUserMapObjectController = userWorld->GetCurrentObjectController();
    userWorld->SetActiveState(false);
    mUserPlayWorld->SetActiveState(true);
    EditorDirector::GetInstance()->GetMainSceneWindow()->SetWorld(mUserPlayWorld);

    // CameraObject *camObject =
    //     static_cast<CameraObject *>(mCurrentPlayMap->CreateEngineEntity<CameraObject>("EdtorCamera"));

    // camObject->GetCameraComponent()->SetPositionLocal({0, 0, -10});
    // camObject->GetCameraComponent()->SetFar(10025.5f);
    // auto editorCameraController =
    // mCurrentPlayMap->CreateEngineEntity<EditorSceneController>("EditorCameraController");
    // editorCameraController->Possess(camObject);
    // mCurrentPlayMap->SetActiveCameraIndex(0);

    // editorCameraController->Intialize(EditorSelectionManager::GetInstance());

    // mEditorPlayMode.SetEditorController(editorCameraController);
    // mEditorPlayMode.SetEditorCameraComponent(camObject->GetCameraComponent());

    OnUserPlayMapChanged(mUserPlayWorld->GetCurrentMap());

    // 세션 상태를 먼저 만들고 현재 맵의 객체와 GameMode를 시작한다.
    mUserPlayWorld->StartPlay();
    mUserPlayWorld->BeginMap();

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

    // GameInstance를 먼저 정리한 뒤 플레이 맵을 종료한다. 실제 맵 삭제는 아래 기존 경로에서 수행한다.
    mUserPlayWorld->EndPlay();

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

    EditorDirector::GetInstance()->GetMainSceneWindow()->SetDebugGridRender(true);
}

void Quad::EditorSceneManager::SetUserCanvas(UI::UICanvas *canvas)
{

    mEditorPlayMode.SetCanvas(canvas);
}

void Quad::EditorSceneManager::OnUserPlayMapChanged(Map *map)
{

    CameraObject *camObject = static_cast<CameraObject *>(map->CreateEngineEntity<CameraObject>("EdtorCamera"));

    camObject->GetCameraComponent()->SetPositionLocal({0, 0, -10});
    camObject->GetCameraComponent()->SetFar(10025.5f);
    auto editorCameraController = map->CreateEngineEntity<EditorSceneController>("EditorCameraController");
    editorCameraController->Possess(camObject);
    //  map->SetActiveCameraIndex(0);

    editorCameraController->Intialize(EditorSelectionManager::GetInstance());

    mEditorPlayMode.SetEditorController(editorCameraController);
    mEditorPlayMode.SetEditorCameraComponent(camObject->GetCameraComponent());
}
