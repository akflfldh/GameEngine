#include "SpacePartitioningStructureFactory.h"

#include"VectorSpace.h"
#include"QuadTree.h"

#include"ISpacePartitioningStructureFactoryImpl.h"

template<typename collider>
Quad::SpacePartitioningStructureFactory<collider>::SpacePartitioningStructureFactory(ISpacePartitioningStructureFactoryImpl * factoryImpl)
    :mFactoryImpl(factoryImpl)
{

}





template<typename collider>
Quad::SpacePartitioningStructure<collider>* Quad::SpacePartitioningStructureFactory<collider>::CreateSpacePartitioningStructure(const std::string& className)
{
    SpacePartitioningStructureFactory<collider> * instance =  Singleton<SpacePartitioningStructureFactory<collider>>:: GetInstance();
    return  instance->mFactoryImpl->CreateSpacePartitioningStructure(className);

  /*  if (className == "VectorSpace")
    {
        return new VectorSpace;
    }*/
}
template<typename collider>
void Quad::SpacePartitioningStructureFactory<collider>::RemoveSpacePartitioningStructure(SpacePartitioningStructure<collider>* spaceStructure)
{

    SpacePartitioningStructureFactory<collider>*  instance = GetInstance();
    instance->mFactoryImpl->RemoveSpacePartitioningStructure(spaceStructure);

    //delete spaceStructure;

}




Quad::SpacePartitioningStructureFactory<Quad::Collider>::SpacePartitioningStructureFactory(ISpacePartitioningStructureFactoryImpl* factoryImpl)
    :mFactoryImpl(factoryImpl)
{

}

Quad::SpacePartitioningStructureFactory<Quad::Collider>* Quad::SpacePartitioningStructureFactory<Quad::Collider>::GetInstance(Quad::ISpacePartitioningStructureFactoryImpl* factoryImpl)
{
    static Quad::SpacePartitioningStructureFactory<Quad::Collider> factory(factoryImpl);

    return &factory;

}


Quad::SpacePartitioningStructure<Quad::Collider>* Quad::SpacePartitioningStructureFactory<Quad::Collider>::CreateSpacePartitioningStructure(const std::string& className)
{

    SpacePartitioningStructureFactory<Collider>* instance = GetInstance();
    return  instance->mFactoryImpl->Create3DSpacePartitioningStructure(className);

}

void Quad::SpacePartitioningStructureFactory<Quad::Collider>::RemoveSpacePartitioningStructure(SpacePartitioningStructure<Quad::Collider>* spaceStructure)
{

    SpacePartitioningStructureFactory<Collider>* instance = GetInstance();
    instance->mFactoryImpl->Remove3DSpacePartitioningStructure(spaceStructure);

}


Quad::SpacePartitioningStructureFactory<Quad::UiCollider>* Quad::SpacePartitioningStructureFactory<Quad::UiCollider>::GetInstance(Quad::ISpacePartitioningStructureFactoryImpl* factoryImpl)
{
    static Quad::SpacePartitioningStructureFactory<Quad::UiCollider> factory(factoryImpl);

    return &factory;

}






Quad::SpacePartitioningStructureFactory<Quad::UiCollider>::SpacePartitioningStructureFactory(ISpacePartitioningStructureFactoryImpl* factoryImpl)
    :mFactoryImpl(factoryImpl)
{

}

Quad::SpacePartitioningStructure<Quad::UiCollider>* Quad::SpacePartitioningStructureFactory<Quad::UiCollider>::CreateSpacePartitioningStructure(const std::string& className)
{

    SpacePartitioningStructureFactory<UiCollider>* instance = GetInstance();
    return  instance->mFactoryImpl->CreateUiSpacePartitioningStructure(className);

}

void Quad::SpacePartitioningStructureFactory<Quad::UiCollider>::RemoveSpacePartitioningStructure(SpacePartitioningStructure<Quad::UiCollider>* spaceStructure)
{

    SpacePartitioningStructureFactory<UiCollider>* instance = GetInstance();
    instance->mFactoryImpl->RemoveUiSpacePartitioningStructure(spaceStructure);

}






template class  CORE_API_LIB Quad::SpacePartitioningStructureFactory<Quad::Collider>;
template class CORE_API_LIB Quad::SpacePartitioningStructureFactory<Quad::UiCollider>;