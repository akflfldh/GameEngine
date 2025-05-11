#pragma once

#include"Predeclare.h"

#include"BaseWIndowController.h"
#include"Utility/Singleton.h"

#include"PopupWindow.h"

#include<vector>
#include<functional>
namespace Quad
{
	class PopupWindowController:public BaseWindowController,public Singleton<PopupWindowController>
	{
		friend class ProjectDirector;
	public:
		PopupWindowController(RenderSystem * renderSystem);
		~PopupWindowController() = default;

		 void Initialize(PopupWindow * window,PopupWindowUiSystem* uiSystem) ;
	

		 void Update(float deltaTime);
		 void UploadObjectToRenderSystem();
		 void EndUpdate(float deltaTime);
		 void Draw();
		 void OnResize(int clientWidth, int clientHeigh);

		 void HandleEvent(Event* pEvent);

		 static void ActivateWindow(int popupPanelID , int screenPosX, int screenPosY);
		 static void DeActivateWindow();

		 virtual PopupWindow* GetWindow() const override;

		 static int RegisterWindowInActiveCallback(const std::function<void()>& callback);
		 static void SetStateWindowInActiveCallback(int index,bool state);

		 void CallInActiveCallback();

		 void HandleInActiveMsg();

		 void SetWindowVisibilityState(bool flag);
		 

		 void SetKeepVisiblity(bool state);
		 bool GetKeepVisiblity() const;

		 void SetVisiblity(bool state);
		 bool GetVisiblity()const;

	private:

		void AdjustWindowPos(int screenPosX, int screenPosY,int width, int height);


	private:
		virtual void Initialize() override;

		PopupWindow* mPopupWindow;
		//RenderSystem* mRenderSystem;

		//uiSystem,renderSystem만있어도될거같은데
	
		PopupWindowUiSystem* mUiSystem;
		
		HMONITOR mMonitorHandle;



							//콜백활성화여부, 
		std::vector<std::pair<bool, std::function<void()>>> mPopupInActiveCallbackTable;

		bool mVisibleState;//맨초기에는 보이지않을것이다.

		//popup 창이 비활성화(InActive)되더라도 계속 보여지는가
		bool mKeepVisiblity;


	};

}