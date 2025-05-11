#include "Gizmo.h"
#include"System.h"
#include"Map/Map.h"
#include"Object/Camera/Camera.h"
#include"KeyEvent.h"
#include"Event/EventTable.h"
#include"Collision/CollisionWorld.h"
#include"Component/ColliderComponent.h"

#include"EditorMap.h"

#include"Mouse.h"












Quad::Gizmo::Gizmo(const std::string& name)
	:Entity(name), /*mHandleX(name + "_HandleX"), mHandleY(name + "_HandleY"), mHandleZ(name + "_HandleZ")
	, mRotationHandleX(name + "_RotationHandleX"), mRotationHandleY(name + "_RotationHandleY"), mRotationHandleZ(name + "_RotationHandle"), mScaleHandleX(name + "_ScaleHandleX"),mScaleHandleY(name+"_ScaleHandleY"),mScaleHandleZ(name+"_ScaleHandleZ")*/
	mHandleX(nullptr), mHandleY(nullptr), mHandleZ(nullptr)
	, mRotationHandleX(nullptr), mRotationHandleY(nullptr), mRotationHandleZ(nullptr), mScaleHandleX(nullptr), mScaleHandleY(nullptr), mScaleHandleZ(nullptr)


{
	SetEngineObjectFlag(true);

}

void Quad::Gizmo::Initialize()
{
	Entity::Initialize();
	SetActiveFlag(false);

	GetSystem()->GetEventTable()->AddObject("KeyDown", this);

	GetTransform().SetIndependentScaleFlag(true);
	GetTransform().SetIndependentRotationFlag(true);
	mMode = EGizmoMode::eTranslation;


	GetTransform().SetPositionLocal({ 0,0,0.0f });


	mDefaultSize = 1;

}

Quad::Gizmo* Quad::Gizmo::Create(EditorMap* map, int mapLayerIndex)
{
	//Gizmo* ob = static_cast<Gizmo*>(map->CreateObject("Gizmo", mapLayerIndex));
	Gizmo* ob = new Gizmo;
	ob->SetSystem(map->GetSystem());
	ob->SetMapLayer(mapLayerIndex);

	ob->Initialize();
	ob->InitCreating(map);

	map->AddGizmo(ob);
	ob->mMap = map;

	int a = 2;
	////gizmo의 모든 자식오브젝트들의 id는 0(설정되지않은값) 이고 , 아마 table에 id가 0으로등록되었을것이니 nullptr로 다시설정해준다.
	//map->mEditSceneGraph.AddNewObjectNode(ob);
	//map->mEditSceneGraph.mObjectIDTable.erase(0);

	//CollisionWorld* currentCollisionWorld = nullptr;
	//currentCollisionWorld = map->mMapLayerVector[mapLayerIndex].mCollisionWorld;


	//std::queue<Object*> objectQueue;
	//objectQueue.push(ob);

	//while (!objectQueue.empty())
	//{
	//	Object* object = objectQueue.front();
	//	objectQueue.pop();

	//	switch (object->GetObjectType())
	//	{
	//	case EObjectType::eEntity:
	//	case EObjectType::eLine:
	//	{
	//		ColliderComponent* colliderComponent = object->GetModel()->GetColliderComponent();
	//		
	//		if (colliderComponent != nullptr)
	//			currentCollisionWorld->AddGizmoCollider(colliderComponent->GetCollider());
	//	}
	//	break;
	//	}

	//	const std::vector<ObjectSmartPointer> childObjectVector = object->GetChildObjectVector();

	//	for_each(childObjectVector.begin(), childObjectVector.end(), [&objectQueue](const ObjectSmartPointer& objectPtr)
	//		{  objectQueue.push(objectPtr.GetPointer()); });

	//}





	//map->mMapLayerVector[mapLayerIndex].mCollisionWorld->AddGizmoCollider
	




	return ob;
}

void Quad::Gizmo::Update(float deltaTime)
{

	Camera* camera = GetSystem()->GetMap()->GetMainCamera(1);
	const DirectX::XMFLOAT3& cameraPos = camera->GetTransform().GetPositionWorld();
	const DirectX::XMFLOAT3& gizmoPos = GetTransform().GetPositionWorld();

	const DirectX::XMFLOAT4X4 & viewMatrixF = camera->GetViewProjMatrix();
	DirectX::XMFLOAT3 gizmoViewPos;
	DirectX::XMStoreFloat3(&gizmoViewPos,DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&gizmoPos),DirectX::XMLoadFloat4x4(&viewMatrixF)));

	DirectX::XMFLOAT3 length;
	DirectX::XMStoreFloat3(&length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cameraPos), DirectX::XMLoadFloat3(&gizmoPos))));

	float nearPlane = camera->GetNearPlane();

	float scaleValue = gizmoViewPos.z*mDefaultSize / (nearPlane*10) ;
	GetTransform().SetScaleLocal({ scaleValue ,scaleValue ,scaleValue });
		
	switch (mMode)
	{
	case EGizmoMode::eRotation:
	{

		{
			








		}







	}
	break;
	}
}

void Quad::Gizmo::OnEvent(Event* event)
{

	const std::string& eventName = event->GetName();


	
	if (!Mouse::GetRButtonState()  && eventName == "KeyDown")
	{
		HandleKeyDownEvent(event);
	}



}

void Quad::Gizmo::SetDestObject(Object* object)
{
	mDestObject = object;
}

void Quad::Gizmo::Move(const DirectX::XMFLOAT3& shift)
{

	DirectX::XMFLOAT3 pos =	GetTransform().GetPositionWorld();
	DirectX::XMStoreFloat3(&pos,DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&shift)));
	//GetTransform().SetPositionWorld(pos);

	GetTransform().SetPositionWorld(pos);
	mDestObject->GetTransform().SetPositionWorld(pos);





}
void Quad::Gizmo::Rotate(const DirectX::XMFLOAT3& yawPitchRoll)
{

	DirectX::XMVECTOR rotation = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&yawPitchRoll), DirectX::XM_PI / 180.0f);
	DirectX::XMFLOAT4 quaternion = mDestObject->GetTransform().GetQuaternionLocal();


	//DirectX::XMStoreFloat3(&rotation, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&rotation), DirectX::XMLoadFloat3(&yawPitchRoll)));

	DirectX::XMStoreFloat4(&quaternion, DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&quaternion), DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation)));



	mDestObject->GetTransform().SetQuaternionLocal({ quaternion });

	


}
void Quad::Gizmo::Scale(const DirectX::XMFLOAT3& scale)
{
	DirectX::XMFLOAT3 scaling = mDestObject->GetTransform().GetScaleLocal();
		
	DirectX::XMStoreFloat3(&scaling, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&scaling), DirectX::XMLoadFloat3(&scale)));

	mDestObject->GetTransform().SetScaleLocal(scaling);



}
bool Quad::Gizmo::GetChildSelectState() const
{

	switch (mMode)
	{
	case EGizmoMode::eTranslation:
		if (mHandleX->GetStateComponent()->GetSelectState() || mHandleY->GetStateComponent()->GetSelectState() ||
			mHandleZ->GetStateComponent()->GetSelectState())
		{
			return true;
		}
		break;
	case EGizmoMode::eRotation:
		
		if (mRotationHandleX->GetStateComponent()->GetSelectState() || mRotationHandleY->GetStateComponent()->GetSelectState() ||
			mRotationHandleZ->GetStateComponent()->GetSelectState())
		{
			return true;
		}

		break;
	case EGizmoMode::eScaling:

		if (mScaleHandleX->GetStateComponent()->GetSelectState() || mScaleHandleY->GetStateComponent()->GetSelectState() ||
			mScaleHandleZ->GetStateComponent()->GetSelectState())
		{
			return true;
		}


	}


	return false;
}

bool Quad::Gizmo::CheckChildHandleSelect(Object* object) const
{
	switch (mMode)
	{
	case EGizmoMode::eTranslation:

		if (object == mHandleX || object == mHandleY || object == mHandleZ)
			return true;

		break;
	case EGizmoMode::eRotation:
		if (object == mRotationHandleX || object == mRotationHandleY || object == mRotationHandleZ)
			return true;
		break;

	case EGizmoMode::eScaling:
		if (object == mScaleHandleX || object == mScaleHandleY || object == mScaleHandleZ)
			return true;
		break;
	}

	return false;
}

Quad::EGizmoMode Quad::Gizmo::GetGizmoMode() const
{
	return mMode;
}

void Quad::Gizmo::SetGizmoMode(EGizmoMode mode)
{
	mMode = mode;
}

void Quad::Gizmo::OnGizmo()
{
	SetDrawFlag(true);
	SetActiveFlag(true);

	Object* selectedObject = mDestObject;
	DirectX::XMFLOAT3 pos= 	selectedObject->GetTransform().GetPositionWorld();
	GetTransform().SetPositionWorld(pos);

	switch (mMode)
	{
	case EGizmoMode::eTranslation:

	{
		mHandleX->SetEntireDrawFlag(true);
		mHandleX->SetEntireSelectAvailableFlag(true);

		mHandleY->SetEntireDrawFlag(true);
		mHandleY->SetEntireSelectAvailableFlag(true);

		mHandleZ->SetEntireDrawFlag(true);
		mHandleZ->SetEntireSelectAvailableFlag(true);

	}
	break;
	case EGizmoMode::eRotation:

	{

		mRotationHandleX->SetEntireDrawFlag(true);
		mRotationHandleX->SetEntireSelectAvailableFlag(true);

		mRotationHandleY->SetEntireDrawFlag(true);
		mRotationHandleY->SetEntireSelectAvailableFlag(true);

		mRotationHandleZ->SetEntireDrawFlag(true);
		mRotationHandleZ->SetEntireSelectAvailableFlag(true);

	}
	break;
	case EGizmoMode::eScaling:

	{

		mScaleHandleX->SetEntireDrawFlag(true);
		mScaleHandleX->SetEntireSelectAvailableFlag(true);

		mScaleHandleY->SetEntireDrawFlag(true);
		mScaleHandleY->SetEntireSelectAvailableFlag(true);

		mScaleHandleZ->SetEntireDrawFlag(true);
		mScaleHandleZ->SetEntireSelectAvailableFlag(true);



	}

	}


}

void Quad::Gizmo::OffGizmo()
{

	SetDrawFlag(false);
	SetActiveFlag(false);
	switch (mMode)
	{
	case EGizmoMode::eTranslation:

	{
		mHandleX->SetEntireDrawFlag(false);
		mHandleX->SetEntireSelectAvailableFlag(false);

		mHandleY->SetEntireDrawFlag(false);
		mHandleY->SetEntireSelectAvailableFlag(false);

		mHandleZ->SetEntireDrawFlag(false);
		mHandleZ->SetEntireSelectAvailableFlag(false);

	}
	break;
	case EGizmoMode::eRotation:

	{

		mRotationHandleX->SetEntireDrawFlag(false);
		mRotationHandleX->SetEntireSelectAvailableFlag(false);

		mRotationHandleY->SetEntireDrawFlag(false);
		mRotationHandleY->SetEntireSelectAvailableFlag(false);

		mRotationHandleZ->SetEntireDrawFlag(false);
		mRotationHandleZ->SetEntireSelectAvailableFlag(false);

	}
	break;
	case EGizmoMode::eScaling:

	{

		mScaleHandleX->SetEntireDrawFlag(false);
		mScaleHandleX->SetEntireSelectAvailableFlag(false);

		mScaleHandleY->SetEntireDrawFlag(false);
		mScaleHandleY->SetEntireSelectAvailableFlag(false);

		mScaleHandleZ->SetEntireDrawFlag(false);
		mScaleHandleZ->SetEntireSelectAvailableFlag(false);

	}
	}






}




void Quad::Gizmo::HandleKeyDownEvent(Event* event)
{

	KeyEvent* keyEvent = (KeyEvent*)event;

	int key = keyEvent->GetKey();
	switch (key)
	{
	case 'W':

	{
		if (GetGizmoMode() != EGizmoMode::eTranslation)
		{

			OffGizmo();
			SetGizmoMode(EGizmoMode::eTranslation);
			OnGizmo();

		}
	}

	break;
	case 'E':
	{
		if (GetGizmoMode() != EGizmoMode::eRotation)
		{

			OffGizmo();
			SetGizmoMode(EGizmoMode::eRotation);
			OnGizmo();

		}
	}
	
	break;
	case 'R':
	{
		if (GetGizmoMode() != EGizmoMode::eScaling)
		{

			OffGizmo();
			SetGizmoMode(EGizmoMode::eScaling);
			OnGizmo();

		}


	}

	}



}

void Quad::Gizmo::InitCreating(Map * map)
{
	Entity::InitCreating();

	GetModel()->RemoveComponent(EComponentType::eMeshComponent);
	GetModel()->RemoveComponent(EComponentType::eColliderComponent);

	//Map* map = GetMap();

	int mapLayerIndex = GetMapLayerID();
	
	

	//mHandleX = static_cast<GizmoHandle*>(map->CreateObject("GizmoHandle"));
	mHandleX = GizmoHandle::Create(map, mapLayerIndex, EAxisType::eX, this);



	//mHandleY = static_cast<GizmoHandle*>(map->CreateObject("GizmoHandle"));
	mHandleY= GizmoHandle::Create(map, mapLayerIndex, EAxisType::eY, this);

	//mHandleZ = static_cast<GizmoHandle*>(map->CreateObject("GizmoHandle"));
	mHandleZ = GizmoHandle::Create(map, mapLayerIndex, EAxisType::eZ, this);


//	mRotationHandleX = static_cast<GizmoRotationHandle*>(map->CreateObject("GizmoRotationHandle"));
	mRotationHandleX = GizmoRotationHandle::Create(map, mapLayerIndex, EAxisType::eX, this);


	//mRotationHandleY = static_cast<GizmoRotationHandle*>(map->CreateObject("GizmoRotationHandle"));
	mRotationHandleY = GizmoRotationHandle::Create(map, mapLayerIndex, EAxisType::eY, this);
	//mRotationHandleZ = static_cast<GizmoRotationHandle*>(map->CreateObject("GizmoRotationHandle"));
	mRotationHandleZ = GizmoRotationHandle::Create(map, mapLayerIndex, EAxisType::eZ, this);

	//mScaleHandleX = static_cast<GizmoScaleHandle*>(map->CreateObject("GizmoScaleHandle"));
	mScaleHandleX = GizmoScaleHandle::Create(map, mapLayerIndex, EAxisType::eX, this);

//	mScaleHandleY = static_cast<GizmoScaleHandle*>(map->CreateObject("GizmoScaleHandle"));
	mScaleHandleY = GizmoScaleHandle::Create(map, mapLayerIndex, EAxisType::eY, this);
	//mScaleHandleZ = static_cast<GizmoScaleHandle*>(map->CreateObject("GizmoScaleHandle"));

	mScaleHandleZ = GizmoScaleHandle::Create(map, mapLayerIndex, EAxisType::eZ, this);












	SetActiveFlag(false);

	GetSystem()->GetEventTable()->AddObject("KeyDown", this);

	GetTransform().SetIndependentScaleFlag(true);
	GetTransform().SetIndependentRotationFlag(true);
	mMode = EGizmoMode::eTranslation;

	//	mHandleX->SetSystem(GetSystem());
		//mHandleY->SetSystem(GetSystem());
	//	mHandleZ->SetSystem(GetSystem());

//	mHandleX->Initialize(EAxisType::eX, this);
//	mHandleY->Initialize(EAxisType::eY, this);
//	mHandleZ->Initialize(EAxisType::eZ, this);

	mHandleX->SetColor({ 1.0f,0.0,0.0 });
	mHandleY->SetColor({ 0, 1.0f ,0 });
	mHandleZ->SetColor({ 0.0, 0.0 , 1.0f });



	mHandleX->GetTransform().SetRotationLocal({ 0,0,-90 });
	mHandleX->GetTransform().SetPositionLocal({ 0.0f,0,0 });

	mHandleY->GetTransform().SetPositionLocal({ 0.0f,0,0 });


	mHandleZ->GetTransform().SetPositionLocal({ 0.0f,0,0 });
	mHandleZ->GetTransform().SetRotationLocal({ 90,0,0 });



	//mHandleX.SetActiveFlag(false);
	mHandleX->SetEntireDrawFlag(false);
	mHandleX->SetEntireSelectAvailableFlag(false);


	//mHandleY.SetActiveFlag(false);
	mHandleY->SetEntireDrawFlag(false);
	mHandleY->SetEntireSelectAvailableFlag(false);

	//mHandleZ.SetActiveFlag(false);
	mHandleZ->SetEntireDrawFlag(false);
	mHandleZ->SetEntireSelectAvailableFlag(false);







	AddChildObject(mHandleX);
	AddChildObject(mHandleY);
	AddChildObject(mHandleZ);












	//rotation Handle
//	mRotationHandleX->SetSystem(GetSystem());
//	mRotationHandleY->SetSystem(GetSystem());
//	mRotationHandleZ->SetSystem(GetSystem());

//	mRotationHandleX->Initialize(EAxisType::eX, this);
	//mRotationHandleY->Initialize(EAxisType::eY, this);
	//->Initialize(EAxisType::eZ, this);
//
	mRotationHandleX->SetScale(2.5f);
	mRotationHandleY->SetScale(2.5f);
	mRotationHandleZ->SetScale(2.5f);

	mRotationHandleX->GetTransform().SetRotationLocal({ -90,0,-90.0f });
	mRotationHandleY->GetTransform().SetRotationLocal({ 0,0,0.0f });
	mRotationHandleZ->GetTransform().SetRotationLocal({ -90.0F,0,0.0f });



	//mRotationHandleX.SetActiveFlag(false);
	mRotationHandleX->SetEntireDrawFlag(false);
	mRotationHandleX->SetEntireSelectAvailableFlag(false);

	//mRotationHandleY.SetActiveFlag(false);
	mRotationHandleY->SetEntireDrawFlag(false);
	mRotationHandleY->SetEntireSelectAvailableFlag(false);

	//mRotationHandleZ.SetActiveFlag(false);
	mRotationHandleZ->SetEntireDrawFlag(false);
	mRotationHandleZ->SetEntireSelectAvailableFlag(false);








	AddChildObject(mRotationHandleX);
	AddChildObject(mRotationHandleY);
	AddChildObject(mRotationHandleZ);



	//scale handle
//	mScaleHandleX->SetSystem(GetSystem());
//	mScaleHandleY->SetSystem(GetSystem());
//	mScaleHandleZ->SetSystem(GetSystem());



	//mScaleHandleX->SetSystem(GetSystem());
	//mScaleHandleY->SetSystem(GetSystem());
	//mScaleHandleZ->SetSystem(GetSystem());

//	mScaleHandleX->Initialize(EAxisType::eX, this);
//	mScaleHandleY->Initialize(EAxisType::eY, this);
//	mScaleHandleZ->Initialize(EAxisType::eZ, this);

	//mScaleHandleX.SetScale(2.5f);
	//mScaleHandleY.SetScale(2.5f);
	//mScaleHandleZ.SetScale(2.5f);

	mScaleHandleX->GetTransform().SetRotationLocal({ 0,0,0. - 90.0f });
	mScaleHandleY->GetTransform().SetRotationLocal({ 0,0,0.0f });
	mScaleHandleZ->GetTransform().SetRotationLocal({ 90.0F,0,0.0f });



	//mRotationHandleX.SetActiveFlag(false);
	mScaleHandleX->SetEntireDrawFlag(false);
	mScaleHandleX->SetEntireSelectAvailableFlag(false);

	//mRotationHandleY.SetActiveFlag(false);
	mScaleHandleY->SetEntireDrawFlag(false);
	mScaleHandleY->SetEntireSelectAvailableFlag(false);

	//mRotationHandleZ.SetActiveFlag(false);
	mScaleHandleZ->SetEntireDrawFlag(false);
	mScaleHandleZ->SetEntireSelectAvailableFlag(false);


	AddChildObject(mScaleHandleX);
	AddChildObject(mScaleHandleY);
	AddChildObject(mScaleHandleZ);



	GetTransform().SetPositionLocal({ 0,0,0.0f });

	mDefaultSize = 1;







}
