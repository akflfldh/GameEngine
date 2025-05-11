#pragma once


#include"Utility/Singleton.h"
#include"Predeclare.h"
#include"Core/CoreDllExport.h"

#include"SpacePartitioningStructure.h"
namespace Quad
{

	class CORE_API_LIB CollisionWorldFactory:public Singleton<CollisionWorldFactory>
	{
	public:
		CollisionWorldFactory(ICollisionWorldFactoryImpl* collisionWorldFactoryImpl);
		~CollisionWorldFactory();


		static CollisionWorld* CreateCollisionWorld(SpacePartitioningStructure<Collider> * spacePartitioningStructure);
		static UiCollisionWorld* CreateUiCollisionWorld(SpacePartitioningStructure<UiCollider> * spacePartitioningStructure);

		static void RemoveCollisionWorld(CollisionWorld* collisionWorld);
		static void RemoveUiCollisionWorld(UiCollisionWorld* collisionWorld);
		//static void Remove

	private:

		ICollisionWorldFactoryImpl* mCollisionWorldFactoryImpl;

	};
}

