#include "InputSystem.h"
#include"Core/KeyBoard.h"
#include<functional>

#include"Core/InputSystemImpl.h"
namespace Quad
{
	InputSystem::InputSystem()
		:mImpl(InputSystemImpl::GetInstance())
	{


	}

	InputSystem::~InputSystem()
	{

	}

	void InputSystem::Update(float deltaTime)
	{
		mImpl->Update(deltaTime);

	}

	void InputSystem::RegisterKeyBehavior(int key, EInputState inputState, const std::string& name)
	{
		mImpl->RegisterActionKeyBehavior(key, inputState, name);
	}

	void InputSystem::RegisterAxisBehaviorKeyValue(int key, float value, const std::string& behaviorName)
	{
		mImpl->RegisterAxisKeyBehavior(key, value, behaviorName);

	}

	void InputSystem::OnEvent(int key, EInputState inputState)
	{

		mImpl->OnEvent(key, inputState);

	}

	void InputSystem::RegisterEnvironment(const std::string& name)
	{
		mImpl->RegisterEnvironment(name);
	}


	void InputSystem::ChangeEnvironment(const std::string& name)
	{

		mImpl->ChangeEnvironment(name);
	}



}