#pragma once


#include"Predeclare.h"
#include"Core/Controller.h"
#include"Core/CoreDllExport.h"
#include<vector>
namespace Quad
{
	

	class CORE_API_LIB BaseWindowController:public Controller
	{
	
	public:
		BaseWindowController(RenderSystem * renderSystem);
		~BaseWindowController() = default;

		virtual BaseWindow* GetWindow() const = 0 ;


		bool GetMousePositionState() const;
		void SetMousePositionState(bool state);

		void AddMsgHanlderComponent(BaseWindowMsgHandlerComponent* component);
		LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



		//mouse handler component를 가지고있어야 유효한값을 유지할것이다.
		void SetClientMousePosX(int pos);
		void SetClientMousePosY(int pos);

		int GetClientMousePosX() const;
		int GetClientMousePosY() const;

		float GetWindowPositionX() const;
		float GetWindowPositionY() const;

		void SetWindowPositionX(float x);
		void SetWindowPositionY(float y);

		RenderSystem* GetRenderSystem() ;
		const RenderSystem* GetRenderSystem() const;
	private:
		bool mMousePositionState;
		std::vector<BaseWindowMsgHandlerComponent*> mWindowMsgHandlerComponentVector;

		int mClientMousePosX = 0;
		int mClientMousePosY = 0;


		float mWindowPositionX;
		float mWindowPositionY;

		RenderSystem* mRenderSystem;
	};

}
