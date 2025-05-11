#include "DirectoryUiEntity.h"

#include"Map/Map.h"
namespace Quad
{
	DirectoryUiEntity::DirectoryUiEntity(const std::string & name)
		:UiEntity(name)
	{

	}
	void DirectoryUiEntity::Initialize()
	{
		UiEntity::Initialize();
	}

	DirectoryUiEntity* DirectoryUiEntity::Create(Map* map, int mapLayerIndex)
	{
		DirectoryUiEntity* ob = static_cast<DirectoryUiEntity*>(map->CreateObject("DirectoryUiEntity", mapLayerIndex));
		//ob->Initialize();
		ob->InitCreating();

		return ob;
	}

	void DirectoryUiEntity::Update(float deltaTime)
	{

		//UiEntity::Update();	
		mTransform.Update();
	}

	void DirectoryUiEntity::InitCreating()
	{
		UiEntity::InitCreating();
	}


}