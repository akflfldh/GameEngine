#include "EditorDirector/DockingSelectBox.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"System/ChildWindowDockingSystem.h"
#include"Core/System.h"
#include"Core/MouseEvent.h"
#include"Event/EventTable.h"

Quad::DockingSelectBox::DockingSelectBox(const std::string& name)
	:UiEntity(name),mDirection(EDockingDirection::eLeft)
{


}

void Quad::DockingSelectBox::Initialize()
{

	UiEntity::Initialize();
	/*GetModel()->GetMeshComponent()->SetMesh(MeshManager::GetMesh("Rectangle"));
	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(TextureManager::GetTexture("DockingSelectBox.png"));
	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName("Docking.effect");*/
	System* system = GetSystem();
	system->RegisterEntityToEvent("LButtonUp", this);
	
	

}

Quad::DockingSelectBox* Quad::DockingSelectBox::Create(Map* map, int mapLayerIndex, EDockingDirection direction)
{

	DockingSelectBox* ob  = static_cast<DockingSelectBox*>(map->CreateObject("DockingSelectBox", mapLayerIndex));
	//ob->Initialize();
	ob->InitCreating(direction);
	

	return ob;
}

void Quad::DockingSelectBox::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);
}

void Quad::DockingSelectBox::OnEvent(Event* event)
{
	UiEntity::OnEvent(event);
	const std::string& eventName = event->GetName();

	//hover이벤트를 처리해야한다.
	if (eventName == "Hover")
	{


		HandleHover(event);
	}else if(eventName=="LButtonUp")
	{

		HandleLButtonUp(event);
	}



}

Quad::EDockingDirection Quad::DockingSelectBox::GetDirection() const
{
	return mDirection;
}

void Quad::DockingSelectBox::SetSize(float width, float height)
{
	mWidth = width;
	mHeight = height;

	GetTransform().SetScaleLocal({ width,height,1.0f });

}

float Quad::DockingSelectBox::GetWidth() const
{
	return mWidth;
}

float Quad::DockingSelectBox::GetHeight() const
{
	return mHeight;
}

void Quad::DockingSelectBox::SetRotataion(float degree)
{
	GetTransform().SetRotationLocal({ 0,0,degree });
}

void Quad::DockingSelectBox::SetPosition(float x, float y, float z)
{
	GetTransform().SetPositionLocal({ x,y,z });
}

bool Quad::DockingSelectBox::GetHoverState() const
{
	return mHoverState;
}

void Quad::DockingSelectBox::SetHoverState(bool state)
{
	mHoverState = state;
}

void Quad::DockingSelectBox::SetDockingDirection(EDockingDirection dir)
{
	mDirection = dir;

}

void Quad::DockingSelectBox::InitCreating(EDockingDirection direction)
{

	UiEntity::InitCreating();

	mDirection = direction;

	MeshComponent* meshComponent = GetModel()->GetMeshComponent();
//	meshComponent->SetMesh(MeshManager::GetMesh("Rectangle"));
	meshComponent->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(TextureManager::GetTexture("DockingSelectBox.png"));
	meshComponent->GetSubMesh()[0].mModelMaterial.SetEffectName("Docking.effect");
}

void Quad::DockingSelectBox::HandleHover(Event* event)
{
	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = (MouseEvent*)event;
	ChildWindowDockingSystem* windowDockingSystem = static_cast<ChildWindowDockingSystem*>(GetSystem());
	if (mouseEvent->GetHoverState())
	{
		stateComponent->SetHoverState(true);
		windowDockingSystem->SelectBoxHover(mDirection);
	}
	else
	{
		stateComponent->SetHoverState(false);
		windowDockingSystem->SelectBoxReleaseHover();
	}
}

void Quad::DockingSelectBox::HandleLButtonUp(Event* event)
{

	//lbuttonUp할떄도 좌표가중요한데 ,일반적으로 마우스 move일떄 벗어나서 이미 
	//hover상태가아닐게예상되는데 ,만약 그냥 윈도우밖으로 빠르게 나가버려서 hover release가 제대로처리되지못헀다면
	//어떻할것이냐,  hover release 하는 그마지막에도 마우스 move 이벤트를 만들어서  select box에게 전송해야할수도있다
	//일단은 선택박스가 화면 테두리에 붙어있는것은아니니.
	//임시방편으로 그냥처리하고 화면이 작아지면 그럴문제가생긴다.
	StateComponent* stateComponent = GetStateComponent();
	bool dockingState = false;
	if(stateComponent->GetHoverState())
	{	//도킹이 이루어져야한다. 
		dockingState = true;
	}
	ChildWindowDockingSystem* windowDockingSystem = static_cast<ChildWindowDockingSystem*>(GetSystem());
	windowDockingSystem->SelectBoxLButtonUp(mDirection, dockingState);
	
	stateComponent->SetHoverState(false);



}

