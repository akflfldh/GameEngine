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
		virtual void OnEvent(Event* event);
		virtual void Update(float deltaTime);
		virtual void Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix) override;
		virtual void Update(float deltaTime, const DirectX::XMMATRIX& transformMatrix)override;

		CLASSNAME(SphereCollider)
		CLASSSIZE(SphereCollider)
		virtual void Serialize() override;
		virtual void DeSerialize()override;

		virtual void SetMesh(Mesh* mesh) override;

	private:

		void Initialize(Mesh * mesh ,const DirectX::XMFLOAT3& center, float radius);

		const DirectX::BoundingSphere& GetBoundingVolume() const;


		DirectX::BoundingSphere mBoundingSphere;

	};

}
