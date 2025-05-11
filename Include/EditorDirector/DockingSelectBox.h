#pragma once

#include"Predeclare.h"
#include"Object/UiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class DockingSelectBox:public UiEntity
	{
	public:
		DockingSelectBox(const std::string& name="");
		virtual ~DockingSelectBox() = default;

		
		virtual void Initialize() override;

		static DockingSelectBox* Create(Map* map, int mapLayerIndex , EDockingDirection direction);


		virtual void Update(float deltaTime) override;
		virtual void OnEvent(Event * event) override;
		CLASSNAME(DockingSelectBox)

		EDockingDirection GetDirection()const;
		
		void SetSize(float width, float height);
		float GetWidth()const;
		float GetHeight()const;

		void SetRotataion(float degree);//z축을 기준으로 회전
		void SetPosition(float x, float y ,float z);

		bool GetHoverState()const;
		void SetHoverState(bool state);


		void SetDockingDirection(EDockingDirection dir);

	protected:
		void InitCreating(EDockingDirection direction);

	private:
		void HandleHover(Event* event);
		void HandleLButtonUp(Event* event);
	

	private:
		EDockingDirection mDirection;

		float mWidth;
		float mHeight;
		bool mHoverState;
		//lbuttonUp할때 hover상태이라면 ->도킹을 요청 프레임컨트롤러에게

	};
	REGISTEREDITORCLASS(DockingSelectBox)
}

