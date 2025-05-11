#pragma once


#include"Predeclare.h"

#include"Component.h"


#include"Core/CoreDllExport.h"

namespace Quad
{
	class CORE_API_LIB StateComponent:public Component<StateComponent>
	{
	public:
		StateComponent();
		~StateComponent() = default;

		void Initialize(Object* destObject);
		virtual void Update(float delta =0) override;


		void SetLButtonDownState(bool state);
		void SetRButtonDownState(bool state);
		void SetHoverState(bool state);
		void SetSelectState(bool state, bool exclusiveEventHandleFla = false);


		bool GetLButtonDownState() const;
		bool GetRButtonDownState() const;
		bool GetHoverState() const;
		bool GetSelectState()const;
		
		void SetExclusiveHoverEventFlag(bool flag);
		bool GetExclusiveHoverEventFlag() const;


	private:
		Object* mDestObject;

		bool mLButtonDownState = false;
		bool mRButtonDownState = false;
		bool mHoverState=false;
		bool mSelectState=false;
		bool mExclusiveHoverEventFlag = false;
	





	};

}
