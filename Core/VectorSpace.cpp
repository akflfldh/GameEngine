#include "VectorSpace.h"
#include"Object/Object.h"
//#include"SizeControllerStickEntity.h"
#include"Collision/CollisionHelper.h"
#include"Collision/Collider.h"
#include"Collision/UiCollider.h"
#include"Object/Camera/Camera.h"
//#include"FolderPanelEntity.h"

//#include"DockingSelectBox.h"
#include"Component/ColliderComponent.h"

#include"Collision/ColliderGenerator.h"



namespace Quad
{
	//VectorSpace& VectorSpace::operator=(const VectorSpace& vectorSpace)
	//{

	//	SpacePartitioningStructure::operator=(vectorSpace);
	//	
	//	Initialize(mColliderMaxNum);
	//
	//	for (int i = 0; i < mColliderMaxNum; ++i)
	//	{
	//		mColliderVector[i] = vectorSpace.mColliderVector[i];
	//	}

	//	return *this;


	//	// TODO: 여기에 return 문을 삽입합니다.
	//}
	//VectorSpace& VectorSpace::operator=(VectorSpace&& vectorSpace)
	//{

	//	SpacePartitioningStructure::operator=(std::move(vectorSpace));


	//	mColliderMaxNum = vectorSpace.mColliderMaxNum;
	//	mColliderVector = std::move(vectorSpace.mColliderVector);

	template<typename collider>
	VectorSpace<collider>::~VectorSpace()
	{

		for (auto& collider : mColliderVector)
		{
			ColliderGenerator::ReleaseCollider(collider);
		}
	}

	//	return *this;
	//	// TODO: 여기에 return 문을 삽입합니다.
	//}
	template<typename collider>
	void VectorSpace<collider>::Initialize(size_t objectMaxNum)
	{
		mColliderMaxNum = objectMaxNum;
		mColliderVector.reserve(objectMaxNum);



	}
	template<typename collider>
	bool VectorSpace<collider>::Copy(SpacePartitioningStructure<collider>* source)
	{

		SpacePartitioningStructure<collider>::Copy(source);

		VectorSpace* space = dynamic_cast<VectorSpace*>(source);
		if (space == nullptr)
			return false;

		mColliderMaxNum = space->mColliderMaxNum;
		mColliderVector.reserve(mColliderMaxNum);

		/*for (int i = 0; i < space->mColliderVector.size(); ++i)
		{
			mColliderVector.push_back(space->mColliderVector[i]);
		}*/
		return true;


	}
	template<typename collider>
	void VectorSpace<collider>::AddCollider(collider* collider)
	{
		mColliderVector.push_back(collider);



	}
	template<typename collider>
	void VectorSpace<collider>::RemoveCollider(collider* coll)
	{

		typename std::vector<collider*>::iterator it = std::find(mColliderVector.begin(), mColliderVector.end(), coll);
		if (it != mColliderVector.end())
			mColliderVector.erase(it);




	}
	template<typename collider>
	void VectorSpace<collider>::DeleteCollider(collider* coll)
	{
		typename std::vector<collider*>::iterator it = std::find(mColliderVector.begin(), mColliderVector.end(), coll);
		if (it != mColliderVector.end())
			mColliderVector.erase(it);
	

	}
	template<typename collider>
	void VectorSpace<collider>::ReBuild()
	{


	}
	template<typename collider>
	void VectorSpace<collider>::FrsutumCullingCollider(std::vector<collider*>& oColliderVector, Camera* camera)
	{
		//카메라의 뷰프러스텀과의 교차판정을 통해서 처리한다.
		//collision helepr의 메서드를 사용하자
		const Collider* viewFrustumCollider = camera->GetViewCollider();

		for (int i = 0; i < mColliderVector.size(); ++i)
		{
			collider* currCollider = mColliderVector[i];
			
			Object* destObject = currCollider->GetDestObject();
			if (destObject->GetName() == "AnimationStateTransitionBlendNode1")
			{
				int a = 2;
			}
			if (destObject->GetDrawFlag() && !destObject->GetKilledState())
			{
			

				if (CollisionHelper::Intersect(currCollider, viewFrustumCollider))
				{

					oColliderVector.push_back(currCollider);
				}
			}
		}


	}
	template<typename collider>
	bool VectorSpace<collider>::RayCasting(std::vector<collider*>& oColliderVector, const Ray& ray)
	{
		return false;
	}
	template<typename collider>
	bool VectorSpace<collider>::RayCastingFirst(collider*& oCollider,  Ray& ray)
	{



		float minT = FLT_MAX;
	

		for (int i = 0; i < mColliderVector.size(); ++i)
		{
			float currParemeterT = FLT_MAX;
			collider* currCollider = mColliderVector[i];
			Object* destObject = currCollider->GetDestObject();
			
			if (destObject->GetSelectAvailableFlag())
			{
				if (CollisionHelper::Intersect(currCollider, ray, currParemeterT))
				{
					if (minT > currParemeterT)
					{
						
						if (!destObject->InnerDetectRayHit(ray))
							continue;

						minT = currParemeterT;
						oCollider = currCollider;
					}
				}
			}
		}
		return (minT == FLT_MAX) ? false : true;
	}
	template<typename collider>
	void VectorSpace<collider>::GetAllCollider(std::vector<collider*>& oColliderVector)
	{

		oColliderVector.resize(mColliderVector.size());
		std::copy(mColliderVector.begin(), mColliderVector.end(), oColliderVector.begin());

	}
	template<typename collider>
	void VectorSpace<collider>::Reset()
	{
		mColliderVector.resize(0);

	}





	template class CORE_API_LIB VectorSpace<Collider>;
	template class CORE_API_LIB VectorSpace<UiCollider>;






}