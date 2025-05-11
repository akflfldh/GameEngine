#pragma once

#include"Predeclare.h"

#include"Utility/Singleton.h"
#include<DirectXMath.h>

namespace Quad
{
	class FrameWindowMenuDirector :public Singleton<FrameWindowMenuDirector>
	{
	public:
		FrameWindowMenuDirector() = default;
		~FrameWindowMenuDirector() = default;

		void Initialize(WindowChromeSystem * frameWindowChromeSystem);
	
		static void SetMainScene();




		
	private:
		void CreateFileMenu(TextBoxPanel* menu, Texture* titleBarTexture);
		void CreateToolsMenu(TextBoxPanel* menu, Texture* titleBarTexture);


		static void CreateFileSubMenuChildPanelUiEntities(ListPanel * parentListPanel);
		
		void CreateToolSubMenuChildPanelUiEntities(ListPanel* parentListPanel);

		
		


		void CreateCreateEntityClassPopupPanel(int popupPanelID);
		void CreateBuildPopupPanel(int popupPanelID);

	private:
		WindowChromeSystem* mFrameWindowChromeSystem;



	};
}
