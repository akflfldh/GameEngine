#pragma once

#include"Object/UiEntity.h"
#include"EditorDirector/ClickableComponent.h"
#include<functional>
#include"EditorMacro.h"
namespace Quad
{

	class ButtonUiEntity :public UiEntity
	{
	public:
		ButtonUiEntity(const std::string& name="");
		~ButtonUiEntity() = default;

		void Initialize() override;

		static ButtonUiEntity* Create(Map* map, int mapLayerIndex = 0);

		void Update(float deltaTime) override;
		
		CLASSNAME(ButtonUiEntity)
		CLASSSIZE(ButtonUiEntity)
		virtual void OnEvent(Event* event) override;

		//유저가 이 버튼이 눌렸을떄 떼어졌을떄 동작에대해서는 
// 이 clickableComponent를 상속해서 유저만의 컴포넌트를만들고(결국 그 clickDown,clickUp 오버라이딩)
// 그 컴포넌트를 ButtonUiEntity에 넘기면된다.
//그럼 그 엔티티는 유저가 정의한 동작을 클릭될때마다 수행될것이다.
//근데 만약 무조건 필요한 동작이있다면 그것은 Clickable컴포넌트에서 상속되지않는 final함수를 만들고 ex ExcuteClickOn '
//이함수안에서는 무조건 필요한작업가 clickDown,clickUp메서드(오버라이딩된)를 호출한다 

		void SetClickableComponent(ClickableComponent* clickableComponent);


		void SetLButtonUpCallback(const std::function<void()>&  callback);
		void SetLButtonDownCallback(const std::function<void()>& callback);
		void SetHoverCallback(const std::function<void()> &callback);
		void SetReleaseHoverCallback(const std::function<void()> &callback);



		const std::function<void()>* GetLButtonUpCallback() const;
		const std::function<void()>* GetLButtonDownCallback() const;
		const std::function<void()>* GetHoverCallback() const;
		const std::function<void()>* GetReleaseHoverCallback() const;

	private:
		virtual void HandleHover(Event* pEvent);
		virtual void HandleLButtonDown(Event* pEvent);
		virtual void HandleLButtonUp(Event* pEvent);
		virtual void HandleMouseMove(Event* pEvent);

	protected:

		void InitCreating();


	private:
		ClickableComponent* mClickableComponent;



		std::function<void()> mLButtonDownCallback = nullptr;
		std::function<void()> mLButtonUpCallback = nullptr;
		std::function<void()> mHoverCallback = nullptr;
		std::function<void()> mReleaseHoverCallback = nullptr;
		//
		bool mClickState;




	};
	REGISTEREDITORCLASS(ButtonUiEntity)

}