#pragma once

#include"header.h"
#include"Predeclare.h"
//#include"Camera.h"
//#include"Ray.h"



#ifdef UTILITY_LIB_EXPORT
#define UTILITY_LIB_API __declspec(dllexport)
#else
#define UTILITY_LIB_API __declspec(dllimport)
#endif


namespace Quad
{
	class UTILITY_LIB_API MathHelper
	{
	public:
		//view space  or world space


		/*static void CalculateRay(DirectX::XMFLOAT2 screenPos, D3D12_VIEWPORT viewPort,
			const Camera& camera, bool optionToWolrd, Ray& oRay);

		static void CalculateScreenToViewPos(DirectX::XMFLOAT2 screenPos, D3D12_VIEWPORT viewPort,
			const Camera& camera, bool optionToWolrd, DirectX::XMFLOAT3& oPos);*/


		static DirectX::XMFLOAT3 XM_CALLCONV GetEulerAnglesFromDCM(DirectX::FXMMATRIX rotationMatrix);
		static bool	XM_CALLCONV EqaulMatrix(DirectX::FXMMATRIX matrixOne, DirectX::CXMMATRIX matrixTwo);

		static DirectX::XMFLOAT3 ConvertQuaternionToEulerAngle(const DirectX::XMFLOAT4 & quaternion);

		static DirectX::XMMATRIX XM_CALLCONV CaculateViewportMatrix(const D3D12_VIEWPORT & viewport);

	private:




	};

}
