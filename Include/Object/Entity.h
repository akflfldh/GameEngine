#pragma once



#include"Object/Object.h"
#include"Component/Model.h"
#include"Collision/Collider.h"
#include"Core/CoreDllExport.h"

namespace Quad
{

	class CORE_API_LIB Entity :public Object
	{
		//friend class controller 을 처리하여 model의 속성을 바꾸자 , public메서드로 제공하기보다는.
		friend class InstancingTable;
	public:
		Entity(const std::string& name = "");
		virtual ~Entity();

		//Entity(const Entity& entity);

		virtual void Initialize();


		virtual void DefaultCreatingInitialize() override;
		
		
		static Entity* Create(Map* map, int mapLayerIndex = 0);


		void Start() override;
		virtual void Update(float deltaTime) override;
		virtual void EndUpdate(float deltaTime) override;
		virtual void OnEvent(Event* event) override;
		virtual void UpdateCollider(float deltaTime);


		CLASSNAME(Entity)
		CLASSSIZE(Entity)

		/*const Model* GetModel()const;
		Model* GetModel();*/

		bool GetInstancingFlag();
		std::wstring GetInstanceGroupName();
		
		bool GetSelectState()const;
		void SetSelectState(bool state);

		//void AddModelComponent(Mesh * mesh=nullptr);
		//Collider* GetCollider() const;
		//void AddColliderComponent(Collider* collider);


		//이 메서드는 아무것도하지않고 인터페이스역할만 한다.
		virtual bool InnerDetectCollsion(Collider* colliderA, Collider* colliderB) override;
		virtual bool InnerDetectRayHit(const Ray& ray) override;

		virtual void Serialize() override;
		virtual void DeSerialize() override;

		void InitCreating();

	protected:

		
	private:
		void SetInstancingData(bool flag, const std::wstring& groupName);


		///void HandleLButtonDown(Event* event);
		void HandleSelect(Event* event);
		void HandleReleaseSelect(Event* event);


	private:
		//Model* mModel;
		//Collider* mCollider;

		bool mInstancingFlag;
		std::wstring mInstanceGroupName;

		bool mSelectState = false;


	};
	REGISTERCLASS(Entity)


}
