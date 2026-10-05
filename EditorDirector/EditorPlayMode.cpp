#include "EditorPlayMode.h"
#include <Core/AnimationSystem.h>
#include <Core/CameraComponent.h>
#include <Core/GameInstance.h>
#include <Core/GameNetworkSystem.h>
#include <Core/Map.h>
#include <Core/ObjectController.h>
#include <Core/PhysicsBridgeSystem.h>
#include <Core/World.h>
#include <Logger/Logger.h>
#include <NetWork/NetWorkSystem.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <UiSystem/UICanvas.h>

EditorPlayMode::EditorPlayMode() : mCanvas(nullptr), mWorld(nullptr)
{

    mEditorMap = new Map();
}

EditorPlayMode::~EditorPlayMode()
{

    delete mEditorMap;
}

void EditorPlayMode::StartPlay()
{
    if (mGameInstance)
        return;

    // GameInstance와 보조 편집 맵은 플레이 세션에 속하며, 사용자 맵이 바뀌어도 유지한다.
    // 사용자 DLL의 등록된 타입으로 생성하며, 같은 이름의 다른 타입을 GameInstance로 취급하지 않는다.
    auto *reflectionSystem = Quad::ReflectionSystem::GetInstance();
    BaseClass *instance = reflectionSystem->CreateClassInstance("UserGameInstance");
    GameInstance *gameInstance = dynamic_cast<GameInstance *>(instance);
    if (gameInstance)
    {
        mGameInstance.reset(gameInstance);
        mGameInstance->SetCanvas(mCanvas);
        mGameInstance->SetWorld(mWorld);
        mGameInstance->Initialize();
    }
    else
    {
        if (instance)
            reflectionSystem->DestoryClassInstance(instance);
        LOG_MESSAGE_ERROR("EditorPlayMode",
                          "UserGameInstance 생성 실패: 사용자 DLL 등록 및 GameInstance 상속을 확인하세요.");
    }

    InitializeGizmo(mEditorMap);
    mEditorMap->Start();
    if (mGameInstance)
        mGameInstance->Start();
}

void EditorPlayMode::BeginMap(::Map *map)
{
    if (map == nullptr)
        return;

    // GameMode와 네트워크의 플레이어 등록은 현재 맵의 객체를 필요로 하므로 맵 시작에 묶는다.
    map->Start();
    Core::GameNetworkSystem::GetInstance()->BindPlayContext(&mGameMode, map);

    mGameMode.SetupPlay(map);
    map->BeginPlay();
    Core::GameNetworkSystem::GetInstance()->BeginPlay();
}

void EditorPlayMode::EndMap(::Map *map)
{
    // 엔티티 종료 콜백이 기존 플레이 컨텍스트를 사용할 수 있도록 맵 종료 후 연결을 해제한다.
    // Unbind는 맵/GameMode 참조만 해제하며 네트워크 연결이나 GameInstance는 종료하지 않는다.
    if (map)
        map->EndPlay();
    Core::GameNetworkSystem::GetInstance()->UnbindPlayContext();

    // 플레이 맵의 삭제는 기존 SceneManager 소유 경로에 맡기고 이 모드의 비소유 참조만 정리한다.
    mEditorController = nullptr;
    mEditorCamearComponent = nullptr;
}

void EditorPlayMode::EndPlay(::Map *map)
{
    // 플레이 객체의 EndPlay/제거 전에 UI와 외부 콜백의 연결을 해제할 기회를 준다.
    // reset은 전용 deleter를 통해 등록된 사용자 타입의 소멸자와 리플렉션 allocator 반환을 수행한다.
    if (mGameInstance)
    {
        mGameInstance->Shutdown();
        mGameInstance.reset();
    }

    EndMap(map);

    if (mCanvas)
    {
        mCanvas->DestroyAllUIElements();
    }

    // 일시 정지 상태는 맵 종료가 아니라 플레이 세션 종료 시 초기화한다.
    mPaused = false;
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
        if (mGameInstance)
            mGameInstance->Update(DeltaTime);

        Core::GameNetworkSystem::GetInstance()->Update();
        if (NetWork::NetWorkSystem::GetInstance()->GetNetWorkRole() != NetWork::ENetWorkRole::eClient)
        {
            mGameMode.ProcessLocalInputActions(map);
        }

        map->Update(DeltaTime);

        // 에니메이션 업데이트
        Core::AnimationSystem *animationSystem = Core::AnimationSystem::GetInstance();

        animationSystem->Update(DeltaTime);

        // 독립 실행과 동일하게 클라이언트에서는 수신한 호스트 상태를 물리 계산으로 덮어쓰지 않는다.
        // Host와 Offline의 물리 갱신 및 클라이언트의 애니메이션 갱신은 유지한다.
        PhysicsBridgeSystem *physicsBridgeSystem = map->GetWorld()->GetPhysicsBridgeSystem();
        if (physicsBridgeSystem &&
            NetWork::NetWorkSystem::GetInstance()->GetNetWorkRole() != NetWork::ENetWorkRole::eClient)
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
        if (mGameInstance)
            mGameInstance->EndUpdate();

        Core::GameNetworkSystem::GetInstance()->EndUpdate();
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
Core::MouseMode EditorPlayMode::GetMouseMode(Map *map) const
{

    if (map == nullptr)
        return Core::MouseMode::Free;

    if (mGameInstance)
        return mGameInstance->GetMouseMode();

    return Core::MouseMode::Free;
}
void EditorPlayMode::SetCanvas(UI::UICanvas *canvas)
{
    mCanvas = canvas;
}
void EditorPlayMode::SetWorld(World *world)
{
    mWorld = world;
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
