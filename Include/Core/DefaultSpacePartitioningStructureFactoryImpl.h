#pragma once

#include"ISpacePartitioningStructureFactoryImpl.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB DefaultSpacePartitioningStructureFactoryImpl:public ISpacePartitioningStructureFactoryImpl
	{
	public:
		DefaultSpacePartitioningStructureFactoryImpl() = default;
		virtual ~DefaultSpacePartitioningStructureFactoryImpl();


		virtual SpacePartitioningStructure<Collider>* Create3DSpacePartitioningStructure(const std::string& className) ;
		virtual void Remove3DSpacePartitioningStructure(SpacePartitioningStructure<Collider>* spaceStructure) ;

		virtual SpacePartitioningStructure<UiCollider>* CreateUiSpacePartitioningStructure(const std::string& className) ;
		virtual void RemoveUiSpacePartitioningStructure(SpacePartitioningStructure<UiCollider>* spaceStructure) ;

	private:
	};

}
