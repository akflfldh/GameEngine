#pragma once

#include"header.h"
#include"Predeclare.h"
#include<functional>
#include"Singleton.h"
#include"Core/CoreDllExport.h"
//
//#include"Collision/Collider.h"
//#include"Collision/UiCollider.h"

namespace Quad
{
	class BaseCollider;
	class CORE_API_LIB CollisionHelper:public Singleton<CollisionHelper>
	{
	public:
		CollisionHelper() = default;
		~CollisionHelper() = default;
		//현재 오브젝트가있는 시스템,map에서의 레이판정
		static bool MousePickingRay(UINT screenPosX, UINT screenPosY, Object* object);

		void Initialize();

		static bool Intersect( BaseCollider* colliderA,  BaseCollider* colliderB);
		static bool Intersect( BaseCollider* collider, const Ray& ray, float& oParameterT);



		//이거는 루트 씬컴포넌트에대해서만 수행하는것으로 변경되어야한다. 
		//static Entity* Intersect(const std::vector<Entity*>& objectVector, const Ray& ray, float& oParameter);
		//static Entity* Intersect(const std::vector<Object*>& objectVector, const Ray& ray, float& oParameter);



	private:

		static bool IntersectCC(const Collider* colliderA, const Collider* colliderB);
		static bool IntersectCR(const Collider* collider, const Ray& ray, float& oParameterT);
		static bool IntersectUCR(const UiCollider* collider, const Ray& ray, float& oParameterT);
		static bool IntersectUC(const UiCollider* uiCollider, const Collider* boxCollider);



	

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
