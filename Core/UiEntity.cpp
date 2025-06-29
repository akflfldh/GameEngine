#include "Object/UiEntity.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"MouseEvent.h"
#include"Component/UiColliderComponent.h"
#include"Map/Map.h"

#include"Component/UiComponent.h"
#include"Component/UIMeshComponent.h"



namespace Quad
{
	UiEntity::UiEntity(const std::string& name)
		:Object(EObjectType::eUiEntity)
	{
		UiComponent * uiComponent =AddComponent<UiComponent>("UiComponent");
		mUiMeshComponent = AddComponent<UIMeshComponent>("MeshComponent");
		uiComponent->RegisterCallback(&UiEntity::HandleUiComponentCallback, this);

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

	void UiEntity::SetWidthHeightLocal(float width, float height)
	{

		mUiMeshComponent->SetWidthHeightLocal(width, height);

	}

	void UiEntity::SetWidthHeightWorld(float width, float height)
	{
		//mUiMeshComponent->sETwIDTHhEIGHTwORLD
	}

	void UiEntity::Serialize()
	{
		Object::Serialize();
	}

	void UiEntity::DeSerialize()
	{
		Object::DeSerialize();
	}

	void UiEntity::HandleUiComponentCallback(const MouseInputData& mouseInputData)
	{
		switch (mouseInputData.mMouseInputType)
		{
		case EMouseInput::eLButtonDown:
			HandleLButtonDown(mouseInputData);
			break;

		case EMouseInput::eLButtonUp:
			HandleLButtonUp(mouseInputData);
			break;

		case EMouseInput::eRButtonDown:
			HandleRButtonDown(mouseInputData);

			break;

		case EMouseInput::eRButtonUp:
			HandleRButtonUp(mouseInputData);

			break;

		case EMouseInput::eMouseMove:
			HandleMouseMove(mouseInputData);

			break;
		}








	}

	Quad::UiComponent* UiEntity::GetUiComponent() const
	{
		return mUiComponent;
	}

	UIMeshComponent* UiEntity::GetUiMeshComponent() const
	{
		return mUiMeshComponent;
	}


	void UiEntity::InitCreating()
	{

	}

	void UiEntity::HandleHover(const MouseInputData& mouseInputData)
	{

	}
	void UiEntity::HandleLButtonDown(const MouseInputData& mouseInputData)
	{

	/*	StateComponent* stateComponent = GetComponent<StateComponent>();


		if (stateComponent->GetHoverState())
		{
			stateComponent->SetLButtonDownState(true);
			if (mHoverSelectTexture != nullptr)
				SetCurrentTexture(mHoverSelectTexture);

			stateComponent->SetSelectState(true, true);
		}*/

	}

	void UiEntity::HandleLButtonUp(const MouseInputData& mouseInputData)
	{

	}

	void UiEntity::HandleRButtonDown(const MouseInputData& mouseInputData)
	{
	}

	void UiEntity::HandleRButtonUp(const MouseInputData& mouseInputData)
	{
	}

	void UiEntity::HandleMouseMove(const MouseInputData& mouseInputData)
	{
	}

	//void UiEntity::HandleLButtonUp(Event* pEvent)
	//{

	//	StateComponent* stateComponent = GetComponent<StateComponent>();
	//	stateComponent->SetLButtonDownState(false);

	//	if (stateComponent->GetHoverState())
	//	{

	//		if (mHoverSelectTexture != nullptr)
	//		{
	//			SetCurrentTexture(mHoverSelectTexture);
	//		}
	//	}
	//	else
	//	{
	//		if (mNotHoverSelectTexture != nullptr)
	//		{
	//			SetCurrentTexture(mNotHoverSelectTexture);
	//		}
	//			
	//	}

	//	stateComponent->SetSelectState(false, false);

	//}


	/*void UiEntity::HandleHover(Event* pEvent)
	{
		StateComponent* stateComponent = GetComponent<StateComponent>();
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


		
	}*/

	//Texture* UiEntity::GetDefaultTexture() const
	//{
	//	return mDefaultTexture;
	//}

	//Texture* UiEntity::GetHoverTexture() const
	//{
	//	return mHoverTexture;
	//}

	//Texture* UiEntity::GetHoverSelectTexture() const
	//{
	//	return mHoverSelectTexture;
	//}

	//Texture* UiEntity::GetNotHoverSelectTexture() const
	//{
	//	return mNotHoverSelectTexture;
	//}

}