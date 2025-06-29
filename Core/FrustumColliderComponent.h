#pragma once



#include"Component/ColliderBaseComponent.h"
#include"DirectXMath.h"
#include<Collision/FrustumCollider.h>
namespace Quad
{
	

	class FrustumColliderComponent:public ColliderBaseComponent
	{
	public:
		FrustumColliderComponent();
		~FrustumColliderComponent();

		virtual void Start() override;//게임플레이 시작시한번호출
		virtual void Update(float delta) override;//게임플레이 매프레임 호출


        void SetNearPlaneLocal(float nearPlane);
        void SetFarPlaneLocal(float farPlane);
        void SetRightSlopeLocal(float rightSlope);
        void SetLeftSlopeLocal(float leftSlope);
        void SetTopSlopeLocal(float topSlope);
        void SetBottomSlopeLocal(float bottomSlope);
        void SetPositionLocal(const DirectX::XMFLOAT3& origin);
        void SetOrientationLocal(const DirectX::XMFLOAT4& orientation);

        float GetNearPlaneLocal() const;
        float GetFarPlaneLocal() const;
        float GetRightSlopeLocal() const;
        float GetLeftSlopeLocal() const;
        float GetTopSlopeLocal() const;
        float GetBottomSlopeLocal() const;
        const DirectX::XMFLOAT3  GetPositionLocal() const;
        const DirectX::XMFLOAT4  GetOrientationLocal() const;

        void SetConfigLocal(float nearPlane, float farPlane, float rightSlope, float leftSlope,
            float topSlope, float bottomSlope, const DirectX::XMFLOAT3& origin = { 0,0,0 }, const DirectX::XMFLOAT4& orientation = { 0,0,0,1.0f });


        virtual  FrustumCollider* GetCollider()const override;

	private:
		mutable FrustumCollider* mFrustumCollider;

	};

}
