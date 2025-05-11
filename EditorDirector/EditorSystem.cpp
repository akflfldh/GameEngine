#include "EditorDirector/EditorSystem.h"


#include"EditorMap.h"
#include"ObjectManager/ObjectManager.h"
#include"ObjectManager/RuntimeObjectManager.h"
#include"ObjectManager/EditGameObjectManager.h"
#include"ObjectManager/RuntimeGameObjectManager.h"

Quad::Map* Quad::EditorSystem::CreateMap(System* system, const std::string& mapName,bool isRuntimeMap, bool isUserGameSystem)
{
    Map* map = nullptr;
    
    if (isUserGameSystem)
    {

        if (isRuntimeMap)
        {
            map = new Map;
            map->mObjectManager = RuntimeGameObjectManager::GetInstance();
            
        }
        else
        {
            map = new EditorMap;
            map->mObjectManager = EditGameObjectManager::GetInstance();
        }

    }
    else
    {
        if (isRuntimeMap)
        {
            map = new Map;
            map->mObjectManager = RuntimeObjectManager::GetInstance();

        }else
        {
            map = new Map;
            map->mObjectManager = EditObjectManager::GetInstance();

        }
    }
    map->SetName(mapName);
    map->Initialize(system, isRuntimeMap);
    map->SetRuntimeMapFlag(isRuntimeMap);
    map->mSystem = system;
    return map;
}
