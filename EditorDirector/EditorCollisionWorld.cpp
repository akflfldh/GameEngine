#include "EditorCollisionWorld.h"
#include"EditorSpacePartitioningStructure.h"
#include"Collision/Collider.h"


Quad::EditorCollisionWorld::EditorCollisionWorld(EditorSpacePartitioningStructure* partitioingStructure)
	:CollisionWorld(partitioingStructure)
{
}

void Quad::EditorCollisionWorld::AddGizmoCollider(Collider* collider)
{
	EditorSpacePartitioningStructure* partitioningStructure = static_cast<EditorSpacePartitioningStructure*>(GetSpacePartitioningStructure());
	


//	partitioningStructure->AddCollider(collider);
	partitioningStructure->AddGizmoCollider(collider);
	


}

bool Quad::EditorCollisionWorld::RayCastingFirstNotIncludeEditorObject(Object*& oObject, Ray& ray)
{

	EditorSpacePartitioningStructure * partitioningStructure =	static_cast<EditorSpacePartitioningStructure*>(GetSpacePartitioningStructure());


	Collider* collider = nullptr;
	bool ret =partitioningStructure->RayCastingFirstNotIncludeEditorObject(collider, ray);

	if (ret)
	{
		oObject = collider->GetDestObject();
	}
	return ret;
}
