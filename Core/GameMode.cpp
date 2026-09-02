#include "GameMode.h"
#include <Core/CameraComponent.h>
#include <Core/CameraObject.h>
#include <Core/ControllableEntity.h>
#include <Core/Map.h>
#include <Core/ObjectController.h>
#include <Core/PlayerStart.h>

GameMode::GameMode() {}

GameMode::~GameMode() {}

void GameMode::SetupPlay(Map *map)
{

    if (map == nullptr)
        return;

    const Core::GameModeSetting &gameModeSetting = map->GetGameModeSetting();

    ObjectController *objectController = ResolvePlayerController(map);

    if (objectController == nullptr)
    {
        objectController = CreateDefaultPlayerController(map, gameModeSetting);
    }

    ControllableEntity *playerObject = ResolvePlayerObject(map);
    if (playerObject == nullptr)
    {
        playerObject = CreateDefaultPlayerObject(map, gameModeSetting);
    }

    if (objectController)
    {

        RegisterController(map, objectController);
    }

    if (objectController && playerObject)
    {
        objectController->Possess(playerObject);
    }

    CameraComponent *cameraComponent = ResolveActiveCamera(map, playerObject);
    if (cameraComponent)
    {
        SetActiveCamera(map, cameraComponent);
    }
}

ObjectController *GameMode::ResolvePlayerController(Map *map)
{

    if (map == nullptr)
        return nullptr;

    if (map->GetObjectControllerNum() == 0)
        return nullptr;

    return map->GetCurrentObjectController();
}

ControllableEntity *GameMode::ResolvePlayerObject(Map *map)
{
    if (map == nullptr)
        return nullptr;

    //  나중에는 이름 대신 tag/component 기반으로 변경
    return dynamic_cast<ControllableEntity *>(map->GetEntity("Player"));
}

CameraComponent *GameMode::ResolveActiveCamera(Map *map, Object *playerObject)
{ /*
    1. player object 안의 CameraComponent를 우선 사용.
    2. 없으면 map에 이미 등록된 첫 번째 camera 사용.
    3. 그래도 없으면 fallback으로 editor camera를 사용하거나, runtime camera object를 생성.
    */

    if (playerObject)
    {
        CameraComponent *cameraCom = playerObject->GetComponent<CameraComponent>();
        if (cameraCom)
        {
            return cameraCom;
        }
    }

    if (map)
    {
        CameraComponent *cameraCom = map->GetCameraComponent(0);
        if (cameraCom)
        {
            return cameraCom;
        }
    }

    return CreateFallbackCamera(map);
}

ObjectController *GameMode::CreateDefaultPlayerController(Map *map, const Core::GameModeSetting &gameModeSetting)
{

    if (map == nullptr)
        return nullptr;

    // RuntimeEntity도 염두할것,
    // PlayerController는 런타임에만 씬에존재하고 저장시에는 존재하지않음으로

    const std::string controllerClassName = gameModeSetting.mDefaultPlayerControllerClassName.empty()
                                                ? "PlayerController"
                                                : gameModeSetting.mDefaultPlayerControllerClassName;

    Object *object = map->CreateEntity(controllerClassName.c_str(), "PlayerController");
    auto objectController = dynamic_cast<ObjectController *>(object);

    if (objectController == nullptr)
    {

        if (object)
        {
            object->Destroy();
        }
        return nullptr;
    }

    if (objectController)
        objectController->AddObjectFlag(Core::EObjectFlag::eTemporary);

    return objectController;
}

ControllableEntity *GameMode::CreateDefaultPlayerObject(Map *map, const Core::GameModeSetting &gameModeSetting)
{
    if (map == nullptr)
        return nullptr;

    if (gameModeSetting.mDefaultPlayerObjectSource == Core::ESpawnObjectSourceType::eNone)
        return nullptr;

    if (gameModeSetting.mDefaultPlayerObjectSource == Core::ESpawnObjectSourceType::eClass &&
        gameModeSetting.mDefaultPlayerObjectClassName.empty())
        return nullptr;

    if (gameModeSetting.mDefaultPlayerObjectSource == Core::ESpawnObjectSourceType::ePrefab &&
        gameModeSetting.mDefaultPlayerObjectPrefabID == NoneAssetID)
        return nullptr;

    PlayerStart *playerStart = ResolvePlayerStart(map);

    Core::SpawnRequestContext request;
    request.mClassName = gameModeSetting.mDefaultPlayerObjectClassName;
    request.mSpawnSourceType = gameModeSetting.mDefaultPlayerObjectSource;
    request.mPrefabID = gameModeSetting.mDefaultPlayerObjectPrefabID;
    request.mInstanceName = "Player";
    if (playerStart)
    {
        request.mPosition = playerStart->GetPositionWorld();
        request.mScale = playerStart->GetScaleWorld();
        request.mRotation = playerStart->GetRotationWorld();
    }

    Object *object = map->SpawnObject(request);
    ControllableEntity *controllableEntity = nullptr;
    if (object)
    {
        controllableEntity = dynamic_cast<ControllableEntity *>(object);
        if (controllableEntity == nullptr)
        {
            object->Destroy();
            object = nullptr;
        }
    }

    if (object)
    {

        object->AddObjectFlag(Core::EObjectFlag::eTemporary);
    }

    return controllableEntity;
}

void GameMode::RegisterController(Map *map, ObjectController *controller)
{
    map->RegisterObjectController(controller);
}

void GameMode::SetActiveCamera(Map *map, CameraComponent *camera)
{
    if (map == nullptr || camera == nullptr)
        return;

    map->SetActiveCamera(camera);
    return;
}

CameraComponent *GameMode::CreateFallbackCamera(Map *map)
{

    if (map == nullptr)
        return nullptr;

    auto cameraObject = map->CreateEntity<CameraObject>("CameraObject");

    if (cameraObject == nullptr)
    {
        return nullptr;
    }

    cameraObject->SetPositionLocal({0.0f, 2.0f, -10.0f});
    // cameraObject->SetFar(10025.5f);

    cameraObject->AddObjectFlag(Core::EObjectFlag::eTemporary);

    return cameraObject->GetCameraComponent();
}

PlayerStart *GameMode::ResolvePlayerStart(Map *map)
{
    if (map == nullptr)
        return nullptr;

    // 이후에는 map에서 playerstart를 캐싱해두는것도 고려해보자
    for (auto object : map->GetEntityList())
    {
        if (PlayerStart *playerStart = dynamic_cast<PlayerStart *>(object))
        {
            return playerStart;
        }
    }

    return nullptr;
}

// void GameMode::SetDefaultPlayerObjectClass(const std::string &className)
//{
//
//     mDefaultPlayerObjectSource = Core::ESpawnObjectSourceType::eClass;
//     mDefaultPlayerObjectPrefabID = NoneAssetID;
//     mDefaultPlayerObjectClassName = className;
// }
//
// void GameMode::SetDefaultPlayerObjectPrefab(CoreAsset::AssetID prefabID)
//{
//
//     mDefaultPlayerObjectSource = Core::ESpawnObjectSourceType::ePrefab;
//     mDefaultPlayerObjectClassName = "";
//     mDefaultPlayerObjectPrefabID = prefabID;
// }
//
// void GameMode::ClearDefaultPlayerObject()
//{
//
//     mDefaultPlayerObjectSource = Core::ESpawnObjectSourceType::eNone;
//     mDefaultPlayerObjectPrefabID = NoneAssetID;
//     mDefaultPlayerObjectClassName = "";
// }
