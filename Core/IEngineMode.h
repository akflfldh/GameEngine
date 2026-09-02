#pragma once

#include <Core/CoreDllExport.h>

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

    virtual void Start(::Map *map) = 0;
    virtual void Update(::Map *map, float DeltaTime) = 0;
    virtual void EndUpdate(::Map *map, float DeltaTime) = 0;
    virtual void CleanUp(::Map *map) = 0;

    virtual ObjectController *GetCurrentObjectController(::Map *map) = 0;
    virtual CameraComponent *GetActiveCameraComponent(::Map *map) = 0;
    virtual void SetPause() = 0;
    virtual void ReleasePause() = 0;

  private:
};

} // namespace Core