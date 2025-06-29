//#include "EditorMap.h"
//#include"Gizmo.h"
//
//#include"Collision/CollisionWorld.h"
//#include<Component/ColliderBaseComponent.h>
//
//#include<ObjectManager/BaseObjectManager.h>
//#include"Object/Camera/Camera.h"
//
//#include"EditorCollisionWorld.h"
//#include"EditorUiCollisionWorld.h"
//
//#include"Object/Object.h"
//
//#include"Camera/CameraHelper.h"
//
//#include"Collision/Collider.h"
//#include"Collision/BaseCollider.h"
//
//#include"ObjectEditorComponentTableSystem.h"
//#include"EditorCoreComponent.h"
//
//
//
//Quad::EditorMap::EditorMap()
//	:mGizmo(nullptr), mEngineMainCamera(10,nullptr),mUserMainCamera(10,nullptr)
//{
//}
//
//Quad::EditorMap::~EditorMap()
//{
//}
//
//void Quad::EditorMap::Update(float deltaTime)
//{
//	//Map::Update(deltaTime);
//	//mAnimationUpdateSystem->Update(deltaTime);
//
//	for (auto objectElement : mStartObjectVector)
//	{
//		objectElement->SetStartObjectFlag(false) ;
//	}
//	mStartObjectVector.clear();
//
//
//	ObjectUpdate(deltaTime);
//
//	mEditSceneGraph.Update(deltaTime);
//
//
//	//mAnimationUpdateSystem->Update(deltaTime);
//}
//
//void Quad::EditorMap::AddGizmo(Gizmo* gizmo, int mapLayerIndex)
//{
//
//
//	//currentCollisionWorld->AddGizmoCollider(collider);
//	//currentCollisionWorld = static_cast<EditorCollisionWorld*>(mDefault3DCollisionWorld);
//
//	
//
//
//}
//
//Quad::Gizmo* Quad::EditorMap::GetGizmo() const
//{
//	return mGizmo;
//}
//
//void Quad::EditorMap::Serialize()
//{
//	//먼저 map의 카메라를  editor  카메라에서 user 카메라로 전환해준다 .
//	SetMainUserCamera();
//
//	Map::Serialize();
//
//	SetMainEngineCamera();
//
//
//
//
//
//}
//
//
//void Quad::EditorMap::DeSerialize()
//{
//
//	Map::DeSerialize();
//
//	//SetMainEngineCamera();
//
//}
//
//
//void Quad::EditorMap::SetMainUserCamera()
//{
//	for (int i = 0; i < mMapLayerVector.size(); ++i)
//	{
//		mMapLayerVector[i].mCamera = mUserMainCamera[i];
//	}
//}
//
//void Quad::EditorMap::SetMainEngineCamera()
//{
//	for (int i = 0; i < mMapLayerVector.size(); ++i)
//	{
//		mMapLayerVector[i].mCamera = mEngineMainCamera[i];
//	}
//}
//
//void Quad::EditorMap::SetMainCamera(Camera* camera, int mapLayerIndex)
//{
//
//	Map::SetMainCamera(camera, mapLayerIndex);
//	mUserMainCamera[mapLayerIndex] = camera;
//}
//
//void Quad::EditorMap::SetMainCamera(unsigned long long id, int mapLayerIndex)
//{
//	Map::SetMainCamera(id, mapLayerIndex);
//
//	mUserMainCamera[mapLayerIndex] = static_cast<Camera*>(mObjectManager->GetObjectFromID(id));
//
//}
//
////void Quad::EditorMap::SetEditorMainCamera(Camera* camera, int mapLayerIndex)
////{
////
////
////
////	mMapLayerVector[mapLayerIndex].mCamera = camera;
////
////}
//
//void Quad::EditorMap::AddEditorMainCamera(Camera* camera,int mapLayerIndex)
//{
//	mEngineMainCamera[mapLayerIndex]=camera;
//	mMapLayerVector[mapLayerIndex].mCamera = camera;
//
//
//	mEditUpdateObjectVector.push_back(camera);
//
//}
//
//bool Quad::EditorMap::RayCastingFirstNotIncludeEditorObject(Object*& oObject, Ray& ray)
//{
//
//
//
//
//
//
//
//
//
//	return false;
//}
//
//bool Quad::EditorMap::RayCastingFirstNotIncludeEditorObject(const DirectX::XMFLOAT2& screenPos, Object*& oObject)
//{
//
//	//ui 
//	Camera* cameraUi = mMapLayerVector[0].mCamera;
//	bool ret = false;
//	if (cameraUi)
//	{
//		Ray rayUi;
//		CameraHelper::CalculateRay(screenPos, mMapLayerVector[0].mViewPort, *cameraUi, true, rayUi);
//		EditorUiCollisionWorld* editorCollisionWorld = static_cast<EditorUiCollisionWorld*>(mDefaultUiCollisionWorld);
//		ret= editorCollisionWorld->RayCastingFirstNotIncludeEditorObject(oObject, rayUi);
//
//		if (ret == true)
//			return true; 
//	}
//
//
//	//3d
//	Camera* camera3D = mMapLayerVector[1].mCamera;
//	if (camera3D)
//	{
//
//		Ray ray;
//		CameraHelper::CalculateRay(screenPos, mMapLayerVector[1].mViewPort, *camera3D, true, ray);
//
//		EditorCollisionWorld* editorCollisionWorld = static_cast<EditorCollisionWorld*>(mDefault3DCollisionWorld);
//		ret = editorCollisionWorld->RayCastingFirstNotIncludeEditorObject(oObject, ray);
//	}
//
//	return ret;
//}
//
//void Quad::EditorMap::ObjectUpdate(float deltaTime)
//{
//
//	for (auto& editorObject : mEditUpdateObjectVector)
//		editorObject->Update(deltaTime);
//
//
//	//if(GetRuntimeMapFlag())
////	Map::ObjectUpdate(deltaTime);
//}
//
//void Quad::EditorMap::SerializeObject(const std::vector<Object*>& objectVector)
//{
//	
//	auto ObjectEditorComponentTableSystem = ObjectEditorComponentTableSystem::GetInstance();
//
//	unsigned long long objectNum = objectVector.size();
//	std::vector<Object*> userObjectVector;
//	userObjectVector.reserve(objectNum);
//	for (auto element : objectVector)
//	{
//
//		//coreEngienSystem에서 각 coreComponent에 접근, 
//		//coreComponent->GetEngineObjectFlag() 확인
//		//userObjectVector.push_back(element);
//		EditorCoreComponent * editorCoreComponent =	ObjectEditorComponentTableSystem->GetComponent< EditorCoreComponent>(element->GetUniqueID());
//		
//		if (editorCoreComponent->GetEngineObjectFlag() == false)
//		{
//			userObjectVector.push_back(element);
//		}
//	}
//
//	Map::SerializeObject(userObjectVector);
//
//}
//
