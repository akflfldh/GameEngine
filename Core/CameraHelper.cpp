#include "Object/Camera/CameraHelper.h"
#include"Object/Camera/Camera.h"
#include"Core/Ray.h"


void Quad::CameraHelper::CalculateRay(DirectX::XMFLOAT2 screenPos,
	D3D12_VIEWPORT viewPort, const Camera& camera, bool optionToWolrd, Ray& oRay)
{

	float ndcX = 2 * (screenPos.x - viewPort.TopLeftX) / viewPort.Width - 1;
	float ndcY = -2 * (screenPos.y - viewPort.TopLeftY) / viewPort.Height + 1;

	ECameraType cameraType = camera.GetCameraType();
	float proj00 = camera.GetProjMatrix()._11;
	float proj11 = camera.GetProjMatrix()._22;
	//if (cameraType == ECameraType::eOrthogonalCamera)
	//{
	//	
	//}



	float viewX = ndcX / proj00;
	float viewY = ndcY / proj11;
	float viewZ = 1.0f;
	DirectX::XMFLOAT3 dir = { 0,0,1.0 };
	DirectX::XMFLOAT3 origin = { 0,0,0 };
	if (cameraType == ECameraType::eOrthogonalCamera)
	{
		origin = { viewX,viewY,0.0f };
	}
	else if (cameraType == ECameraType::ePerspectiveCamera)
	{
		DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(DirectX::XMVectorSet(viewX, viewY, viewZ, 0.0f)));
	}


	if (optionToWolrd)
	{
		DirectX::XMFLOAT4X4 viewMatrixF = camera.GetViewMatrix();
		DirectX::XMMATRIX inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&viewMatrixF));
		DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&dir), inverseViewMatrix)));

		DirectX::XMStoreFloat3(&origin, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&origin), inverseViewMatrix));

	}

	oRay.SetDir(dir);
	oRay.SetOrigin(origin);


	return;

}

void Quad::CameraHelper::CalculateScreenToViewPos(DirectX::XMFLOAT2 screenPos, D3D12_VIEWPORT viewPort, const Camera& camera, bool optionToWolrd, DirectX::XMFLOAT3& oPos)
{
	float ndcX = 2 * (screenPos.x - viewPort.TopLeftX) / viewPort.Width - 1;
	float ndcY = -2 * (screenPos.y - viewPort.TopLeftY) / viewPort.Height + 1;

	const DirectX::XMFLOAT4X4& projMatrix = camera.GetProjMatrix();

	float proj00 = projMatrix._11;
	float proj11 = projMatrix._22;


	//항상 가까운 평면에 존재한다고처리한다. 
	float viewZ = camera.GetNearPlane();
	float viewX = ndcX / proj00 * viewZ;
	float viewY = ndcY / proj11 * viewZ;
	oPos = { viewX,viewY,viewZ };
	if (!optionToWolrd)
	{
		return;
	}

	const DirectX::XMFLOAT4X4& viewMatrix = camera.GetViewMatrix();
	DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&oPos), DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&viewMatrix)));

	return;

}


