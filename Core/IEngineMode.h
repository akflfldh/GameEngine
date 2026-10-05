#pragma once

#include <Core/CoreDllExport.h>
#include <Core/CoreType.h>

class Map;
class ObjectController;
class CameraComponent;

namespace Core
{

class CORE_API_LIB IEngineMode
{
  public:
    IEngineMode();
    virtual ~IEngineMode() = 0;

    // 플레이 세션의 상태를 준비한다. 맵 전환에서는 다시 호출하지 않는다.
    virtual void StartPlay() = 0;
    // 각 맵의 시작 처리를 수행한다. 편집 모드도 맵 준비에는 이 진입점을 사용한다.
    virtual void BeginMap(::Map *map) = 0;
    // 현재 맵의 동작과 참조만 종료한다. GameInstance와 플레이 세션은 다음 맵에서도 유지한다.
    virtual void EndMap(Map *map) = 0;

    // 맵 전환이 아닌 플레이 세션 종료다. map이 없어도 모드 소유 런타임 상태를 정리한다.
    virtual void EndPlay(::Map *map) = 0;
    virtual void Update(::Map *map, float DeltaTime) = 0;
    virtual void EndUpdate(::Map *map, float DeltaTime) = 0;
    virtual void CleanUp(::Map *map) = 0;

    virtual ObjectController *GetCurrentObjectController(::Map *map) = 0;
    virtual CameraComponent *GetActiveCameraComponent(::Map *map) = 0;
    virtual void SetPause() = 0;
    virtual void ReleasePause() = 0;

    virtual MouseMode GetMouseMode(Map *map) const = 0;

  private:
};

} // namespace Core
