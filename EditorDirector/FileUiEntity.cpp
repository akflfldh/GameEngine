#include "EditorDirector/FileUiEntity.h"
#include"Map/Map.h"
namespace Quad
{

	

	FileUiEntity::FileUiEntity(const std::string& name)
		:UiEntity(name)
	{

	}

	void FileUiEntity::Initialize()
	{
		UiEntity::Initialize();
	}

	FileUiEntity* FileUiEntity::Create(Map* map, int mapLayerIndex)
	{
		FileUiEntity* ob = static_cast<FileUiEntity*>(map->CreateObject("FileUiEntity", mapLayerIndex));
		//ob->Initialize();
		ob->InitCreating();
		return ob;
	}

	void FileUiEntity::Update(float deltaTime)
	{
		UiEntity::Update(deltaTime);
		//mTransform.Update();
	}

	void FileUiEntity::InitCreating()
	{
		UiEntity::InitCreating();
	}


}