#include "Collision/CollisionHelper.h"

#include"Object/Object.h"
#include"Object/Entity.h" 
#include"MathHelper.h"
#include"Map/Map.h"
#include"System.h"
#include"Transform.h"
#include"Collision/Collider.h"
#include"Collision/BoxCollider.h"
#include"Collision/FrustumCollider.h"
#include"Collision/SphereCollider.h"
#include"Collision/OrientedCollider.h"
#include"Component/ColliderComponent.h"
#include"Collision/UiCollider.h"
#include"Collision/RectCollider.h"

namespace Quad
{
	std::array<std::array<std::function<bool(const Collider*, const Collider*)>, 4>, 4>CollisionHelper::mIntersectFuncMatrix;

	bool CollisionHelper::MousePickingRay(UINT screenPosX, UINT screenPosY, Object* object)
	{
		
		//System* system = object->mSystem;
		//Quad::Map* map = system->GetMap();
		//D3D12_VIEWPORT viewPort = system->GetViewPort();
		//Camera* camera = map->GetGameCamera();

		//Ray ray;
		//MathHelper::CalculateRay({ (float)screenPosX,(float)screenPosY }, viewPort, *camera,
		//	true, ray);

		////근데t가음수가나오는경우가있던데.
		////반직선이라서 그러면안되는데
		//float t = 0.0f;
		//if (object->GetTransform().GetBoundingVolume()->Intersect(ray, t))
		//{
		//	return true;
		//}
		//return false;
		return false;
	}

	void CollisionHelper::Initialize()
	{
		int boundingSphereIndex = (int)EColliderType::eSphere;
		int boundingBoxIndex = (int)EColliderType::eBox;
		int boundingOrientedIndex = (int)EColliderType::eOrientedBox;
		int boundingFrustumIndex = (int)EColliderType::eFrustum;
	


		mIntersectFuncMatrix[boundingSphereIndex][boundingSphereIndex] = CollisionHelper::IntersectSphereX2;
		mIntersectFuncMatrix[boundingSphereIndex][boundingBoxIndex] = CollisionHelper::IntersectBoxSp;
		mIntersectFuncMatrix[boundingSphereIndex][boundingOrientedIndex] = CollisionHelper::IntersectSpOBox;
		mIntersectFuncMatrix[boundingSphereIndex][boundingFrustumIndex] = CollisionHelper::IntersectSpF;


		mIntersectFuncMatrix[boundingBoxIndex][boundingSphereIndex] = CollisionHelper::IntersectBoxSp;
		mIntersectFuncMatrix[boundingBoxIndex][boundingBoxIndex] = CollisionHelper::IntersectBoxX2;
		mIntersectFuncMatrix[boundingBoxIndex][boundingOrientedIndex] = CollisionHelper::IntersectBoxOBox;
		mIntersectFuncMatrix[boundingBoxIndex][boundingFrustumIndex] = CollisionHelper::IntersectBoxF;


		mIntersectFuncMatrix[boundingOrientedIndex][boundingSphereIndex] = CollisionHelper::IntersectSpOBox;
		mIntersectFuncMatrix[boundingOrientedIndex][boundingBoxIndex] = CollisionHelper::IntersectBoxOBox;
		mIntersectFuncMatrix[boundingOrientedIndex][boundingOrientedIndex] = CollisionHelper::IntersectOrientedBoxX2;
		mIntersectFuncMatrix[boundingOrientedIndex][boundingFrustumIndex] = CollisionHelper::IntersectFOBox;


		mIntersectFuncMatrix[boundingFrustumIndex][boundingSphereIndex] = CollisionHelper::IntersectSpF;
		mIntersectFuncMatrix[boundingFrustumIndex][boundingBoxIndex] = CollisionHelper::IntersectBoxF;
		mIntersectFuncMatrix[boundingFrustumIndex][boundingOrientedIndex] = CollisionHelper::IntersectFOBox;
		mIntersectFuncMatrix[boundingFrustumIndex][boundingFrustumIndex] = CollisionHelper::IntersectFrustumX2;



	}

	bool CollisionHelper::Intersect(const Collider* colliderA, const Collider* colliderB)
	{


		int colliderTypeA=(int)colliderA->GetColliderType();
		int colliderTypeB=(int)colliderB->GetColliderType();

		return	mIntersectFuncMatrix[colliderTypeA][colliderTypeB](colliderA, colliderB);
		
	
	}

	bool CollisionHelper::Intersect(const Collider* collider,const Ray& ray, float& oParameterT)
	{
		
		EColliderType colliderType = collider->GetColliderType();
		

		DirectX::XMFLOAT3 dirF = ray.GetDir();
		DirectX::XMFLOAT3 originF = ray.GetOrigin();


		DirectX::XMVECTOR dir=DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dirF));
		DirectX::XMVECTOR origin = DirectX::XMLoadFloat3(&originF);
		origin = DirectX::XMVectorSetW(origin, 1.0f);

		switch (colliderType)
		{
		case EColliderType::eBox:
		{
			const BoxCollider* boxCollider = (const BoxCollider*)collider;
			return boxCollider->GetBoundingVolume().Intersects(origin, dir, oParameterT);
		}
		break;
		case EColliderType::eSphere:
		{
			const SphereCollider* sphereCollider = (const SphereCollider*)collider;
			return sphereCollider->GetBoundingVolume().Intersects(origin, dir, oParameterT);
		}
		break;
		case EColliderType::eFrustum:
		{
			const FrustumCollider* frustumCollider = (const FrustumCollider*)collider;
			return frustumCollider->GetBoundingVolume().Intersects(origin, dir, oParameterT);
		}
		break;
		case EColliderType::eOrientedBox:
		{
			const OrientedBoxCollider* orientedBoxCollider = (const OrientedBoxCollider*)collider;
			return orientedBoxCollider->GetBoundingVolume().Intersects(origin, dir, oParameterT);

		}
		break;

		}
		return false;
	}

	bool CollisionHelper::Intersect(const UiCollider* collider, const Ray& ray, float& oParameterT)
	{
		EUiColliderType uiColliderType = collider->GetColliderType();
		
		if (uiColliderType == EUiColliderType::eRect)
		{
			const RectCollider* rectCollider = static_cast<const RectCollider*>(collider);


			const DirectX::XMFLOAT3& topLeftPoint = rectCollider->GetTopLeftPoint();
			const DirectX::XMFLOAT3& topRightPoint = rectCollider->GetTopRightPoint();
			const DirectX::XMFLOAT3& bottomLeftPoint = rectCollider->GetBottomLeftPoint();
			const DirectX::XMFLOAT3& bottomRightPoint = rectCollider->GetBottomRightPoint();


			//ray는 직교투영카메라에서의 ray라고 가정한다.
			//먼저 z값 단순비교
			if (topLeftPoint.z < ray.GetOrigin().z)
				return false;

			DirectX::XMFLOAT3 rayOrigin =ray.GetOrigin();
			rayOrigin.z = topLeftPoint.z;

			const DirectX::XMFLOAT3  p0 = bottomRightPoint;
			const DirectX::XMFLOAT3  p1 = bottomLeftPoint;
			const DirectX::XMFLOAT3  p2 = topRightPoint;

			//v1 = p1-p0
			DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));

			//v2 = p2-p0
			DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p0));

			//r = q - p0
			DirectX::XMVECTOR r = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&rayOrigin), DirectX::XMLoadFloat3(&p0));

			

			//r * v1 
			float rDotv1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(r, v1));


			if (rDotv1 < 0.0f)
				return false;

			//r*v2

			float rDotv2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(r, v2));

			if (rDotv2<0.0f)
				return false;


			//  (r*v1)/(v1*v1) <= 1.0f

			float v1Dotv1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v1, v1));

			if (rDotv1 / v1Dotv1 > 1.0f)
				return false;

			// (r*v2)/(v2*v2)<=1.0f

			float v2Dotv2 =DirectX::XMVectorGetX(DirectX::XMVector3Dot(v2, v2));

			if (rDotv2 / v2Dotv2 > 1.0f)
				return false;

			oParameterT = topLeftPoint.z;

			return true;

		}





		return false;
	}

	bool CollisionHelper::Intersect(const UiCollider* uiCollider, const Collider* collider)
	{
		//rect Collider , box Collider == <ui, orthogonalCamera>

		if (uiCollider->GetColliderType() == EUiColliderType::eRect && collider->GetColliderType() == EColliderType::eBox)
		{
			const RectCollider* rectCollider = static_cast<const RectCollider*>(uiCollider);
			const BoxCollider* boxCollider = static_cast<const BoxCollider*>(collider);

			const DirectX::XMFLOAT3& topLeftPoint = rectCollider->GetTopLeftPoint();
			const DirectX::XMFLOAT3& topRightPoint = rectCollider->GetTopRightPoint();
			const DirectX::XMFLOAT3& bottomLeftPoint = rectCollider->GetBottomLeftPoint();
			const DirectX::XMFLOAT3& bottomRightPoint = rectCollider->GetBottomRightPoint();



			//먼저 Z먼저 판정한다(uiCollider의 z는 모두 동일하기에 하나의 점의 z값이 boxCollider내부에있는지 판정한다)
			const DirectX::BoundingBox& boundingBox =boxCollider->GetBoundingVolume();


			if (topLeftPoint.z < (boundingBox.Center.z - boundingBox.Extents.z)
				|| topLeftPoint.z>(boundingBox.Center.z + boundingBox.Extents.z))
			{
				return false;
			}

			//aabb, obb의 2차원검사인것 분리축정리, 4개의 모서리의 수직벡터에대해서 판정  




			return true;



		}










		return true;
	}

	Entity* CollisionHelper::Intersect(const std::vector<Entity*>& objectVector, const Ray& ray, float& oParameter)
	{
		float minT = FLT_MAX;


		Entity* entity = nullptr;
		for (int i = 0; i < objectVector.size(); ++i)
		{
			float t = FLT_MAX;


			Collider* collider = objectVector[i]->GetModel()->GetColliderComponent()->GetCollider();
			if (collider == nullptr)
				continue;


			if (Intersect(collider, ray, t))
			{
				if (minT > t)
				{
					minT = t;
					entity = objectVector[i];
				}
			}

		}

		return entity;


	}


	bool CollisionHelper::IntersectBoxX2(const Collider* colliderA, const Collider* colliderB)
	{
		const BoxCollider* boxColliderA = (const BoxCollider*)colliderA;
		const BoxCollider* boxColliderB = (const BoxCollider*)colliderB;
		
		const DirectX::BoundingBox& dxBoundingBoxA = boxColliderA->GetBoundingVolume();
		const DirectX::BoundingBox& dxBoundingBoxB = boxColliderB->GetBoundingVolume();
		
		return dxBoundingBoxA.Intersects(dxBoundingBoxB);
	}

	bool CollisionHelper::IntersectSphereX2(const Collider* colliderA, const Collider* colliderB)
	{
		const SphereCollider* sphereColliderA = (const SphereCollider*)colliderA;
		const SphereCollider* sphereColliderB = (const SphereCollider*)colliderB;

		const DirectX::BoundingSphere& dxBoundingSphereA = sphereColliderA->GetBoundingVolume();
		const DirectX::BoundingSphere& dxBoundingSphereB = sphereColliderB->GetBoundingVolume();
		
		return dxBoundingSphereA.Intersects(dxBoundingSphereB);
	}

	bool CollisionHelper::IntersectFrustumX2(const Collider* colliderA, const Collider* colliderB)
	{

		const FrustumCollider* frustumColliderA = (const FrustumCollider*)colliderA;
		const FrustumCollider* frustumColliderB = (const FrustumCollider*)colliderB;

		const DirectX::BoundingFrustum& dxBoundingFrustumA = frustumColliderA->GetBoundingVolume();
		const DirectX::BoundingFrustum& dxBoundingFrustumB = frustumColliderB->GetBoundingVolume();

		return dxBoundingFrustumA.Intersects(dxBoundingFrustumB);

	
	}

	bool CollisionHelper::IntersectOrientedBoxX2(const Collider* colliderA, const Collider* colliderB)
	{
		const OrientedBoxCollider* OrientedBoxColliderA = (const OrientedBoxCollider*)colliderA;
		const OrientedBoxCollider* OrientedBoxColliderB = (const OrientedBoxCollider*)colliderB;

		const DirectX::BoundingOrientedBox& dxBoundingOrientedBoxA = OrientedBoxColliderA->GetBoundingVolume();
		const DirectX::BoundingOrientedBox& dxBoundingOrientedBoxB = OrientedBoxColliderB->GetBoundingVolume();

		return dxBoundingOrientedBoxA.Intersects(dxBoundingOrientedBoxB);

	}

	bool CollisionHelper::IntersectBoxSp(const Collider* colliderA, const Collider* colliderB)
	{

		const BoxCollider* boxCollider=nullptr;
		const SphereCollider* sphereCollider=nullptr;
		if (colliderA->GetColliderType() == EColliderType::eBox)
		{
			boxCollider = (const BoxCollider*)colliderA;
			sphereCollider = (const SphereCollider*)colliderB;
		
		}
		else
		{
			boxCollider = (const BoxCollider*)colliderB;
			sphereCollider = (const SphereCollider*)colliderA;
		}

		const DirectX::BoundingBox& dxBoundingBox = boxCollider->GetBoundingVolume();
		const DirectX::BoundingSphere& dxBoundingSphere = sphereCollider->GetBoundingVolume();


		return dxBoundingBox.Intersects(dxBoundingSphere);
	}

	bool CollisionHelper::IntersectBoxOBox(const Collider* colliderA, const Collider* colliderB)
	{
		const BoxCollider* boxCollider = nullptr;
		const OrientedBoxCollider* orientedBoxCollider = nullptr;
		if (colliderA->GetColliderType() == EColliderType::eBox)
		{
			boxCollider = (const BoxCollider*)colliderA;
			orientedBoxCollider = (const OrientedBoxCollider*)colliderB;

		}
		else
		{
			boxCollider = (const BoxCollider*)colliderB;
			orientedBoxCollider = (const OrientedBoxCollider*)colliderA;
		}

		const DirectX::BoundingBox& dxBoundingBox = boxCollider->GetBoundingVolume();
		const DirectX::BoundingOrientedBox& dxBoundingOrientedBox = orientedBoxCollider->GetBoundingVolume();

		return dxBoundingBox.Intersects(dxBoundingOrientedBox);


	}

	bool CollisionHelper::IntersectBoxF(const Collider* colliderA, const Collider* colliderB)
	{

		const BoxCollider* boxCollider = nullptr;
		const FrustumCollider* frustumCollider = nullptr;
		if (colliderA->GetColliderType() == EColliderType::eBox)
		{
			boxCollider = (const BoxCollider*)colliderA;
			frustumCollider = (const FrustumCollider*)colliderB;

		}
		else
		{
			boxCollider = (const BoxCollider*)colliderB;
			frustumCollider = (const FrustumCollider*)colliderA;
		}

		const DirectX::BoundingBox& dxBoundingBox = boxCollider->GetBoundingVolume();
		const DirectX::BoundingFrustum& dxBoundingFrustum = frustumCollider->GetBoundingVolume();


		return dxBoundingBox.Intersects(dxBoundingFrustum);
	}

	bool CollisionHelper::IntersectSpF(const Collider* colliderA, const Collider* colliderB)
	{

		const SphereCollider* sphereCollider = nullptr;
		const FrustumCollider* frustumCollider = nullptr;
		if (colliderA->GetColliderType() == EColliderType::eSphere)
		{
			sphereCollider = (const SphereCollider*)colliderA;
			frustumCollider = (const FrustumCollider*)colliderB;

		}
		else
		{
			sphereCollider = (const SphereCollider*)colliderB;
			frustumCollider = (const FrustumCollider*)colliderA;
		}

		const DirectX::BoundingSphere& dxBoundingSphere= sphereCollider->GetBoundingVolume();
		const DirectX::BoundingFrustum& dxBoundingFrustum = frustumCollider->GetBoundingVolume();


		return dxBoundingSphere.Intersects(dxBoundingFrustum);

	}

	bool CollisionHelper::IntersectSpOBox(const Collider* colliderA, const Collider* colliderB)
	{

		const SphereCollider* sphereCollider = nullptr;
		const OrientedBoxCollider* orientedBoxCollider = nullptr;
		if (colliderA->GetColliderType() == EColliderType::eSphere)
		{
			sphereCollider = (const SphereCollider*)colliderA;
			orientedBoxCollider = (const OrientedBoxCollider*)colliderB;

		}
		else
		{
			sphereCollider = (const SphereCollider*)colliderB;
			orientedBoxCollider = (const OrientedBoxCollider*)colliderA;
		}

		const DirectX::BoundingSphere& dxBoundingSphere = sphereCollider->GetBoundingVolume();
		const DirectX::BoundingOrientedBox& dxBoundingOrientedBox = orientedBoxCollider->GetBoundingVolume();


		return dxBoundingSphere.Intersects(dxBoundingOrientedBox);
	}

	bool CollisionHelper::IntersectFOBox(const Collider* colliderA, const Collider* colliderB)
	{

		const FrustumCollider* frustumCollider = nullptr;
		const OrientedBoxCollider* orientedBoxCollider = nullptr;
		if (colliderA->GetColliderType() == EColliderType::eFrustum)
		{
			frustumCollider = (const FrustumCollider*)colliderA;
			orientedBoxCollider = (const OrientedBoxCollider*)colliderB;

		}
		else
		{
			frustumCollider = (const FrustumCollider*)colliderB;
			orientedBoxCollider = (const OrientedBoxCollider*)colliderA;
		}

		const DirectX::BoundingFrustum& dxBoundingFrustum = frustumCollider->GetBoundingVolume();
		const DirectX::BoundingOrientedBox& dxBoundingOrientedBox = orientedBoxCollider->GetBoundingVolume();


		return dxBoundingFrustum.Intersects(dxBoundingOrientedBox);
	}



}