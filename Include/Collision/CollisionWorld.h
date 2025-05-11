#pragma once


#include"Predeclare.h"
#include<vector>
#include"Core/CoreDllExport.h"
#include"SpacePartitioningStructure.h"
namespace Quad
{
	class CORE_API_LIB CollisionWorld
	{
	public:
		CollisionWorld(SpacePartitioningStructure<Collider>* partitioingStructure);
		~CollisionWorld();
		
		//
		CollisionWorld(const CollisionWorld& world) =delete ;
		CollisionWorld(CollisionWorld&& world) = delete ;

		CollisionWorld& operator=(const CollisionWorld& world);
		CollisionWorld& operator=(CollisionWorld&& world);

		//void Copy(CollisionWorld * source , Collision)



		void Initialize();
		void Update();
		void Reset();


		void AddCollider(Collider* collider);
		void RemoveCollider(Collider* collider);
		//void AddGizmoCollider(Collider* collider);
		//virtual void FrsutumCullingObjects(std::vector<Object*>& oObjectVector /* CAMERA */);
		virtual bool RayCasting(std::vector<Object*>& oObjectVector, const Ray& ray);
		virtual bool RayCastingFirst(Object*& oObject,  Ray& ray);
		void FrsutumCullingObjects(std::vector<Object*>& oObjectVector, Camera* caemra);

		void GetAllObject(std::vector<Object*>& oObjectVector);

		const char* GetSpacePartitioningClassName() const;

		SpacePartitioningStructure<Collider>* GetSpacePartitioningStructure();
	private:




	private:
		SpacePartitioningStructure<Collider>* mPartitioningStructure;
		


	};

}
