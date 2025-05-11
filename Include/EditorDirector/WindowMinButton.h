#pragma once

#include"Predeclare.h"
#include"WindowButton.h"
#include"EditorMacro.h"
namespace Quad
{
	class WindowMinButton:public WindowButton
	{
	public:
		WindowMinButton(const std::string& name="");
		virtual ~WindowMinButton() = default;

		virtual void Initialize() override;

		static WindowMinButton* Create(Map* map, int mapLayerIndex = 0);

		virtual void OnEvent(Event * event) override;
		//virtual void SetWidthHeight(float width, float height)override;
		virtual void ChangeWindowSize(UINT width, UINT height) override;

		//누를때
		virtual void HandlePress();

		//마우스가 위로올라올때,
		virtual void HandleHover();
		virtual void HandleNotHover();
		virtual void HandlePressHover();	//눌른상태에서 hover
		virtual void HandlePressNotHover();	//눌른상태에서 hover

		//누르고 띌때
		virtual void HandleReleaseHover();
		virtual void HandleReleaseNotHover() ;

		CLASSSIZE(WindowMinButton)
		CLASSNAME(WindowMinButton)
	protected:
		void InitCreating();

	private:
		void HandleWindowResizeEvent(Event* pEvent);




	};
	REGISTEREDITORCLASS(WindowMinButton)
}

