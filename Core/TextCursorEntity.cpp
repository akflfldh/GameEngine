#include "Core/TextCursorEntity.h"

#include"Core/TextCharacterPanelEntity.h"
#include"Core/TextBox.h"
#include"Map/Map.h"
Quad::TextCursorEntity::TextCursorEntity()
	:UiEntity("")
{
}

Quad::TextCursorEntity::~TextCursorEntity()
{
}

void Quad::TextCursorEntity::Initialize()
{
	UiEntity::Initialize();

}

Quad::TextCursorEntity* Quad::TextCursorEntity::Create(Map* map, int mapLayerIndex)
{
	TextCursorEntity* ob = static_cast<TextCursorEntity*>(map->CreateObject("TextCursorEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating();
	
	return ob;
}

void Quad::TextCursorEntity::Update(float deltaTime)
{

}

void Quad::TextCursorEntity::OnEvent(Event* event)
{
	

}

int Quad::TextCursorEntity::GetPositionInText() const
{
	return mPositionInText;
}

void Quad::TextCursorEntity::SetPositionInText(int position)
{

	mPositionInText = position;

}

void Quad::TextCursorEntity::InitCreating()
{

	UiEntity::InitCreating();
	SetTexture("Green.png");
	SetEffect("DefaultUi.effect");
}
