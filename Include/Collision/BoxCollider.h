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
        BoxCollider();
        ~BoxCollider();
        virtual void Update(const DirectX::XMFLOAT4X4& transformMatrix) override;
        virtual void Update(const DirectX::XMMATRIX& transformMatrix)override;
        CLASSNAME(BoxCollider)
        CLASSSIZE(BoxCollider)

        virtual void Serialize() override;
        virtual void DeSerialize() override;
        void SetMinMaxVertexLocal(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max);
        void SetMinMaxVertexLocal(const std::pair<DirectX::XMFLOAT3,DirectX::XMFLOAT3> & mimMax);

        virtual void SetWidthLocalX(float width);
        virtual void SetHeightLocalY(float height);
        virtual void SetDepthLocalZ(float depth);

        void SetPositionLocal(const DirectX::XMFLOAT3& pos) ;

    protected:

        const DirectX::BoundingBox& GetBoundingVolume() const;
    protected:

        DirectX::BoundingBox mBoundingBox;
        DirectX::BoundingBox mBoundingBoxOrigin;

    };

}
