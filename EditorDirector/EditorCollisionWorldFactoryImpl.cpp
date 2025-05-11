#include "EditorCollisionWorldFactoryImpl.h"
#include"EditorCollisionWorld.h"
#include"EditorUiCollisionWorld.h"

#include"EditorSpacePartitioningStructure.h"
#include"EditorUiSpacePartitioningStructure.h"
#include"SpacePartitioningStructureFactory.h"
Quad::EditorCollisionWorldFactoryImpl::~EditorCollisionWorldFactoryImpl()
{
}

Quad::CollisionWorld* Quad::EditorCollisionWorldFactoryImpl::CreateCollisionWorld(SpacePartitioningStructure<Collider>* partitioingStructure)
{

	return new EditorCollisionWorld(static_cast<EditorSpacePartitioningStructure*>(partitioingStructure));
}

Quad::UiCollisionWorld* Quad::EditorCollisionWorldFactoryImpl::CreateUiCollisionWorld(SpacePartitioningStructure<UiCollider>* partitioingStructure)
{

	return new EditorUiCollisionWorld(static_cast<EditorUiSpacePartitioningStructure*>(partitioingStructure));

}

void Quad::EditorCollisionWorldFactoryImpl::RemoveCollisionWorld(CollisionWorld* collisionWorld)
{

	auto spacePartitioningStructureFactory = SpacePartitioningStructureFactory<Collider>::GetInstance();
	spacePartitioningStructureFactory->RemoveSpacePartitioningStructure(collisionWorld->GetSpacePartitioningStructure());


	delete collisionWorld;

}
