#include "Collision/CollisionWorld.h"
#include"SpacePartitioningStructure.h"
#include"Collision/Collider.h"
#include"Object/Entity.h"
#include"Object/Camera/Camera.h"


Quad::CollisionWorld::CollisionWorld(SpacePartitioningStructure<Collider>* partitioingStructure)
	:mPartitioningStructure(partitioingStructure)
{



}
Quad::CollisionWorld::~CollisionWorld()
{

	delete mPartitioningStructure;


}
//
//Quad::CollisionWorld::CollisionWorld(const CollisionWorld& world)
//	:mPartitioningStructure(nullptr)
//{
//	
//	mPartitioningStructure = new SpacePartitioningStructure(*world.mPartitioningStructure);
//
//
//
//}
//
//Quad::CollisionWorld::CollisionWorld(CollisionWorld&& world)
//	:mPartitioningStructure(world.mPartitioningStructure)
//{
//
//
//
//	world.mPartitioningStructure = nullptr;
//}

Quad::CollisionWorld& Quad::CollisionWorld::operator=(const CollisionWorld& world)
{

	mPartitioningStructure->Copy(world.mPartitioningStructure);
	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::CollisionWorld& Quad::CollisionWorld::operator=(CollisionWorld&& world)
{
	mPartitioningStructure = world.mPartitioningStructure;
	world.mPartitioningStructure = nullptr;
	return *this;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::CollisionWorld::Initialize()
{
	


}

void Quad::CollisionWorld::Update()
{
	//mPartitioingStructure->Update()


}

void Quad::CollisionWorld::Reset()
{
	mPartitioningStructure->Reset();
}

void Quad::CollisionWorld::AddCollider(Collider* collider)
{
	//단일 충돌체만 추가
	mPartitioningStructure->AddCollider(collider);
	if (collider->GetDestObject()->GetName() == "OrthogoanlCamera2")
	{
		int a = 2;
	}

}

void Quad::CollisionWorld::RemoveCollider(Collider* collider)
{
	mPartitioningStructure->RemoveCollider(collider);
}

//void Quad::CollisionWorld::AddGizmoCollider(Collider* collider)
//{
//	mPartitioningStructure->AddCollider(collider);
//	mPartitioningStructure->AddGizmoCollider(collider);
//}

bool Quad::CollisionWorld::RayCasting(std::vector<Object*>& oObjectVector, const Ray& ray)
{

	std::vector<Collider*> colliderVector;

	bool ret =mPartitioningStructure->RayCasting(colliderVector, ray);
	

	oObjectVector.reserve(colliderVector.size());
	std::for_each(colliderVector.begin(), colliderVector.end(), 
		[&oObjectVector](Collider* collider) {oObjectVector.push_back(collider->GetDestObject()); });

	return ret;

}

bool Quad::CollisionWorld::RayCastingFirst(Object*& oObject,  Ray& ray)
{

	Collider* collider = nullptr;
	bool ret = mPartitioningStructure->RayCastingFirst(collider, ray); //그 collider을 가지고있는게 맞을거같은데? 공간분할구조가 근데 어차피 object이긴하지.
	if (ret)
	{
		oObject = collider->GetDestObject();
	}
	return ret;

}

void Quad::CollisionWorld::FrsutumCullingObjects(std::vector<Object*>& oObjectVector , Camera * camera)
{
	std::vector<Collider*> colliderVector;

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

void Quad::CollisionWorld::GetAllObject(std::vector<Object*>& oObjectVector)
{



}

const char* Quad::CollisionWorld::GetSpacePartitioningClassName() const
{

	return mPartitioningStructure->GetClassTypeName();
}

Quad::SpacePartitioningStructure<Quad::Collider>* Quad::CollisionWorld::GetSpacePartitioningStructure()
{

	return mPartitioningStructure;
}