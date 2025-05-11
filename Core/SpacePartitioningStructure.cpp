#include "SpacePartitioningStructure.h"

namespace Quad
{
	template<typename collider>
	SpacePartitioningStructure<collider>::~SpacePartitioningStructure()
	{

	}
	//SpacePartitioningStructure& SpacePartitioningStructure::operator=(const SpacePartitioningStructure& space)
	//{

	//	mGizmoColliderVector.resize(space.GetGizmoColliderVector().size());

	//	for (int i = 0; i < mGizmoColliderVector.size(); ++i)
	//	{
	//		mGizmoColliderVector[i] = space.mGizmoColliderVector[i];
	//	}

	//	return *this;

	//	// TODO: 여기에 return 문을 삽입합니다.
	//}
	//SpacePartitioningStructure& SpacePartitioningStructure::operator=(SpacePartitioningStructure&& space)
	//{

	//	mGizmoColliderVector = std::move(space.mGizmoColliderVector);
	//	return *this;
	//	// TODO: 여기에 return 문을 삽입합니다.
	//}
	template<typename collider>
	bool SpacePartitioningStructure<collider>::Copy(SpacePartitioningStructure<collider>* source)
	{

	/*	mGizmoColliderVector.resize(source->GetGizmoColliderVector().size());

		for (int i = 0; i < mGizmoColliderVector.size(); ++i)
		{
			mGizmoColliderVector[i] = source->mGizmoColliderVector[i];
		}*/

		return true;

	}


	//const std::vector<Collider*>& SpacePartitioningStructure::GetGizmoColliderVector() const
	//{

	//	return mGizmoColliderVector;
	//	// TODO: 여기에 return 문을 삽입합니다.
	//}

	template class CORE_API_LIB SpacePartitioningStructure<Collider>;
	template class CORE_API_LIB SpacePartitioningStructure<UiCollider>;

}