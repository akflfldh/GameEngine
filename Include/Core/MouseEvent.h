#pragma once


#include"Core/Event.h"
#include"Core/Ray.h"

#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB MouseEvent :public Event
	{

	public:
		MouseEvent(const std::string& name);
		~MouseEvent() = default;


		void SetClientPosition(int positionX, int positionY);
		int GetClientPositionX() const;
		int GetClientPositionY() const;

		void SetPreClientPosition(int positionX, int positionY);
		int GetPreClientPositionX() const;
		int GetPreClientPositionY() const;

		void SetScreenPosition(int positionX, int positionY);
		void SetPreScreenPosition(int positionX, int positionY);

		int GetScreenPositionX() const;
		int GetScreenPositionY() const;
		int GetPreScreenPositionX() const;
		int GetPreScreenPositionY() const;
		
		void SetRay(Ray ray);
		Ray GetRay()const;

		
		void SetHoverState(bool state);
		bool GetHoverState() const;

		void SetGlobalFlag(bool flag);
		bool GetGlobalFlag() const;

		void SetWheelShift(int shift);
		int GetWheelShift() const;

	private:
		int mClientPositionX;
		int mClientPositionY;
		int mScreenPositionX;
		int mScreenPositionY;


		int mPreClientPositionX;
		int mPreClientPositionY;
		int mPreScreenPositionX;
		int mPreScreenPositionY;

		Ray mRay;


		bool mHoverState;//선택한 엔티티위에서 L버튼up여부

		
		bool mGlobalEventFlag;// global event로 온건지 , 마우스피킹의 판정을에 검출되서 유일하게온것인지 구별하자.


		//메모리를 절약하기위해서는 유니온을 사용해야할것

		int mWheelShift;

	};

}