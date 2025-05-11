#pragma once

#include"header.h"

#include"Core/CoreDllExport.h"


namespace Quad
{
	class CORE_API_LIB Ray
	{
	public:
		Ray();
		Ray(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 dir);
		~Ray();

		void SetOrigin(DirectX::XMFLOAT3 origin);
		void SetDir(DirectX::XMFLOAT3 dir);

		DirectX::XMFLOAT3 GetOrigin() const;
		DirectX::XMFLOAT3 GetDir() const;

		bool IsInWorldSpace() const;
		void SetWorldSpaceFlag(bool flag);
		
		void SetParameterT(float t);
		float GetParameterT()const;

	private:
		bool mWorldSpaceFlag;
		DirectX::XMFLOAT3 mOrigin;
		DirectX::XMFLOAT3 mDir;

		float mParameterT;

	};
}

