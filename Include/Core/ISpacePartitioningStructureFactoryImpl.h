#pragma once

#include<string>
#include"Core/CoreDllExport.h"

#include"SpacePartitioningStructure.h"
#include"Collision/Collider.h"
#include"Collision/UiCollider.h"
namespace Quad
{

	class CORE_API_LIB ISpacePartitioningStructureFactoryImpl
	{
	public:
		ISpacePartitioningStructureFactoryImpl() = default;
		virtual ~ISpacePartitioningStructureFactoryImpl()= 0;


		virtual SpacePartitioningStructure<Collider>* Create3DSpacePartitioningStructure(const std::string& className) = 0;
		virtual void Remove3DSpacePartitioningStructure(SpacePartitioningStructure<Collider>* spaceStructure) = 0;

		virtual SpacePartitioningStructure<UiCollider>* CreateUiSpacePartitioningStructure(const std::string& className) = 0;
		virtual void RemoveUiSpacePartitioningStructure(SpacePartitioningStructure<UiCollider>* spaceStructure) = 0;

	private:
	};







}
