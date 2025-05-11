#include "Model.h"
#include"Parser/JsonParser.h"
#include"ResourceManager/MeshManager.h"
#include"Collision/ColliderGenerator.h"
#include"Object/Entity.h"
#include"Object/UiEntity.h"

#include"Map/Map.h"

#include"Allocator/BinnedAllocator.h"

#include"Component/MeshComponent.h"
#include"Component/AnimationComponent.h"
#include"Component/ColliderComponent.h"
#include"Component/UiColliderComponent.h"

#include<vector>

namespace Quad
{
	BinnedAllocator* GetModelComponentAllocator(unsigned int size)
	{
		
		static BinnedAllocator allocator16(16);
		static BinnedAllocator allocator64(64);
		static BinnedAllocator allocator80(80);
		static BinnedAllocator allocator96(96);
		static BinnedAllocator allocator128(128);
		static BinnedAllocator allocator256(256);
		static BinnedAllocator allocator512(512);
		static BinnedAllocator allocator1024(1024);
		static BinnedAllocator allocator2048(2048);


		static std::vector<int> sizeVector = { 16,64,80,96,128,256,512,1024,2048 };
		static std::vector<BinnedAllocator*> allocatorVector = {&allocator16, &allocator64,&allocator80,&allocator96,
		&allocator128,&allocator256,&allocator512,&allocator1024,&allocator2048 };


		for (int i = 0; i < sizeVector.size(); ++i)
		{

			int allocSize = sizeVector[i];
			if (size <= allocSize)
			{
				return allocatorVector[i];
			}
		}
		throw std::bad_alloc();


	}



	Model::Model(SceneElement* destSceneElement)
		:mDestSceneElement(destSceneElement),mModelComponentVector(4,nullptr)
	{
		mModelComponentVector.assign(4, nullptr);
	/*	for (int i = 0; i < 4; ++i)
		{
			if (mModelComponentVector[i] != nullptr)
				int a = 2;
		}*/
	}
	Model::~Model()
	{
	
		for (int i = 0; i < mModelComponentVector.size(); ++i)
		{
			
			RemoveComponent(EComponentType(i));

		}

		/*for (auto& element : mModelComponentVector)
		{
			if (element != nullptr)
			{
				




				BinnedAllocator* allocator = GetModelComponentAllocator(element->GetClassSize());
				allocator->Release(element);
			}


			RemoveComponent(element->)
		}*/




	}

	void Model::Initialize()
	{

		
	}

	void Model::Start()
	{
	//	mAnimationComponent.Start();


	}

	void Model::Update(float deltaTime)
	{
		/*if (mMeshPointer == nullptr)
			return;

		if(mMeshPointer->GetMeshType()==EMeshType::eSknningMesh)
			mAnimationComponent.Update(deltaTime);*/


	}

	






	void Model::Serialize(const std::string& tag)
	{
		std::vector<int> componentIDVector;
		
		for (int i = 0; i < mModelComponentVector.size(); ++i)
		{
			if (mModelComponentVector[i] != nullptr)
				componentIDVector.push_back(i);
		}
		int componentSize = componentIDVector.size();
		JsonParser::Write("Model_ComponentIDVectorSize_"+tag, componentSize);

		JsonParser::Write("Model_ComponentIDVector_"+tag, componentIDVector.begin(), componentIDVector.end());


		for (auto& element : mModelComponentVector)
		{
			if (element != nullptr)
			{
				element->Serialize(tag);
			}
		}

	}
	void Model::DeSerialize(const std::string& tag)
	{
		int currentReadObjectIndex = 0;
		std::vector<int> componentIDVector;
		int componentIDSize = 0;
		JsonParser::Read("Model_ComponentIDVectorSize_"+tag, componentIDSize);
		componentIDVector.resize(componentIDSize);
		JsonParser::Read("Model_ComponentIDVector_"+tag, componentIDVector.begin(),componentIDVector.end());

		for (auto& element : componentIDVector)
		{
			
			AddComponernt(EComponentType(element));
			mModelComponentVector[element]->DeSerialize(tag);		//mesh컴포넌트가 먼저 역직렬화되고, 그떈 콜리전컴포넌트는 존재하지않고 ,따라서 메시설정에서 충돌체를 만들지않는다
			//충돌체는 콜리전컴포넌트를 역직렬화할때만든다.(메시와 상관없이 충돌체만 존재할수도있으니깐)

		}
		
		//일단 충돌컴포넌트내부 충돌체가 참조하는 DestObject는 내부에서 직렬화하지않고 역직렬화할때도 여기서 수행한다.
		//현재 ObjectManager가 여러종류라서 지금당장은 정확히 어떤 manager를 사용할지 결정할수없다.(물론 GameObjectManager의 런타임,edit둘중하나이다)
		if (mModelComponentVector[(int)EComponentType::eColliderComponent] != nullptr)
		{
			static_cast<ColliderComponent*>(mModelComponentVector[(int)EComponentType::eColliderComponent])->GetCollider()->SetDestObject(static_cast<Object*>(mDestSceneElement));
		}

		/*for (auto& element : mModelComponentVector)
		{
			if (element != nullptr)
				element->DeSerialize(tag);
		}*/
			

	}
	void Model::AddComponernt(EComponentType componentType)
	{


		if (mModelComponentVector[(int)componentType] != nullptr)
			return;



		ModelBaseComponent* component = nullptr;
		switch (componentType)
		{

		case EComponentType::eMeshComponent:

			component = new (GetModelComponentAllocator(sizeof(MeshComponent))->Alloc())MeshComponent(mDestSceneElement);


			break;
		case EComponentType::eAnimationComponent:
			component = new (GetModelComponentAllocator(sizeof(AnimationComponent))->Alloc())AnimationComponent(mDestSceneElement);

			break;
		case EComponentType::eColliderComponent:
		{
			if (mDestSceneElement->GetSceneElementType() == ESceneElementType::eObject)
			{
				Object* object = static_cast<Object*>(mDestSceneElement);
				if (object->GetObjectType() != EObjectType::eUiEntity)
				{
					component = new (GetModelComponentAllocator(sizeof(ColliderComponent))->Alloc())ColliderComponent(mDestSceneElement);
				}
			}
		}
			break;
		case EComponentType::eUiColliderComponent:
		{
			if (mDestSceneElement->GetSceneElementType() == ESceneElementType::eObject)
			{
				Object* object = static_cast<Object*>(mDestSceneElement);
				EObjectType objecType = object->GetObjectType();
				if (objecType == EObjectType::eUiEntity || objecType ==EObjectType::eSpline)
				{
					component = new (GetModelComponentAllocator(sizeof(UiColliderComponent))->Alloc())UiColliderComponent(object);
				}
			}
		}

		}

		mModelComponentVector[(int)componentType] = component;


		



	}

	void Model::RemoveComponent(EComponentType componentType)
	{

		if (mModelComponentVector[(int)componentType] == nullptr)
			return;

		
		switch (componentType)
		{

		case EComponentType::eMeshComponent:
		{
			MeshComponent* meshComponent = static_cast<MeshComponent*>(mModelComponentVector[(int)componentType]);
			meshComponent->~MeshComponent();
			GetModelComponentAllocator(sizeof(MeshComponent))->Release(meshComponent);
		
		}
			break;
		case EComponentType::eAnimationComponent:
		{
			AnimationComponent* animationComponent = static_cast<AnimationComponent*>(mModelComponentVector[(int)componentType]);
			animationComponent->~AnimationComponent();
			GetModelComponentAllocator(sizeof(AnimationComponent))->Release(animationComponent);
		}
			break;
		case EComponentType::eColliderComponent:
		{

			ColliderComponent* colliderComponent = static_cast<ColliderComponent*>(mModelComponentVector[(int)componentType]);
			colliderComponent->~ColliderComponent();
			GetModelComponentAllocator(sizeof(ColliderComponent))->Release(colliderComponent);


		}
			break;
		case EComponentType::eUiColliderComponent:
		{

			UiColliderComponent* uiColliderComponent = static_cast<UiColliderComponent*>(mModelComponentVector[(int)componentType]);
			uiColliderComponent->~UiColliderComponent();
			GetModelComponentAllocator(sizeof(UiColliderComponent))->Release(uiColliderComponent);


		}
		break;
		}

		mModelComponentVector[(int)componentType] = nullptr;




	}

	Quad::ModelBaseComponent* Model::GetComponent(EComponentType componentType) const
	{

		return mModelComponentVector[(int)componentType];
	}

	MeshComponent* Model::GetMeshComponent() const
	{
		return (MeshComponent*)mModelComponentVector[(int)EComponentType::eMeshComponent];
	}
	

	AnimationComponent* Model::GetAnimationComponent() const
	{
		return (AnimationComponent*)mModelComponentVector[(int)EComponentType::eAnimationComponent];

	}

	ColliderComponent* Model::GetColliderComponent() const
	{
		return (ColliderComponent*)mModelComponentVector[(int)EComponentType::eColliderComponent];
	}


	UiColliderComponent* Model::GetUiColliderComponent() const
	{
		return (UiColliderComponent*)mModelComponentVector[(int)EComponentType::eUiColliderComponent];
	}

	MeshComponent* Model::GetMeshComponent()
	{
		return (MeshComponent*)mModelComponentVector[(int)EComponentType::eMeshComponent];;
	}

	AnimationComponent* Model::GetAnimationComponent()
	{
		return  (AnimationComponent*)mModelComponentVector[(int)EComponentType::eAnimationComponent];
	}

	ColliderComponent* Model::GetColliderComponent()
	{
		return (ColliderComponent*)mModelComponentVector[(int)EComponentType::eColliderComponent];;
	}

	
}