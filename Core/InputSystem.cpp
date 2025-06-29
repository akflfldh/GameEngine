#include"Core/InputSystem.h"

Quad::InputSystem::~InputSystem()
{
}

void Quad::InputSystem::OnMouseEvent(EMouseInput mouseInput, int channel)
{
	InputSystem* inputSystem = GetInstance();

	for (auto& callbackVector : inputSystem->mMouseInputTable[channel][mouseInput])
	{
		for (auto& callback : callbackVector.second)
		{
			callback();
		}
	}

}

void Quad::InputSystem::RegisterMouseAction(EMouseInput mouseInput, const std::string& actionTag, std::function<void()> callback,int channel)
{

	InputSystem * inputSystem = GetInstance();
	inputSystem->mMouseInputTable[channel][mouseInput][actionTag].push_back(std::move(callback));

}


Quad::InputSystem* Quad::InputSystem::GetInstance()
{
	static InputSystem inputSystem;

	return &inputSystem;

}