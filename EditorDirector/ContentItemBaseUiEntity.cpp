#include "ContentItemBaseUiEntity.h"
#include"ContentItemImageUiEntity.h"
#include"TextBox.h"
#include"Map/Map.h"
#include"Core/Event.h"


Quad::ContentItemBaseUiEntity::ContentItemBaseUiEntity()
	:PanelUiEntity()
{
}

void Quad::ContentItemBaseUiEntity::ChangeContentItem(ContentItem* item)
{
	mContentItem = item;
	mContentItemImageUiEntity->SetTexture(item);
	mContentItemNameText->SetText(item->GetName());

	switch (mUiEntityType)
	{
	case EType::eFileUiVersion:
	{

	}
	break;

	case EType::eDragAndDropVersion:
	{


	}
	break;
	}


}

void Quad::ContentItemBaseUiEntity::Initialize()
{
	PanelUiEntity::Initialize();

}

Quad::ContentItemBaseUiEntity* Quad::ContentItemBaseUiEntity::Create(Map* map, int mapLayerIndex, ContentItem* item, EType type)
{

	ContentItemBaseUiEntity* ob = static_cast<ContentItemBaseUiEntity*>(map->CreateObject("ContentItemBaseUiEntity", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(item, type);

	return ob;

}



void Quad::ContentItemBaseUiEntity::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);
}

void Quad::ContentItemBaseUiEntity::OnEvent(Event* event)
{
	const std::string& eventName = event->GetName();

	StateComponent* stateComponent = GetStateComponent();
	if (eventName == "LButtonDown" && stateComponent->GetHoverState())
	{
		int a = 2;
		stateComponent->SetSelectState(true);
	}




	PanelUiEntity::OnEvent(event);

	

}

Quad::ContentItem* Quad::ContentItemBaseUiEntity::GetContentItem() const
{
	return mContentItem;
}

void Quad::ContentItemBaseUiEntity::SetOnOff(bool state)
{
	mContentItemImageUiEntity->SetDrawFlag(state);
	mContentItemNameText->SetDrawFlag(state);
	//mContentItemNameText->SetTextDrawFlag(state);

	mContentItemNameText->SetActiveFlag(state);
	//	mContentItemNameText->SetEntireSelectAvailableFlag(state);
		//mContentItemNameText->OnOffDrawFlag(state);

		//OnOffDrawFlag에서 true로설정되어도 여기서는 기본적으로 false이여야하니 다시설정해준다.
	mContentItemNameText->SetSelectAvailableFlag(false);

	SetDrawFlag(state);
	//SetSelectAvailableFlag(state);
	SetActiveFlag(state);
	if (state == true)
	{
		//transform의 더티플래그를 켜서 콜라이더같은것이 정확히 동기화되도록하자.
		GetTransform().SetDirtyFlag(true);
		mContentItemImageUiEntity->GetTransform().SetDirtyFlag(true);
		mContentItemNameText->GetTransform().SetDirtyFlag(true);
	}


}

Quad::ContentItem* Quad::ContentItemBaseUiEntity::GetAsset() const
{
	return mContentItem;
}

void Quad::ContentItemBaseUiEntity::InitCreating(ContentItem* item, EType type)
{
	PanelUiEntity::InitCreating();


	SetEffect("DefaultUi.effect");
	SetTexture("Black.png", EUiEntityMouseState::eDefault);
	SetTexture("Gray.png", EUiEntityMouseState::eHover);
	SetTexture("Blue.png", EUiEntityMouseState::eHoverSelect);
	SetTexture("Black.png", EUiEntityMouseState::eNotHoverSelect);

	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();

	mContentItemImageUiEntity = ContentItemImageUiEntity::Create(map, mapLayerIndex, item);
	mUiEntityType = type;


	if (item->GetEContentItemType() == EContentItemType::eLogicalFolder)
	{
		SetTexture("Transparent.png");
	}

	//mContentItemImageUiEntity->Initialize(item);

	mContentItemImageUiEntity->GetTransform().SetIndependentScaleFlag(true);



	mContentItemImageUiEntity->SetSelectBlockFlag(false);
	mContentItemImageUiEntity->SetSelectAvailableFlag(false);




	int nameLength = item->GetName().size();
	//int fontSize = entity->mContentItemNameText->GetFontSize();
	int	textBoxSize = 200;//	 nameLength* fontSize;
	//int	textBoxSize = 80;//	 nameLength* fontSize;

	SetName(item->GetName());
	switch (type)
	{
	case EType::eFileUiVersion:
	{
		mContentItemNameText = TextBox::Create(map, mapLayerIndex, 100);


		SetSize(100, 140);
		SetPosition(0, 0, 10);
		mContentItemImageUiEntity->SetSize(90, 90);
		mContentItemImageUiEntity->SetPosition(0, 20, 9);


		//entity->mContentItemNameText->Initialize(50, 40,true);
		//mContentItemNameText->Initialize(100);
		mContentItemNameText->SetName(GetName() + "_textBox");
		mContentItemNameText->SetSize(100, 40);
		mContentItemNameText->SetPosition(0, -50.0f, 8.0f);


		//entity->mContentItemNameText->SetCharacterInsertTextBoxMaxWIdth(140);
		mContentItemNameText->SetAlignmentDirection(ETextAlignmentDirection::eCenter);
		mContentItemNameText->SetTexture("Transparent.png");
		//
		mContentItemNameText->SetText(item->GetName());
		mContentItemNameText->SetDrawFlag(true);
		mContentItemNameText->SetTextDrawFlag(true);
		mContentItemNameText->SetPosition(0, -50.0f, 8.0f);
		mContentItemNameText->GetTransform().SetIndependentScaleFlag(true);
		mContentItemNameText->SetSelectBlockFlag(false);
		mContentItemNameText->SetSelectAvailableFlag(false);
		mContentItemNameText->SetEntireSelectAvailableFlag(false);

		//entity->mContentItemNameText->OnOffReSizeFlag(false);
		//entity->mContentItemNameText->OnOffTextModifyFlag(false);


	}
	break;
	case EType::eDragAndDropVersion:
	{
		mContentItemNameText = TextBox::Create(map, mapLayerIndex, 255);


		SetSize(100 + textBoxSize, 100);
		SetPosition(0, 0, 10);
		mContentItemImageUiEntity->SetSize(90, 90);
		mContentItemImageUiEntity->SetPosition(-150, 0, 9);
		mContentItemImageUiEntity->SetPosition(50 - (100 + textBoxSize) / 2.0f, 0, 9);


		//	mContentItemNameText->Initialize(255);
		mContentItemNameText->SetSize(textBoxSize, 40);
		mContentItemNameText->SetTexture("Transparent.png");
		mContentItemNameText->SetText(item->GetName());
		mContentItemNameText->SetPosition(100 - (100 + textBoxSize) / 2.0f + textBoxSize / 2.0f, 0.0f, 9.0f);
		mContentItemNameText->SetPosition(50, 0.0f, 9.0f);
		mContentItemNameText->GetTransform().SetIndependentScaleFlag(true);
		mContentItemNameText->SetSelectBlockFlag(false);
		mContentItemNameText->SetSelectAvailableFlag(false);
		mContentItemNameText->SetDrawFlag(true);
		mContentItemNameText->SetTextDrawFlag(true);

	}
	break;
	}


	AddChildPanelUiEntity(mContentItemNameText);
	AddChildPanelUiEntity(mContentItemImageUiEntity);
	//entity->AddChildObject(entity->mContentItemImageUiEntity);
	//entity->AddChildObject(entity->mContentItemNameText);

	mContentItem = item;

}

Quad::TextBox* Quad::ContentItemBaseUiEntity::GetContentItemNameText()const
{
	return mContentItemNameText;
}

Quad::ContentItemImageUiEntity* Quad::ContentItemBaseUiEntity::GetContentItemImageUiEntity()const
{
	return mContentItemImageUiEntity;
}

