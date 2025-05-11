#include "GameMapInstanceGenerator.h"

#include"Map/Map.h"
#include"ObjectManager/RuntimeGameObjectManager.h"



Quad::Map* Quad::GameMapInstanceGenerator::CreateMap(System* system, const std::string& mapName)
{
    Map* map = new Map;

    map->mObjectManager = RuntimeGameObjectManager::GetInstance();

    map->SetName(mapName);
    map->Initialize(system, true);
    map->SetRuntimeMapFlag(true);
    map->mSystem = system;

    return map;
}
