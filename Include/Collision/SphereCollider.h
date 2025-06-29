#pragma once

#include"Collision/Collider.h"
#include"DirectXCollision.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB SphereCollider:public Collider
	{
		friend class ColliderGenerator;
		friend class CollisionHelper;
	public:
		SphereCollider();
		~SphereCollider();

		virtual void Update(const DirectX::XMFLOAT4X4& transformMatrix) override;
		virtual void Update(const DirectX::XMMATRIX& transformMatrix)override;

		void SetRadiusLocal(float radius);
		virtual void SetPositionLocal(const DirectX::XMFLOAT3& pos) override;

		CLASSNAME(SphereCollider)
		CLASSSIZE(SphereCollider)
		virtual void Serialize() override;
		virtual void DeSerialize()override;

	private:
		const DirectX::BoundingSphere& GetBoundingVolume() const;
		DirectX::BoundingSphere mBoundingSphere;
		DirectX::BoundingSphere mBoundingSphereOrigin;

	};

}
