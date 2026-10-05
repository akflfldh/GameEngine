#include "GameRuntimeMode.h"
#include <Core/AnimationSystem.h>
#include <Core/GameInstance.h>
#include <Core/Map.h>
#include <Core/ObjectController.h>
#include <Core/PhysicsBridgeSystem.h>
#include <Core/World.h>
#include <Logger/Logger.h>
#include <NetWork/NetWorkSystem.h>
#include <ReflectSystem/ReflectionSystem.h>
#include <core/GameNetworkSystem.h>

Quad::GameRuntimeMode::GameRuntimeMode() {}

Quad::GameRuntimeMode::~GameRuntimeMode() {}

void Quad::GameRuntimeMode::StartPlay()
{
    if (mGameInstance)
        return;

    // 세션 시작에서만 생성한다. BeginMap에서는 기존 인스턴스를 유지하므로 공통 UI가 다시 생성되지 않는다.
    // 사용자 DLL의 등록 타입을 사용하고, 이름이 같아도 GameInstance 파생 타입이 아니면 소유하지 않는다.
    auto *reflectionSystem = ReflectionSystem::GetInstance();
    BaseClass *instance = reflectionSystem->CreateClassInstance("UserGameInstance");
    GameInstance *gameInstance = dynamic_cast<GameInstance *>(instance);
    if (gameInstance == nullptr)
    {
        if (instance)
            reflectionSystem->DestoryClassInstance(instance);
        LOG_MESSAGE_ERROR("GameRuntimeMode",
                          "UserGameInstance 생성 실패: 사용자 DLL 등록 및 GameInstance 상속을 확인하세요.");
        return;
    }

    mGameInstance.reset(gameInstance);
    mGameInstance->SetCanvas(mCanvas);
    mGameInstance->SetWorld(mWorld);
    mGameInstance->Initialize();
    mGameInstance->Start();
}

void Quad::GameRuntimeMode::BeginMap(::Map *map)
{

    if (map == nullptr)
        return;

    map->Start();
    mGameMode.SetupPlay(map);

    Core::GameNetworkSystem::GetInstance()->BindPlayContext(&mGameMode, map);
    map->BeginPlay();
    Core::GameNetworkSystem::GetInstance()->BeginPlay();
}

void Quad::GameRuntimeMode::EndMap(::Map *map)
{
    // 맵 종료 콜백 이후 해당 플레이 컨텍스트만 해제한다. 연결과 세션 공통 UI는 다음 맵에서도 유지한다.
    // 맵 에셋의 소유권은 기존 경로에 남기므로 여기서 맵을 삭제하지 않는다.
    if (map)
        map->EndPlay();
    Core::GameNetworkSystem::GetInstance()->UnbindPlayContext();
}

void Quad::GameRuntimeMode::EndPlay(::Map *map)
{
    // 맵 객체와 UI 시스템이 유효할 때 세션의 UI와 외부 연결부터 정리한다.
    // reset은 일반 delete가 아니라 등록된 사용자 타입의 소멸자와 리플렉션 allocator 반환을 수행한다.
    if (mGameInstance)
    {
        mGameInstance->Shutdown();
        mGameInstance.reset();
    }

    EndMap(map);
}

void Quad::GameRuntimeMode::Update(::Map *map, float DeltaTime)
{

    if (!map)
        return;

    if (mGameInstance)
        mGameInstance->Update(DeltaTime);

    Core::GameNetworkSystem::GetInstance()->Update();
    if (NetWork::NetWorkSystem::GetInstance()->GetNetWorkRole() != NetWork::ENetWorkRole::eClient)
    {
        mGameMode.ProcessLocalInputActions(map);
    }

    map->Update(DeltaTime);

    auto animationSystem = Core::AnimationSystem::GetInstance();
    animationSystem->Update(DeltaTime);
    // 예측 없는 최소 버전에서는 클라이언트의 Transform을 호스트 상태로만 갱신한다.
    // 애니메이션 갱신은 유지하고, Host와 Offline에서만 물리 결과를 반영한다.
    PhysicsBridgeSystem *physicsBridgeSystem = map->GetWorld()->GetPhysicsBridgeSystem();
    if (physicsBridgeSystem &&
        NetWork::NetWorkSystem::GetInstance()->GetNetWorkRole() != NetWork::ENetWorkRole::eClient)
    {

        physicsBridgeSystem->Update(map, DeltaTime);
    }
}

void Quad::GameRuntimeMode::EndUpdate(::Map *map, float DeltaTime)
{

    if (map)
    {
        if (mGameInstance)
            mGameInstance->EndUpdate();

        Core::GameNetworkSystem::GetInstance()->EndUpdate();
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

void Quad::GameRuntimeMode::SetCanvas(UI::UICanvas *canvas)
{
    mCanvas = canvas;
}
void Quad::GameRuntimeMode::SetWorld(World *world)
{
    mWorld = world;
}

Core::MouseMode Quad::GameRuntimeMode::GetMouseMode(Map *map) const
{

    if (mGameInstance)
    {
        return mGameInstance->GetMouseMode();
    }

    return Core::MouseMode::Free;

    // if (map && map->GetObjectControllerNum() > 0)
    //     return map->GetCurrentObjectController()->GetMouseMode();

    // return map->GetGameModeSetting().mMouseMode;
}
