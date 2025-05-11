#include "GameMapSystem.h"

#include"Map/Map.h"

#include"ObjectManager/RuntimeObjectManager.h"
#include"ObjectManager/RuntimeGameObjectManager.h"

Quad::Map* Quad::GameMapSystem::CreateMap(System* system, const std::string& mapName, bool isUserGameSystem)
{
    Map* map = new Map;

    if (isUserGameSystem)
    {
        map->mObjectManager = RuntimeGameObjectManager::GetInstance();
    }
    else
    {
        map->mObjectManager = RuntimeObjectManager::GetInstance();
        
    }
    map->SetName(mapName);
    map->Initialize(system,true);
    map->SetRuntimeMapFlag(true);
    map->mSystem = system;
    
    return map;

}
