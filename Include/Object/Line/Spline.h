#pragma once


#include"Predeclare.h"

#include"Object/Line/LineBase.h"
#include<vector>
#include"Object/Macro.h"


//Spline은 일단 2D
namespace Quad
{
	class Curve;
	class CORE_API_LIB Spline:public LineBase
	{
		friend class LineFactory;

	public:
		enum class ESplineRenderStyle
		{
			eThinLine=0,		//실제라인,
			eThickLine			//삼각형메시를이용하여 두꼐를 가진다.
		};


	public:
		Spline(const std::string& name = "");
		virtual ~Spline();

		virtual void Initialize()override;

		//Initialize()호출후, Default로초기화하는경우
		//엔진에서 사용함으로 유저는다음을 작성한다.
		virtual void DefaultCreatingInitialize() override;



		virtual void OnEvent(Event* event) override;
		virtual void Start() override;
		virtual void Update(float deltaTime) override;
		virtual void EndUpdate(float deltaTime) override;


		//ex) num : 2  == 1번 curve뒤에 넣겠다.
		void AddCurve(Curve* curve, int num);
		void AddCurve(Curve* curve);


		Curve* GetCurve(int index);

		//curve의 컨트롤포인트,기울기, 삭제,추가할때마다 명시적으로 호출해줄것	
		void Evaulate();


		void SetThickness(float value);
		float GetThickness() const;

		virtual void Serialize() override;
		virtual void DeSerialize() override;


		void SetSplineRenderStyle(Quad::Spline::ESplineRenderStyle renderStyle);


		CLASSNAME(Spline)


	private:
		void EvaulateThickLine(std::vector<DirectX::XMFLOAT3>& vertexVector);



	private:
		std::vector<Curve*> mCurveVector;
		ESplineRenderStyle mSplineRenderStyle;

		float mThickness;


	};


}
