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
        ~FrustumCollider();
        virtual void OnEvent(Event* event)override;
        virtual void Update(float deltaTime) override;
        virtual void Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix) override;
        virtual void Update(float deltaTime, const DirectX::XMMATRIX& transformMatrix)override;

        CLASSNAME(FrustumCollider)
        CLASSSIZE(FrustumCollider)
        virtual void Serialize() override;
        virtual void DeSerialize() override;

        virtual void SetMesh(Mesh* mesh) override;

    protected:
        void Initialize(Mesh * mesh, 
            float nearPlane, float farPlane, float rightSlope, float leftSlope,
            float topSlope, float bottomSlope, const DirectX::XMFLOAT3& origin = { 0,0,0 }, const DirectX::XMFLOAT4& orientation = { 0,0,0,1.0f });

        void Initialize(Mesh * mesh ,
            const DirectX::BoundingFrustum& boundingFrustum);

        const DirectX::BoundingFrustum& GetBoundingVolume() const;


        FrustumCollider();


        DirectX::BoundingFrustum mBoundingFrustum;
        DirectX::BoundingFrustum mBoundingFrustumOrigin;



    };

}
