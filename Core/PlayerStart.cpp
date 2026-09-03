#include "PlayerStart.h"
#include <Core/Map.h>

PlayerStart::PlayerStart() {}

PlayerStart::~PlayerStart() {}

void PlayerStart::OnAddedToMap()
{

    if (Map *map = GetMap())
    {
        map->RegisterPlayerStart(this);
    }

    Entity::OnAddedToMap();
}

void PlayerStart::OnRemovedFromMap()
{

    if (Map *map = GetMap())
    {
        map->UnRegisterPlayerStart(this);
    }
    Entity::OnRemovedFromMap();
}
