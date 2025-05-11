#include "EditorUiCollisionWorld.h"

#include"EditorUiSpacePartitioningStructure.h"

Quad::EditorUiCollisionWorld::EditorUiCollisionWorld(EditorUiSpacePartitioningStructure* partitioningStructure)
    :UiCollisionWorld(partitioningStructure)
{
}

bool Quad::EditorUiCollisionWorld::RayCastingFirstNotIncludeEditorObject(Object*& oObject, Ray& ray)
{
    
    return    RayCastingFirst(oObject, ray);

}
