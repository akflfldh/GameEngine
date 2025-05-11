#include "System/UiSystem.h"

Quad::UiSystem::UiSystem(ESystemID systemID, const std::string& name)
	:System(ESystemType::eMainSystem,systemID,name)
{
}

Quad::UiSystem::~UiSystem()
{
}



void Quad::UiSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
	System::Initialize(clientWidth, clientHeight, map);
}

void Quad::UiSystem::Update(float deltaTime, bool playMode)
{
	System::Update(deltaTime,playMode);
}

void Quad::UiSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth, clientHeight);
}
