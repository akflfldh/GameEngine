#pragma once

#include"Object/UiEntity.h"
#include"Predeclare.h"

#include"EditorMacro.h"
namespace Quad
{
    class SizeControllerStickEntity :public UiEntity
    {
    public:
		SizeControllerStickEntity(const std::string& name="");
		virtual ~SizeControllerStickEntity() = default;

		virtual void Initialize() override;

		static SizeControllerStickEntity* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;

		CLASSNAME(SizeControllerStickEntity)

		virtual void SetWidthHeight(float width, float height) ;
		//virtual void ChangeWindowSize(UINT width, UINT height) ;
		virtual void SetPosition(float frameWindowClientPositionX,float frameWindowClientPositionY);
		float GetPositionX()const;
		float GetPositionY()const;


		ESizeControlStickDirection GetDirection()const;
		void SetDirection(ESizeControlStickDirection direction);

	protected:
		void InitCreating();

	private:
		void HandleHoverEvent(Event* pEvent);
		void HandleLButtonDownEvent(Event* pEvent);
		void HandleLButtonUpEvent(Event* pEvent);
		void HandleMouseMoveEvent(Event* pEvent);

		//void HandleMouseMoveEvent(Event* pEvent);
		


    private:


		float mFrameWindowClientPositionX;
		float mFrameWindowClientPositionY;

		ESizeControlStickDirection mDirection;	//수평막대인가 수직막대인가.


    };
	REGISTEREDITORCLASS(SizeControllerStickEntity)

}

