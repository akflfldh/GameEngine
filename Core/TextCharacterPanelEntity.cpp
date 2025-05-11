#include "Core/TextCharacterPanelEntity.h"
#include"Map/Map.h"



Quad::TextCharacterPanelEntity::TextCharacterPanelEntity()
	:PanelUiEntity(""), mLineNum(0)
{




}

Quad::TextCharacterPanelEntity::~TextCharacterPanelEntity()
{



}



void Quad::TextCharacterPanelEntity::BeforeRemoveBehavior()
{
	PanelUiEntity::BeforeRemoveBehavior();

	Map* map = GetMap();
	map->RequestRemoveObject(mCharacterEntity);

}

void Quad::TextCharacterPanelEntity::InitCreating(float width, float height)
{
	PanelUiEntity::InitCreating();

	SetSize(width, height);
	SetEffect("DefaultUi.effect");
	SetTexture("Red.png");
	//SetEffect(L"TextCharacterPanelEntity.effect")

	//GetTransform().GetPositionWorld();

	Map* map = GetMap();
	mCharacterEntity = TextCharacterEntity::Create(map, GetMapLayerID());
		//static_cast<TextCharacterEntity*>(map->CreateObject("TextCharacterEntity"));

	//mCharacterEntity->SetSystem(GetSystem());
//	mCharacterEntity->Initialize();
	mCharacterEntity->GetTransform().SetIndependentScaleFlag(true);
	mCharacterEntity->SetSelectAvailableFlag(false);
	SetDrawFlag(false);
	AddChildPanelUiEntity(mCharacterEntity);


	mCharacterEntity->SetPosition(0, 0, -1.0f);


	//SetDrawFlag(false);
}



void Quad::TextCharacterPanelEntity::Initialize()
{
	PanelUiEntity::Initialize();

}

Quad::TextCharacterPanelEntity* Quad::TextCharacterPanelEntity::Create(Map* map, int mapLayerIndex, float width, float height)
{
	TextCharacterPanelEntity* ob = static_cast<TextCharacterPanelEntity*>(map->CreateObject("TextCharacterPanelEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating(width,height);
	return ob;
}

void Quad::TextCharacterPanelEntity::Update(float deltaTime)
{



}

void Quad::TextCharacterPanelEntity::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);

}

void Quad::TextCharacterPanelEntity::SetDrawFlag(bool flag)
{
	Object::SetDrawFlag(flag);
	//mCharacterEntity.SetDrawFlag(flag);

}

void Quad::TextCharacterPanelEntity::SetPosition(float x, float y, float z)
{
	UiEntity::SetPosition(x, y, z);
	mCharacterEntity->GetTransform().UpdateWorldMatrix(GetTransform().GetWorldMatrix());

}

void Quad::TextCharacterPanelEntity::SetGlyphTexture(Texture* texture)
{

	mCharacterEntity->SetTexture(texture);
}

void Quad::TextCharacterPanelEntity::SetAdvanceX(float advance)
{
	mAdvanceX = advance;
}

float Quad::TextCharacterPanelEntity::GetAdvanceX() const
{
	return mAdvanceX;
}

void Quad::TextCharacterPanelEntity::SetGlyphMetrics(const GlyphMetrics& metrics ,float advance, float baselineLocalPosY)
{

	mAdvanceX = metrics.mHoriAdvance;
	float panelHeight  = GetHeight();
	SetSize(mAdvanceX, panelHeight);

	mCharacterEntity->SetSize(metrics.mWidth, metrics.mHeight);


	float characterEntityLocalPosX = -1.0f * mAdvanceX / 2  +metrics.mHoriBearingX + metrics.mWidth / 2;
	float characterEntityLocalPosY =(baselineLocalPosY+ metrics.mHoriBearingY  + baselineLocalPosY  + (metrics.mHoriBearingY - metrics.mHeight))/2;

	mCharacterEntity->SetPosition(characterEntityLocalPosX, characterEntityLocalPosY,-1.0f);
	mCharacterEntity->GetTransform().UpdateWorldMatrix(GetTransform().GetWorldMatrix());

}

void Quad::TextCharacterPanelEntity::SetCharacter(wchar_t ch)
{
	mCharacter = ch;
}

wchar_t Quad::TextCharacterPanelEntity::GetCharacter() const
{
	return mCharacter;
}

Quad::TextCharacterEntity* Quad::TextCharacterPanelEntity::GetCharacterEntity()
{
	return mCharacterEntity;
}



void Quad::TextCharacterPanelEntity::SetLineNum(int num)
{
	mLineNum = num;
}

int Quad::TextCharacterPanelEntity::GetLineNum() const
{
	return mLineNum;
}

void Quad::TextCharacterPanelEntity::SetStencilRefValue(unsigned char value)
{
	Object::SetStencilRefValue(value);
	mCharacterEntity->SetStencilRefValue(value);

}

