#pragma once
#include"ICameraEventComponent.h"
#include"Core/CoreDllExport.h"
namespace Quad
{
	class OrthogoanlCamera;
	//윈도우의 사이즈가 변해도 카메라가 상-하 , 좌-우, 한쪽에 특정좌표값으로 고정되도록하는 컴포넌트 
	//직교투영 카메라 전용
	class CORE_API_LIB CameraFixWindowResizeEventComponent:public ICameraEventComponent
	{
		
		enum class EVerticalLineDir
		{
			eLeft=0,
			eRight
		};

		enum class EHorizontalLineDir
		{
			eTop = 0,
			eBottom
		};

	public:
		CameraFixWindowResizeEventComponent();
		virtual ~CameraFixWindowResizeEventComponent();

		void Initialize(OrthogoanlCamera* destCamera);

			//ex) left,top을 기준으로 하겠다.
		void SetLineDir(EVerticalLineDir vericalLine, EHorizontalLineDir horizontalLine);

		void SetVerticalLineOrigin(float origine);
		void SetHorizontalLineOrigin(float origine);

		virtual void OnEvent(Event* pEvent);

	private:
		void AdjustCameraPos();


		EVerticalLineDir mVerticalLineDir;
		EHorizontalLineDir mHorizontalLineDir;


		//창의 크기가 변해서 직교투영카메라의 뷰프러스텀의 크기가변하여도 
		//  항상 origine이 위치의 기준이 되어 위치를 설정한다..
		// top,left = 0이고 카메라 뷰프러스텀의 가로 =100, 세로=100이면  카메라의 위치는 x = left+가로/2, y= top -세로/2가 된다.
 		float mVerticalLineOrigine;
		float mHorizontalLineOrigine;

		OrthogoanlCamera* mDestCamera;
	};

}
