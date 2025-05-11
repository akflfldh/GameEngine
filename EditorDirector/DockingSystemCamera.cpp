#include "EditorDirector/DockingSystemCamera.h"
#include"Map/Map.h"
#include"Core/WindowResizeEvent.h"
Quad::DockingSystemCamera::DockingSystemCamera(const std::string& name)
	:OrthogoanlCamera(name)
{
}

void Quad::DockingSystemCamera::Initialize()
{
	OrthogoanlCamera::Initialize();

}

Quad::DockingSystemCamera* Quad::DockingSystemCamera::Create(Map* map, int mapLayerIndex, UINT width, UINT height)
{

	DockingSystemCamera* ob = static_cast<DockingSystemCamera*>(map->CreateObject("DockingSystemCamera", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating(width,height);

	return ob;
}

void Quad::DockingSystemCamera::Update(float deltaTime)
{
	OrthogoanlCamera::Update(deltaTime);
}

void Quad::DockingSystemCamera::OnEvent(Event* event)
{
	OrthogoanlCamera::OnEvent(event);
	const std::string eventName = event->GetName();

	if (eventName == "WindowResizeEvent")
	{
		//HandleWindowResizeEvent(event);
	}


}

void Quad::DockingSystemCamera::InitCreating(UINT width, UINT height)
{

	OrthogoanlCamera::InitCreating(width, height);

}

void Quad::DockingSystemCamera::HandleWindowResizeEvent(Event* pEvent)
{

	WindowResizeEvent* resizeEvent = static_cast<WindowResizeEvent*>(pEvent);

	float width = (float)resizeEvent->GetWindowWidth();
	float height = (float)resizeEvent->GetWindowHeight();


	SetViewWidthAndHeight(width, height);



}
