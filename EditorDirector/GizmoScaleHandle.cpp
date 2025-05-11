#include "GizmoScaleHandle.h"
#include"Object/Line/Line.h"
#include"ObjectFactory/LineFactory.h"
#include"ResourceManager/MeshManager.h"
#include"Collision/ColliderGenerator.h"
#include"Event/EventTable.h"
#include"System.h"
#include"MouseEvent.h"
#include"Object/Camera/Camera.h"
#include"MathHelper.h"
#include"Mouse.h"
#include"Component/ColliderComponent.h"


#include"Gizmo.h"



Quad::GizmoScaleHandle::GizmoScaleHandle(const std::string& name)
	:Entity(name),mBox(nullptr)
{
	SetEngineObjectFlag(true);
	


}

void Quad::GizmoScaleHandle::Initialize()
{
	Entity::Initialize();
	GetModel()->RemoveComponent(EComponentType::eMeshComponent);



	EventTable * eventTable = GetSystem()->GetEventTable();
	eventTable->AddObject("MouseMove", this);
	eventTable->AddObject("Hover", this);
	eventTable->AddObject("LButtonDown", this);
	eventTable->AddObject("LButtonUp", this);


	GetTransform().SetPositionWorld({ 0,0,0 });


	////mLine = LineFactory::CreateLine({ 0,0,0 }, { 0,5,0 });
	//mLine->SetStartVertexPosition({ 0,0,0 });
	//mLine->SetEndVertexPosition({ 0,5,0 });
	////mLine->SetSystem(GetSystem());
	//mLine->SetEffectName("GizmoLine.effect");
	//mLine->SetEngineObjectFlag(true);
	
	//mBox->SetSystem(GetSystem());
	
	//mBox->GetTransform().SetPositionWorld({ 0,5,0 });
	//mBox->SetEngineObjectFlag(true);

	//std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> minMaxPoint = mBox->GetModel()->GetMeshComponent()->GetMesh()->GetMinMaxPoint();
	//minMaxPoint.second.y += 5;

	//GetModel()->GetColliderComponent()->SetCollider(ColliderGenerator::CreateBoxCollider(minMaxPoint.first, minMaxPoint.second));
	//AddColliderComponent(ColliderGenerator::CreateBoxCollider(minMaxPoint.first, minMaxPoint.second));





	mHoverColor = { 1.0f,1.0f,0.0f };
	
	/*switch(mAxisType)
	{ 
	case EAxisType::eX:
	{
		mLine->SetName(GetName() + "_LineX");
		mBox->SetName(GetName() + "_BoxX");
		mDefaultColor = { 1.0f,0.0,0.0f };
	}
	break;
	case EAxisType::eY:
	{
		mLine->SetName(GetName() + "_LineY");
		mBox->SetName(GetName() + "_BoxY");
		mDefaultColor = { 0.0f,1.0,0.0f };
	}
	break;
	case EAxisType::eZ:
	{
		mLine->SetName(GetName() +"_LineZ");
		mBox->SetName(GetName() + "_BoxZ");
		mDefaultColor = { 0.0f,0.0,1.0f };

	}
	break;

	}

	SetColor(mDefaultColor);

	AddChildObject(mLine);
	AddChildObject(mBox);*/


}

Quad::GizmoScaleHandle* Quad::GizmoScaleHandle::Create(Map* map, int mapLayerIndex, EAxisType axisType, Gizmo* gizmo)
{
//	GizmoScaleHandle* ob = static_cast<GizmoScaleHandle*>(map->CreateObject("GizmoScaleHandle", mapLayerIndex));
	GizmoScaleHandle* ob = new GizmoScaleHandle;
	ob->SetSystem(map->GetSystem());
	ob->SetMapLayer(mapLayerIndex);
	//ob->SetMap(map);
	ob->Initialize();
	ob->InitCreating(axisType,gizmo,map);

	return ob;
}

void Quad::GizmoScaleHandle::Update(float deltaTime)
{
}

void Quad::GizmoScaleHandle::OnEvent(Event* event)
{

	const std::string& eventName = event->GetName();

	if (eventName == "Hover")
	{
		HandleHover(event);
	}
	else if (eventName == "LButtonDown")
	{
		HandleLButtonDown(event);
	}
	else if (eventName == "LButtonUp")
	{
		HandleLButtonUp(event);
	}
	else if (eventName == "MouseMove")
	{
		HandleMouseMove(event);
	}




}

void Quad::GizmoScaleHandle::UpdateCollider(float deltaTime)
{






}

void Quad::GizmoScaleHandle::SetColor(const DirectX::XMFLOAT3& color)
{

	mLine->GetModelSubMesh()->mModelMaterial.SetColor(color);
	mBox->GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetColor(color);
	
}

Quad::EAxisType Quad::GizmoScaleHandle::GetAxisType() const
{
	return mAxisType;
}

void Quad::GizmoScaleHandle::HandleHover(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;

	if (mouseEvent->GetHoverState())
	{
		if (!Mouse::GetRButtonState())
		{
			stateComponent->SetHoverState(true);
			SetColor(mHoverColor);
		}
	}
	else
	{
		stateComponent->SetHoverState(false);
	
		if(!stateComponent->GetLButtonDownState())
			SetColor(mDefaultColor);

	}
	


}


void Quad::GizmoScaleHandle::HandleLButtonDown(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
		Mouse::CaptureObject(this);
	}


}

void Quad::GizmoScaleHandle::HandleLButtonUp(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);
		Mouse::ReleaseCaptureObject(this);

	}
}

void Quad::GizmoScaleHandle::HandleMouseMove(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();


	if (stateComponent->GetLButtonDownState())
	{
		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		//mouseEvent->GetClientPositionX() 
		System* system = GetSystem();
		Camera* camera = system->GetMap()->GetMainCamera(GetMapLayerID());
		D3D12_VIEWPORT viewPort = system->GetViewPort();




		DirectX::XMFLOAT3 gizmoHandleStartPos = mGizmo->GetTransform().GetPositionWorld();
		DirectX::XMFLOAT3 gizmoHandleEndPos = gizmoHandleStartPos;
		switch (GetAxisType())
		{
		case EAxisType::eX:
			gizmoHandleEndPos.x += 1.0f;
			break;
		case EAxisType::eY:
			gizmoHandleEndPos.y += 1.0f;
			break;
		case EAxisType::eZ:
			gizmoHandleEndPos.z += 1.0f;

			break;
		}


		const DirectX::XMFLOAT4X4& viewProjMatrixF = camera->GetViewProjMatrix();
		DirectX::XMMATRIX viewProjMatrix = DirectX::XMLoadFloat4x4(&viewProjMatrixF);

		DirectX::XMVECTOR gizmoHandleStartNdcPos = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&gizmoHandleStartPos),
			viewProjMatrix);

		DirectX::XMVECTOR gizmoHandleEndNdcPos = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&gizmoHandleEndPos),
			viewProjMatrix);


		DirectX::XMMATRIX viewportMatrix = MathHelper::CaculateViewportMatrix(viewPort);

		DirectX::XMFLOAT3 gizmoHandleStartScreenPos;
		DirectX::XMFLOAT3 gizmoHandleEndScreenPos;

		DirectX::XMStoreFloat3(&gizmoHandleStartScreenPos, DirectX::XMVector3Transform(gizmoHandleStartNdcPos, viewportMatrix));
		DirectX::XMStoreFloat3(&gizmoHandleEndScreenPos, DirectX::XMVector3Transform(gizmoHandleEndNdcPos, viewportMatrix));

		gizmoHandleStartScreenPos.z = 0;
		gizmoHandleEndScreenPos.z = 0;

		DirectX::XMVECTOR gizmoHandleScreenVector = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&gizmoHandleEndScreenPos), DirectX::XMLoadFloat3(&gizmoHandleStartScreenPos)));

		DirectX::XMFLOAT3 currPos;
		DirectX::XMFLOAT3 prePos;

		float screenShiftX = (float)mouseEvent->GetClientPositionX() - (float)mouseEvent->GetPreClientPositionX();
		float screenShiftY = (float)mouseEvent->GetClientPositionY() - (float)mouseEvent->GetPreClientPositionY();

		DirectX::XMVECTOR mouseScreenVector = DirectX::XMVectorSet(screenShiftX, screenShiftY, 0, 0);


		float screenShift = 0;
		DirectX::XMStoreFloat(&screenShift, DirectX::XMVector2Dot(gizmoHandleScreenVector, mouseScreenVector));




		//DirectX::XMFLOAT3 length;
		//DirectX::XMStoreFloat3(&length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cameraPos), DirectX::XMLoadFloat3(&gizmoHandlePos))));


		DirectX::XMFLOAT3 shift = { 0,0,0 };

		switch (GetAxisType())
		{
		case EAxisType::eX:
		{

			shift.x += screenShift / 10.0F;
		}
		break;
		case EAxisType::eY:

		{
			shift.y += screenShift / 10.0F;
		}
		break;

		case EAxisType::eZ:

		{
			shift.z += screenShift / 10.0F;
		}
		break;

		}

		//	GetTransform().SetPositionWorld(newPosition);

		mGizmo->Scale(shift);



	}



}

void Quad::GizmoScaleHandle::InitCreating(EAxisType axisType, Gizmo* gizmo  ,Map * map)
{


	mGizmo = gizmo;
	mAxisType = axisType;

	//Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	//mLine = static_cast<Line*>(map->CreateObject("Line"));
	mLine = LineFactory::CreateLine({ 0,0,0 }, { 0,5,0 });
	mLine->SetSystem(map->GetSystem());
	mLine->SetMapLayer(mapLayerIndex);
//	mLine->SetMap(map);



	mBox = new Entity;
	mBox->SetSystem(map->GetSystem());
	mBox->SetMapLayer(mapLayerIndex);
	//mBox->SetMap(map);
	mBox->Initialize();
	mBox->InitCreating();



	GetTransform().SetPositionWorld({ 0,0,0 });
	

	//mLine->SetStartVertexPosition({ 0,0,0 });
	//mLine->SetEndVertexPosition({ 0,5,0 });
	//mLine->SetSystem(GetSystem());
	mLine->SetEffectName("GizmoLine.effect");
	mLine->SetEngineObjectFlag(true);

	//mBox->SetSystem(GetSystem());

//	mBox->Initialize();
	mBox->GetModel()->GetMeshComponent()->SetMesh(MeshManager::GetMesh("Cube"));
	mBox->GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName("Gizmo.effect");
	mBox->GetTransform().SetPositionWorld({ 0,5,0 });
	mBox->SetEngineObjectFlag(true);

	std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> minMaxPoint = mBox->GetModel()->GetMeshComponent()->GetMesh()->GetMinMaxPoint();
	minMaxPoint.second.y += 5;

	GetModel()->GetColliderComponent()->SetCollider(ColliderGenerator::CreateBoxCollider(minMaxPoint.first, minMaxPoint.second));
	//AddColliderComponent(ColliderGenerator::CreateBoxCollider(minMaxPoint.first, minMaxPoint.second));





	mHoverColor = { 1.0f,1.0f,0.0f };

	switch (mAxisType)
	{
	case EAxisType::eX:
	{
		mLine->SetName(GetName() + "_LineX");
		mBox->SetName(GetName() + "_BoxX");
		mDefaultColor = { 1.0f,0.0,0.0f };
	}
	break;
	case EAxisType::eY:
	{
		mLine->SetName(GetName() + "_LineY");
		mBox->SetName(GetName() + "_BoxY");
		mDefaultColor = { 0.0f,1.0,0.0f };
	}
	break;
	case EAxisType::eZ:
	{
		mLine->SetName(GetName() + "_LineZ");
		mBox->SetName(GetName() + "_BoxZ");
		mDefaultColor = { 0.0f,0.0,1.0f };

	}
	break;

	}

	SetColor(mDefaultColor);

	AddChildObject(mLine);
	AddChildObject(mBox);



}
