#include "EditorDirector/EditorMapManager.h"


#include"EditorRuntimeUserObjectManager.h"
#include"EditorStaticUserObjectManager.h"
#include"EditorEngineObjectManager.h"

#include"EditorMap.h"


Quad::Map* Quad::EditorMapManager::CreateGamePlayMap(System* system, const std::string& mapName, bool isRuntimeMap)
{

    Map* map = nullptr;

    if (isRuntimeMap)
    {
        map = new Map;
        map->mObjectManager = EditorRuntimeUserObjectManager::GetInstance();
    }
    else
    {
        map = new Map ; //EditorMap
        map->mObjectManager = EditorStaticUserObjectManager::GetInstance();
    }

    map->SetName(mapName);
    map->Initialize(system, isRuntimeMap);
    map->SetRuntimeMapFlag(isRuntimeMap);

    return map;
}

Quad::Map* Quad::EditorMapManager::CreateEngineSystemMap(System* system, const std::string& mapName, bool isRuntimeMap)
{
    Map* map = nullptr;

    if (isRuntimeMap)
    {  
        map = new Map;
        map->mObjectManager = EditorEngineObjectManager::GetInstance();
    }
    map->SetName(mapName);
    map->Initialize(system, isRuntimeMap);
    map->SetRuntimeMapFlag(isRuntimeMap);

    return map;
}

void Quad::EditorMapManager::RemoveMap(Map* map)
{

    delete map;

}
