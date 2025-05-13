#include "Game3DSystem.h"

Quad::Game3DSystem::Game3DSystem()
	:System(Quad::ESystemType::eMainSystem,Quad::ESystemID::eGamePlayMainSystem,"",ESystemMode::eRunTimeMode)
{


}

Quad::Game3DSystem::~Game3DSystem()
{
}

void Quad::Game3DSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
	System::Initialize(clientWidth, clientHeight, map);
	OnRuntimeMode();


}

void Quad::Game3DSystem::Update(float deltaTime, bool playMode)
{
	System::Update(deltaTime, playMode);

}

void Quad::Game3DSystem::EndUpdate(float deltaTime)
{
	System::EndUpdate(deltaTime);
}

void Quad::Game3DSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	System::OnResize(clientWidth, clientHeight);

}

void Quad::Game3DSystem::AddEntity(Object* entity, int mapLayer, bool egineEditObject)
{

}
