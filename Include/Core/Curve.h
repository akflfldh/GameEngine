#pragma once

#include<DirectXMath.h>
#include<vector>

#include"Core/CoreDllExport.h"

#include<vector>
namespace Quad
{

	class CORE_API_LIB Curve
	{

	public:
		Curve();
		virtual ~Curve() = 0;


		virtual void Evaulate(std::vector<DirectX::XMFLOAT3>& oVertexVector) = 0;


	protected:
		void SetControlPointVector(const std::vector < DirectX::XMFLOAT3>& controlPointVector);
		void SetControlPointVector(std::vector < DirectX::XMFLOAT3>&&controlPointVector);

		void SetControlPoint(const DirectX::XMFLOAT3& controlPoint, unsigned int index);

		const std::vector<DirectX::XMFLOAT3>& GetControlPointVector() const;



	



	private:
		std::vector<DirectX::XMFLOAT3> mControlPointVector;
	




	};

}
