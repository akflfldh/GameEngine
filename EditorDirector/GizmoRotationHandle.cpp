#include "GizmoRotationHandle.h"
#include"ResourceManager/MeshManager.h"
#include"System.h"
#include"Map/Map.h"
#include"Object/Camera/Camera.h"
#include"Event/EventTable.h"
#include"MouseEvent.h"
#include"Mouse.h"
#include"Gizmo.h"

Quad::GizmoRotationHandle::GizmoRotationHandle(const std::string& name)
	:Entity(name)
{
	SetEngineObjectFlag(true);
}

void Quad::GizmoRotationHandle::Initialize()
{

	Entity::Initialize();


	//원점위치
	GetTransform().SetPositionWorld({ 0,0,0 });
	mActiveQuadrantIndex = 1;

	mInnerCircleRadius = 0.8f;
	mOuterCircleRadius = 1.0f;
	mDefaultInnerCircleRadius = 0.8f;
	mDefaultOuterCircleRadius = 1.0f;


	GetSystem()->GetEventTable()->AddObject("MouseMove", this);
	GetSystem()->GetEventTable()->AddObject("LButtonDown", this);
	GetSystem()->GetEventTable()->AddObject("LButtonUp", this);
	GetSystem()->GetEventTable()->AddObject("Hover", this);

	mHoverColor = { 1.0f,1.0f,0.0f };


}

Quad::GizmoRotationHandle* Quad::GizmoRotationHandle::Create(Map* map, int mapLayerIndex, EAxisType axisType, Gizmo* gizmo)
{
	//GizmoRotationHandle* ob = static_cast<GizmoRotationHandle*>(map->CreateObject("GizmoRotationHandle", mapLayerIndex));
	GizmoRotationHandle* ob = new GizmoRotationHandle;
	ob->SetSystem(map->GetSystem());
	ob->SetMapLayer(mapLayerIndex);
	//ob->SetMap(map);
	ob->Initialize();
	ob->InitCreating(axisType,gizmo);

	return ob;
}

void Quad::GizmoRotationHandle::SetColor(const DirectX::XMFLOAT3& color)
{
	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetColor(color);

}

void Quad::GizmoRotationHandle::SetScale(float scale)
{

	GetTransform().SetScaleLocal({ scale,1.0f,scale });

	float scaleWorld =	GetTransform().GetScaleWorld().x;

	mInnerCircleRadius = mDefaultInnerCircleRadius *scaleWorld;
	mOuterCircleRadius = mDefaultOuterCircleRadius * scaleWorld;
	

}

void Quad::GizmoRotationHandle::Update(float deltaTime)
{
	Entity::Update(deltaTime);


	Camera* camera = GetSystem()->GetMap()->GetMainCamera(1);
	const DirectX::XMFLOAT3 & cameraPos =	camera->GetTransform().GetPositionWorld();
	//const DirectX::XMFLOAT3& handlePos = GetTransform().GetPositionWorld();
	const DirectX::XMFLOAT3& handlePos = mGizmo->GetTransform().GetPositionWorld();
	
	DirectX::XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir,DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cameraPos), DirectX::XMLoadFloat3(&handlePos)));

	switch (mAxisType)
	{
	case EAxisType::eX:
		
		if (dir.y >=0)
		{
			if (dir.z >= 0)
			{
				ActiveQuadrant(1);
			}
			else
			{
				ActiveQuadrant(2);
			}

		}
		else
		{
			if (dir.z >= 0)
			{
				ActiveQuadrant(4);
			}
			else
			{
				ActiveQuadrant(3);
			}


		}


		break;
	case EAxisType::eY:
	{
		int a = 2;
		if (dir.x >= 0)
		{
			if (dir.z >= 0)
			{
				ActiveQuadrant(1);
			}
			else
			{
				ActiveQuadrant(4);
			}

		}
		else
		{
			if (dir.z >= 0)
			{
				ActiveQuadrant(2);
			}
			else
			{
				ActiveQuadrant(3);
			}


		}

	}
		break;
	case EAxisType::eZ:

		if (dir.y >= 0)
		{
			if (dir.x >= 0)
			{
				ActiveQuadrant(1);
			}
			else
			{
				ActiveQuadrant(2);
			}

		}
		else
		{
			if (dir.x >= 0)
			{
				ActiveQuadrant(4);
			}
			else
			{
				ActiveQuadrant(3);
			}


		}


		break;
	}


	float scaleWorld = GetTransform().GetScaleWorld().x;
	mInnerCircleRadius = mDefaultInnerCircleRadius * scaleWorld;
	mOuterCircleRadius = mDefaultOuterCircleRadius * scaleWorld;



}

void Quad::GizmoRotationHandle::OnEvent(Event* event)
{

	//Entity::OnEvent(event);
	const std::string& eventName = event->GetName();

	if (eventName == "Hover")
	{
		HandleHover(event);

	}else if (eventName == "LButtonDown")
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

Quad::EAxisType Quad::GizmoRotationHandle::GetAxisType() const
{
	return mAxisType;
}

bool Quad::GizmoRotationHandle::InnerDetectCollsion(Collider* colliderA, Collider* colliderB)
{
	return true;
}

bool Quad::GizmoRotationHandle::InnerDetectRayHit(const Ray& ray)
{

	DirectX::XMFLOAT3 pos = GetTransform().GetPositionWorld();

	//일단어떤 사분면이 활성화된상태인가.

	float parameterT = ray.GetParameterT();
	const DirectX::XMFLOAT3 &rayOrigin = ray.GetOrigin();
	const DirectX::XMFLOAT3 & rayDir = ray.GetDir();

	DirectX::XMFLOAT3 hitPoint;
	DirectX::XMStoreFloat3(&hitPoint ,DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rayOrigin),
		DirectX::XMVectorMultiply(DirectX::XMVectorSet(parameterT, parameterT, parameterT, 0.0f), DirectX::XMLoadFloat3(&rayDir))));

	DirectX::XMFLOAT3 dirFromCircleOriginToHitPointF;
	DirectX::XMVECTOR  dirFromCircleOriginToHitPoint = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&hitPoint), DirectX::XMLoadFloat3(&pos));
	DirectX::XMStoreFloat3(&dirFromCircleOriginToHitPointF, dirFromCircleOriginToHitPoint);


	DirectX::XMFLOAT3 lengthSq;
	DirectX::XMStoreFloat3(&lengthSq, DirectX::XMVector3LengthSq(dirFromCircleOriginToHitPoint));

	switch (mAxisType)
	{
	case EAxisType::eX:
	{


		switch (mActiveQuadrantIndex)
		{
		case 1:
		{
			if (dirFromCircleOriginToHitPointF.y < 0 || dirFromCircleOriginToHitPointF.z < 0)
			{
				//SetColor({ 1.0f,0.0f,0.0f });
				return false;
			}

		}
		break;
		case 2:
		{
			if (dirFromCircleOriginToHitPointF.y < 0 || dirFromCircleOriginToHitPointF.z > 0)
			{
				///SetColor({ 1.0f,0.0f,0.0f });
				return false;
			}

		}
		break;
		case 3:
		{
			if (dirFromCircleOriginToHitPointF.y > 0 || dirFromCircleOriginToHitPointF.z > 0)
			{
				//SetColor({ 1.0f,0.0f,0.0f });
				return false;
			}
		}
		break;
		case 4:
		{
			if (dirFromCircleOriginToHitPointF.y > 0 || dirFromCircleOriginToHitPointF.z < 0)
			{
				//SetColor({ 1.0f,0.0f,0.0f });
				return false;
			}
		}
		break;

		}





		if ((lengthSq.x >= (mInnerCircleRadius * mInnerCircleRadius)) && (lengthSq.x <= (mOuterCircleRadius * mOuterCircleRadius)))
		{

			//SetColor({ 1.0f,1.0f,1.0f });
			return true;
		}
		else
		{
			////SetColor({ 1.0f,0.0f,0.0f });
			return false;
		}




	}
	break;
	case EAxisType::eY:
	{

		switch (mActiveQuadrantIndex)
		{
		case 1:
		{
			if (dirFromCircleOriginToHitPointF.x < 0 || dirFromCircleOriginToHitPointF.z < 0)
			{
				//SetColor({ 0.0f,1.0f,0.0f });
				return false;
			}

		}
		break;
		case 2:
		{
			if (dirFromCircleOriginToHitPointF.x > 0 || dirFromCircleOriginToHitPointF.z < 0)
			{
				//SetColor({ 0.0f,1.0f,0.0f });
				return false;
			}

		}
		break;
		case 3:
		{
			if (dirFromCircleOriginToHitPointF.x > 0 || dirFromCircleOriginToHitPointF.z > 0)
			{
				//SetColor({ 0.0f,1.0f,0.0f });
				return false;
			}
		}
		break;
		case 4:
		{
			if (dirFromCircleOriginToHitPointF.x < 0 || dirFromCircleOriginToHitPointF.z > 0)
			{
				//SetColor({ 0.0f,1.0f,0.0f });
				return false;
			}
		}
		break;

		}




		if ((lengthSq.x >= (mInnerCircleRadius * mInnerCircleRadius)) && (lengthSq.x <= (mOuterCircleRadius * mOuterCircleRadius)))
		{

			//SetColor({ 1.0f,1.0f,1.0f });
			return true;
		}
		else
		{
			//SetColor({ 0.0f,1.0f,0.0f });
			return false;
		}


	}
	break;
	case EAxisType::eZ:
	{


		switch (mActiveQuadrantIndex)
		{ 
		case 1:
		{
			if (dirFromCircleOriginToHitPointF.x < 0 || dirFromCircleOriginToHitPointF.y < 0)
			{
				//SetColor({ 0.0f,0.0f,1.0f });
				return false;
			}

		}
		break;
		case 2:
		{
			if (dirFromCircleOriginToHitPointF.x > 0 || dirFromCircleOriginToHitPointF.y < 0)
			{
				//SetColor({ 0.0f,0.0f,1.0f });
				return false;
			}

		}
		break;
		case 3:
		{
			if (dirFromCircleOriginToHitPointF.x > 0 || dirFromCircleOriginToHitPointF.y > 0)
			{
				//SetColor({ 0.0f,0.0f,1.0f });
				return false;
			}
		}
		break;
		case 4:
		{
			if (dirFromCircleOriginToHitPointF.x < 0 || dirFromCircleOriginToHitPointF.y > 0)
			{
				////SetColor({ 0.0f,0.0f,1.0f });
				return false;
			}
		}
		break;

		}




		if ((lengthSq.x >= (mInnerCircleRadius * mInnerCircleRadius)) && (lengthSq.x <= (mOuterCircleRadius * mOuterCircleRadius)))
		{

			//SetColor({ 1.0f,1.0f,0.0f });
			return true;
		}
		else
		{
			//SetColor({ 0.0f,0.0f,1.0f });
			return false;
		}



	}

	break;
	}






	return true;
}

void Quad::GizmoRotationHandle::HandleLButtonDown(Event* pEvent)
{
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;


	
	StateComponent* stateComponent = GetStateComponent();


	if (stateComponent->GetHoverState())
	{
		stateComponent->SetLButtonDownState(true);
		Mouse::CaptureObject(this);

	}







}

void Quad::GizmoRotationHandle::HandleLButtonUp(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);
		if (!stateComponent->GetHoverState())
		{
			SetColor(mDefaultColor);
		}


		Mouse::ReleaseCaptureObject(this);


	}

}

void Quad::GizmoRotationHandle::HandleHover(Event* pEvent)
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

void Quad::GizmoRotationHandle::HandleMouseMove(Event* pEvent)
{



	StateComponent* stateComponent = GetStateComponent();
	
	
	if (stateComponent->GetLButtonDownState())
	{
		MouseEvent* mouseEvent = (MouseEvent*)pEvent;
		int shiftScreenPosX = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();
		int shiftScreenPosY = mouseEvent->GetClientPositionY() - mouseEvent->GetPreClientPositionY();



		switch (mAxisType)
		{
		case EAxisType::eX:
		{
			mGizmo->Rotate({ (float)(shiftScreenPosX + shiftScreenPosY),0,0 });


		}
		break;
		case EAxisType::eY:
		{
			mGizmo->Rotate({ 0, (float)(shiftScreenPosX + shiftScreenPosY),0 });

			

		}
		break;
		case EAxisType::eZ:
		{

			mGizmo->Rotate({ 0,0, (float)(shiftScreenPosX + shiftScreenPosY) });

		}
		break;

		}







	}










}

void Quad::GizmoRotationHandle::ActiveQuadrant(int num)
{

	mActiveQuadrantIndex = num;



	DirectX::XMMATRIX rotationPrePivotTransform = DirectX::XMMatrixSet(1.0f, 0.0f, 0.0f, 0.0f,
																		0.0f, 1.0f, 0.0f, 0.0f,
																		0.0f, 0.0f, 1.0f, 0.0f,
																		-0.5f, -0.5f, 0.0f, 1.0f);


	DirectX::XMMATRIX rotationPostPivotTransform = DirectX::XMMatrixSet(1.0f, 0.0f, 0.0f, 0.0f,
																		0.0f, 1.0f, 0.0f, 0.0f,
																		0.0f, 0.0f, 1.0f, 0.0f,
																		0.5f, 0.5f, 0.0f, 1.0f);


	//tex 변환행렬
	if (num == 1)
	{
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetTextureTransformMatrix(0, Utility::GetIdentityMatrixF());
	}
	else if (num == 2)
	{
		DirectX::XMMATRIX rotationTransform;
		/*if (mAxisType == EAxisType::eX)
		{
			rotationTransform = DirectX::XMMatrixSet(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

		}
		else*/
		{

			 rotationTransform = DirectX::XMMatrixSet(-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}





		DirectX::XMMATRIX texTransformMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(rotationPrePivotTransform, rotationTransform), rotationPostPivotTransform));
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetTextureTransformMatrix(0, texTransformMatrix);
	}
	else if (num == 3)
	{
		DirectX::XMMATRIX rotationTransform;

		
			 rotationTransform = DirectX::XMMatrixSet(-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		
		DirectX::XMMATRIX texTransformMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(rotationPrePivotTransform, rotationTransform), rotationPostPivotTransform));
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetTextureTransformMatrix(0, texTransformMatrix);
	}
	else if (num == 4)
	{

		DirectX::XMMATRIX rotationTransform;
		/*if (mAxisType == EAxisType::eX)
		{
			rotationTransform = DirectX::XMMatrixSet(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);

		}
		else*/
		{
			 rotationTransform = DirectX::XMMatrixSet(1.0f, 0.0f, 0.0f, 0.0f,
				0.0f,-1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}
		DirectX::XMMATRIX texTransformMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(rotationPrePivotTransform, rotationTransform), rotationPostPivotTransform));
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetTextureTransformMatrix(0, texTransformMatrix);
	}

	//실제 충돌판정을 하는 콜백함수도 넣어준다.




}

void Quad::GizmoRotationHandle::InitCreating(EAxisType axisType, Gizmo * gizmo)
{

	mGizmo = gizmo;
	mAxisType = axisType;

	GetModel()->GetMeshComponent()->SetMesh(MeshManager::GetMesh("GizmoRotationCircle"));
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

	GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName("GizmoRotation.effect");

	//원점위치
	GetTransform().SetPositionWorld({ 0,0,0 });
	mActiveQuadrantIndex = 1;

	mInnerCircleRadius = 0.8f;
	mOuterCircleRadius = 1.0f;
	mDefaultInnerCircleRadius = 0.8f;
	mDefaultOuterCircleRadius = 1.0f;


	mHoverColor = { 1.0f,1.0f,0.0f };

}


