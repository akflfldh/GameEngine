#include "Curve.h"

Quad::Curve::Curve()
{


}

Quad::Curve::~Curve()
{
}

void Quad::Curve::SetControlPointVector(const std::vector<DirectX::XMFLOAT3>& controlPointVector)
{
	mControlPointVector = controlPointVector;

}

void Quad::Curve::SetControlPointVector(std::vector<DirectX::XMFLOAT3>&& controlPointVector)
{
	mControlPointVector = std::move(controlPointVector);
}

void Quad::Curve::SetControlPoint(const DirectX::XMFLOAT3& controlPoint, unsigned int index)
{
	mControlPointVector[index] = controlPoint;

}

const std::vector<DirectX::XMFLOAT3>& Quad::Curve::GetControlPointVector() const
{

	return mControlPointVector;
	// TODO: 여기에 return 문을 삽입합니다.
}
