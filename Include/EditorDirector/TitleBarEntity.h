#pragma once

#include"Predeclare.h"
#include"WindowControlEntity.h"
#include"WindowCloseButton.h"

#include"Macro.h"
#include"EditorMacro.h"
namespace Quad
{
	class TitleBarEntity:public WindowControlEntity
	{
	public:
		
		TitleBarEntity();
		virtual ~TitleBarEntity();

		virtual void Initialize() override;
		

		static TitleBarEntity* Create(Map* map, int mapLayerIndex = 0);

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event * pEvent) override;


		CLASSNAME(TitleBarEntity)
		void SetSize(float width,float height) override;
		void ChangeWindowSize(UINT width, UINT height) override;
	protected:
		void InitCreating();


	private:
	

		void HandleMouseMove(Event* pEvent);
		void HandleLButtonUp(Event* pEvent);
		void HandleWindowResize(Event* pEvent);
		void HandleLButtonDown(Event* pEvent);
		void HandleHover(Event* pEvent);
		//WindowCloseButton mWindowCloseButton;
	};
	REGISTEREDITORCLASS(TitleBarEntity)

}
