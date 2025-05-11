#include "EditorDirector/FileUiSystemCamera.h"
#include"Map/Map.h"
#include"Core/Event.h"

Quad::FileUiSystemCamera::FileUiSystemCamera(const std::string& name)
	:OrthogoanlCamera(name)
{
}

void Quad::FileUiSystemCamera::Initialize()
{
	OrthogoanlCamera::Initialize();

}

Quad::FileUiSystemCamera* Quad::FileUiSystemCamera::Create(Map *map , int mapLayerIndex ,UINT width, UINT height)
{
	FileUiSystemCamera* ob = static_cast<FileUiSystemCamera*>(map->CreateObject("FileUiSystemCamera", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating(width, height);
	return ob;
}

void Quad::FileUiSystemCamera::Update(float deltaTime)
{
	OrthogoanlCamera::Update(deltaTime);
}

void Quad::FileUiSystemCamera::OnEvent(Event* event)
{
	OrthogoanlCamera::OnEvent(event);
	const std::string& eventName = event->GetName();

	/*if (eventName == "WindowResizeEvent")
	{

	}*/

}

void Quad::FileUiSystemCamera::InitCreating(UINT width, UINT height)
{
	OrthogoanlCamera::InitCreating(width, height);

}
