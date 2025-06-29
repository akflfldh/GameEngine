#include "Core/TextCharacterPanelEntity.h"
#include"Map/Map.h"

#include"ScissorRectComponent.h"

#include<Component/UIMeshComponent.h>
#include<Component/UiComponent.h>
#include<Component/UIMeshComponent.h>


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
	UIMeshComponent* uiMeshComponent = GetUiMeshComponent();

	uiMeshComponent->SetWidthHeightLocal(width, height);
	uiMeshComponent->SetTexture("Red.png");
	AddComponent<ScissorRectComponent>("ScissorRectComponent");



	Map* map = GetMap();
	mCharacterEntity = TextCharacterEntity::Create(map, GetMapLayerID());
		//static_cast<TextCharacterEntity*>(map->CreateObject("TextCharacterEntity"));

	//mCharacterEntity->SetSystem(GetSystem());
//	mCharacterEntity->Initialize();

	mCharacterEntity->GetRootSceneComponent()->SetIndependentScaleFlag(true);
	mCharacterEntity->GetUiComponent()->SetSelectAvailableFlag(false);


	SetDrawFlag(false);
	AddChildPanelUiEntity(mCharacterEntity);


	mCharacterEntity->SetObjectPositionLocal(0, 0, -1.0f);




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


void Quad::TextCharacterPanelEntity::SetDrawFlag(bool flag)
{
	Object::SetDrawFlag(flag);
	mCharacterEntity->SetDrawFlag(flag);

}


void Quad::TextCharacterPanelEntity::SetGlyphTexture(Texture* texture)
{
	mCharacterEntity->GetUiMeshComponent()->SetTexture(texture);
	//mCharacterEntity->SetTexture(texture);
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

	UIMeshComponent * meshComponent =	GetUiMeshComponent();

	mAdvanceX = metrics.mHoriAdvance;
	float panelHeight  = meshComponent->GetHeightLocal();
	meshComponent->SetWidthLocal(mAdvanceX);


	//SetSize(mAdvanceX, panelHeight);

	UIMeshComponent * characterEntityMeshComponent = 	mCharacterEntity->GetUiMeshComponent();
	characterEntityMeshComponent->SetWidthHeightLocal(metrics.mWidth, metrics.mHeight);


	//mCharacterEntity->SetSize(metrics.mWidth, metrics.mHeight);


	float characterEntityLocalPosX = -1.0f * mAdvanceX / 2  +metrics.mHoriBearingX + metrics.mWidth / 2;
	float characterEntityLocalPosY =(baselineLocalPosY+ metrics.mHoriBearingY  + baselineLocalPosY  + (metrics.mHoriBearingY - metrics.mHeight))/2;


	characterEntityMeshComponent->GetTransform().SetPositionLocal(characterEntityLocalPosX, characterEntityLocalPosY, -1.0f);


	//mCharacterEntity->SetPosition(characterEntityLocalPosX, characterEntityLocalPosY,-1.0f);
	//mCharacterEntity->GetTransform().UpdateWorldMatrix(GetTransform().GetWorldMatrix());

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

