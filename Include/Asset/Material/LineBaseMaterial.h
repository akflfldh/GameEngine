#pragma once



#include"Predeclare.h"
#include<DirectXMath.h>

#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB LineBaseMaterial
	{
	public:
		LineBaseMaterial() = default;
		~LineBaseMaterial() = default;

		void SetColor(const DirectX::XMFLOAT3& color);

		const DirectX::XMFLOAT3& GetColor()const;



	private:
		DirectX::XMFLOAT3 mColor;
		


	};

}
