#include "EditorSpacePartitioningStructure.h"

#include"Collision/CollisionHelper.h"
#include"Object/Object.h"
#include"Collision/Collider.h"



Quad::EditorSpacePartitioningStructure::EditorSpacePartitioningStructure(SpacePartitioningStructure* implSpaceStructure)
	:mImplSpaceStructure(implSpaceStructure)
{
}

const char* Quad::EditorSpacePartitioningStructure::GetClassTypeName() const
{
	return mImplSpaceStructure->GetClassTypeName();
}

void Quad::EditorSpacePartitioningStructure::AddCollider(Collider* collider)
{

	mImplSpaceStructure->AddCollider(collider);

}

void Quad::EditorSpacePartitioningStructure::RemoveCollider(Collider* collider)
{

	mImplSpaceStructure->RemoveCollider(collider);
}

void Quad::EditorSpacePartitioningStructure::DeleteCollider(Collider* collider)
{

	mImplSpaceStructure->DeleteCollider(collider);
}

void Quad::EditorSpacePartitioningStructure::ReBuild()
{

	mImplSpaceStructure->ReBuild();
}

void Quad::EditorSpacePartitioningStructure::FrsutumCullingCollider(std::vector<Collider*>& oColliderVector, Camera* camera)
{
	//gizmo를 이루는 object의 개수는 별로 안되니 무조건 추가
	for (auto& gizmoCollider : mGizmoColliderVector)
	{
		oColliderVector.push_back(gizmoCollider);
	}


	mImplSpaceStructure->FrsutumCullingCollider(oColliderVector, camera);





}


bool Quad::EditorSpacePartitioningStructure::RayCasting(std::vector<Collider*>& oColliderVector, const Ray& ray)
{
	return mImplSpaceStructure->RayCasting(oColliderVector, ray);
	
}

bool Quad::EditorSpacePartitioningStructure::RayCastingFirst(Collider*& oCollider, Ray& ray)
{
	const std::vector<Collider*>& gizmoColliderVector = GetGizmoColliderVector();

	float minT = FLT_MAX;
	for (int i = 0; i < gizmoColliderVector.size(); ++i)
	{
		float currParemeterT = FLT_MAX;
		Collider* currCollider = gizmoColliderVector[i];
		Object* destObject = currCollider->GetDestObject();

		if (destObject->GetSelectAvailableFlag())
		{
			if (CollisionHelper::Intersect(currCollider, ray, currParemeterT))
			{
				if (minT > currParemeterT)
				{
					ray.SetParameterT(currParemeterT);
					//내부 충돌판정실패
					if (!destObject->InnerDetectRayHit(ray))
						continue;

					minT = currParemeterT;
					oCollider = currCollider;
				}
			}
		}
	}
	if (minT != FLT_MAX)
	{
		return true;
	}


	return	mImplSpaceStructure->RayCastingFirst(oCollider, ray);
}

void Quad::EditorSpacePartitioningStructure::GetAllCollider(std::vector<Collider*>& oColliderVector)
{
	mImplSpaceStructure->GetAllCollider(oColliderVector);

}



bool Quad::EditorSpacePartitioningStructure::RayCastingFirstNotIncludeEditorObject(Collider*& oCollider, Ray& ray) const
{
	return	mImplSpaceStructure->RayCastingFirst(oCollider, ray);
}

void Quad::EditorSpacePartitioningStructure::AddGizmoCollider(Collider* collider)
{

	mGizmoColliderVector.push_back(collider);
}

const std::vector<Quad::Collider*>& Quad::EditorSpacePartitioningStructure::GetGizmoColliderVector() const
{
	return mGizmoColliderVector;
	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::EditorSpacePartitioningStructure::Reset()
{

	mImplSpaceStructure->Reset();
}

Quad::SpacePartitioningStructure<Quad::Collider>* Quad::EditorSpacePartitioningStructure::GetImplSpaceStructure()
{

	return mImplSpaceStructure;
}
