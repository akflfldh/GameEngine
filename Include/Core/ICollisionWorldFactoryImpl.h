#pragma once


#include"Predeclare.h"
#include"Core//CoreDllExport.h"

namespace Quad
{
	template<typename collider>
	class SpacePartitioningStructure;

	class CORE_API_LIB ICollisionWorldFactoryImpl
	{
	public:
		ICollisionWorldFactoryImpl() = default;
		virtual ~ICollisionWorldFactoryImpl() =0;

		virtual CollisionWorld* CreateCollisionWorld(SpacePartitioningStructure<Collider>* partitioingStructure) = 0;
		virtual UiCollisionWorld* CreateUiCollisionWorld(SpacePartitioningStructure<UiCollider>* partitioingStructure) = 0;
		virtual void RemoveCollisionWorld(CollisionWorld* collisionWorld) = 0;


	private:

	};

}
