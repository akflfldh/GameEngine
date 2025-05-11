#include "PopupSystemCamera.h"
#include"WindowResizeEvent.h"
#include"Map/Map.h"

Quad::PopupSystemCamera::PopupSystemCamera(const std::string& name)
	:OrthogoanlCamera(name)
{
}

void Quad::PopupSystemCamera::Initialize()
{
	OrthogoanlCamera::Initialize();



}

Quad::PopupSystemCamera* Quad::PopupSystemCamera::Create(Map* map, int mapLayerIndex, UINT width, UINT height)
{

	PopupSystemCamera* ob = static_cast<PopupSystemCamera*>(map->CreateObject("PopupSystemCamera", mapLayerIndex));
	ob->Initialize();
	ob->InitCreating(width, height);



	return ob;
}

void Quad::PopupSystemCamera::Update(float deltaTime)
{
	OrthogoanlCamera::Update(deltaTime);
}

void Quad::PopupSystemCamera::OnEvent(Event* event)
{
	OrthogoanlCamera::OnEvent(event);

	const std::string& eventName = event->GetName();

	if (eventName == "WindowResizeEvent")
	{
		HandleWindowResizeEvent(event);
	}
}

void Quad::PopupSystemCamera::InitCreating(UINT width, UINT height)
{
	OrthogoanlCamera::InitCreating(width, height);
}

void Quad::PopupSystemCamera::HandleWindowResizeEvent(Event* pEvent)
{

	WindowResizeEvent* resizeEvent = (WindowResizeEvent*)pEvent;
	/*int width= resizeEvent->GetWindowWidth();
	int height = resizeEvent->GetWindowHeight();

	float z = GetTransform().GetPositionWorld().z;*/
	//GetTransform().SetPositionWorld({ (float)width / 2,-1.0f * height / 2,z });

}


