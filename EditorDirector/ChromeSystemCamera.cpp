#include "EditorDirector/ChromeSystemCamera.h"
#include"Core/WindowResizeEvent.h"
#include"Map/Map.h"

Quad::ChromeSystemCamera::ChromeSystemCamera(const std::string& name)
	:OrthogoanlCamera()
{

}

void Quad::ChromeSystemCamera::Initialize()
{
	OrthogoanlCamera::Initialize();
	

}

Quad::ChromeSystemCamera* Quad::ChromeSystemCamera::Create(Map* map, int mapLayerIndex, UINT width, UINT height)
{
	ChromeSystemCamera* ob = static_cast<ChromeSystemCamera*>(map->CreateObject("ChromeSystemCamera", mapLayerIndex));
	ob->Initialize();
	ob->InitCreating(width, height);
	return ob;
}

void Quad::ChromeSystemCamera::Update(float deltaTime)
{
	OrthogoanlCamera::Update(deltaTime);
}

void Quad::ChromeSystemCamera::OnEvent(Event* event)
{
	const std::string& aa = GetName();
	OrthogoanlCamera::OnEvent(event);
	const std::string eventName = event->GetName();

	/*if (eventName == "WindowResizeEvent")
	{
		HandleWindowResizeEvent(event);
	}*/


}

void Quad::ChromeSystemCamera::InitCreating(UINT width, UINT height)
{

	OrthogoanlCamera::InitCreating(width, height);
	GetTransform().SetPositionLocal({ width / 2.0f,3000 - height / 2.0f, 0.0f });




}

void Quad::ChromeSystemCamera::HandleWindowResizeEvent(Event* pEvent)
{

	WindowResizeEvent* windowResizeEvent = (WindowResizeEvent*)pEvent;

	int windowWidth = windowResizeEvent->GetWindowWidth();
	int windowHeight = windowResizeEvent->GetWindowHeight();
//	SetViewWidth(windowWidth);

//	SetViewHeight(windowHeight);
	GetTransform().SetPositionWorld({ windowWidth / 2.0f, 3000 - windowHeight / 2.0f,0.0f });
	SetViewWidthAndHeight(windowWidth, windowHeight);

	UpdateViewMatrix();
	//EvaluateProjMatrix();

}
