#pragma once

#include"Predeclare.h"
#include<vector>
#include"Core/Ray.h"
#include"Core/CoreDllExport.h"

namespace Quad
{
	class Object;

	template<typename collider>
	class  SpacePartitioningStructure
	{
	public:
		SpacePartitioningStructure() = default;
		virtual ~SpacePartitioningStructure() = 0;

		SpacePartitioningStructure(const SpacePartitioningStructure& space) = delete;
		SpacePartitioningStructure(SpacePartitioningStructure&& space) = delete;

		SpacePartitioningStructure& operator=(const SpacePartitioningStructure& space)= delete;
		SpacePartitioningStructure& operator=(SpacePartitioningStructure&& space) =delete;

		virtual bool Copy(SpacePartitioningStructure* source);

		virtual const char* GetClassTypeName() const = 0;

		virtual void AddCollider(collider* collider) = 0;
		virtual void RemoveCollider(collider* collider) = 0;
		virtual void DeleteCollider(collider* collider) = 0;

		virtual void ReBuild() = 0;

		virtual void FrsutumCullingCollider(std::vector<collider*>& oColliderVector, Camera * caemra) = 0;
		virtual bool RayCasting(std::vector<collider*>& oColliderVector, const Ray& ray) = 0;
		virtual bool RayCastingFirst(collider*& oCollider, Ray& ray) = 0;
		virtual void GetAllCollider(std::vector<collider*>& oColliderVector)=0;


		//void AddGizmoCollider(Collider* collider);
	//	const std::vector<Collider*> & GetGizmoColliderVector() const;


		//콜라이더를 컨테이너에서 모두 없애야한다.(일단 기즈모는 무시해보자)
		virtual void Reset() = 0;
		

	private:
		
		

		//std::vector<Collider* > mGizmoColliderVector;





	};

}