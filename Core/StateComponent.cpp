#include "Object/StateComponent.h"

Quad::StateComponent::StateComponent()
	:mDestObject(nullptr),mLButtonDownState(false),mHoverState(false),mSelectState(false), mExclusiveHoverEventFlag(false)
{


}

void Quad::StateComponent::Initialize(Object* destObject)
{
	mDestObject = destObject;
}

void Quad::StateComponent::Update(float delta)
{
}

void Quad::StateComponent::SetLButtonDownState(bool state)
{
	mLButtonDownState = state;
}

void Quad::StateComponent::SetRButtonDownState(bool state)
{
	mRButtonDownState = state;
}

void Quad::StateComponent::SetHoverState(bool state)
{
	mHoverState = state;
}

void Quad::StateComponent::SetSelectState(bool state , bool exclusiveEventHandleFlag)
{

	mSelectState = state;
}

bool Quad::StateComponent::GetLButtonDownState() const
{
	return mLButtonDownState;
}

bool Quad::StateComponent::GetRButtonDownState() const
{
	return mRButtonDownState;
}

bool Quad::StateComponent::GetHoverState() const
{
	return mHoverState;
}

bool Quad::StateComponent::GetSelectState() const
{
	return mSelectState;
}

void Quad::StateComponent::SetExclusiveHoverEventFlag(bool flag)
{
	mExclusiveHoverEventFlag = flag;
}

bool Quad::StateComponent::GetExclusiveHoverEventFlag() const
{
	return mExclusiveHoverEventFlag;
}
