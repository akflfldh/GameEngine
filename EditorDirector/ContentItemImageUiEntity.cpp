#include "ContentItemImageUiEntity.h"
#include"Map/Map.h"
Quad::ContentItemImageUiEntity::ContentItemImageUiEntity(const std::string& name)
	:UiEntity(name)
{

}

void Quad::ContentItemImageUiEntity::Initialize()
{
	UiEntity::Initialize();

}


Quad::ContentItemImageUiEntity* Quad::ContentItemImageUiEntity::Create(Map* map, int mapLayerIndex, ContentItem* contentItem)
{
	ContentItemImageUiEntity* ob = static_cast<ContentItemImageUiEntity*>(map->CreateObject("ContentItemImageUiEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating(contentItem);

	return ob;
}

void Quad::ContentItemImageUiEntity::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);
}

void Quad::ContentItemImageUiEntity::OnEvent(Event* event)
{
	UiEntity::OnEvent(event);
}

void Quad::ContentItemImageUiEntity::SetTexture(ContentItem* contentItem)
{
	//일단 에셋의 타입을보고 텍스처를 설정하자.
	switch (contentItem->GetEContentItemType())
	{
	case EContentItemType::eAsset:


	{
		Asset* asset = (Asset*)contentItem;
		switch (asset->GetAssetType())
		{
		case EAssetType::eMesh:
			UiEntity::SetTexture("MeshAsset.png");
			break;
		case EAssetType::eMaterial:
			UiEntity::SetTexture("MaterialAsset.png");
			break;
		case EAssetType::eTexture:
			UiEntity::SetTexture((Texture*)asset);
			break;
		case EAssetType::eSkeleton:
			UiEntity::SetTexture("SkeletonAsset.png");
			break;
		case EAssetType::eAnimationClip:
			UiEntity::SetTexture("AnimAsset.png");
			break;
		}
	}
	break;
	case EContentItemType::eLogicalFolder:
	{
		UiEntity::SetTexture("Folder.png");
	}
	break;
	case EContentItemType::eProject:

	{
		UiEntity::SetTexture("None.png");
	}
	break;
	}

}

void Quad::ContentItemImageUiEntity::InitCreating(ContentItem* contentItem)
{
	UiEntity::InitCreating();

	SetEffect("DefaultUi.effect");
	SetTexture(contentItem);
}

