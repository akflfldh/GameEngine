#include "Object/Entity.h"
#include"Collision/Collider.h"
#include"Collision/ColliderGenerator.h"
#include"ResourceManager/MeshManager.h"
#include"Map/Map.h"


#include"Component/MeshComponent.h"
#include"Core/Event.h"


namespace Quad
{


	Entity::Entity(const std::string& name)
		:Object(name, EObjectType::eEntity)
	{

		GetModel()->AddComponernt(EComponentType::eMeshComponent);
		GetModel()->AddComponernt(EComponentType::eColliderComponent);
	}


	void Entity::Initialize()
	{
		

	}

	void Entity::DefaultCreatingInitialize()
	{

		InitCreating();
	}



	Entity* Entity::Create(Map* map, int mapLayerIndex)
	{

		Entity* ob = static_cast<Entity*>(map->CreateObject("Entity", mapLayerIndex));
		//ob->Initialize();
		//ob->InitCreating();

		return ob;;
	}


	Entity::~Entity()
	{
		//delete mModel;
		//delete mCollider;
	}

	void Entity::Start()
	{
		Object::Start();
		InitCreating();
	//	mModel->Start();

	}

	void Entity::Update(float deltaTime)
	{

		//유저의 업데이트
		mTransform.Update();
		//if(mModel!=nullptr)
		//	mModel->Update(deltaTime);

		Object::Update(deltaTime);
		//유저업데이트 끝
	}

	void Entity::EndUpdate(float deltaTime)
	{
	}

	void Entity:: OnEvent(Event* event)
	{

		const std::string& eventName = event->GetName();

		if (eventName == "LButtonDown")
		{


		}
		else if (eventName == "LButtonUp")
		{



		}
		else if (eventName == "MouseMove")
		{

		}
		else if (eventName == "Select")
		{//클릭되어지는 순간부터
			HandleSelect(event);
		}
		else if (eventName == "ReleaseSelect")
		{
			HandleReleaseSelect(event);
		}





	}

	void Entity::UpdateCollider(float deltaTime)
	{
		//mModel->GetCollider()->Update(deltaTime);
	}


	//const Model* Entity::GetModel() const
	//{

	//	return mModel;
	//	// TODO: 여기에 return 문을 삽입합니다.
	//}

	//Model* Entity::GetModel()
	//{
	//	return mModel;
	//	// TODO: 여기에 return 문을 삽입합니다.
	//}


	bool Entity::GetInstancingFlag()
	{
		return mInstancingFlag;
	}

	std::wstring Entity::GetInstanceGroupName()
	{
		return mInstanceGroupName;
	}

	bool Entity::GetSelectState() const
	{
		return mSelectState;
	}

	void Entity::SetSelectState(bool state)
	{
		mSelectState = state;
	}



	bool Entity::InnerDetectCollsion(Collider* colliderA, Collider* colliderB)
	{
		return true;
	}

	bool Entity::InnerDetectRayHit(const Ray& ray)
	{
		return true;
	}

	void Entity::Serialize()
	{
		Object::Serialize();

		//mModel->Serialize();

	}

	void Entity::DeSerialize()
	{
		Object::DeSerialize();

		//mModel->DeSerialize();
	}


	void Entity::InitCreating()
	{
	
		Quad::Model* model = GetModel();
		Quad::MeshComponent* meshComponent = model->GetMeshComponent();
		meshComponent->SetMesh(MeshManager::GetMesh("Cube"));



	}

	void Entity::SetInstancingData(bool flag, const std::wstring & groupName)
	{
		mInstancingFlag = flag;
		mInstanceGroupName= groupName;
	}

	void Entity::HandleSelect(Event* event)
	{

		SetSelectState(true);

	}

	void Entity::HandleReleaseSelect(Event* event)
	{
		SetSelectState(false);
	}

}