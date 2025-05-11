#include "EditorMap.h"
#include"Gizmo.h"

#include"Collision/CollisionWorld.h"
#include"Component/ColliderComponent.h"

#include"ObjectManager/EditGameObjectManager.h"
#include"Object/Camera/Camera.h"

#include"EditorCollisionWorld.h"
#include"EditorUiCollisionWorld.h"

#include"Object/Object.h"

#include"Camera/CameraHelper.h"



Quad::EditorMap::EditorMap()
	:mGizmo(nullptr), mEngineMainCamera(10,nullptr),mUserMainCamera(10,nullptr)
{
}

Quad::EditorMap::~EditorMap()
{
}

void Quad::EditorMap::Update(float deltaTime)
{
	//Map::Update(deltaTime);
	//mAnimationUpdateSystem->Update(deltaTime);

	for (auto objectElement : mStartObjectVector)
	{
		objectElement->SetStartObjectFlag(false) ;
	}
	mStartObjectVector.clear();


	ObjectUpdate(deltaTime);

	mEditSceneGraph.Update(deltaTime);


	//mAnimationUpdateSystem->Update(deltaTime);
}

void Quad::EditorMap::AddGizmo(Gizmo* gizmo, int mapLayerIndex)
{

	SceneGraph* sceneGraph = GetSceneGraph();
	//auto& objectIDTable = sceneGraph->GetObjectIDTable();
	
	MapLayer & mapLayer = GetMapLayer(mapLayerIndex);



	//gizmo의 모든 자식오브젝트들의 id는 0(설정되지않은값) 이고 , 아마 table에 id가 0으로등록되었을것이니 nullptr로 다시설정해준다.
	sceneGraph->AddNewObjectNode(gizmo);
//	objectIDTable.erase(0);

	EditorCollisionWorld* currentCollisionWorld = nullptr;
	//currentCollisionWorld = mMapLayerVector[0].mCollisionWorld;
	//currentCollisionWorld =static_cast<EditorCollisionWorld*>(mapLayer.mCollisionWorld);
	currentCollisionWorld = static_cast<EditorCollisionWorld *>(mDefault3DCollisionWorld);


	std::queue<Object*> objectQueue;
	objectQueue.push(gizmo);

	while (!objectQueue.empty())
	{
		Object* object = objectQueue.front();
		objectQueue.pop();

		switch (object->GetObjectType())
		{
		case EObjectType::eEntity:
		case EObjectType::eLine:
		{
			ColliderComponent* colliderComponent = object->GetModel()->GetColliderComponent();

			if (colliderComponent != nullptr)
				currentCollisionWorld->AddGizmoCollider(colliderComponent->GetCollider());
		}
		break;
		}


		mEditUpdateObjectVector.push_back(object);

		const std::vector<ObjectSmartPointer> childObjectVector = object->GetChildObjectVector();

		for_each(childObjectVector.begin(), childObjectVector.end(), [&objectQueue](const ObjectSmartPointer& objectPtr)
			{  objectQueue.push(objectPtr.GetPointer()); });

	}


	mGizmo = gizmo;



}

Quad::Gizmo* Quad::EditorMap::GetGizmo() const
{
	return mGizmo;
}

void Quad::EditorMap::Serialize()
{
	//먼저 map의 카메라를  editor  카메라에서 user 카메라로 전환해준다 .
	SetMainUserCamera();

	Map::Serialize();

	SetMainEngineCamera();





}


void Quad::EditorMap::DeSerialize()
{

	Map::DeSerialize();

	//SetMainEngineCamera();

}


void Quad::EditorMap::SetMainUserCamera()
{
	for (int i = 0; i < mMapLayerVector.size(); ++i)
	{
		mMapLayerVector[i].mCamera = mUserMainCamera[i];
	}
}

void Quad::EditorMap::SetMainEngineCamera()
{
	for (int i = 0; i < mMapLayerVector.size(); ++i)
	{
		mMapLayerVector[i].mCamera = mEngineMainCamera[i];
	}
}

void Quad::EditorMap::SetMainCamera(Camera* camera, int mapLayerIndex)
{

	Map::SetMainCamera(camera, mapLayerIndex);
	mUserMainCamera[mapLayerIndex] = camera;
}

void Quad::EditorMap::SetMainCamera(unsigned long long id, int mapLayerIndex)
{
	Map::SetMainCamera(id, mapLayerIndex);
	mUserMainCamera[mapLayerIndex] = static_cast<Camera*>(mObjectManager->GetObjectFromID(id));

}

//void Quad::EditorMap::SetEditorMainCamera(Camera* camera, int mapLayerIndex)
//{
//
//
//
//	mMapLayerVector[mapLayerIndex].mCamera = camera;
//
//}

void Quad::EditorMap::AddEditorMainCamera(Camera* camera,int mapLayerIndex)
{
	mEngineMainCamera[mapLayerIndex]=camera;
	mMapLayerVector[mapLayerIndex].mCamera = camera;


	mEditUpdateObjectVector.push_back(camera);

}

bool Quad::EditorMap::RayCastingFirstNotIncludeEditorObject(Object*& oObject, Ray& ray)
{









	return false;
}

bool Quad::EditorMap::RayCastingFirstNotIncludeEditorObject(const DirectX::XMFLOAT2& screenPos, Object*& oObject)
{

	//ui 
	Camera* cameraUi = mMapLayerVector[0].mCamera;
	bool ret = false;
	if (cameraUi)
	{
		Ray rayUi;
		CameraHelper::CalculateRay(screenPos, mMapLayerVector[0].mViewPort, *cameraUi, true, rayUi);
		EditorUiCollisionWorld* editorCollisionWorld = static_cast<EditorUiCollisionWorld*>(mDefaultUiCollisionWorld);
		ret= editorCollisionWorld->RayCastingFirstNotIncludeEditorObject(oObject, rayUi);

		if (ret == true)
			return true; 
	}


	//3d
	Camera* camera3D = mMapLayerVector[1].mCamera;
	if (camera3D)
	{

		Ray ray;
		CameraHelper::CalculateRay(screenPos, mMapLayerVector[1].mViewPort, *camera3D, true, ray);

		EditorCollisionWorld* editorCollisionWorld = static_cast<EditorCollisionWorld*>(mDefault3DCollisionWorld);
		ret = editorCollisionWorld->RayCastingFirstNotIncludeEditorObject(oObject, ray);
	}

	return ret;
}

void Quad::EditorMap::ObjectUpdate(float deltaTime)
{

	for (auto& editorObject : mEditUpdateObjectVector)
		editorObject->Update(deltaTime);


	//if(GetRuntimeMapFlag())
//	Map::ObjectUpdate(deltaTime);
}

