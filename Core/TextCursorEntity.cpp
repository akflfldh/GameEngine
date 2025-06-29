//#include "Core/TextCursorEntity.h"
//
//#include"Core/TextCharacterPanelEntity.h"
//#include"Core/TextBox.h"
//#include"Map/Map.h"
//
//#include"ScissorRectComponent.h"
//#include<Component/UIMeshComponent.h>
//
//Quad::TextCursorEntity::TextCursorEntity()
//	:UiEntity("")
//{
//}
//
//Quad::TextCursorEntity::~TextCursorEntity()
//{
//}
//
//void Quad::TextCursorEntity::Initialize()
//{
//	UiEntity::Initialize();
//
//}
//
//Quad::TextCursorEntity* Quad::TextCursorEntity::Create(Map* map, int mapLayerIndex)
//{
//	TextCursorEntity* ob = static_cast<TextCursorEntity*>(map->CreateObject("TextCursorEntity", mapLayerIndex));
////	ob->Initialize();
//	ob->InitCreating();
//	
//	return ob;
//}
//
//void Quad::TextCursorEntity::Update(float deltaTime)
//{
//
//}
//
//
//int Quad::TextCursorEntity::GetPositionInText() const
//{
//	return mPositionInText;
//}
//
//void Quad::TextCursorEntity::SetPositionInText(int position)
//{
//
//	mPositionInText = position;
//
//}
//
//void Quad::TextCursorEntity::InitCreating()
//{
//
//	UiEntity::InitCreating();
//
//
//	AddComponent<ScissorRectComponent>("ScissorRectComponent");
//
//	GetUiMeshComponent()->SetTexture("Green.png");
//	GetUiMeshComponent()->SetEffect("DefaultUi.effect");
//}
