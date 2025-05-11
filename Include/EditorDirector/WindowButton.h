#pragma once

#include"Predeclare.h"


#include"WindowControlEntity.h"

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
		virtual void HandleMouseMove(Event * pEvent);
		virtual void HandleLButtonUp(Event* pEvent) override;
		virtual void HandleHover(Event* pEvent) override;
		virtual void HandleLButtonDown(Event* pEvent) override;


		bool mSelectState = false;
	};
	
}

