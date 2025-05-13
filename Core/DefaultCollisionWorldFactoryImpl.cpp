#include "DefaultCollisionWorldFactoryImpl.h"

#include"Collision/CollisionWorld.h"
#include"Collision/UiCollisionWorld.h"
#include"SpacePartitioningStructureFactory.h"

Quad::DefaultCollisionWorldFactoryImpl::~DefaultCollisionWorldFactoryImpl()
{
}

Quad::CollisionWorld* Quad::DefaultCollisionWorldFactoryImpl::CreateCollisionWorld(SpacePartitioningStructure<Collider>* partitioingStructure)
{
	CollisionWorld* collisionWorld = new CollisionWorld(partitioingStructure);

	return collisionWorld;
}

Quad::UiCollisionWorld* Quad::DefaultCollisionWorldFactoryImpl::CreateUiCollisionWorld(SpacePartitioningStructure<UiCollider>* partitioingStructure)
{

	UiCollisionWorld* collisionWorld = new UiCollisionWorld(partitioingStructure);

	return collisionWorld;
}

void Quad::DefaultCollisionWorldFactoryImpl::RemoveCollisionWorld(CollisionWorld* collisionWorld)
{
	auto spacePartitioningStructureFactory = SpacePartitioningStructureFactory<Collider>::GetInstance();
	spacePartitioningStructureFactory->RemoveSpacePartitioningStructure(collisionWorld->GetSpacePartitioningStructure());

	delete collisionWorld;


}
