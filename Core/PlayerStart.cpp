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
}