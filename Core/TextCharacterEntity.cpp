#include "Core/TextCharacterEntity.h"
#include"Map/Map.h"
Quad::TextCharacterEntity::TextCharacterEntity()
	:UiEntity("")
{
}

Quad::TextCharacterEntity::~TextCharacterEntity()
{
}

void Quad::TextCharacterEntity::Initialize()
{
	UiEntity::Initialize();

	

}

Quad::TextCharacterEntity* Quad::TextCharacterEntity::Create(Map* map, int mapLayerIndex)
{
	TextCharacterEntity* ob = static_cast<TextCharacterEntity*>(map->CreateObject("TextCharacterEntity", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating();
	return ob;

	return ob;
}

void Quad::TextCharacterEntity::Update(float deltaTime)
{
}

void Quad::TextCharacterEntity::OnEvent(Event* event)
{
	UiEntity::OnEvent(event);



}

void Quad::TextCharacterEntity::SetCh(wchar_t ch)
{
	mCh = ch;
}

void Quad::TextCharacterEntity::InitCreating()
{
	UiEntity::InitCreating();
	SetEffect("TextCharacter.effect");
}
