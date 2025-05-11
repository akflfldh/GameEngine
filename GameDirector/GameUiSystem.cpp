#include "GameUiSystem.h"

Quad::GameUiSystem::GameUiSystem()
	:System(Quad::ESystemType::eUiSystem,Quad::ESystemID::eGamePlayUiSystem,"")
{
}

Quad::GameUiSystem::~GameUiSystem()
{
}

void Quad::GameUiSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
	System::Initialize(clientWidth, clientHeight, map);

	OnRuntimeMode();

}

void Quad::GameUiSystem::Update(float deltaTime, bool playMode)
{
	System::Update(deltaTime);
}

void Quad::GameUiSystem::EndUpdate()
{
	System::EndUpdate();
}

void Quad::GameUiSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth, clientHeight);
}

void Quad::GameUiSystem::AddEntity(Object* entity, int mapLayer, bool egineEditObject)
{

}
