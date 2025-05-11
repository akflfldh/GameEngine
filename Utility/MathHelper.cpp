#include "Utility/MathHelper.h"



namespace Quad
{
	

	DirectX::XMFLOAT3 MathHelper::GetEulerAnglesFromDCM(DirectX::FXMMATRIX rotationMatrix)
	{


		//1 - 2 - 3 오일러회전을 가정

		float roll,pitch,yaw;
		DirectX::XMFLOAT4X4 rotationMatrixF;
		DirectX::XMStoreFloat4x4(&rotationMatrixF, rotationMatrix);
		pitch =	std::asin(rotationMatrixF._13);
		roll = std::atan2(-1.0f * rotationMatrixF._23, rotationMatrixF._33);
		yaw = std::atan2(-1.0f * rotationMatrixF._12 , rotationMatrixF._11);


		return { roll,pitch,yaw };
	}

	bool XM_CALLCONV MathHelper::EqaulMatrix(DirectX::FXMMATRIX matrixOne, DirectX::CXMMATRIX matrixTwo)
	{
		for (int i = 0; i < 4; ++i)
		{
			DirectX::XMFLOAT4 result;
			DirectX::XMStoreFloat4(&result, DirectX::XMVectorEqual(matrixOne.r[i], matrixTwo.r[i]));
			if (result.x == 0)
				return false;
		}
		return true;
	}

	DirectX::XMFLOAT3 MathHelper::ConvertQuaternionToEulerAngle(const DirectX::XMFLOAT4& quaternion)
	{
		float qx = quaternion.x;
		float qy = quaternion.y;
		float qz = quaternion.z;
		float qw = quaternion.w;

		float qx2 = qx * qx;
		float qy2 = qy * qy;
		float qz2 = qz * qz;
		float qw2 = qw * qw;
	
		float x = atan2(2 * (qy * qz + qw * qx), qw2 + qz2 - qx2 - qy2);
		float y = asin(2 * (qw * qy - qx * qz));
		float z = atan2(2 * (qx * qy + qz * qw), qw2 + qx2 - qy2 - qz2);
		
		x *= (180.0f / DirectX::XM_PI);
		y *= (180.0f / DirectX::XM_PI);
		z *=  (180.0f / DirectX::XM_PI);

	/*	if (y == 90.0f || y == -90.0f)
			y += 0.01f;*/

		return { x,y,z };

	}

	 DirectX::XMMATRIX XM_CALLCONV MathHelper::CaculateViewportMatrix(const D3D12_VIEWPORT& viewport)
	{


		DirectX::XMMATRIX scalingMatrix =		DirectX::XMMatrixScaling(viewport.Width / 2.0f, -1.0f*viewport.Height / 2.0f,viewport.MaxDepth-viewport.MinDepth);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(viewport.TopLeftX + viewport.Width / 2.0f, viewport.TopLeftY + viewport.Height / 2.0f, viewport.MinDepth);

		
		return DirectX::XMMatrixMultiply(scalingMatrix, translationMatrix);


		// TODO: 여기에 return 문을 삽입합니다.
	}


}