#pragma once


#include"Predeclare.h"

#include"Core/SceneElement.h"
#include"Core/Transform.h"
#include<unordered_map>
#include<string>
#include<vector>
#include"Object/ObjectType.h"
#include"Object/StateComponent.h"
#include<functional>
#include"Component.h"
#include"Object/Macro.h"
#include"Utility/HandleTable.h"
#include"Utility/UniqueIDTable.h"
#include"Object/ObjectSmartPointer.h"


#include"Core/CoreDllExport.h"
#include"Collision/Collider.h"
#include"Collision/UiCollider.h"




namespace Quad
{
	template<typename T>
	class SpacePartitioningStructure;

	class CORE_API_LIB Object:public SceneElement
	{
	public:
		friend class GamePlayWindowController;
		friend class CollisionHelper;
		friend class Map;
		friend class SceneGraph;
		friend class SpacePartitioningStructure<Collider>;
		//friend class SpacePartitioningStructure<UiCollider>;
		friend class DockingWindowController;
		friend class System;
		friend class UniqueIDTable<Object>;


		Object(const std::string& name,EObjectType objectType);
		virtual ~Object() ;


		virtual Transform& GetTransform() override;
		virtual const Transform& GetTransform() const override;



		void SetTransform(Transform& transform);

		virtual void Initialize() = 0;

		//Initialize()호출후, Default로초기화하는경우
		//엔진에서 사용함으로 유저는다음을 작성한다.
		virtual void DefaultCreatingInitialize() = 0; 
				


		virtual void OnEvent(Event* event) = 0;
		virtual void Start();
		virtual void Update(float deltaTime) = 0;
		virtual void EndUpdate(float deltaTime) = 0;

	


		virtual void SetDrawFlag(bool flag);
		bool GetDrawFlag()const;

		void SetEntireDrawFlag(bool flag);
		bool GetEntrieDrawFlag()const;


		virtual void SetActiveFlag(bool flag);
		bool GetActiveFlag()const;

		void SetSelectFlag(bool flag);
		void SendSelectEvent(bool state,bool exclusiveEventHandleFlag = false);
		bool GetSelectFlag()const;

		bool AddChildObject(Object* childObject);


		//파생클래스에서 개별적으로 childOject리스트를 유지한다면 이 메서드들을 오버라이딩해야한다.
		//부모자식관계를 끊는다. (자식은 씬그래프의 루트노드가 부모가 된다)
		virtual void RemoveChildObject(Object* childObject);
		//child vector에서 childObject만 제거하는메서드
		virtual bool RemoveChildObjectInVector(Object* childObject);

		const std::vector<ObjectSmartPointer>& GetChildObjectVector() const;

		virtual void SetStencilRefValue(unsigned char value);
		unsigned char GetStencilRefValue()const;

		const std::string &  GetControllerName() const;
		
		virtual void SetSystem(System* system) override;
	//	TaskWindow* GetWindow()const;
		virtual System* GetSystem() const override;


		const std::string GetName()const;
		void SetName(const std::string& name);

		//?????? ->id?? ?????? ???????.???????????? ??????????.
		//?? ?????? ?????(name)?? ????.
		void SetIDState(bool state);
		bool GetIDState()const;

		void SetSelectKeepingFlag(bool flag);
		bool GetSelectKeepingFlag()const;
		
		void SetSelectAvailableFlag(bool flag);
		bool GetSelectAvailableFlag()const;

		void SetSelectBlockFlag(bool flag);
		bool GetSelectBlockFlag()const;

		void SetEntireSelectAvailableFlag(bool flag);
		bool GetEntireSelectAvailableFlag()const;


		EObjectType GetObjectType()const;

		const StateComponent* GetStateComponent() const ;
		StateComponent* GetStateComponent();


		//?? ?????? ????????????? ?????????????? ???.
		virtual bool InnerDetectCollsion(Collider* colliderA, Collider* colliderB) ;
		virtual bool InnerDetectRayHit(const Ray& ray) ;

		Object* GetParentObject()const;

		void RegisterAcceptEvent(const std::string& eventName);
		void RemoveAcceptEvent(const std::string& eventName);
	//	void RegisterAcceptEvent(int eventID);


		
		void RegisterEventCallback(const std::string& eventName, const std::function<void(Event* pEvent)> & callback);
		std::function<void(Event*)> * GetEventCallback(const std::string& name,int index=0);
		void CallEventCallback(const std::string& name, Event* pEvent);


		void SetUpdateCallback(const std::function<void(Object * object, float deltaTime)>& callback);
		const std::function<void(Object * object, float deltaTime)> * GetUpdateCallback()const;


		void HandleDefaultEvent(Event* pEvent);
		//unsigned long long GetID() const;





		void AddComponent(BaseComponent * component);
		BaseComponent* GetComponent(int componentID)const;
		
		
		//CLASSNAME(Object)
		virtual const char* GetClassTypeName() const = 0;
		CLASSSIZE(Object);


		void SetMapLayer(int mapLayerID);
		int GetMapLayerID() const;


		unsigned long long GetUniqueID() const;

	
		virtual  Map* GetMap() const override;
		virtual  Map* GetMap() override; 


		virtual void Serialize() = 0;
		virtual void DeSerialize() = 0;



		//유저는 건들지 말것 (기즈모 등를위한것)
		bool GetEnginObjectFlag() const;
		void SetEngineObjectFlag(bool flag);


		bool GetKilledState() const;

		bool GetStartObjectFlag() const;
		void SetStartObjectFlag(bool flag);


		
		//map에 제거요청을 했을때 수행되어야하는 동작(map에 제거요청을 하면 이동작을 수행하겠다)
		virtual void BeforeRemoveBehavior();

		void SetMap(Map* map);


		

	protected:
		Transform mTransform;
		std::string mName;

		//std::unordered_map <std::string,

		Map* mMap=nullptr;					//?????????? ??????? map

		

		void SetIsAddedToSceneFlag(bool state);
		bool GetIsAddedToSceneFlag() const;
		
		void UpdateComponent(float delta);
	
	private:
		void SetUniqueID(unsigned long long id);
		void SetKilledState(bool state);






		unsigned long long mUniqueID;
		//QOBHANDLE mObjectHandle;

		System* mSystem;	// ?????? map??????????? ??????? ??????? ?? map????????????? ?????????????.
		

		//Object* mParentObject;
		ObjectSmartPointer mParentObject;
		//std::vector<Object*> mChildObjectVector;
		std::vector<ObjectSmartPointer> mChildObjectVector;

		bool mDrawFlag;
		bool mEntireDrawFlag;
		bool mActiveFlag;
		bool mSelectFlag;		//?????????? ????.
		bool mIDState = true;		//??????id???????.

		//?????????? LButtonUp???????? ??????? select?????? ?????????????
		bool mSelectKeepingFlag = true;	
		//?????????????????? ??????
		bool mSelectAvailableFlag = true;
		//??????????? ??????????? ????????? ?????????????? ???? 
		bool mSelectBlockFlag = true;
		
		bool mEntireSelectAvailableFlag = true;

		unsigned char mStencilRefValue = 0;
		EObjectType mObjectType;


		StateComponent mStateComponent;

		bool mIsAddedToSceneState =false;




		std::function<void(Object * object ,float deltaTime)> mUpdateCallback = nullptr;
		std::unordered_map<std::string, std::vector<std::function<void(Event* pEvent)>>> mEventCallbackUnMap;


							//componentID, 
		std::unordered_map<int, BaseComponent*> mComponentTable;


		int mMapLayerID = 0;
		
		


		bool mIsEngineObject=false ;

		bool mIsKilledState = false;



		//처음생성된 오브젝트인지 나타내는 플래그 
		//그러한오브젝트들은 생성된프레임말고 다음프레임에서부터 올바르게 update가 수행이된다.
		bool mStartObjectFlag;


	};

}
