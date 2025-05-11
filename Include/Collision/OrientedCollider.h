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
		virtual void OnEvent(Event* event);
		virtual void Update(float deltaTime);
		virtual void Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix) override;
		virtual void Update(float deltaTime, const DirectX::XMMATRIX& transformMatrix)override;

		CLASSNAME(OrientedBoxCollider)
		CLASSSIZE(OrientedBoxCollider)
		virtual void Serialize() override;
		virtual void DeSerialize() override;

		virtual void SetMesh(Mesh* mesh) override;
	private:

		void Initialize(Mesh * mesh ,const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& extents);

		const DirectX::BoundingOrientedBox& GetBoundingVolume() const;



		DirectX::BoundingOrientedBox mBoundingOrientedBox;

	};

}
