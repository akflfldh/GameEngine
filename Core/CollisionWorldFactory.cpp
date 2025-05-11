#include "CollisionWorldFactory.h"

#include"ICollisionWorldFactoryImpl.h"

Quad::CollisionWorldFactory::CollisionWorldFactory(ICollisionWorldFactoryImpl* collisionWorldFactoryImpl)
    :mCollisionWorldFactoryImpl(collisionWorldFactoryImpl)
{
}

Quad::CollisionWorldFactory::~CollisionWorldFactory()
{


}

Quad::CollisionWorld* Quad::CollisionWorldFactory::CreateCollisionWorld(SpacePartitioningStructure<Collider>* spacePartitioningStructure)
{


    auto instance = GetInstance();

    return instance->mCollisionWorldFactoryImpl->CreateCollisionWorld(spacePartitioningStructure);
}

Quad::UiCollisionWorld* Quad::CollisionWorldFactory::CreateUiCollisionWorld(SpacePartitioningStructure<UiCollider>* spacePartitioningStructure)
{


    auto instance = GetInstance();

    return instance->mCollisionWorldFactoryImpl->CreateUiCollisionWorld(spacePartitioningStructure);




    return nullptr;
}

void Quad::CollisionWorldFactory::RemoveCollisionWorld(CollisionWorld* collisionWorld)
{



}

void Quad::CollisionWorldFactory::RemoveUiCollisionWorld(UiCollisionWorld* collisionWorld)
{






}
