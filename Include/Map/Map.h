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
		D3D12_VIEWPORT mViewPort = { 0,0,0,0,0,0 };
		 
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
		friend class EditorSystem;
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


		void PrintObjectName();

		void Initialize(System* system, bool playMode = false, DirectX::XMFLOAT3 backgroundColor = { 0.1f,0.1f,0.6f });

		//map??泥섏쓬?쒖옉?좊븣, 媛숈씠 議댁옱?섎뒗 object?ㅼ쓽 start?몄텧
		void Start();

		
		virtual void Update(float deltaTime);
		virtual void EndUpdate(float deltaTime);
		virtual void ObjectUpdate(float deltaTime);



		//??쭅?ы솕?섏??딄퀬, ?덈줈 留듭쓣 留뚮뱶?붽꼍?곗뿉 ?ㅼ쓬硫붿꽌?쒕? ?몄텧?섏뿬 湲곕낯map layer瑜?援ъ텞?쒕떎.
		void CreateDefaultMapLayer();
									//gameWindow占쏙옙 占쏙옙占쏙옙占싹곤옙, 占쏙옙占쏙옙占쏙옙window占쏙옙占쏙옙 占쌓삼옙true占싹곤옙
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


		const SceneGraph* GetSceneGraph()const;
		SceneGraph* GetSceneGraph();
	//	SpacePartitioningStructure* GetSpaceParitioningStructure() const;
		const CollisionWorld* GetCollisionWorld()const;
		CollisionWorld* GetCollisionWorld();

		//ui,3d ?듯빀
		bool RayCastingFirst(Ray& ray, Object*& oObject);
		bool RayCastingFirst(const DirectX::XMFLOAT2 & screenPos, Object*& oObject);



		void SetName(const std::string& name);
		const std::string& GetName() const;

		RenderMapItem GetRenderMapItem() const;
		//void GetObjectVectorInViewFrustum(std::vector<Object*>& oObjectVector);
		virtual const std::vector<MapLayer> GetObjectsInViewFrustum() ; 
		

		Camera* GetMainCamera(int mapLayerIndex =0)const;


		void SetViewPort(FLOAT topLeftX, FLOAT topLeftY, FLOAT width,
			FLOAT height, FLOAT minDepth, FLOAT maxDepth, int mapLayer=0);
		D3D12_VIEWPORT GetViewPort(int mapLayer=0) const;
		


		System* GetSystem() const;
		BaseObjectManager* GetObjectManager() const;

		void CreateMapLayer(int mID, int mDepthPriority, Camera* camera, D3D12_VIEWPORT viewport, RenderTargetTexture* texture= nullptr, Texture* depthStencilBuffer=nullptr);


		MapLayer& GetMapLayer(int index) ;
		const MapLayer& GetMapLayer(int index) const;


		//void SetMainEngineCamera(Camera* camera, int mapLayerIndex = 0);

		//?먰븿??紐⑤몢 userCamera?먮쭔 ?대떦?섎뒗 ?⑥닔?대떎
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
#ifdef EditorMode
		//void SetMainUserCamera();
	//	void SetMainEngineCamera();
#endif


		RegisterAnimStateTransitionCallbackClassManager* GetRegisterAnimStateTransitionCallbackClassManager() const;


	protected:
		void SetRuntimeMapFlag(bool flag);
		void AddObject(Object* object, int mapLayer = 0);
		bool RemoveObject(Object* object);


		std::string mName;
		SceneGraph mEditSceneGraph;
		//SceneGraph mEngineSceneGraph;	

		//map layer蹂꾨줈 light?멸굅媛숈?
		std::vector<std::vector<Light*>> mDirectionalLightVector;
		std::vector<LineBase*> mLineBaseVector;


		std::vector<MapLayer> mMapLayerVector;
		std::vector<MapLayerOption> mMapLayerOptionVector;
		RenderMapItem mRenderMapItem;
		System* mSystem;


		int mRuntimeMapFlag = 0;//0?대㈃ edit map, 0???꾨땲硫?runtime map  

		//BaseObjectFactory* mObjectFactory;
		BaseObjectManager* mObjectManager;

		
		std::unique_ptr<AnimationUpdateSystem> mAnimationUpdateSystem;


		//id 
		std::unordered_map<unsigned long long, Object*> mObjectIDTable;

		std::vector<Object*> mObjectVector;
		std::vector<Object*> mStartObjectVector;


		UiCollisionWorld* mDefaultUiCollisionWorld;		//0
		CollisionWorld* mDefault3DCollisionWorld;		//1


		RegisterAnimStateTransitionCallbackClassManager* mRegisterAnimStateTransitionCallbackClassManager;



	};


}
