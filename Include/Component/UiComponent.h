#pragma once


#include"BaseComponent.h"
#include"Object/Macro.h"
#include"InputType.h"

#include<functional>
#include<unordered_map>



namespace Quad
{

	class CORE_API_LIB UiComponent:public BaseComponent
	{
	public:
		UiComponent();
		virtual ~UiComponent();


		virtual void Start() override;//게임플레이 시작시한번호출
		virtual void Update(float delta) override;//게임플레이 매프레임 호출 //하는거없음


		void HandleEvent(const MouseInputData & data);

		void RegisterCallback(const std::function<void(const MouseInputData &)> & callback);

		template<typename T>
		void RegisterCallback(void(T::*callback)(const MouseInputData & ), T* caller);


		bool GetHoverState() const;
		bool GetLButtonDownState() const;
		bool GetRButtonDownState() const;


		void SetSelectKeepingFlag(bool flag);
		bool GetSelectKeepingFlag()const;

		void SetSelectAvailableFlag(bool flag);
		bool GetSelectAvailableFlag()const;

		void SetSelectBlockFlag(bool flag);
		bool GetSelectBlockFlag()const;

		void SetEntireSelectAvailableFlag(bool flag);
		bool GetEntireSelectAvailableFlag()const;
		
		virtual void SetDrawFlag(bool flag);
		bool GetDrawFlag()const;

		CLASSNAME(UiComponent)
		CLASSNAMESTATIC(UiComponent)

	private:

		

		void OnLButtonDown();
		void OnLButtonUp();
		void OnRButtonDown();
		void OnRButtonUp();
		void OnMouseMove();

		void SetHoverState(bool state);
		void SetLButtonDownState(bool state);
		void SetRButtonDownState(bool state);





		void SetSelectFlag(bool flag);
	//	void SendSelectEvent(bool state, bool exclusiveEventHandleFlag = false);
		bool GetSelectFlag()const;
;

		void SetExclusiveHoverEventFlag(bool flag);
		bool GetExclusiveHoverEventFlag() const;



	private:


		bool mActiveFlag;
		bool mSelectFlag;

		bool mSelectKeepingFlag = true;

		bool mSelectAvailableFlag = true;

		bool mSelectBlockFlag = true;

		bool mEntireSelectAvailableFlag = true;

		

		bool mHoverState  =false;
		bool mLButtonDownState = false;	//ui가 눌렸다.
		bool mRButtonDownState = false;	//ui가 눌렸다.

		bool mSelectState = false;
		bool mExclusiveHoverEventFlag = false;

		Texture* mDefaultTexture;
		Texture* mHoverTexture;
		Texture* mHoverSelectTexture;
		Texture* mNotHoverSelectTexture;



		std::vector< std::function<void(const MouseInputData&)>> mCallbackVector;


		

	};
	REGISTERCOMPONENTCLASS(UiComponent)

		template<typename T>
	inline void UiComponent::RegisterCallback(void(T::* callback)(const MouseInputData&), T* caller)
	{

		RegisterCallback(std::bind(callback, caller,std::placeholders::_1));

	}

}
