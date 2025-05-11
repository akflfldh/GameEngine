#include "EditorSpacePartitioningStructrureFactoryImpl.h"

#include"SpacePartitioningStructure.h"
#include"VectorSpace.h"
#include"EditorSpacePartitioningStructure.h"
#include"EditorUiSpacePartitioningStructure.h"
#include"Collision/Collider.h"



Quad::SpacePartitioningStructure<Quad::Collider>* Quad::EditorSpacePartitioningStructrureFactoryImpl::Create3DSpacePartitioningStructure(const std::string& className)
{
    if (className == "VectorSpace")
    {
        return   new EditorSpacePartitioningStructure(new VectorSpace<Quad::Collider>);
    }
}

void Quad::EditorSpacePartitioningStructrureFactoryImpl::Remove3DSpacePartitioningStructure(SpacePartitioningStructure<Quad::Collider>* spaceStructure)
{
    EditorSpacePartitioningStructure* rapperSpaceStructure = static_cast<EditorSpacePartitioningStructure*>(spaceStructure);
   
    SpacePartitioningStructure<Quad::Collider>* implSpaceStructure =    rapperSpaceStructure->GetImplSpaceStructure();

    delete rapperSpaceStructure;

    delete implSpaceStructure;



}

Quad::SpacePartitioningStructure<Quad::UiCollider>* Quad::EditorSpacePartitioningStructrureFactoryImpl::CreateUiSpacePartitioningStructure(const std::string& className)
{
    if (className == "VectorSpace")
    {
        return   new EditorUiSpacePartitioningStructure(new VectorSpace<Quad::UiCollider>);
    }
}

void Quad::EditorSpacePartitioningStructrureFactoryImpl::RemoveUiSpacePartitioningStructure(SpacePartitioningStructure<UiCollider>* spaceStructure)
{
    EditorUiSpacePartitioningStructure* rapperSpaceStructure = static_cast<EditorUiSpacePartitioningStructure*>(spaceStructure);

    SpacePartitioningStructure<Quad::UiCollider>* implSpaceStructure = rapperSpaceStructure->GetImplSpaceStructure();

    delete rapperSpaceStructure;

    delete implSpaceStructure;
}
