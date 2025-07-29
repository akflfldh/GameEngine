#pragma once



#include"SuperController.h"
#include<Windows.h>
namespace Quad
{
	class BaseWindow;

	class SuperAssetBrowerController:public SuperController
	{
		public:
			static SuperAssetBrowerController* GetInstance();
			virtual ~SuperAssetBrowerController() ;
			virtual void Initialize() override ;

			virtual void PreUpdate() override;
			virtual void Update(float deltaTime) override;
			virtual void EndUpdate() override;
			virtual void Draw() override;

			virtual BaseWindow* GetWindow() override;


			void TestRButtonDown();

	private:
		LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		SuperAssetBrowerController();
	private:
		BaseWindow* mWindow;
	};

}
