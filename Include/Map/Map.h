#pragma once

#include"Predeclare.h"

#include"Map/SceneGraph.h"

#include"SpacePartitioningStructure.h"
#include"Asset/Texture/RenderTargetTexture.h"
#include<string>
#include"Core/CoreDllExport.h"
#include"Core/AnimationUpdateSystem.h"
#include<unordered_map>



#define EditorMode

namespace Quad
{
	class RegisterAnimStateTransitionCallbackClassManager;

	class Camera;
	
	struct RenderMapItem
	{
		DirectX::XMFLOAT3 mBackgroundColor;
	};
	
	
	struct MapLayer
	{
		int mID = 0;
		int mDepthPriority = 0;

		Camera* mCamera = nullptr;

		//mapLayer의 width, height는 0.0~1.0f까지가 유효하다.
		//system의 클라이언트 영역에대한 비례값으로 표현
		D3D12_VIEWPORT mViewPort = { 0,0,1.0f,1.0f,0,1.0f };

		//전역 값을 가지는 viewport
		D3D12_VIEWPORT mViewPortGlobal = { 0,0,0,0,0,0 };
		 
		std::vector<Object*> mRenderObjectVector;

		//RenderTarget  default = nullptr
		RenderTargetTexture* mRenderTarget = nullptr;
		Texture* mDepthStencilBuffer = nullptr;
	};

	struct MapLayerOption
	{
		bool mViewportAutoFlag = true;

	};





	class CORE_API_LIB Map
	{
		friend class System;
		friend class EditorMapManager;
		/*friend class EditObjectManager;
		friend class RuntimeObjectManager;
		friend class RuntimeGameObjectManager;
		friend class EditGameObjectManager;*/

		friend class BaseObjectManager;

		//friend class Gizmo;
		friend class ProjectDirector;
		friend class GameMapSystem;
		friend class GameMapInstanceGenerator;

		

	public:
		Map();
		~Map();

	/*	void Initialize(SceneGraph* sceneGraph, SpacePartitioningStructure* spacePartitioningStructure,
			UINT clientWidth, UINT clientHeight, Camera* gameCamera,DirectX::XMFLOAT3 backgroundColor = { 0.1f,0.1f,0.6f });*/


		void Initialize(System* system, bool playMode = false, DirectX::XMFLOAT3 backgroundColor = { 0.1f,0.1f,0.6f });


		void Start();

		
		virtual void Update(float deltaTime);
		virtual void EndUpdate(float deltaTime);
		virtual void ObjectUpdate(float deltaTime);



		
		void CreateDefaultMapLayer();
									
		//void Update(float deltaTime, bool gamePlayMode  =true );
		void OnResize(UINT clientWidth, UINT clientHeight);


	
		bool RequestRemoveObject(Object* object);
		//void AddGizmo(Gizmo* gizmo);
		void ChangeParentObject(Object* newParentObject, Object* childObject);
		void DeleteObject(Object* object);
		Object* FindObject(const std::string& name) const;
		Object* FindObject(unsigned long long id) const;

		void SetChildObject(Object* parentObject, Object* childObject);

		//void DeleteObject(const std::string & name);



	//	SpacePartitioningStructure* GetSpaceParitioningStructure() const;
		const CollisionWorld* GetCollisionWorld()const;
		CollisionWorld* GetCollisionWorld();

		UiCollisionWorld* GetUiCollisionWorld() const;

		
		bool RayCastingFirst(Ray& ray, Object*& oObject);
		bool RayCastingFirst(const DirectX::XMFLOAT2 & screenPos, Object*& oObject);



		void SetName(const std::string& name);
		const std::string& GetName() const;

		RenderMapItem GetRenderMapItem() const;
		//void GetObjectVectorInViewFrustum(std::vector<Object*>& oObjectVector);
		virtual const std::vector<MapLayer> GetObjectsInViewFrustum() ; 
		

		Camera* GetMainCamera(int mapLayerIndex =0)const;

		//지역적
		void SetViewPort(FLOAT topLeftX, FLOAT topLeftY, FLOAT widthRate,
			FLOAT heightRate, FLOAT minDepth, FLOAT maxDepth, int mapLayer=0);


		//system viewport를 바탕으로 지역적 viewport값에 따라서 전역값설정
		void SetSystemViewPortAll();

	
		//mapLayer의 viewport 전역을 갱신한다.
		void UpdateViewPortGlobal(int mapLayerIndex);



		//void SetViewPortLayerAll(float topLeftX, float topLeftY, float widthRate, float heightRate, float minDepth, float maxDepth);

		//지역적 비례값
		D3D12_VIEWPORT GetViewPort(int mapLayer=0) const;

		//전역 값 
		D3D12_VIEWPORT GetViewPortGlobal(int mapLayer = 0) const;


		System* GetSystem() const;
		BaseObjectManager* GetObjectManager() const;

		void CreateMapLayer(int mID, int mDepthPriority, Camera* camera, D3D12_VIEWPORT viewport, MapLayerOption mapLayerOption = MapLayerOption{}, RenderTargetTexture* texture= nullptr, Texture* depthStencilBuffer=nullptr);


		MapLayer& GetMapLayer(int index) ;
		const MapLayer& GetMapLayer(int index) const;


		//void SetMainEngineCamera(Camera* camera, int mapLayerIndex = 0);

		virtual void SetMainCamera(Camera* camera,int mapLayerIndex = 0);
		virtual void SetMainCamera(unsigned long long id,int mapLayerIndex = 0);
		



		void Reset();


		virtual void Serialize();
		virtual void DeSerialize();



		Object* CreateObject(const std::string& className,int mapLayerIndex=0);


		
		int GetRuntimeMapFlag() const;


		void RemoveCollider(Collider* collider, int mappLayerIndex = 0 );
		void AddCollider(Collider* collider, int mapLayerIndex = 0);


		bool ChangeObjectName(Object* object, const std::string& newName);



		//Gizmo* GetGizmo() const;


		void SetPauseState(bool state);
		bool GetPauseState();




		bool AddObjectToAnimationUpdateSystem(Object* object);
		bool RemoveObjectInAnimationUpdateSystem(Object* object);


		void SetDefault3DCollisionWorld(CollisionWorld* collisionWorld);
		void SetDefaultUiCollisionWorld(UiCollisionWorld* collisionWorld);


		void SetViewportAutoFlag(bool flag, int mapLayerIndex);
		const bool GetViewportAutoFlag(int mapLayerIndex) const;


		void NotifyResizeMapLayer(int mapLayerIndex);
#ifdef EditorMode
		//void SetMainUserCamera();
	//	void SetMainEngineCamera();
#endif


		RegisterAnimStateTransitionCallbackClassManager* GetRegisterAnimStateTransitionCallbackClassManager() const;




	protected:
		const Scene::SceneGraph* GetSceneGraph()const;
		Scene::SceneGraph* GetSceneGraph();



		void SetRuntimeMapFlag(bool flag);
		void AddObject(Object* object, int mapLayer = 0);
		bool RemoveObject(Object* object);//object manager가 kill상태의 object를 완전히 제거할떄사용




		virtual void SerializeObject(const std::vector<Object*>& objectVector);
		void SerializeMapLayer();

		virtual void DeSerializeObject();
		void DeSerializeMapLayer();








	protected:


		std::string mName;
		Scene::SceneGraph mEditSceneGraph;
		//SceneGraph mEngineSceneGraph;	

		//map layer蹂꾨줈 light?멸굅媛숈?
		std::vector<std::vector<Light*>> mDirectionalLightVector;
		std::vector<LineBase*> mLineBaseVector;


		std::vector<MapLayer> mMapLayerVector;
		std::vector<MapLayerOption> mMapLayerOptionVector;
		RenderMapItem mRenderMapItem;
		System* mSystem;


		int mRuntimeMapFlag = 0;//


		BaseObjectManager* mObjectManager;

		
		std::unique_ptr<AnimationUpdateSystem> mAnimationUpdateSystem;



		//map내에있는 object들의 정보유지
		//id 
		std::unordered_map<unsigned long long, Object*> mObjectIDTable;

		std::vector<Object*> mObjectVector;
		std::vector<Object*> mStartObjectVector;

		UiCollisionWorld* mDefaultUiCollisionWorld;		//0
		CollisionWorld* mDefault3DCollisionWorld;		//1

		RegisterAnimStateTransitionCallbackClassManager* mRegisterAnimStateTransitionCallbackClassManager;

	};


}
