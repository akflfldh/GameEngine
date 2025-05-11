#include "BoundaryStick.h"
#include"ResizeHandle.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"MouseEvent.h"
#include"MouseClickSelectEvent.h"
#include"Event/EventTable.h"
#include"System.h"



Quad::BoundaryStick::BoundaryStick(const std::string& name)
	:UiEntity(name)
{
	SetIDState(false);
}

Quad::BoundaryStick::~BoundaryStick()
{
	delete mResizeHandle;
}

void Quad::BoundaryStick::Initialize()
{
	UiEntity::Initialize();


	System * system = 	GetSystem();	
	system->RegisterEntityToEvent("LButtonDown", this);
	system->RegisterEntityToEvent("LButtonUp", this);
	system->RegisterEntityToEvent("MouseMove", this);
	system->RegisterEntityToEvent("Select", this);


	//EventTable::AddObject("LButtonDown", this);
	//EventTable::AddObject("LButtonUp", this);
	//EventTable::AddObject("MouseMove", this);
	//EventTable::AddObject("Select", this);
}

void Quad::BoundaryStick::HandleSelectEvent(Event* event)
{
	MouseClickSelectEvent* selectEvent = (MouseClickSelectEvent*)event;
	
	UiEntity::OnEvent(event);


}

void Quad::BoundaryStick::HandleReleaseSelectEvent(Event* event)
{

	UiEntity::OnEvent(event);

}

void Quad::BoundaryStick::HandleMouseMoveEvent(Event* event)
{
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)event;

	if (stateComponent->GetSelectState())
	{
		float distanceX = float(mouseEvent->GetClientPositionX()) - float(mouseEvent->GetPreClientPositionX());
		float distanceY =-1.0f*(float(mouseEvent->GetClientPositionY()) - float(mouseEvent->GetPreClientPositionY()));
		
		
		Transform& dstTransform = mDstEntity->GetTransform();
		DirectX::XMFLOAT3 dstPositionWorld =dstTransform.GetPositionWorld();
		dstPositionWorld.x += distanceX;
		dstPositionWorld.y += distanceY;

		mDstEntity->GetTransform().SetPositionWorld(dstPositionWorld);






	}

}

void Quad::BoundaryStick::InitCreating(int direction)
{

	UiEntity::InitCreating();
	mDirection = direction;




	MeshComponent* meshComponent = static_cast<MeshComponent*>(GetModel()->GetComponent(EComponentType::eMeshComponent));
	meshComponent->SetTextureToAllSubMesh(TextureManager::GetTexture("BoundaryStick.bmp"));
	SetEffect("DefaultUi.effect");

	//이게 필요한건가
	//Mesh* mesh = GetModel()->GetMesh();
	//GetTransform().ChangeBoundingVolume(new BoundingOrientedBox(*mesh->GetOrientedBoundingBox()));

	Map* map = GetMap();

	//mResizeHandle = new ResizeHandle("");
	mResizeHandle = static_cast<ResizeHandle*>(map->CreateObject("ResizeHandle"));
	//	mResizeHandle->SetSystem(GetSystem());
	mResizeHandle->Initialize(mDirection);
	MeshComponent* resizeHandleMeshComponent = static_cast<MeshComponent*>(mResizeHandle->GetModel()->GetComponent(EComponentType::eMeshComponent));
	resizeHandleMeshComponent->SetTextureToAllSubMesh(TextureManager::GetTexture("ResizeHandle.bmp"));
	mResizeHandle->SetDrawFlag(false);
	mResizeHandle->SetEntireDrawFlag(false);

	DirectX::XMFLOAT3 stickPositionWorld = GetTransform().GetPositionWorld();

	//resize handle이 stick 중앙에 온다.
	stickPositionWorld.z -= 1.0f;
	mResizeHandle->GetTransform().SetPositionLocal(stickPositionWorld);
	mResizeHandle->GetTransform().SetScaleLocal({ 10.f,10.f,1.0f });

	mResizeHandle->GetTransform().SetIndependentScaleFlag(true);
	//	mResizeHandle->GetTransform().SetIndependentRotationFlag(true);
		//위치조정하고
	AddChildObject(mResizeHandle);



}



Quad::BoundaryStick* Quad::BoundaryStick::Create(Map* map, int mapLayerIndex, int direction)
{
	BoundaryStick* ob = static_cast<BoundaryStick*>(map->CreateObject("BoundaryStick", mapLayerIndex));
	ob->Initialize();
	ob->InitCreating(direction);

	return ob;;
}

void Quad::BoundaryStick::Update(float deltaTime)
{
	UiEntity::Update(deltaTime);





}

void Quad::BoundaryStick::OnEvent(Event* event)
{
	
	std::string eventName = event->GetName();

	if(eventName =="Select")
	{
		HandleSelectEvent(event);
	}
	else if (eventName == "ReleaseSelect")
	{
		HandleReleaseSelectEvent(event);
	}
	else if (eventName == "MouseMove")
	{

		HandleMouseMoveEvent(event);

	}
	else
	{
		UiEntity::OnEvent(event);
	}


	


	SetDrawFlag(true);
	SetEntireDrawFlag(true);
	mResizeHandle->SetDrawFlag(true);
	mResizeHandle->SetEntireDrawFlag(true);

	//mResizeHandle->OnEvent(event);

}

void Quad::BoundaryStick::SetDstEntity(UiEntity* entity)
{
	mDstEntity = entity;
	if (mResizeHandle != nullptr)
		mResizeHandle->SetDstEntity(entity);
}
