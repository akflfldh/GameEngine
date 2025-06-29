#pragma once
#include "Collision/Collider.h"
#include"DirectXCollision.h"

#include"Core/CoreDllExport.h"

namespace Quad
{
    class CORE_API_LIB FrustumCollider :  public Collider
    {
        friend class ColliderGenerator;
        friend class CollisionHelper;
    public:
        FrustumCollider();
        ~FrustumCollider();
        virtual void Update(const DirectX::XMFLOAT4X4& transformMatrix) override;
        virtual void Update(const DirectX::XMMATRIX& transformMatrix)override;


        void SetNearPlaneLocal(float nearPlane);
        void SetFarPlaneLocal(float farPlane);
        void SetRightSlopeLocal(float rightSlope);
        void SetLeftSlopeLocal(float leftSlope);
        void SetTopSlopeLocal(float topSlope);
        void SetBottomSlopeLocal(float bottomSlope);
        virtual void SetPositionLocal(const DirectX::XMFLOAT3& origin) override;
        void SetOrientationLocal(const DirectX::XMFLOAT4 & orientation);

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

        CLASSNAME(FrustumCollider)
        CLASSSIZE(FrustumCollider)
        virtual void Serialize() override;
        virtual void DeSerialize() override;

       


    protected:
        const DirectX::BoundingFrustum& GetBoundingVolume() const;

        DirectX::BoundingFrustum mBoundingFrustum;
        DirectX::BoundingFrustum mBoundingFrustumOrigin;



    };

}
