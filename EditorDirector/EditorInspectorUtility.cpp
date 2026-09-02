#include "EditorInspectorUtility.h"

#include <Core/Component.h>
#include <Core/Map.h>
#include <Core/Object.h>
#include <CoreAsset/Asset.h>
#include <CoreBase/BaseClass.h>

void Quad::CommitInspectorEdit(BaseClass *target)
{

    if (target)
    {
        target->FlushPropertyDirty();
    }

    if (Object *object = dynamic_cast<Object *>(target))
    {
        if (Map *map = object->GetMap())
        {
            map->MarkAssetDirty();
        }
    }
    else if (Component *com = dynamic_cast<Component *>(target))
    {
        Object *object = com->GetOwnerObject();

        if (object)
        {
            if (Map *map = object->GetMap())
            {
                map->MarkAssetDirty();
            }
        }
    }

    return;
}
