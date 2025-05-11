#include "Object/UiEntity.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"MouseEvent.h"
#include"Component/MeshComponent.h"
#include"Map/Map.h"


namespace Quad
{
	UiEntity::UiEntity(const std::string& name)
		:Object(name,EObjectType::eUiEntity),mDefaultTexture(nullptr),mHoverTexture(nullptr),mHoverSelectTexture(nullptr), mNotHoverSelectTexture(nullptr)
	{

		GetModel()->AddComponernt(Quad::EComponentType::eMeshComponent);
		GetModel()->AddComponernt(Quad::EComponentType::eUiColliderComponent);


	}
	UiEntity::~UiEntity()
	{

	}

	void UiEntity::Initialize()
	{
		//Object::Initialize();

	
		//SetSize(mWidth,mHeight);//
	}

	Quad::UiEntity * UiEntity::Create(Map * map, int mapLayerIndex)
	{
		UiEntity* ob = static_cast<UiEntity*>(map->CreateObject("UiEntity", mapLayerIndex));
		//ob->Initialize();
		ob->InitCreating();

		return ob;
	}

	void UiEntity::DefaultCreatingInitialize()
	{
		InitCreating();

	}


	void UiEntity::Update(float deltaTime)
	{
		Object::Update(deltaTime);

	}

	void UiEntity::EndUpdate(float deltaTime)
	{
		
	}

	void UiEntity::OnEvent(Event* event)
	{
		//Entity::OnEvent(event);

		std::string eventName = event->GetName();
		if (eventName == "LButtonDown")
		{
		
			HandleLButtonDown(event);
			
		}
		else if (eventName == "LButtonUp")
		{
			HandleLButtonUp(event);
		}
		else if (eventName == "Select")
		{
			//HandleSelectEvent(event);
			
		}
		else if (eventName == "ReleaseSelect")
		{
			SetCurrentTexture(mDefaultTexture);
		}
		else if (eventName == "Hover")
		{
			HandleHover(event);
		}
		else
		{
			HandleDefaultEvent(event);
		}



	}

	
	//디폴트텍스처는 무조건지정해야됌
	void UiEntity::SetTexture(const std::string& name, EUiEntityMouseState uiEntityMouseState )
	{
		switch (uiEntityMouseState)
		{
		case EUiEntityMouseState::eDefault:
			mDefaultTexture = TextureManager::GetTexture(name);

			GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(mDefaultTexture);
			break;
		case EUiEntityMouseState::eHover:
			mHoverTexture = TextureManager::GetTexture(name);

			break;
		case EUiEntityMouseState::eHoverSelect:
			mHoverSelectTexture = TextureManager::GetTexture(name);
			break;
		case EUiEntityMouseState::eNotHoverSelect:
			mNotHoverSelectTexture = TextureManager::GetTexture(name);

			break;
		}

	}

	void UiEntity::SetTexture(Texture* texture, EUiEntityMouseState uiEntityMouseState)
	{

		switch (uiEntityMouseState)
		{
		case EUiEntityMouseState::eDefault:
			mDefaultTexture = texture;
			GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(mDefaultTexture);
			break;
		case EUiEntityMouseState::eHover:
			mHoverTexture = texture;

			break;
		case EUiEntityMouseState::eHoverSelect:
			mHoverSelectTexture = texture;
			break;
		case EUiEntityMouseState::eNotHoverSelect:
			mNotHoverSelectTexture = texture;

			break;
		}


	}

	void UiEntity::SetSize(float width, float height)
	{

		mWidth = width;
		mHeight = height;

		GetTransform().SetScaleLocal({ width,height,1.0f });

	}

	void UiEntity::SetPosition(float x, float y, float z)
	{

		GetTransform().SetPositionLocal({ x,y,z });
		

	}

	void UiEntity::SetPosition(const DirectX::XMFLOAT3& posLocal)
	{
		GetTransform().SetPositionLocal(posLocal);
	}

	void UiEntity::SetColorItensity(const DirectX::XMFLOAT3& itensity)
	{
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetColorIntensity(itensity);
	}

	void UiEntity::SetColor(const DirectX::XMFLOAT3& color)
	{
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetColor(color);

	}

	void UiEntity::SetTextureTransform(const DirectX::XMFLOAT4X4& transform)
	{
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetTextureTransformMatrix(0, transform);
	}

	float UiEntity::GetWidth() const
	{
		return mWidth;
	}

	float UiEntity::GetHeight() const
	{
		return mHeight;
	}

	void UiEntity::SetEffect(const std::string& name)
	{
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetEffectName(name);
	}

	void UiEntity::SetDrawFlag(bool flag)
	{
		
		Object::SetDrawFlag(flag);


	}

	void UiEntity::Serialize()
	{
		Object::Serialize();
	}

	void UiEntity::DeSerialize()
	{
		Object::DeSerialize();
	}


	void UiEntity::InitCreating()
	{
		GetModel()->GetMeshComponent()->SetMesh(MeshManager::GetMesh("Rectangle"));
		//Initialize(MeshManager::GetMesh("Rectangle"));
		mWidth = 1.0f;
		mHeight = 1.0f;
	}

	void UiEntity::SetCurrentTexture(Texture* texture)
	{
		GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetDiffuseMap(texture);
	}

	void UiEntity::HandleLButtonDown(Event* pEvent)
	{

		StateComponent* stateComponent = GetStateComponent();


		if (stateComponent->GetHoverState())
		{
			stateComponent->SetLButtonDownState(true);
			if (mHoverSelectTexture != nullptr)
				SetCurrentTexture(mHoverSelectTexture);

			stateComponent->SetSelectState(true, true);
		}

	}

	void UiEntity::HandleLButtonUp(Event* pEvent)
	{

		StateComponent* stateComponent = GetStateComponent();
		stateComponent->SetLButtonDownState(false);

		if (stateComponent->GetHoverState())
		{

			if (mHoverSelectTexture != nullptr)
			{
				SetCurrentTexture(mHoverSelectTexture);
			}
		}
		else
		{
			if (mNotHoverSelectTexture != nullptr)
			{
				SetCurrentTexture(mNotHoverSelectTexture);
			}
				
		}

		stateComponent->SetSelectState(false, false);

	}


	void UiEntity::HandleHover(Event* pEvent)
	{
		StateComponent* stateComponent = GetStateComponent();
		MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	
	
		if (mouseEvent->GetHoverState())
		{
			stateComponent->SetHoverState(true);
			if (mHoverTexture != nullptr)
			{
				SetCurrentTexture(mHoverTexture);
			}
		}
		else
		{
			stateComponent->SetHoverState(false);
			if (mHoverTexture != nullptr)
			{
				SetCurrentTexture(mNotHoverSelectTexture);
			}
		}


		
	}

	Texture* UiEntity::GetDefaultTexture() const
	{
		return mDefaultTexture;
	}

	Texture* UiEntity::GetHoverTexture() const
	{
		return mHoverTexture;
	}

	Texture* UiEntity::GetHoverSelectTexture() const
	{
		return mHoverSelectTexture;
	}

	Texture* UiEntity::GetNotHoverSelectTexture() const
	{
		return mNotHoverSelectTexture;
	}

}