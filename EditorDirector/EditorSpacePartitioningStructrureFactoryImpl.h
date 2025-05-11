#pragma once
#include"Core/ISpacePartitioningStructureFactoryImpl.h"

namespace Quad
{
	class Collider;
	class UiCollider;
	class EditorSpacePartitioningStructrureFactoryImpl:public ISpacePartitioningStructureFactoryImpl
	{
	public:

		EditorSpacePartitioningStructrureFactoryImpl() = default;
		virtual ~EditorSpacePartitioningStructrureFactoryImpl() = default;


		//virtual SpacePartitioningStructure<Collider>* CreateSpacePartitioningStructure(const std::string& className) override;
		//virtual void RemoveSpacePartitioningStructure(SpacePartitioningStructure<Collider>* spaceStructure) override;

		virtual SpacePartitioningStructure<Collider>* Create3DSpacePartitioningStructure(const std::string& className) override;
		virtual void Remove3DSpacePartitioningStructure(SpacePartitioningStructure<Collider>* spaceStructure) override;

		virtual SpacePartitioningStructure<UiCollider>* CreateUiSpacePartitioningStructure(const std::string& className)override;
		virtual void RemoveUiSpacePartitioningStructure(SpacePartitioningStructure<UiCollider>* spaceStructure) override;

	private:

	};

}
