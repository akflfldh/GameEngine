#pragma once

#include"Predeclare.h"


#include"WindowControlEntity.h"
#include<InputType.h>

namespace Quad
{
	class WindowButton:public WindowControlEntity
	{
	public:
		WindowButton(const std::string & name="");
		virtual ~WindowButton() = default;

		virtual void Initialize() override;

		static WindowButton* Create(Map* map, int mapLayerIndex = 0);



		virtual void Update(float deltaTime) override final;
		virtual void OnEvent(Event * event) override;
		//virtual void SetWidthHeight(float width, float height)override;
		virtual void ChangeWindowSize(UINT width, UINT height) override;


		CLASSNAME(WindowButton)
		CLASSSIZE(WindowButton)
		//누를때
		virtual void HandlePress() = 0;

		//마우스가 위로올라올때,
		virtual void HandleHover() = 0;
		virtual void HandleNotHover() = 0;
		virtual void HandlePressHover() = 0;	//눌른상태에서 hover
		virtual void HandlePressNotHover() = 0;	//눌른상태에서 hover

		//누르고 띌때
		virtual void HandleReleaseHover() = 0; 
		virtual void HandleReleaseNotHover() = 0;

		//void SetSelectState(bool state);
		//bool GetSelectState() const;
	protected:
		void InitCreating();


	private:
		virtual void HandleMouseMove(const MouseInputData& mouseInputData);
		virtual void HandleLButtonUp(const MouseInputData& mouseInputData) override;
		virtual void HandleHover(const MouseInputData& mouseInputData) override;
		virtual void HandleLButtonDown(const MouseInputData& mouseInputData) override;


		bool mSelectState = false;
	};
	
}

