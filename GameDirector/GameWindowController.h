#pragma once


#include"BaseWindowController.h"

namespace Quad
{
	class GameWindow;
	class Game3DSystem;
	class GameUiSystem;
	class RenderSystem;
	

	class GameWindowController:public BaseWindowController
	{
	public:
		GameWindowController(GameWindow * gameWindow);
		~GameWindowController() = default;

		virtual BaseWindow* GetWindow() const;

		void Update(float deltaTime);
		void UploadObjectToRenderSystem();
		void EndUpdate();
		void Draw();

		void OnResize(int clientWidth, int clientHeigh, int direction);


	private:
		GameWindow* mWindow;

		Game3DSystem* mGame3DSystem;
		GameUiSystem* mGameUiSystem;
	
	};


}