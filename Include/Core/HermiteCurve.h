#pragma once


#include"Core/Curve.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB HermiteCurve:public Curve
	{
	public:
		HermiteCurve();
		~HermiteCurve();
		void Evaulate(std::vector<DirectX::XMFLOAT3>& oVertexVector) override;

	private:

	};

}
