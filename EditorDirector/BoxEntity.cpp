#include "BoxEntity.h"


Quad::BoxEntity::BoxEntity(const std::string& name)
	:UiEntity(name)
{

}

void Quad::BoxEntity::Initialize()
{
	UiEntity::Initialize();
}

void Quad::BoxEntity::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);
}

void Quad::BoxEntity::OnEvent(Event* event)
{

}
