#include "GizmoHandle.h"
#include"ObjectFactory/LineFactory.h"
#include"ResourceManager/MeshManager.h"

#include"Collision/ColliderGenerator.h"
#include"MouseEvent.h"

#include"System.h"
#include"Map/Map.h"
#include"Object/Camera/Camera.h"
#include"MathHelper.h"
#include"Mouse.h"
#include<string>

#include"Component/ColliderComponent.h"
#include"Gizmo.h"

#include"Mouse.h"


Quad::GizmoHandle::GizmoHandle(const std::string& name)
	:Entity(name),mCone(nullptr)
{
	GetModel()->RemoveComponent(EComponentType::eMeshComponent);
	SetEngineObjectFlag(true);

}
void Quad::GizmoHandle::Initialize()
{
	Entity::Initialize();


	RegisterAcceptEvent("LButtonDown");
	RegisterAcceptEvent("MouseMove");
	RegisterAcceptEvent("LButtonUp");

	GetModel()->RemoveComponent(EComponentType::eMeshComponent);

	//충돌체는 가지고있어야겠다.
	GetTransform().SetPositionLocal({ 0,0,0 });





	//mLine = LineFactory::CreateLine({ 0,0,0 }, { 0,5,0 });
	//mLine = static_cast<Line*> (GetMap()->CreateObject("Line"));
	//mLine->SetStartVertexPosition({ 0,0,0 });
	//mLine->SetEndVertexPosition({ 0,5,0 });
	//mLine->SetEngineObjectFlag(true);

	////mLine->SetSystem(GetSystem());
	////mLine->SetName(GetName() + "_Line");
	////mLine->GetMaterial().SetColor({ 1.0F,1.0F,1.0F });

	//mLine->GetTransform().SetIndependentScaleFlag(false);
	//
	//mLine->SetEffectName("GizmoLine.effect");



	//mCone.SetSystem(GetSystem());

 //	mCone->Initialize(MeshManager::GetMesh("Cone"));
	//mCone->SetEngineObjectFlag(true);


	//mCone->GetTransform().SetPositionLocal({ 0,5,0 });


	//MeshComponent* coneMeshComponent = static_cast<MeshComponent*>(mCone->GetModel()->GetComponent(EComponentType::eMeshComponent));
	//coneMeshComponent->GetSubMesh()[0].mModelMaterial.SetEffectName("Gizmo.effect");


	//std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> minMaxPoint = coneMeshComponent->GetMesh()->GetMinMaxPoint();
	//minMaxPoint.second.y += 5;

	//ColliderComponent* colliderComponent = static_cast<ColliderComponent*>(GetModel()->GetComponent(EComponentType::eColliderComponent));
	//colliderComponent->SetCollider(ColliderGenerator::CreateBoxCollider(minMaxPoint.first, minMaxPoint.second));
	////AddColliderComponent(ColliderGenerator::CreateBoxCollider(minMaxPoint.first, minMaxPoint.second));
	


	//AddChildObject(mLine);
	//AddChildObject(mCone);

	mHoverColor = { 1.0f,1.0f,0.0f };
}

Quad::GizmoHandle* Quad::GizmoHandle::Create(Map* map, int mapLayerIndex , EAxisType axisType, Gizmo * gizmo)
{

	//GizmoHandle * ob = static_cast<GizmoHandle*>(map->CreateObject("GizmoHandle", mapLayerIndex));
	GizmoHandle* ob = new GizmoHandle;
	//ob->mMap = map;
	ob->SetMapLayer(mapLayerIndex);
	ob->SetSystem(map->GetSystem());
	ob->Initialize();
	ob->InitCreating(map,mapLayerIndex,axisType,gizmo);

	//map->AddGizmo(ob);


	return ob;
}

void Quad::GizmoHandle::SetColor(const DirectX::XMFLOAT3& color)
{
	mLine->GetModelSubMesh()->mModelMaterial.SetColor(color);
	MeshComponent * coneMeshComponent =static_cast<MeshComponent*>(mCone->GetModel()->GetComponent(EComponentType::eMeshComponent));
	coneMeshComponent->GetSubMesh()[0].mModelMaterial.SetColor(color);



}

void Quad::GizmoHandle::Update(float deltaTime)
{




}

void Quad::GizmoHandle::OnEvent(Event* event)
{
	Entity::OnEvent(event);


	const std::string& eventName = event->GetName();

	if (eventName == "LButtonDown")
	{
		HandleLButtonDown(event);
	}
	else if (eventName == "Hover")
	{
		HandleHover(event);

	}
	else if (eventName == "MouseMove")
	{
		HandleMouseMove(event);
	}
	else if (eventName == "LButtonUp")
	{
		HandleLButtonUp(event);

	}





}

Quad::EAxisType Quad::GizmoHandle::GetAxisType() const
{
	return mAxisType;
}

void Quad::GizmoHandle::HandleLButtonDown(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{

		Mouse::CaptureObject(this);
		stateComponent->SetLButtonDownState(true);
		stateComponent->SetSelectState(true);

	}


}

void Quad::GizmoHandle::HandleLButtonUp(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);
		stateComponent->SetSelectState(false);
		Mouse::ReleaseCaptureObject(this);
	}
	if (!stateComponent->GetHoverState())
	{
		SetColor(mDefaultColor);
	}

}

void Quad::GizmoHandle::HandleHover(Event* pEvent)
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
		if (!stateComponent->GetLButtonDownState())
		{
			SetColor(mDefaultColor);
		}		
		
		stateComponent->SetHoverState(false);
	}


}

void Quad::GizmoHandle::HandleMouseMove(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;

	if (stateComponent->GetSelectState())
	{
		//원근투영임으로
		//멀리잇는물체는 조금만 픽셀이움직여도 많이 움직이고
		//가까이있는물체는 조금움직이고 


		//카메라공간에서 각 축위에서 얼만큼움직였는지 알아내고, (2차원을 3차원으로바꾸는것이니 z는 0일것이고)
		//월드공간에서의 카메라공간의 축위에서의 변화에 , 카메라와 핸들의 거리차이를 고려해서 이동시키자.
		System* system = GetSystem();
		Camera* camera = system->GetMap()->GetMainCamera(GetMapLayerID());
		D3D12_VIEWPORT viewPort = system->GetViewPort();




		DirectX::XMFLOAT3 gizmoHandleStartPos =	mGizmo->GetTransform().GetPositionWorld();
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

		DirectX::XMVECTOR gizmoHandleEndNdcPos= DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&gizmoHandleEndPos),
			viewProjMatrix);


		DirectX::XMMATRIX viewportMatrix = MathHelper::CaculateViewportMatrix(viewPort);

		DirectX::XMFLOAT3 gizmoHandleStartScreenPos;
		DirectX::XMFLOAT3 gizmoHandleEndScreenPos;

		DirectX::XMStoreFloat3(&gizmoHandleStartScreenPos,DirectX::XMVector3Transform(gizmoHandleStartNdcPos, viewportMatrix));
		DirectX::XMStoreFloat3(&gizmoHandleEndScreenPos,DirectX::XMVector3Transform(gizmoHandleEndNdcPos, viewportMatrix));

		gizmoHandleStartScreenPos.z = 0;
		gizmoHandleEndScreenPos.z = 0;

		DirectX::XMVECTOR gizmoHandleScreenVector= DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&gizmoHandleEndScreenPos), DirectX::XMLoadFloat3(&gizmoHandleStartScreenPos)));

		DirectX::XMFLOAT3 currPos;
		DirectX::XMFLOAT3 prePos;

		float screenShiftX = (float)mouseEvent->GetClientPositionX() - (float)mouseEvent->GetPreClientPositionX();
		float screenShiftY = (float)mouseEvent->GetClientPositionY() - (float)mouseEvent->GetPreClientPositionY();

		DirectX::XMVECTOR mouseScreenVector = DirectX::XMVectorSet(screenShiftX, screenShiftY, 0, 0);

		
		float screenShift = 0;
		DirectX::XMStoreFloat(&screenShift, DirectX::XMVector2Dot(gizmoHandleScreenVector, mouseScreenVector));
		


		DirectX::XMFLOAT3 cameraPosW = camera->GetTransform().GetPositionWorld();
		DirectX::XMFLOAT3 gizmoPosW = mGizmo->GetTransform().GetPositionWorld();
		DirectX::XMFLOAT3 length;
		DirectX::XMStoreFloat3(&length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cameraPosW), DirectX::XMLoadFloat3(&gizmoPosW))));


		



		DirectX::XMFLOAT3 shift = { 0,0,0 };

		switch (GetAxisType())
		{
		case EAxisType::eX:
		{

			shift.x += screenShift/100.0F * length.x;
		}
		break;
		case EAxisType::eY:

		{
			shift.y += screenShift/100.0F * length.x;
		}
		break;

		case EAxisType::eZ:

		{	
			shift.z += screenShift/100.0F * length.x;
		}
		break;

		}
		
	//	GetTransform().SetPositionWorld(newPosition);
		OutputDebugString(std::to_wstring(shift.x).c_str());
		OutputDebugString(L"\n");
		mGizmo->Move(shift);




		







	}



}

void Quad::GizmoHandle::InitCreating(Map * map, int mapLayerIndex , EAxisType axisType, Gizmo* gizmo)
{
	mAxisType = axisType;
	mGizmo = gizmo;


	//Map* map = GetMap();
	//int mapLayerIndex = GetMapLayerID();
	//mCone = Entity::Create(map, mapLayerIndex);
	mCone = new Entity;

	mCone->SetSystem(map->GetSystem());

	mCone->SetMapLayer(mapLayerIndex);
	mCone->Initialize();
	mCone->InitCreating();
	//mCone->SetMap(map);


	mCone->GetModel()->GetMeshComponent()->SetMesh(MeshManager::GetMesh("Cone"));
	mCone->SetEngineObjectFlag(true);


	mCone->GetTransform().SetPositionLocal({ 0,5,0 });
	MeshComponent* coneMeshComponent = static_cast<MeshComponent*>(mCone->GetModel()->GetComponent(EComponentType::eMeshComponent));
	coneMeshComponent->GetSubMesh()[0].mModelMaterial.SetEffectName("Gizmo.effect");



	//mLine = static_cast<Line*> (GetMap()->CreateObject("Line"));
	mLine = LineFactory::CreateLine({ 0,0,0 }, { 0,0,1 });
	mLine->SetSystem(map->GetSystem());
	mLine->SetMapLayer(mapLayerIndex);
	//mLine->SetMap(map);
	mLine->Initialize();
	mLine->SetStartVertexPosition({ 0,0,0 });
	mLine->SetEndVertexPosition({ 0,5,0 });
	mLine->SetEngineObjectFlag(true);

	//mLine->SetSystem(GetSystem());
	//mLine->SetName(GetName() + "_Line");
	//mLine->GetMaterial().SetColor({ 1.0F,1.0F,1.0F });

	mLine->GetTransform().SetIndependentScaleFlag(false);

	mLine->SetEffectName("GizmoLine.effect");



	std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> minMaxPoint = coneMeshComponent->GetMesh()->GetMinMaxPoint();
	minMaxPoint.second.y += 5;

	ColliderComponent* colliderComponent = static_cast<ColliderComponent*>(GetModel()->GetComponent(EComponentType::eColliderComponent));
	colliderComponent->SetCollider(ColliderGenerator::CreateBoxCollider(minMaxPoint.first, minMaxPoint.second));
	//AddColliderComponent(ColliderGenerator::CreateBoxCollider(minMaxPoint.first, minMaxPoint.second));



	AddChildObject(mLine); 
	AddChildObject(mCone);



	switch (axisType)
	{
	case EAxisType::eX:
		mDefaultColor = { 1.0f,0.0,0.0f };
		break;
	case EAxisType::eY:
		mDefaultColor = { 0.0f,1.0,0.0f };
		break;
	case EAxisType::eZ:
		mDefaultColor = { 0.0f,0.0,1.0f };
		break;
	}

	SetColor(mDefaultColor);


}

