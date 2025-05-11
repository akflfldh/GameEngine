#include "Map/Map.h"

#include"Object/Camera/Camera.h"

#include"Collision/CollisionWorld.h"
#include"Collision/UiCollisionWorld.h"
#include"Object/Entity.h"
#include"Object/Light/Light.h"
#include"Object/Line/LineBase.h"
//#include"Gizmo.h"

#include"Application.h"

#include"Object/Camera/FrustumCamera.h"
#include"Object/Camera/OrthogoanlCamera.h"
#include"ObjectFactory/ObjectFactory.h"
#include"ObjectFactory/RuntimeObjectFactory.h"
#include"VectorSpace.h"

#include"Parser/JsonParser.h"
#include"SpacePartitioningStructureFactory.h"
#include"Component/ColliderComponent.h"
#include"ObjectManager/RuntimeObjectManager.h"

#include"CollisionWorldFactory.h"
#include"Component/UiColliderComponent.h"

#include"Object/Camera/CameraHelper.h"


#include"RegisterAnimStateTransitionCallbackClassManager.h"

#include<sstream>
#include"RegisterAnimStateTransitionCallbackClassManager.h"

namespace Quad
{

	Quad::Map::Map()
		:mObjectManager(nullptr), mAnimationUpdateSystem(new AnimationUpdateSystem), mRegisterAnimStateTransitionCallbackClassManager(new RegisterAnimStateTransitionCallbackClassManager(this))

	{
		

	}

	Quad::Map::~Map()
	{						
		delete mRegisterAnimStateTransitionCallbackClassManager;
	}


	void Map::PrintObjectName()
	{

		OutputDebugStringA("ssssssssssssssssssssssssssssss\n");
		for (auto& element : mObjectVector)
		{
			std::stringstream ss;
			ss << element->GetName() << "\n";
			OutputDebugStringA(ss.str().c_str());
		}
		OutputDebugStringA("sssssssssssssssssssssssssssssss\n");

	}

	void Map::Initialize(System* system, bool playMode, DirectX::XMFLOAT3 backgroundColor)
	{

		mSystem = system;

		mDirectionalLightVector.resize(10);

		mRenderMapItem.mBackgroundColor = backgroundColor;

		mRuntimeMapFlag = playMode;
	//	mEngineSceneGraph.Initialize();
		//mCurrentSceneGraph = mInitSceneGraph;
		//mCurrentCollsionWorld = mInitCollisionWorld;


		mMapLayerVector.reserve(10);
		mEditSceneGraph.Initialize(mObjectManager, mRuntimeMapFlag);

	}

	void Map::Start()
	{
		//mEditSceneGraph.Start();
		mRegisterAnimStateTransitionCallbackClassManager->Start();



	}

	void Map::Update(float deltaTime)
	{

		//mStartUpdateSystem->Start();

		for (auto objectElement : mStartObjectVector)
		{
			objectElement->mStartObjectFlag = false;
		}
		mStartObjectVector.clear();

		ObjectUpdate(deltaTime);


		mEditSceneGraph.Update(deltaTime);

		mAnimationUpdateSystem->Update(deltaTime);

	}

	void Map::EndUpdate(float deltaTime)
	{
		for (auto object : mObjectVector)
		{
			object->EndUpdate(deltaTime);
		}


	}

	void Map::ObjectUpdate(float deltaTime)
	{

		for (auto object : mObjectVector)
		{

			if (object->GetStartObjectFlag() == false)
				object->Update(deltaTime);
		}
	}


	void Map::CreateDefaultMapLayer()
	{
		//default map layer
		float clientWidth = mSystem->GetClientWidth();
		//float clientHeight = mSystem->GetWindow()->GetClientHeight();
		float clientHeight = mSystem->GetClientHeight();
		



		//default UI

		VectorSpace<UiCollider>* mainSpacePartitionStructureUi = static_cast<VectorSpace<UiCollider>*>(SpacePartitioningStructureFactory<UiCollider>::CreateSpacePartitioningStructure("VectorSpace"));
		mainSpacePartitionStructureUi->Initialize(10000);
		//CollisionWorld* mainCollisionWorld = new CollisionWorld(mainSpacePartitionStructure);
		UiCollisionWorld* mainCollisionWorldUi = CollisionWorldFactory::CreateUiCollisionWorld(mainSpacePartitionStructureUi);

		mDefaultUiCollisionWorld = mainCollisionWorldUi;

		

		CreateMapLayer(0, 0, nullptr, { 0,0,(float)clientWidth,(float)clientHeight,0.0f,1.0f });


		OrthogoanlCamera* uiMainCamera = OrthogoanlCamera::Create(this, 0, clientWidth, clientHeight);

		SetMainCamera(uiMainCamera, 0);

		//default 3D



		VectorSpace<Collider>* mainSpacePartitionStructure = static_cast<VectorSpace<Collider>*>(SpacePartitioningStructureFactory<Collider>::CreateSpacePartitioningStructure("VectorSpace"));
		mainSpacePartitionStructure->Initialize(10000);
		//CollisionWorld* mainCollisionWorld = new CollisionWorld(mainSpacePartitionStructure);
		CollisionWorld* mainCollisionWorld = CollisionWorldFactory::CreateCollisionWorld(mainSpacePartitionStructure);


		mainCollisionWorld->Initialize();
		mDefault3DCollisionWorld = mainCollisionWorld;



		CreateMapLayer(1, 0, nullptr, { 0,0,(float)clientWidth,(float)clientHeight,0.0f,1.0f });

	//	FrustumCamera* mainCamera = static_cast<FrustumCamera*>(mObjectManager->CreateObject("FrustumCamera", mName + "_MainCamera"));
		FrustumCamera* mainCamera = FrustumCamera::Create(this, 1, DirectX::XM_PI / 2, clientWidth / clientHeight);
		
		SetMainCamera(mainCamera,1 );




	}

	void Quad::Map::OnResize(UINT clientWidth, UINT clientHeight)
	{

		/*for (int i = 0; i < mMapLayerVector.size(); ++i)
		{
			if (mMapLayerVector[i].mCamera != nullptr)
			{
				mMapLayerVector[i].mCamera->SetViewWidthAndHeight(clientWidth, clientHeight);

			}
		}*/
	}

	//Gizmo* Map::GetGizmo() const
	//{
	//	return mGizmo;
	//}

	void Map::SetPauseState(bool state)
	{
		mEditSceneGraph.SetPauseState(state);

	}

	bool Map::GetPauseState()
	{
		return mEditSceneGraph.GetPauseState();
	}

	bool Map::AddObjectToAnimationUpdateSystem(Object* object)
	{

		mAnimationUpdateSystem->AddObject(object);
		return true;
	}

	bool Map::RemoveObjectInAnimationUpdateSystem(Object* object)
	{
		mAnimationUpdateSystem->RemoveObject(object);

		return true;
	}

	void Map::SetDefault3DCollisionWorld(Quad::CollisionWorld * collisionWorld)
	{ 
		mDefault3DCollisionWorld = collisionWorld;
	}

	void Map::SetDefaultUiCollisionWorld(UiCollisionWorld* collisionWorld)
	{
		mDefaultUiCollisionWorld = collisionWorld;

	}

	void Map::SetViewportAutoFlag(bool flag, int mapLayerIndex)
	{
		if (mMapLayerOptionVector.size() <= mapLayerIndex)
			return;

		mMapLayerOptionVector[mapLayerIndex].mViewportAutoFlag = flag;

	}

	const bool Map::GetViewportAutoFlag(int mapLayerIndex) const
	{

		if (mMapLayerOptionVector.size() <= mapLayerIndex)
			assert("GetViewportAutoFlag 인덱스 범위 에러\n");


		return mMapLayerOptionVector[mapLayerIndex].mViewportAutoFlag;
	}

	/*void Map::SetMainUserCamera()
	{
		for(int i=0; i<mMapLayerVector.size(); ++i)
		{
			mMapLayerVector[i].mCamera = mUserMainCamera[i];
		}
	}

	void Map::SetMainEngineCamera()
	{
		for (int i = 0; i < mMapLayerVector.size(); ++i)
		{
			mMapLayerVector[i].mCamera = mEngineMainCamera[i];
		}
	}*/

	Quad::RegisterAnimStateTransitionCallbackClassManager* Map::GetRegisterAnimStateTransitionCallbackClassManager() const
	{
		return mRegisterAnimStateTransitionCallbackClassManager;
	}

	void Map::SetRuntimeMapFlag(bool flag)
	{
		mRuntimeMapFlag = flag;
	}

	void Quad::Map::AddObject(Object* object, int mapLayerID )
	{
	

		//scene graph Add
		if (mEditSceneGraph.Add(object) == false)
			return; //?대? ?ㅼ뼱媛??ㅻ툕?앺듃.


		if (object->GetObjectType() == EObjectType::eUiEntity || object->GetObjectType()==EObjectType::eSpline)
		{//ui
			object->SetMapLayer(0);
		}
		else
		{//3d
			object->SetMapLayer(1);

			//object->SetMapLayer(mapLayerID);
		}

		//coliiderWorld




		std::queue<Object*> objectQueue;
		objectQueue.push(object);
		
		while (!objectQueue.empty())
		{
			Object * object =objectQueue.front();
			objectQueue.pop();

			switch (object->GetObjectType())
			{
			case EObjectType::eEntity:

			{
				Entity* entity = (Entity*)object;
				Collider* collider = entity->GetModel()->GetColliderComponent()->GetCollider();
				if (collider != nullptr)
					mDefault3DCollisionWorld->AddCollider(collider);
			}
			break;

			case EObjectType::eLight:
			{
				Light* light = (Light*)object;
				if (light->GetLightType() == ELightType::eDirect)
				{
					mDirectionalLightVector[mapLayerID].push_back(light);
				}

			}
			break;

			case EObjectType::eLine:
			{
				ColliderComponent * colliderComponent = object->GetModel()->GetColliderComponent();
				if (colliderComponent != nullptr)
				{
					mDefault3DCollisionWorld->AddCollider(colliderComponent->GetCollider());
					//	mLineBaseVector.push_back((LineBase*)object);
				}
			}
			break;
			case EObjectType::eUiEntity:
			case EObjectType::eSpline:
			{
				UiColliderComponent* uiColliderComponent = object->GetModel()->GetUiColliderComponent();
				if (uiColliderComponent != nullptr)
				{
					mDefaultUiCollisionWorld->AddCollider(uiColliderComponent->GetCollider());
					//	mLineBaseVector.push_back((LineBase*)object);
				}
			}
			break;
			}



			const std::vector<ObjectSmartPointer>  & childObjectVector = object->GetChildObjectVector();

			for_each(childObjectVector.begin(), childObjectVector.end(), [&objectQueue](const ObjectSmartPointer & object)
				{  objectQueue.push(object.GetPointer()); });

		}






	




		object->mMap = this;





	}

	bool Map::RemoveObject(Object* object)
	{

		mEditSceneGraph.RemoveObject(object);


	
		//CollisionWorld * currentCollisionWorld = mMapLayerVector[object->GetMapLayerID()].mCollisionWorld;

	

		//	mSceneGraph
	//	mEditSceneGraph.RemoveObject(object);
		


		Collider* collider = nullptr;
		UiCollider* uiCollider = nullptr;
		EObjectType objectType = object->GetObjectType();
		switch (objectType)
		{
		case EObjectType::eEntity:
		{
			collider = ((Entity*)object)->GetModel()->GetColliderComponent()->GetCollider();
		}
			break;
		case EObjectType::eCamera:
		{

			collider = ((Camera*)object)->GetModel()->GetColliderComponent()->GetCollider();
		}
			break;
		case EObjectType::eLight:
		{
			//collider = ((Light*)object)->GetCollider();
		}
			break;
		case EObjectType::eLine:
		{
			ColliderComponent* colliderComponent = object->GetModel()->GetColliderComponent();
			if (colliderComponent)
				collider = colliderComponent->GetCollider();
		}
			break;
		case EObjectType::eUiEntity:
		case EObjectType::eSpline:
		{
			UiColliderComponent* colliderComponent = object->GetModel()->GetUiColliderComponent();
			if (colliderComponent)
				uiCollider = colliderComponent->GetCollider();
		}
			break;

		}




		//if (collider != nullptr)
		//{
			if (uiCollider  &&  (objectType == EObjectType::eUiEntity || objectType ==EObjectType::eSpline) )
			{
				mDefaultUiCollisionWorld->RemoveCollider(uiCollider);
			}
			else if(collider)
			{
				mDefault3DCollisionWorld->RemoveCollider(collider);

			}

		//}

		auto objectIt = std::find(mObjectVector.begin(), mObjectVector.end(), object);
		if (objectIt != mObjectVector.end())
		{
			mObjectVector.erase(objectIt);
		}
		



		AnimationComponent* animComponent = object->GetModel()->GetAnimationComponent();
		if (animComponent)
		{
			mRegisterAnimStateTransitionCallbackClassManager->Release(animComponent);
		}

		return true;
	}

	bool Map::RequestRemoveObject(Object* object)
	{

		if (object == nullptr)
			return false;

		if (object->GetKilledState())
			return false;	//이미 제거플래그가 켜진상태


		std::stack<Object*> objectStack;	//자식이 맨위에 존재하도록하여 자식부터 삭제요청이되도록한다.
		std::queue<Object*> objectQueue;	//stack에 올바르게 object들을 넣기위해 필요한 보조queue

		objectQueue.push(object);
		Object* parentObject = nullptr;
		while (!objectQueue.empty())
		{
			parentObject = objectQueue.front();
			objectQueue.pop();

			objectStack.push(parentObject);

			Object* childObject = nullptr;
			for (auto childObjectPtr : parentObject->GetChildObjectVector())
			{
				childObject = childObjectPtr.GetPointer();
				if(childObject!=nullptr)
					objectQueue.push(childObject);
			}
		}


		while(!objectStack.empty())
		{
			Object* removeObject = objectStack.top();
			objectStack.pop();

			removeObject->BeforeRemoveBehavior();
			removeObject->SetKilledState(true);
			mObjectManager->KillObject(removeObject);
		}






	//	object->BeforeRemoveBehavior();
	//	object->SetKilledState(true);
		
	//	mObjectManager->KillObject(object);
		
		return true;
	}



	void Quad::Map::ChangeParentObject(Object* newParentObject, Object* childObject)
	{
		mEditSceneGraph.ChangeParent(newParentObject, childObject);
	}

	void Map::DeleteObject(Object* object)
	{
		//mEditSceneGraph.RemoveObject(object);


	}

	Object* Map::FindObject(const std::string& name) const
	{

		Object * object = mObjectManager->GetObjectFromName(name);

		return object;
	}

	Object* Map::FindObject(unsigned long long id) const
	{
		auto objectIt = mObjectIDTable.find(id);
		if (objectIt != mObjectIDTable.end())
			return objectIt->second;

		return nullptr;

	}

	void Map::SetChildObject(Object* parentObject, Object* childObject)
	{

		mEditSceneGraph.ChangeParent(parentObject, childObject);

	}

	

	const SceneGraph* Quad::Map::GetSceneGraph() const
	{
		return &mEditSceneGraph;
	}

	SceneGraph* Map::GetSceneGraph()
	{
		return &mEditSceneGraph;
	}


	const CollisionWorld* Map::GetCollisionWorld() const
	{
		return mDefault3DCollisionWorld;

	}

	CollisionWorld* Map::GetCollisionWorld()
	{
		return  mDefault3DCollisionWorld;
	}

	bool Map::RayCastingFirst(Ray& ray, Object*& oObject)
	{

		bool ret =mDefaultUiCollisionWorld->RayCastingFirst(oObject,ray);

		if (!ret)
		{
			mDefault3DCollisionWorld->RayCastingFirst(oObject, ray);
		}

		return ret;
	}

	bool Map::RayCastingFirst(const DirectX::XMFLOAT2& screenPos, Object*& oObject)
	{

		//ray를 각 환경에맞게 world공간으로 변환후 넘긴다.
		
		const MapLayer& mapLayerUi = mMapLayerVector[0];
		const MapLayer& mapLayer3D = mMapLayerVector[1];
		bool ret = false;
		if (mapLayerUi.mCamera)
		{
			Ray ray;
			CameraHelper::CalculateRay(screenPos, mapLayerUi.mViewPort, *mapLayerUi.mCamera, true, ray);

			ret = mDefaultUiCollisionWorld->RayCastingFirst(oObject, ray);
		}


		if (!ret)
		{
			if (mapLayer3D.mCamera)
			{
				Ray ray;
				CameraHelper::CalculateRay(screenPos, mapLayer3D.mViewPort, *mapLayer3D.mCamera, true, ray);
				ret = mDefault3DCollisionWorld->RayCastingFirst(oObject, ray);
			}
		}

		return ret;
	}

	void Quad::Map::SetName(const std::string& name)
	{
		mName = name;
	}

	const std::string& Map::GetName() const
	{
		return mName;
		// TODO: 여기에 return 문을 삽입합니다.
	}



	RenderMapItem Quad::Map::GetRenderMapItem() const
	{
		return mRenderMapItem;
	}



	const std::vector<MapLayer> Map::GetObjectsInViewFrustum() 
	{
		std::vector<MapLayer> oMapLayerVector;

		for (int i = 0; i < mMapLayerVector.size(); ++i)
		{
			mMapLayerVector[i].mRenderObjectVector.clear();
		}

		{
			//ui  - 0
			MapLayer& currentUiMapLayer = mMapLayerVector[0];

			std::vector<Object*>& objectVector = currentUiMapLayer.mRenderObjectVector;
			std::vector<Light*>& directionalLightVector = mDirectionalLightVector[0];


			for (int i = 0; i < directionalLightVector.size(); ++i)
			{
				objectVector.push_back(directionalLightVector[i]);
			}

			Camera* currentMainCamera = currentUiMapLayer.mCamera;
			if (currentMainCamera != nullptr)
			{
				//mDefaultUiCollisionWorld->FrsutumCullingObjects(currentUiMapLayer.mRenderObjectVector, currentMainCamera);
				mDefaultUiCollisionWorld->FrsutumCullingObjects(currentUiMapLayer.mRenderObjectVector, currentMainCamera);

				if (currentUiMapLayer.mRenderObjectVector.size() != 0)
				{
					oMapLayerVector.push_back(std::move(currentUiMapLayer));

				}

			}


		}


		//3d - 1
		{
			MapLayer& current3DMapLayer = mMapLayerVector[1];

			std::vector<Object*>& objectVector = current3DMapLayer.mRenderObjectVector;
			std::vector<Light*>& directionalLightVector = mDirectionalLightVector[1];


			for (int i = 0; i < directionalLightVector.size(); ++i)
			{
				objectVector.push_back(directionalLightVector[i]);
			}

			Camera* currentMainCamera = current3DMapLayer.mCamera;
			if (currentMainCamera != nullptr)
			{
				mDefault3DCollisionWorld->FrsutumCullingObjects(current3DMapLayer.mRenderObjectVector, currentMainCamera);
				if (current3DMapLayer.mRenderObjectVector.size() != 0)
				{
					oMapLayerVector.push_back(std::move(current3DMapLayer));

				}

			}

		}




		//for (int i = 0; i < mMapLayerVector.size(); ++i)
		//{
		//	MapLayer& currentMapLayer = mMapLayerVector[i];
		//
		//	std::vector<Object*>& objectVector = currentMapLayer.mRenderObjectVector;
		//	std::vector<Light*>& directionalLightVector = mDirectionalLightVector[i];
		//

		//	for (int i = 0; i < directionalLightVector.size(); ++i)
		//	{
		//		objectVector.push_back(directionalLightVector[i]);
		//	}

		//	Camera* currentMainCamera = currentMapLayer.mCamera;
		//

		//	
		//	CollisionWorld* currentCollisionWorld = currentMapLayer.mCollisionWorld;
		//	if (currentCollisionWorld != nullptr && currentMapLayer.mCamera !=nullptr)
		//	{
		//		currentCollisionWorld->FrsutumCullingObjects(currentMapLayer.mRenderObjectVector, currentMainCamera);
		//	}
		//}

		return oMapLayerVector;

	}


	Camera* Quad::Map::GetMainCamera(int MapLayerID) const
	{
		return mMapLayerVector[MapLayerID].mCamera;
	}

	void Map::SetViewPort(FLOAT topLeftX, FLOAT topLeftY, FLOAT width, FLOAT height, FLOAT minDepth, FLOAT maxDepth , int mapLayer )
	{

		D3D12_VIEWPORT& viewport = mMapLayerVector[mapLayer].mViewPort;

		viewport.TopLeftX = topLeftX;
		viewport.TopLeftY = topLeftY;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = minDepth;
		viewport.MaxDepth = maxDepth;
	
		//mRunTimeMapLayerVector[mapLayer].mViewPort = viewport;
	

		//GetGameCamera()->SetAspect(mViewPort.Width / mViewPort.Height);
	}

	D3D12_VIEWPORT Map::GetViewPort(int mapLayer) const
	{
		return mMapLayerVector[mapLayer].mViewPort;
		//return mViewPort;
	}


	System * Map::GetSystem() const
	{
		return mSystem;
	}

	Quad::BaseObjectManager* Map::GetObjectManager() const
	{
		return mObjectManager;
	}

	void Map::CreateMapLayer(int id, int depthPriority ,Camera * camera,D3D12_VIEWPORT viewport, RenderTargetTexture * texture,Texture * depthStencilBuffer)
	{

		MapLayer mapLayer;
		mapLayer.mID = id;
		mapLayer.mDepthPriority = depthPriority;
		mapLayer.mCamera = camera;
		//mapLayer.mCollisionWorld = initCollisionWorld;


		mapLayer.mViewPort = viewport;
		mapLayer.mRenderTarget = texture;
		mapLayer.mDepthStencilBuffer = depthStencilBuffer;
		mMapLayerVector.push_back(mapLayer);

		mMapLayerOptionVector.push_back(MapLayerOption{});


	}

	MapLayer& Map::GetMapLayer(int id)
	{

		return mMapLayerVector[id];
		// TODO: 占쏙옙占썩에 return 占쏙옙占쏙옙 占쏙옙占쏙옙占쌌니댐옙.
	}

	const MapLayer& Map::GetMapLayer(int id) const
	{
		return mMapLayerVector[id];
		// TODO: 占쏙옙占썩에 return 占쏙옙占쏙옙 占쏙옙占쏙옙占쌌니댐옙.
	}

	void Map::SetMainCamera(Camera* camera,int mapLayerIndex)
	{
		//mMapLayerVector[mapLayerIndex].mPlayCamera = camera; 

		//이것이 기본동작(게임프로그램에대해)
		mMapLayerVector[mapLayerIndex].mCamera = camera;
//
//#ifdef EditorMode
//		if (GetRuntimeMapFlag())
//		{
//			mMapLayerVector[mapLayerIndex].mCamera = camera;
//		}
//		else
//		{
//			mUserMainCamera[mapLayerIndex] = camera;
//		}
//
//#else
//
//		mMapLayerVector[mapLayerIndex].mCamera = camera;
//
//#endif
		
		
	}

	void Map::SetMainCamera(unsigned long long id, int mapLayerIndex)
	{

		
		Object* camera = FindObject(id);
		if (camera->GetObjectType() != EObjectType::eCamera)
		{
			return;
		}

		SetMainCamera(static_cast<Camera*>(camera), mapLayerIndex);

	}

	void Map::Reset()
	{

		CollisionWorldFactory::RemoveCollisionWorld(mDefault3DCollisionWorld);
		CollisionWorldFactory::RemoveUiCollisionWorld(mDefaultUiCollisionWorld);
		mDefault3DCollisionWorld = nullptr;
		mDefaultUiCollisionWorld = nullptr;


	//	const std::unordered_map<unsigned long long, Object*>& ObjectIDTable = mEditSceneGraph.GetObjectIDTable();


		mMapLayerVector.clear();
		



		mRegisterAnimStateTransitionCallbackClassManager->Clear();
		for (auto& element : mObjectVector)
		{
		
			mObjectManager->ReleaseObject(element);
		} 

		mEditSceneGraph.Reset();

		mAnimationUpdateSystem->Reset();
		



		
		mObjectVector.clear();
		mStartObjectVector.clear();
		mObjectIDTable.clear();
		for(auto & directonalLightLayerVector : mDirectionalLightVector)
			directonalLightLayerVector.clear();
	}

	void Map::Serialize()
	{
		//map layer seralize();

		JsonParser::StartWrite();


		//scene graph seralize();
		//const std::unordered_map<unsigned long long, Object*>& objectIDTable = mEditSceneGraph.GetObjectIDTable();



		//unsigned long long objectNum = objectIDTable.size();
		unsigned long long objectNum = mObjectVector.size();
		for (auto& element : mObjectVector)
		{
			if (element->GetEnginObjectFlag() == true)
				objectNum--;
		}







		JsonParser::StartWriteObject();
		JsonParser::Write("ObjectNum", objectNum);


		for (auto element : mObjectVector)
		{
			if (element->GetEnginObjectFlag() != true)
			{
				JsonParser::StartWriteObject();
				element->Serialize();
			}
		}


		JsonParser::StartWriteObject();
		JsonParser::Write("MapLayerNum", mMapLayerVector.size());

		for (auto& element : mMapLayerOptionVector)
		{
			JsonParser::StartWriteObject();
			JsonParser::Write("MapLayer_OptionViewportAutoFlag", element.mViewportAutoFlag);
		}

		

		for (auto& element : mMapLayerVector)
		{
			JsonParser::StartWriteObject();
			JsonParser::Write("MapLayer_ID", element.mID);
			JsonParser::Write("MapLayer_DepthPriority", element.mDepthPriority);
			JsonParser::Write("MapLayer_Width", element.mViewPort.Width);
			JsonParser::Write("MapLayer_Height", element.mViewPort.Height);
			JsonParser::Write("MapLayer_TopLeftX", element.mViewPort.TopLeftX);
			JsonParser::Write("MapLayer_TopLeftY", element.mViewPort.TopLeftY);
			JsonParser::Write("MapLayer_MinDepth", element.mViewPort.MinDepth);
			JsonParser::Write("MapLayer_MaxDepth", element.mViewPort.MaxDepth);
			

			unsigned long long cameraID = 0;
			if (element.mCamera != nullptr)
				cameraID = element.mCamera->GetUniqueID();
			JsonParser::Write("MapLayer_CameraID", cameraID);
		}
		JsonParser::Write("MapLayer_DefaultUiCollisionWorldClassName", mDefaultUiCollisionWorld->GetSpacePartitioningClassName());
		JsonParser::Write("MapLayer_Default3DCollisionWorldClassName", mDefault3DCollisionWorld->GetSpacePartitioningClassName());


		mRegisterAnimStateTransitionCallbackClassManager->Serialize();


	}

	void Map::DeSerialize()
	{
	



		JsonParser::ReadStart();

		unsigned int currentObjectIndex = 0;


		unsigned long long objectNum = 0;
		JsonParser::Read("ObjectNum", objectNum);
		currentObjectIndex++;


		mObjectVector.reserve(objectNum);
		std::vector<Object*> objectVector(objectNum,nullptr);

		if (objectNum != 3)
		{
			int a = 2;
		}
		for (unsigned long long i = 0; i < objectNum; ++i)
		{
			JsonParser::SetCurrentIndex(currentObjectIndex);
			
			std::string className;
			std::string objectName;
			unsigned long long id = 0;
			JsonParser::Read("ClassName", className);
			JsonParser::Read("Object_Name", objectName);
			JsonParser::Read("Object_ID", id);



			
			Object * object = mObjectManager->CreateObjectFromFile(className, objectName, id);






			object->SetSystem(GetSystem());
			object->Initialize();
			mObjectVector.push_back(object);

			currentObjectIndex++;
		}

		currentObjectIndex = 1;
		for (auto& element : mObjectVector)
		{
			JsonParser::SetCurrentIndex(currentObjectIndex);
			
			element->DeSerialize();
			element->mStartObjectFlag = false;	//역직렬화(로드)되는 오브젝트들은 다음프레임까지 기다릴필요없이 바로 update된다.
			currentObjectIndex++;
		}
		


		JsonParser::SetCurrentIndex(currentObjectIndex++);
		int mapLayerNum = 0;
		JsonParser::Read("MapLayerNum", mapLayerNum);
		mMapLayerVector.resize(mapLayerNum);
		mMapLayerOptionVector.resize(mapLayerNum);


		for (auto& element : mMapLayerOptionVector)
		{
			JsonParser::SetCurrentIndex(currentObjectIndex++);
			element.mViewportAutoFlag = JsonParser::ReadBool("MapLayer_OptionViewportAutoFlag");

		}
		


		for (auto& element : mMapLayerVector)
		{
			JsonParser::SetCurrentIndex(currentObjectIndex++);

			JsonParser::Read("MapLayer_ID", element.mID);
			JsonParser::Read("MapLayer_DepthPriority", element.mDepthPriority);
			JsonParser::Read("MapLayer_Width", element.mViewPort.Width);
			JsonParser::Read("MapLayer_Height", element.mViewPort.Height);
			JsonParser::Read("MapLayer_TopLeftX", element.mViewPort.TopLeftX);
			JsonParser::Read("MapLayer_TopLeftY", element.mViewPort.TopLeftY);
			JsonParser::Read("MapLayer_MinDepth", element.mViewPort.MinDepth);
			JsonParser::Read("MapLayer_MaxDepth", element.mViewPort.MaxDepth);
			unsigned long long mainCameraID = 0;
			JsonParser::Read("MapLayer_CameraID", mainCameraID);
			Camera* camera = nullptr;
			if(mainCameraID != 0)
				camera = static_cast<Camera*>(mObjectManager->GetObjectFromID(mainCameraID));
			//element.mCamera = camera;
			SetMainCamera(camera, element.mID);
		}
		std::string defaultUiCollisionSpaceClassName;
		std::string default3DCollisionSpaceClassName;
		
		JsonParser::Read("MapLayer_DefaultUiCollisionWorldClassName", defaultUiCollisionSpaceClassName);
		JsonParser::Read("MapLayer_Default3DCollisionWorldClassName", default3DCollisionSpaceClassName);



		SpacePartitioningStructure<UiCollider>* defaultUiSpacePartitioningSstructure = SpacePartitioningStructureFactory< UiCollider>::CreateSpacePartitioningStructure(defaultUiCollisionSpaceClassName);
		mDefaultUiCollisionWorld = CollisionWorldFactory::CreateUiCollisionWorld(defaultUiSpacePartitioningSstructure);
		mDefaultUiCollisionWorld->Initialize();
		

		SpacePartitioningStructure<Collider>* default3DSpacePartitioningSstructure = SpacePartitioningStructureFactory< Collider>::CreateSpacePartitioningStructure(default3DCollisionSpaceClassName);
		mDefault3DCollisionWorld = CollisionWorldFactory::CreateCollisionWorld(default3DSpacePartitioningSstructure);
		mDefault3DCollisionWorld->Initialize();




		for (auto& element : mObjectVector)
		{
			AddObject(element,element->GetMapLayerID());

			AnimationComponent* animComponent = element->GetModel()->GetAnimationComponent();
			if (animComponent!=nullptr && animComponent->GetAnimationComponentState())
			{
				AddObjectToAnimationUpdateSystem(element);
			}
			//element->Start();

		}


		mRegisterAnimStateTransitionCallbackClassManager->DeSerialize();
		//RegisterAnimStateTransitionCallbackClassManager::Start();


	}

	Quad::Object* Map::CreateObject(const std::string& className, int mapLayerIndex)
	{
		
		unsigned long long  num = 1;
		std::string objectName = className + std::to_string(num);

		Object* object = nullptr;
		if (GetRuntimeMapFlag())
		{

			
			while (1)
			{
				object = mObjectManager->CreateObject(className, objectName);
				if (object != nullptr)
					break;
				num++;
				objectName = className + std::to_string(num);
			}

		

		}
		else
		{
			while (1)
			{
				object = mObjectManager->CreateObject(className, objectName);
				if (object != nullptr)
					break;
				num++;
				objectName = className + std::to_string(num);
			}
		}


			
		object->mMap = this;
		object->SetName(objectName);
		object->SetSystem(GetSystem());
		AddObject(object, mapLayerIndex);
		
		object->Initialize();
		object->Start();
		
		mStartObjectVector.push_back(object);
		mObjectVector.push_back(object);
		mObjectIDTable[object->GetUniqueID()] = object;




		return object;


	}

	int Map::GetRuntimeMapFlag() const
	{
		return mRuntimeMapFlag;
	}

	void Map::RemoveCollider(Collider* collider, int mapLayerIndex)
	{

		mDefault3DCollisionWorld->RemoveCollider(collider);

		//mMapLayerVector[mapLayerIndex].mCollisionWorld->RemoveCollider(collider);
	}

	void Map::AddCollider(Collider* collider, int mapLayerIndex)
	{

		mDefault3DCollisionWorld->AddCollider(collider);

	//	mMapLayerVector[mapLayerIndex].mCollisionWorld->AddCollider(collider);

	}

	bool Map::ChangeObjectName(Object* object, const std::string& newName)
	{
		if (object == nullptr)
			return false;

		return mObjectManager->ChangeObjectName(object->GetUniqueID(), object->GetName(), newName);
		
	}

}