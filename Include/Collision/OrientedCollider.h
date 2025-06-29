#pragma once

#include"Predeclare.h"


#include"Collision/Collider.h"
#include"DirectXCollision.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB OrientedBoxCollider:public Collider
	{
		friend class ColliderGenerator;
		friend class CollisionHelper;
	public:
		OrientedBoxCollider();
		~OrientedBoxCollider();
		virtual void Update(const DirectX::XMFLOAT4X4& transformMatrix) override;
		virtual void Update(const DirectX::XMMATRIX& transformMatrix)override;


		virtual void SetPositionLocal(const DirectX::XMFLOAT3& pos) override;

		CLASSNAME(OrientedBoxCollider)
		CLASSSIZE(OrientedBoxCollider)
		virtual void Serialize() override;
		virtual void DeSerialize() override;
	private:

		const DirectX::BoundingOrientedBox& GetBoundingVolume() const;


		DirectX::BoundingOrientedBox mBoundingOrientedBoxOrigin;
		DirectX::BoundingOrientedBox mBoundingOrientedBox;

	};

}
