#pragma once

#include"SpacePartitioningStructure.h"
#include"Collision/UiCollider.h"

namespace Quad
{
	class UiCollider;

	class CORE_API_LIB UiCollisionWorld
	{
	public:
		UiCollisionWorld(SpacePartitioningStructure<UiCollider>* partitioingStructure);
		~UiCollisionWorld();

		//
		UiCollisionWorld(const UiCollisionWorld& world) = delete;
		UiCollisionWorld(UiCollisionWorld&& world) = delete;

		UiCollisionWorld& operator=(const UiCollisionWorld& world);
		UiCollisionWorld& operator=(UiCollisionWorld&& world);

		//void Copy(CollisionWorld * source , Collision)



		void Initialize();
		void Update();
		void Reset();


		void AddCollider(UiCollider* collider);
		void RemoveCollider(UiCollider* collider);
		//void AddGizmoCollider(Collider* collider);
		//virtual void FrsutumCullingObjects(std::vector<Object*>& oObjectVector /* CAMERA */);
		virtual bool RayCasting(std::vector<Object*>& oObjectVector, const Ray& ray);
		virtual bool RayCastingFirst(Object*& oObject, Ray& ray);
		void FrsutumCullingObjects(std::vector<Object*>& oObjectVector, Camera* caemra);

		void GetAllObject(std::vector<Object*>& oObjectVector);

		const char* GetSpacePartitioningClassName() const;

		SpacePartitioningStructure<UiCollider>* GetSpacePartitioningStructure();
	private:




	private:
		SpacePartitioningStructure<UiCollider>* mPartitioningStructure;


	};

}