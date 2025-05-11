#pragma once

#include"Predeclare.h"
#include<string>

#include"CoreDllExport.h"

#include"Utility/Singleton.h"
#include"SpacePartitioningStructure.h"
#include"Core/ISpacePartitioningStructureFactoryImpl.h"
namespace Quad
{

	template<typename collider>
	class  SpacePartitioningStructureFactory
	{
	public:
		SpacePartitioningStructureFactory(ISpacePartitioningStructureFactoryImpl* factoryImpl);
		static SpacePartitioningStructureFactory* GetInstance(ISpacePartitioningStructureFactoryImpl* factoryImpl);

		static SpacePartitioningStructure<collider>* CreateSpacePartitioningStructure(const std::string & className);

		static void RemoveSpacePartitioningStructure(SpacePartitioningStructure<collider>* spaceStructure);

	private:
		ISpacePartitioningStructureFactoryImpl* mFactoryImpl;

	};

	template<>
	class  SpacePartitioningStructureFactory<Collider>
	{
	public:
		static SpacePartitioningStructureFactory<Collider>* GetInstance(ISpacePartitioningStructureFactoryImpl* factoryImpl =nullptr);

		static SpacePartitioningStructure<Collider>* CreateSpacePartitioningStructure(const std::string& className);

		static void RemoveSpacePartitioningStructure(SpacePartitioningStructure<Collider>* spaceStructure);

	private:
		SpacePartitioningStructureFactory(ISpacePartitioningStructureFactoryImpl* factoryImpl);

		ISpacePartitioningStructureFactoryImpl* mFactoryImpl;

	};

	template<>
	class  SpacePartitioningStructureFactory<UiCollider>
	{
	public:
		
		static SpacePartitioningStructureFactory<UiCollider>* GetInstance(ISpacePartitioningStructureFactoryImpl* factoryImpl =nullptr);

		static SpacePartitioningStructure<UiCollider>* CreateSpacePartitioningStructure(const std::string& className);

		static void RemoveSpacePartitioningStructure(SpacePartitioningStructure<UiCollider>* spaceStructure);

	private:
		SpacePartitioningStructureFactory(ISpacePartitioningStructureFactoryImpl* factoryImpl);

		ISpacePartitioningStructureFactoryImpl* mFactoryImpl;

	};


}