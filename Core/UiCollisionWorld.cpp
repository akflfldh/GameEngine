#include "Collision/UiCollisionWorld.h"
#include"SpacePartitioningStructureFactory.h"
#include"Object/Object.h"
Quad::UiCollisionWorld::UiCollisionWorld(SpacePartitioningStructure<UiCollider>* partitioningStructure)
	:mPartitioningStructure(partitioningStructure)
{
	

}

Quad::UiCollisionWorld::~UiCollisionWorld()
{
	SpacePartitioningStructureFactory<UiCollider>::RemoveSpacePartitioningStructure(mPartitioningStructure);
}

Quad::UiCollisionWorld& Quad::UiCollisionWorld::operator=(UiCollisionWorld&& world)
{
	mPartitioningStructure = world.mPartitioningStructure;
	world.mPartitioningStructure = nullptr;
	return *this;

	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::UiCollisionWorld::Initialize()
{


}

void Quad::UiCollisionWorld::Update()
{
}

void Quad::UiCollisionWorld::Reset()
{
	mPartitioningStructure->Reset();

}

void Quad::UiCollisionWorld::AddCollider(UiCollider* collider)
{
	mPartitioningStructure->AddCollider(collider);

}

void Quad::UiCollisionWorld::RemoveCollider(UiCollider* collider)
{
	mPartitioningStructure->RemoveCollider(collider);
	



}

bool Quad::UiCollisionWorld::RayCasting(std::vector<Object*>& oObjectVector, const Ray& ray)
{




	return false;
}

bool Quad::UiCollisionWorld::RayCastingFirst(Object*& oObject, Ray& ray)
{
	UiCollider* collider = nullptr;
	bool ret =mPartitioningStructure->RayCastingFirst(collider, ray);
	if (ret)
	{
		oObject = collider->GetDestObject();
	}

	return ret;
}

void Quad::UiCollisionWorld::FrsutumCullingObjects(std::vector<Object*>& oObjectVector, Camera* camera)
{


	std::vector<UiCollider*> colliderVector;

	//if (camera->GetCameraType() == ECameraType::ePerspectiveCamera)
	mPartitioningStructure->FrsutumCullingCollider(colliderVector, camera);
	//else
		//mPartitioningStructure->GetAllCollider(colliderVector);


	oObjectVector.reserve(colliderVector.size());

	//std::for_each(colliderVector.begin(), colliderVector.end(), [&oObjectVector](Collider* collider)
	//	{  oObjectVector.push_back(collider->GetDestObject());  });


	for (int i = 0; i < colliderVector.size(); ++i)
	{
		Object* object = colliderVector[i]->GetDestObject();

		if (object->GetDrawFlag())
		{

			oObjectVector.push_back(object);

		}
	}


	return;
}

void Quad::UiCollisionWorld::GetAllObject(std::vector<Object*>& oObjectVector)
{


}

const char* Quad::UiCollisionWorld::GetSpacePartitioningClassName() const
{

	return mPartitioningStructure->GetClassTypeName();
}

Quad::SpacePartitioningStructure<Quad::UiCollider>* Quad::UiCollisionWorld::GetSpacePartitioningStructure()
{
	return mPartitioningStructure;
}

Quad::UiCollisionWorld& Quad::UiCollisionWorld::operator=(const UiCollisionWorld& world)
{
	mPartitioningStructure->Copy(world.mPartitioningStructure);
	return *this;

	// TODO: 여기에 return 문을 삽입합니다.
}
