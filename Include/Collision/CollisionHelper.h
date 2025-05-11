#pragma once

#include"header.h"
#include"Predeclare.h"
#include<functional>
#include"Singleton.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB CollisionHelper:public Singleton<CollisionHelper>
	{
	public:
		CollisionHelper() = default;
		~CollisionHelper() = default;
		//현재 오브젝트가있는 시스템,map에서의 레이판정
		static bool MousePickingRay(UINT screenPosX, UINT screenPosY, Object* object);

		void Initialize();

		static bool Intersect(const Collider* colliderA, const Collider* colliderB);
		static bool Intersect(const Collider* collider,const Ray& ray, float& oParameterT);
		static bool Intersect(const UiCollider* collider, const Ray& ray, float& oParameterT);
		static bool Intersect(const UiCollider* uiCollider, const Collider* boxCollider);


		static Entity* Intersect(const std::vector<Entity*>& objectVector, const Ray& ray, float& oParameter);
		//static Entity* Intersect(const std::vector<Object*>& objectVector, const Ray& ray, float& oParameter);



	private:


	

		static bool  IntersectBoxX2(const Collider* boundingColliderA, const Collider* boundingColliderB);
		static bool  IntersectSphereX2(const Collider* boundingColliderA, const Collider* boundingColliderB);
		static bool  IntersectFrustumX2(const Collider* boundingColliderA,const Collider* boundingColliderB);
		static bool  IntersectOrientedBoxX2(const Collider* boundingColliderA, const Collider* boundingColliderB);


		static bool IntersectBoxSp(const Collider* boundingColliderA, const Collider* boundingColliderB);
		static bool IntersectBoxOBox(const Collider* boundingColliderA, const Collider* boundingColliderB);
		static bool IntersectBoxF(const Collider* boundingColliderA, const Collider* boundingColliderB);
		static bool IntersectSpF(const Collider* boundingColliderA, const Collider* boundingColliderB);
		static bool IntersectSpOBox(const Collider* boundingColliderA, const Collider* boundingColliderB);
		static bool IntersectFOBox(const Collider* boundingColliderA, const Collider* boundingColliderB);


	private:
		static std::array<std::array<std::function<bool(const Collider*,const Collider*)>, 4>, 4> mIntersectFuncMatrix;




	};

}
