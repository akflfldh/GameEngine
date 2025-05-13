#include "DefaultSpacePartitioningStructureFactoryImpl.h"

#include"VectorSpace.h"

Quad::DefaultSpacePartitioningStructureFactoryImpl::~DefaultSpacePartitioningStructureFactoryImpl()
{
}

Quad::SpacePartitioningStructure<Quad::Collider>* Quad::DefaultSpacePartitioningStructureFactoryImpl::Create3DSpacePartitioningStructure(const std::string& className)
{
    if (className == "VectorSpace")
    {
        return   new VectorSpace<Quad::Collider>;
    }
}

void Quad::DefaultSpacePartitioningStructureFactoryImpl::Remove3DSpacePartitioningStructure(SpacePartitioningStructure<Collider>* spaceStructure)
{
    delete spaceStructure;

}

Quad::SpacePartitioningStructure<Quad::UiCollider>* Quad::DefaultSpacePartitioningStructureFactoryImpl::CreateUiSpacePartitioningStructure(const std::string& className)
{
    if (className == "VectorSpace")
    {
        return   new VectorSpace<Quad::UiCollider>;
    }
}

void Quad::DefaultSpacePartitioningStructureFactoryImpl::RemoveUiSpacePartitioningStructure(SpacePartitioningStructure<UiCollider>* spaceStructure)
{
    delete spaceStructure;

}
