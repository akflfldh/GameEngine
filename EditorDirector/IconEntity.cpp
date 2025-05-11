#include "IconEntity.h"
#include"System.h"
Quad::IconEntity::IconEntity(const std::string& name)
	:UiEntity(name)
{
}

Quad::IconEntity::~IconEntity()
{
}

void Quad::IconEntity::Initialize()
{
	UiEntity::Initialize();
	

}

Quad::IconEntity* Quad::IconEntity::Create(Map* map, int mapLayerIndex)
{
	IconEntity* ob = static_cast<IconEntity*>(map->CreateObject("IconEntity", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating();

	return ob;
}

void Quad::IconEntity::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);

}

void Quad::IconEntity::OnEvent(Event* event)
{
	UiEntity::OnEvent(event);
}

void Quad::IconEntity::InitCreating()
{
	UiEntity::InitCreating();
	SetEffect("DefaultUi.effect");
}
