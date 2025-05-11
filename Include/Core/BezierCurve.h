#pragma once

#include"Curve.h"
#include"Core/CoreDllExport.h"
#include<vector>
namespace Quad

{
	//3차베지어곡선

	class CORE_API_LIB BezierCurve:public Curve
	{
	public:
		BezierCurve();
		~BezierCurve();

		//index<4
		void SetControlPoint(const DirectX::XMFLOAT3& controlPoint, unsigned int index);

		bool GetControlPoint(int index, DirectX::XMFLOAT3& oControlPoint) const;

		void Evaulate(std::vector<DirectX::XMFLOAT3> & oVertexVector) override;
	private:




	};

}
