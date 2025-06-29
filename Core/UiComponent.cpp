#include "Component/UiComponent.h"

Quad::UiComponent::UiComponent()
{
}

Quad::UiComponent::~UiComponent()
{
}

void Quad::UiComponent::Start()
{
}

void Quad::UiComponent::Update(float delta)
{
}

void Quad::UiComponent::HandleEvent(const MouseInputData& data)
{	
	for (const auto& callbackElement : mCallbackVector)
	{
		callbackElement(data);
	}

	switch (data.mMouseInputType)
	{
	case EMouseInput::eLButtonDown:

		OnLButtonDown();
		break;
	case EMouseInput::eLButtonUp:

		OnLButtonUp();
		break;
	case EMouseInput::eRButtonDown:

		OnRButtonDown();

		break;
	case EMouseInput::eRButtonUp:

		OnRButtonUp();

		break;
	case EMouseInput::eMouseMove:

		OnMouseMove();
		break;

	}



	
}

void Quad::UiComponent::RegisterCallback(const std::function<void(const MouseInputData &)>& callback)
{

	mCallbackVector.push_back(callback);

}

bool Quad::UiComponent::GetHoverState() const
{
	return mHoverState;
}

bool Quad::UiComponent::GetLButtonDownState() const
{
	return mLButtonDownState;
}

bool Quad::UiComponent::GetRButtonDownState() const
{
	return mRButtonDownState;
}

void Quad::UiComponent::OnLButtonDown()
{
	SetLButtonDownState(true);





}

void Quad::UiComponent::OnLButtonUp()
{

	SetRButtonDownState(true);



}

void Quad::UiComponent::OnRButtonDown()
{
	SetLButtonDownState(false);


}

void Quad::UiComponent::OnRButtonUp()
{
	SetRButtonDownState(false);

}

void Quad::UiComponent::OnMouseMove()
{

	//hover 설정


}

void Quad::UiComponent::SetHoverState(bool state)
{
	mHoverState = state;
}

void Quad::UiComponent::SetLButtonDownState(bool state)
{
	mLButtonDownState = state;
}

void Quad::UiComponent::SetRButtonDownState(bool state)
{
	mRButtonDownState = state;
}


void Quad::UiComponent::SetSelectKeepingFlag(bool flag)
{
}

bool Quad::UiComponent::GetSelectKeepingFlag() const
{
	return false;
}

void Quad::UiComponent::SetSelectAvailableFlag(bool flag)
{
}

bool Quad::UiComponent::GetSelectAvailableFlag() const
{
	return false;
}

void Quad::UiComponent::SetSelectBlockFlag(bool flag)
{
}

bool Quad::UiComponent::GetSelectBlockFlag() const
{
	return false;
}

void Quad::UiComponent::SetEntireSelectAvailableFlag(bool flag)
{
}

bool Quad::UiComponent::GetEntireSelectAvailableFlag() const
{
	return false;
}

void Quad::UiComponent::SetDrawFlag(bool flag)
{
}

bool Quad::UiComponent::GetDrawFlag() const
{
	return false;
}



void Quad::UiComponent::SetSelectFlag(bool flag)
{
}

bool Quad::UiComponent::GetSelectFlag() const
{
	return false;
}
