#include "TransformNode.h"

Quad::ETransformType Quad::TransformNode::GetTransformType() const
{
    return mTransformType;
}

void Quad::TransformNode::SetTransformType(Quad::ETransformType transformType)
{
    mTransformType = transformType;
    switch (mTransformType)
    {
    case ETransformType::eScale:
    case ETransformType::eTranslation:
        mTransform.w = 0.0f;
    }



}

void Quad::TransformNode::SetScale(const DirectX::XMFLOAT3& scale)
{
    mTransform.x = scale.x;
    mTransform.y = scale.y;
    mTransform.z = scale.z;
    mTransform.w = 0.0f;
}

void Quad::TransformNode::SetOrientation(const DirectX::XMFLOAT4& quaternion)
{
    mTransform = quaternion;
}

void Quad::TransformNode::SetTranslation(const DirectX::XMFLOAT3& translation)
{
    mTransform.x = translation.x;
    mTransform.y = translation.y;
    mTransform.z = translation.z;
    mTransform.w = 1.0f;
}

const DirectX::XMFLOAT4 Quad::TransformNode::GetTransform() const
{
    return mTransform; 
}
