#include "ButtonUiEntitiy.h"
#include"Collision/CollisionHelper.h"
#include"MouseEvent.h"
#include"GameRenderWindow.h"
#include"Mouse.h"


namespace Quad
{

	ButtonUiEntity::ButtonUiEntity(const std::string& name)
		:UiEntity(name)
	{

	}

	void ButtonUiEntity::Initialize()
	{
		UiEntity::Initialize();


	
		mClickState = false;

	}

	ButtonUiEntity* ButtonUiEntity::Create(Map* map, int mapLayerIndex)
	{
		ButtonUiEntity* ob = static_cast<ButtonUiEntity*>(map->CreateObject("ButtonUiEntity", mapLayerIndex));
	//	ob->Initialize();
		ob->InitCreating();

		return ob;
	}

	void ButtonUiEntity::Update(float deltaTime)
	{
		UiEntity::Update(deltaTime);

	


	}

	void ButtonUiEntity::OnEvent(Event* event)
	{
		const std::string& eventName = event->GetName();


		if (eventName == "LButtonDown")
		{

			HandleLButtonDown(event);

		}else if(eventName =="LButtonUp")
		{

			HandleLButtonUp(event);
		
		}
		else if (eventName == "Hover")
		{
			HandleHover(event);

		}
		else if (eventName == "MouseMove")
		{

			HandleMouseMove(event);
		}
		else
		{
			UiEntity::OnEvent(event);
		}


		

	}

	void ButtonUiEntity::SetClickableComponent(ClickableComponent* clickableComponent)
	{
		mClickableComponent = clickableComponent;



	}

	void ButtonUiEntity::SetLButtonUpCallback(const std::function<void()>  &callback)
	{
		mLButtonUpCallback = callback;
	}

	void ButtonUiEntity::SetLButtonDownCallback(const std::function<void()> &callback)
	{
		mLButtonDownCallback = callback;
	}

	void ButtonUiEntity::SetHoverCallback(const std::function<void()> & callback)
	{
		mHoverCallback = callback;
	}

	void ButtonUiEntity::SetReleaseHoverCallback(const std::function<void()>  & callback)
	{
		mReleaseHoverCallback = callback;
	}

	const std::function<void()> *ButtonUiEntity::GetLButtonUpCallback() const
	{

		if (!mLButtonUpCallback)
			return nullptr;
		else
	


		return &mLButtonUpCallback;
	}

	const std::function<void()>* ButtonUiEntity::GetHoverCallback() const
	{
		if (mHoverCallback == nullptr)
			return nullptr;



		return &mHoverCallback;
	}

	const std::function<void()>* ButtonUiEntity::GetReleaseHoverCallback() const
	{
		if (mReleaseHoverCallback == nullptr)
			return nullptr;

		return &mReleaseHoverCallback;
	}

	void ButtonUiEntity::HandleHover(Event* pEvent)
	{

		StateComponent* stateComponent = GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		bool hoverState = mouseEvent->GetHoverState();
		if (hoverState == false)
		{
			int a = 2;
		}

		stateComponent->SetHoverState(hoverState);

		if (hoverState)
		{	

			if (stateComponent->GetLButtonDownState())
				return;

			

			if(mHoverCallback!=nullptr)
				mHoverCallback();
		}
		else
		{
			if (stateComponent->GetLButtonDownState())
				return;

			if(mReleaseHoverCallback!=nullptr)
			mReleaseHoverCallback();
			

		}

	}

	void ButtonUiEntity::HandleLButtonDown(Event* pEvent)
	{
		StateComponent* stateComponent = GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;
		

		if (stateComponent->GetHoverState())
		{

			stateComponent->SetLButtonDownState(true);

	


			if(mLButtonDownCallback!=nullptr)
				mLButtonDownCallback();


			Mouse::CaptureObject(this);
		}
	}

	void ButtonUiEntity::HandleLButtonUp(Event* pEvent)
	{
		StateComponent* stateComponent = GetStateComponent();

		if (stateComponent->GetLButtonDownState())
		{
			stateComponent->SetLButtonDownState(false);
			if (stateComponent->GetHoverState())
			{
				if(mLButtonUpCallback!=nullptr)
				mLButtonUpCallback();
				
			}

			Mouse::ReleaseCaptureObject(this);
		}
	}


	void ButtonUiEntity::HandleMouseMove(Event* pEvent)
	{







	}

	void ButtonUiEntity::InitCreating()
	{
		UiEntity::InitCreating();
		SetEffect("DefaultUi.effect");
	}


}