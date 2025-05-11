#include "EditorDirector/DragAndDropUiEntity.h"
#include"Map/Map.h"
Quad::DragAndDropUiEntity::DragAndDropUiEntity()
	:UiEntity("")
{
}

void Quad::DragAndDropUiEntity::Initialize()
{
	UiEntity::Initialize();
	

}

Quad::DragAndDropUiEntity* Quad::DragAndDropUiEntity::Create(Map* map, int mapLayerIndex)
{
	DragAndDropUiEntity* ob = static_cast<DragAndDropUiEntity*>(map->CreateObject("DragAndDropUiEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating();

	return ob;
}

void Quad::DragAndDropUiEntity::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);
}

void Quad::DragAndDropUiEntity::OnEvent(Event* event)
{

}

void Quad::DragAndDropUiEntity::InitCreating()
{

	UiEntity::InitCreating();


	SetTexture("Blue.png");
	SetEffect("DefaultUi.effect");
	SetSize(200, 200);
	SetPosition(0, 0, 10.0f);
}

