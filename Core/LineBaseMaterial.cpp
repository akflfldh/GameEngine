#include "Asset/Material/LineBaseMaterial.h"

void Quad::LineBaseMaterial::SetColor(const DirectX::XMFLOAT3& color)
{
	mColor = color;
}

//void Quad::LineBaseMaterial::SetColor(DirectX::XMVECTOR color)
//{
//
//	DirectX::XMStoreFloat3(&mColor, color);
//
//}

const DirectX::XMFLOAT3& Quad::LineBaseMaterial::GetColor() const
{
	// TODO: 여기에 return 문을 삽입합니다.

	return mColor;

}
