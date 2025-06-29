#pragma once


#include"Predeclare.h"

#include"Core/SceneElement.h"
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

#include<type_traits>


#include"Core/CoreDllExport.h"
#include"Collision/Collider.h"
#include"Collision/UiCollider.h"



namespace Quad
{
	namespace Scene
	{
		class SceneGraph;
		struct Node;
	}
	template<typename T>
	class SpacePartitioningStructure;
	

	class SceneComponent;
	
	//typename class SceneGraph::Node;

	class CORE_API_LIB Object:public SceneElement
	{
	public:
		friend class GamePlayWindowController;
		friend class CollisionHelper;
		friend class Map;
		friend class Scene::SceneGraph;
		friend class SpacePartitioningStructure<Collider>;
		//friend class SpacePartitioningStructure<UiCollider>;
		friend class DockingWindowController;
		friend class System;
		friend class UniqueIDTable<Object>;


		Object(EObjectType objectType = EObjectType::eObject);
		virtual ~Object() ;


		virtual Transform& GetTransform() override;
		virtual const Transform& GetTransform() const override;


		virtual void Initialize();

		//Initialize()호출후, Default로초기화하는경우
		//엔진에서 사용함으로 유저는다음을 작성한다.
		virtual void DefaultCreatingInitialize(); 
				


		virtual void OnEvent(Event* event);
		virtual void Start();
		virtual void Update(float deltaTime);
		virtual void EndUpdate(float deltaTime);

	
		void SetObjectPositionLocal(const DirectX::XMFLOAT3& pos);
		void SetObjectPositionLocal(float x, float y, float z);
		void SetObjectPositionWorld(const DirectX::XMFLOAT3& pos);
		void SetObjectPositionWorld(float x, float y, float z);
		

		DirectX::XMFLOAT3 GetObjectPositionLocal() const;
		DirectX::XMFLOAT3 GetObjectPositionWorld() const;
		



		DirectX::XMFLOAT4 GetObjectQuaternionLocal() const;
		DirectX::XMFLOAT4 GetObjectQuaternionWorld() const;
		void SetObjectQuaternionWorld(const DirectX::XMFLOAT4& quaternion);



		DirectX::XMFLOAT3 GetObjectLookWorld() const;
		DirectX::XMFLOAT3 GetObjectRightWorld() const;
		DirectX::XMFLOAT3 GetObjectUpWorld() const;








		virtual void SetDrawFlag(bool flag);
		bool GetDrawFlag()const;

		void SetEntireDrawFlag(bool flag);
		bool GetEntrieDrawFlag()const;

		virtual void SetActiveFlag(bool flag);
		bool GetActiveFlag()const;

		bool AddChildObject(Object* childObject);
		

		//파생클래스에서 개별적으로 childOject리스트를 유지한다면 이 메서드들을 오버라이딩해야한다.
		//부모자식관계를 끊는다. (자식은 씬그래프의 루트노드가 부모가 된다)
		virtual void RemoveChildObject(Object* childObject);
		//child vector에서 childObject만 제거하는메서드
		virtual bool RemoveChildObjectInVector(Object* childObject);

		const std::vector<Object*>& GetChildObjectVector() const;

		virtual void SetStencilRefValue(unsigned char value);
		unsigned char GetStencilRefValue()const;

		const std::string &  GetControllerName() const;
		
		virtual void SetSystem(System* system) override;
	//	TaskWindow* GetWindow()const;
		virtual System* GetSystem() const override;


		EObjectType GetObjectType()const;

		virtual bool InnerDetectCollsion(Collider* colliderA, Collider* colliderB) ;
		virtual bool InnerDetectRayHit(const Ray& ray) ;

		Object* GetParentObject()const;

		void RegisterAcceptEvent(const std::string& eventName);
		void RemoveAcceptEvent(const std::string& eventName);


		template<typename T> 
		T* AddComponent(const char * componentName);					//몇몇 component들은 추가할려면 object가 componentChannel을 가질것을 필요로한다..

		template<typename T>
		T* GetComponent(const char * componentName)const;
		
		
		//최상위component들에대해서만 작동
		void RemoveComponent(const char* componentName);



		//CLASSNAME(Object)
		CLASSNAME(Object)
		CLASSNAMESTATIC(Object)
		//virtual const char* GetClassTypeName() const ;
		CLASSSIZE(Object)


		void SetMapLayer(int mapLayerID);
		int GetMapLayerID() const;


		unsigned long long GetUniqueID() const;

	
		virtual  Map* GetMap() const override;
		virtual  Map* GetMap() override; 


		virtual void Serialize();
		virtual void DeSerialize();


		bool GetKilledState() const;

		bool GetStartObjectFlag() const;
		void SetStartObjectFlag(bool flag);


		
		//map에 제거요청을 했을때 수행되어야하는 동작(map에 제거요청을 하면 이동작을 수행하겠다)
		virtual void BeforeRemoveBehavior();

		void SetMap(Map* map);



	public:
		DirectX::XMFLOAT4X4 GetTransformWorldMatrix() const;
		SceneComponent* GetRootSceneComponent() const;


	protected:
		

		Map* mMap=nullptr;				
		SceneComponent* mRootSceneComponent;
		
		//void UpdateComponent(float delta);
	
	private:
		void SetUniqueID(unsigned long long id);
		void SetKilledState(bool state);

		BaseComponent* AddComponent(const char* componentTypeName,const char* componentName);
		BaseComponent* GetComponent(const char * componentName) const;


		void UpdateSceneComponentWorldTransformReculsivly();




		unsigned long long mUniqueID;

		System* mSystem;	
		
		Object* mParentObject;
		std::vector<Object*> mChildObjectVector;

		bool mDrawFlag;
		bool mEntireDrawFlag;
		bool mActiveFlag;
		unsigned char mStencilRefValue = 0;
		EObjectType mObjectType;
		bool mIsAddedToSceneState =false;

							//componentName, 
		std::unordered_map<std::string, BaseComponent*> mComponentTable;


		int mMapLayerID = 0;
		
		bool mIsKilledState = false;



		//처음생성된 오브젝트인지 나타내는 플래그 
		//그러한오브젝트들은 생성된프레임말고 다음프레임에서부터 올바르게 update가 수행이된다.
		bool mStartObjectFlag;






		Quad::Scene::Node * mSceneGraphNode;//sceneGraph에서 여러가지 node작업을수행할시 빠른 작업을위해 캐싱한다
		//이렇게얻는이점이 커플링 정도의 증가보다 훨씬 이득임으로,적절한 등가교환이다.


	};
	REGISTERCLASS(Object)





	template<typename T>
	inline T * Object::AddComponent(const char * componentName)
	{
		static_assert(std::is_base_of<BaseComponent, T>::value,
			"Component타입이 아니다.\n");


		
		T* component = nullptr;

		component = static_cast<T*>(AddComponent(T::GetClassTypeNameStatic(), componentName));

		



		return component;

	}

	template<typename T>
	inline T* Object::GetComponent(const char* componentName) const
	{
		static_assert(std::is_base_of<BaseComponent, T>::value,
			"Component타입이 아니다.\n");


	
		T* component = dynamic_cast<T*>(GetComponent(componentName));
		

		return component;
	}

}
