#pragma once

#include"ICollisionWorldFactoryImpl.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB DefaultCollisionWorldFactoryImpl:public ICollisionWorldFactoryImpl
	{
	public:
		DefaultCollisionWorldFactoryImpl() = default;
		virtual ~DefaultCollisionWorldFactoryImpl() ;

		virtual CollisionWorld* CreateCollisionWorld(SpacePartitioningStructure<Collider>* partitioingStructure);
		virtual UiCollisionWorld* CreateUiCollisionWorld(SpacePartitioningStructure<UiCollider>* partitioingStructure);
		virtual void RemoveCollisionWorld(CollisionWorld* collisionWorld) ;



	private:

	};

}
