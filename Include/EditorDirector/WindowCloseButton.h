#pragma once


//ui가 아니라 아에 windowControl이라는 부모클래스를 만들어서 파생시키면 더 괜찮을수있겠다.
//그리고이들은 ndc공간을 기준으로 하니까.

#include"Predeclare.h"

#include"WindowButton.h"
#include"EditorMacro.h"
namespace Quad
{
	class WindowCloseButton :public WindowButton
	{
	public:
		WindowCloseButton(const std::string & name="");
		~WindowCloseButton() = default;

		
		void Initialize() override;
		static WindowCloseButton* Create(Map* map, int mapLayerIndex = 0);


		virtual void OnEvent(Event* event) override;

		//마우스가 위로올라올때,
		virtual void HandleHover() override;	//그냥 마우스가 위로올라온상태
		virtual void HandleNotHover()override;	//올라왔다 내려간상태
		virtual void HandlePressHover() override;	//눌른상태에서 hover
		virtual void HandlePressNotHover() override;	//눌른상태에서 hover
		//누를떄(누른상태유지)
		virtual void HandlePress() override;

		//누르고 띌때
		virtual void HandleReleaseHover() override;	//눌렀다 띌떄 위에있는상ㅇ태
		virtual void HandleReleaseNotHover() override;	//눌렀다 뛸때 위에없는상태

	//	virtual void SetWidthHeight(float width, float height) override;
		virtual void ChangeWindowSize(UINT width, UINT height) override;
		
		CLASSSIZE(WindowCloseButton)
		CLASSNAME(WindowCloseButton)

	protected:
		void InitCreating();

	private:
		void HandleWindowResizeEvent(Event* pEvent);




	};
	REGISTEREDITORCLASS(WindowCloseButton)
}

