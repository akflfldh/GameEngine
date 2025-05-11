#pragma once

#include"Core/ICollisionWorldFactoryImpl.h"

#include"SpacePartitioningStructure.h"
namespace Quad
{
	class EditorCollisionWorldFactoryImpl:public ICollisionWorldFactoryImpl
	{
	public:
		EditorCollisionWorldFactoryImpl() = default;
		virtual ~EditorCollisionWorldFactoryImpl();

		virtual CollisionWorld* CreateCollisionWorld(SpacePartitioningStructure<Collider>* partitioingStructure)override;
		virtual UiCollisionWorld* CreateUiCollisionWorld(SpacePartitioningStructure<UiCollider>* partitioingStructure)override;
		virtual void RemoveCollisionWorld(CollisionWorld* collisionWorld) override;
	private:



	};

}
