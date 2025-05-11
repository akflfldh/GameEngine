#pragma once

#include "Collision/Collider.h"
#include"DirectXCollision.h"

#include"Core/CoreDllExport.h"

namespace Quad
{

    class CORE_API_LIB BoxCollider :
        public Collider
    {

        friend class ColliderGenerator;
        friend class CollisionHelper;

    public:
        ~BoxCollider();
        virtual void OnEvent(Event* event) override;
        virtual void Update(float deltaTime) override;
        virtual void Update(float deltaTime, const DirectX::XMFLOAT4X4& transformMatrix) override;
        virtual void Update(float deltaTime, const DirectX::XMMATRIX& transformMatrix)override;
        CLASSNAME(BoxCollider)
        CLASSSIZE(BoxCollider)

        virtual void Serialize() override;
        virtual void DeSerialize() override;
        virtual void SetMesh(Mesh* mesh) override;
        void SetMinMaxVertex(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max);

        virtual void SetWidthX(float width);
        virtual void SetHeightY(float height);
        virtual void SetDepthZ(float depth);

        virtual void SetPosition(const DirectX::XMFLOAT3& pos);


    protected:
        void Initialize(const DirectX::XMFLOAT3 & center, const DirectX::XMFLOAT3& extents,Mesh * mesh);

        void Initialize(const DirectX::BoundingBox& boundingBox,Mesh * mesh);

        BoxCollider();

        const DirectX::BoundingBox& GetBoundingVolume() const;
    protected:

        DirectX::BoundingBox mBoundingBox;
        DirectX::BoundingBox mBoundingBoxOrigin;

    };

}
