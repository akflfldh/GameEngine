#include "ScrollBarEntity.h"
#include"System.h"
#include"MouseEvent.h"
#include"Mouse.h"
#include"Window.h"
#include"Object/Camera/Camera.h"


Quad::ScrollBarEntity::ScrollBarEntity(const std::string& name)
	:PanelUiEntity(name),mScrollDirection(EScrollDirection::eVertical),mMaxAvailableShiftValue(0), mCurrentShiftValue(0), mWheelShiftFactor(1)
{
}



void Quad::ScrollBarEntity::Initialize()
{
	PanelUiEntity::Initialize();


}

void Quad::ScrollBarEntity::Update(float deltaTime)
{

	PanelUiEntity::Update(deltaTime);

}

void Quad::ScrollBarEntity::OnEvent(Event* event)
{

	PanelUiEntity::OnEvent(event);

	const std::string& eventName = event->GetName();
	if (eventName == "Wheel")
	{
		HandleWheel(event);
	}

}



Quad::EScrollDirection Quad::ScrollBarEntity::GetScrollDirection() const
{
	return mScrollDirection;
}

void Quad::ScrollBarEntity::SetMaxAvailableShiftValue(float max)
{
	mMaxAvailableShiftValue = max;

}

float Quad::ScrollBarEntity::GetMaxAvailableShiftValue() const
{
	return mMaxAvailableShiftValue;
}

float Quad::ScrollBarEntity::GetCurrentShiftValue() const
{
	return mCurrentShiftValue;
}




void Quad::ScrollBarEntity::SetDefaultPositionLocal(const DirectX::XMFLOAT3& defaultPositionLocal)
{
	mDefaultPositionLocal = defaultPositionLocal;
//	GetTransform().SetPositionLocal(mDefaultPositionLocal);


}

void Quad::ScrollBarEntity::Reset()
{
	GetTransform().SetPositionLocal(mDefaultPositionLocal);
	mCurrentShiftValue = 0.0f;

}

void Quad::ScrollBarEntity::Shift(float shift)
{

	mCurrentShiftValue += shift;
	if (mCurrentShiftValue < 0)
		mCurrentShiftValue = 0.0f;

	if (mCurrentShiftValue > mMaxAvailableShiftValue)
		mCurrentShiftValue = mMaxAvailableShiftValue;

	DirectX::XMFLOAT3 posLocal = mDefaultPositionLocal;
	switch (mScrollDirection)
	{
	case EScrollDirection::eVertical:

		//수직는 밑으로 이동한다
		posLocal.y -= mCurrentShiftValue;
		break;

	case EScrollDirection::eHorizontal:
		posLocal.x += mCurrentShiftValue;
		break;

	}
	GetTransform().SetPositionLocal(posLocal);


	

}

void Quad::ScrollBarEntity::SetShiftValue(float shiftValue)
{
	if (shiftValue < 0)
		mCurrentShiftValue = 0;
	else if (shiftValue > mMaxAvailableShiftValue)
		mCurrentShiftValue = mMaxAvailableShiftValue;

	DirectX::XMFLOAT3 posLocal = mDefaultPositionLocal;
	switch (mScrollDirection)
	{
	case EScrollDirection::eVertical:

		//수직는 밑으로 이동한다
		posLocal.y -= mCurrentShiftValue;
		break;

	case EScrollDirection::eHorizontal:
		posLocal.x += mCurrentShiftValue;
		break;

	}
	GetTransform().SetPositionLocal(posLocal);


	//shiftValue를설정햇으니 마우스로 움직인것처럼 처리해주어야한다.
	StateComponent* stateCompoent = GetStateComponent();
	stateCompoent->SetLButtonDownState(true);
	auto callback = GetEventCallback("MouseMove");
	if (callback != nullptr)
		(*callback)(nullptr);
	stateCompoent->SetLButtonDownState(false);

}

void Quad::ScrollBarEntity::SetWheelShiftFactor(float wheelShiftFactor)
{
	mWheelShiftFactor = wheelShiftFactor;
}

void Quad::ScrollBarEntity::InitCreating(EScrollDirection direction, float width, float height, float maxAvailableShiftValue)
{
	PanelUiEntity::InitCreating();

	SetSize(width, height);
	mScrollDirection = direction;
	mMaxAvailableShiftValue = maxAvailableShiftValue;

}

void Quad::ScrollBarEntity::HandleLButtonDown(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{

		stateComponent->SetLButtonDownState(true);
		Mouse::CaptureObject(this);
	}

}

void Quad::ScrollBarEntity::HandleLButtonUp(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{

		stateComponent->SetLButtonDownState(false);
		Mouse::ReleaseCaptureObject(this);

	}


}

void Quad::ScrollBarEntity::HandleWheel(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	int wheelShift = mouseEvent->GetWheelShift();
	//휠은 밑으로 내리면 음수이니 -1을 곱해준다
	wheelShift = -1 * wheelShift / 120 * mWheelShiftFactor;



	//mouse move와 동일(마우스 클릭으로 이동할떄와 동일)
	Shift(wheelShift);

	stateComponent->SetLButtonDownState(true);
	auto callback = GetEventCallback("MouseMove");
	if (callback != nullptr)
		(*callback)(pEvent);
	stateComponent->SetLButtonDownState(false);

}

void Quad::ScrollBarEntity::HandleMouseMove(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{	
		MouseEvent* mouseEvent = (MouseEvent*)pEvent;
		int mouseClientShift = 0;
		if (mScrollDirection == EScrollDirection::eVertical)
		{

			mouseClientShift = mouseEvent->GetClientPositionY() - mouseEvent->GetPreClientPositionY();

		}
		else if(mScrollDirection==EScrollDirection::eHorizontal)
		{
			mouseClientShift = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();
		}

		Shift(mouseClientShift);
	}

	auto callback = GetEventCallback("MouseMove");
	if (callback != nullptr)
		(*callback)(pEvent);





}


Quad::ScrollBarEntity* Quad::ScrollBarEntity::Create(Map* map, int mapLayerIndex, EScrollDirection direction, float width, float height, float maxAvailableShiftValue)
{
	ScrollBarEntity* ob = static_cast<ScrollBarEntity*>(map->CreateObject("ScrollBarEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating(direction, width, height, maxAvailableShiftValue);

	return ob;
}

