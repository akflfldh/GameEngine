#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>
#include <CoreAsset/AssetType.h>
/*
GameMode의 역할:

-맵이 시작될때 오브젝트 컨트롤러 생성 or 찾기 , 플레이할 오브젝트 생성 or 찾기,

-오브젝트 컨트롤러가 오브젝트 소유하기(빙의Possess)

-활성화할 카메라 선택

-map에 오브젝트 컨트롤러 등록하기




 Player Object / Pawn
  -> 맵에 미리 배치 가능
  -> 없으면 GameMode가 spawn 가능

  Player Controller
  -> 맵에 미리 배치하지 않는 것을 기본 정책으로 추천
  -> GameMode가 play 시작 시 생성
  -> map runtime controller list에 등록
  -> player object를 possess




*/

class Map;
class ObjectController;
class Object;
class CameraComponent;
class PlayerStart;
class ControllableEntity;

// struct

// map
/*








*/

class CORE_API_LIB GameMode
{

  public:
    GameMode();
    virtual ~GameMode();

    virtual void SetupPlay(Map *map);

    //  void SetDefaultPlayerObjectClass(const std::string &className);
    //   void SetDefaultPlayerObjectPrefab(CoreAsset::AssetID prefabID);
    //    void ClearDefaultPlayerObject();

  protected:
    virtual ObjectController *ResolvePlayerController(Map *map);
    virtual ControllableEntity *ResolvePlayerObject(Map *map);
    virtual CameraComponent *ResolveActiveCamera(Map *map, Object *playerObject);

    virtual ObjectController *CreateDefaultPlayerController(Map *map, const Core::GameModeSetting &gameModeSetting);
    virtual ControllableEntity *CreateDefaultPlayerObject(Map *map, const Core::GameModeSetting &gameModeSetting);

    virtual void RegisterController(Map *map, ObjectController *controller);
    virtual void SetActiveCamera(Map *map, CameraComponent *camera);
    virtual CameraComponent *CreateFallbackCamera(Map *map);

    virtual PlayerStart *ResolvePlayerStart(Map *map);

  private:
    //    나중에 WorldSettings나 editor UI에서 GameMode 설정을 주입
    //   Core::ESpawnObjectSourceType mDefaultPlayerObjectSource = Core::ESpawnObjectSourceType::eNone;
    //   std::string mDefaultPlayerObjectClassName;
    //   CoreAsset::AssetID mDefaultPlayerObjectPrefabID = NoneAssetID;

    // 그러면 단순히 클래스이름은
};
