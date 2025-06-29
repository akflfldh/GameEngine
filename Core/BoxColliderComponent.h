#pragma once

#include"Component.h"
#include"Component/ColliderBaseComponent.h"
#include"DirectXMath.h"
#include"Collision/BoxCollider.h"

namespace Quad
{

	class BoxCollider;
	class BoxColliderComponent : public ColliderBaseComponent
	{
	public:
		BoxColliderComponent();
		~BoxColliderComponent();
	
		void Initialize();

		virtual void Start() override;//게임플레이 시작시한번호출
		virtual void Update(float delta) override;//게임플레이 매프레임 호출

		void SetMinMaxVertexLocal(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max);

		virtual void SetWidthLocalX(float width);
		virtual void SetHeightLocalY(float height);
		virtual void SetDepthLocalZ(float depth);

		void SetPositionLocal(const DirectX::XMFLOAT3& pos);

		virtual  BoxCollider* GetCollider() const override;

	private:
		mutable BoxCollider* mBoxCollider;
	


	};


}