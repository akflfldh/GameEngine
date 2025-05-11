#include "ContentItemUiEntity.h"
#include"EffectTable.h"
#include"ContentItemImageUiEntity.h"
//#include"Text.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"System.h"
#include"MouseEvent.h"
#include"DragAndDropWindowController.h"
#include"Window.h"

#include"Core/EventDispatcher.h"
#include"NotifyDragAndDropEvent.h"
#include"MathHelper.h"
#include"Collision/CollisionHelper.h"
#include"Mouse.h"
#include"System/FileUiUiSystem.h"
#include"TextBox.h"
#include"KeyBoard.h"

#include"Component/UiColliderComponent.h"



Quad::ContentItemUiEntity::ContentItemUiEntity()
	:ContentItemBaseUiEntity()
{


}

void Quad::ContentItemUiEntity::Initialize()
{
	ContentItemBaseUiEntity::Initialize();
	
}



Quad::ContentItemUiEntity* Quad::ContentItemUiEntity::Create(Map* map, int mapLayerIndex, ContentItem* item, EType type)
{
	ContentItemUiEntity* ob = static_cast<ContentItemUiEntity*>(map->CreateObject("ContentItemUiEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating(item, type);
	

	return ob;
}

//Quad::ContentItem* Quad::ContentItemUiEntity::GetContentItem() const
//{
//	return mContentItem;
//}

void Quad::ContentItemUiEntity::HandleHover(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;


	//텍스트박스가 활성화된상태에서는 그냥 상태설정만하고 넘어간다
	if (mContentItemNameTextSelectState)
	{
		stateComponent->SetHoverState(mouseEvent->GetHoverState());
		return;
	}


	if (mouseEvent->GetHoverState())
	{
		stateComponent->SetHoverState(true);


		if (stateComponent->GetLButtonDownState() ||stateComponent->GetSelectState())
			return;


		SetCurrentTexture(GetHoverTexture());
		TextBox* contentItemNameText = GetContentItemNameText();
		contentItemNameText->SetTextColor({ 0.0f,0.0f,0.0f });
		//mContentItemNameText->SetCharacterColorIntensity({ 2.0f,2.0f,2.0f });

	}
	else
	{
		stateComponent->SetHoverState(false);

		OutputDebugString(L"NotHover item\n");
		if (!stateComponent->GetSelectState())
		{
				stateComponent->SetSelectState(false);
				SetCurrentTexture(GetDefaultTexture());
				TextBox* contentItemNameText = GetContentItemNameText();
				contentItemNameText->SetTextColor({ 1.0f,1.0f,1.0f });
			
		}

	}


}

void Quad::ContentItemUiEntity::HandleMouseMove(Event* pEvent)
{



	StateComponent* stateComponet = GetStateComponent();

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	if (stateComponet->GetLButtonDownState())
	{
		HWND windowHandle  =GetSystem()->GetWindow()->GetWindowHandle();
		//HWND windowHandle = GetSystem()->GetWindow()->GetWindowHandle();
		POINT screenMousePos = { mouseEvent->GetClientPositionX(),mouseEvent->GetClientPositionY() };
		ClientToScreen(windowHandle, &screenMousePos);
		SetWindowPos(DragAndDropWindowController::GetWindow()->GetWindowHandle(), HWND_TOPMOST, screenMousePos.x+10, screenMousePos.y+10, 0, 0, SWP_NOSIZE);



		if (!DragAndDropWindowController::GetWindowRunningState())
		{
			//SetCapture(windowHandle);
			Mouse::CaptureObject(this);
			ContentItem* contentItem = GetContentItem();
			DragAndDropWindowController::RunWindow(contentItem);
		}





		NotifyDragAndDropEvent* notifyDragAndDropEvent = new NotifyDragAndDropEvent();
		notifyDragAndDropEvent->SetContentItem(GetContentItem());
		notifyDragAndDropEvent->SetScreenPosX(mouseEvent->GetScreenPositionX());
		notifyDragAndDropEvent->SetScreenPosY(mouseEvent->GetScreenPositionY());
		notifyDragAndDropEvent->SetLButtonUpState(false);
		EventDispatcher::SendEvent(notifyDragAndDropEvent, ESystemID::eGamePlayMainSystem);

		NotifyDragAndDropEvent* notifyDragAndDropEventAttr = new NotifyDragAndDropEvent;
		notifyDragAndDropEventAttr->SetContentItem(GetContentItem());
		notifyDragAndDropEventAttr->SetScreenPosX(mouseEvent->GetScreenPositionX());
		notifyDragAndDropEventAttr->SetScreenPosY(mouseEvent->GetScreenPositionY());
		notifyDragAndDropEventAttr->SetLButtonUpState(false);
		EventDispatcher::SendEvent(notifyDragAndDropEventAttr, ESystemID::eAttributeMainSystem);

		mContentItemNameTextSelectState = false;

	}
	else
	{

		//ray판정을 통해 자식entity들에게 이벤트를 전달한다.
		//특히 textbox에게

		if (stateComponet->GetHoverState())
		{

			Ray ray = mouseEvent->GetRay();
			float t = FLT_MAX;
			TextBox* contentItemNameText = GetContentItemNameText();

			if (CollisionHelper::Intersect(contentItemNameText->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t))
			{
				if (!contentItemNameText->GetStateComponent()->GetHoverState())
				{
					MouseEvent hover("Hover");
					hover.SetHoverState(true);
					contentItemNameText->OnEvent(&hover);
					SetChildHoverObject(contentItemNameText);
				}
			}
			else
			{
				if (GetChildHoverObject() != nullptr)
				{
					MouseEvent hover("Hover");
					hover.SetHoverState(false);
					contentItemNameText->OnEvent(&hover);

				}
			}

			contentItemNameText->OnEvent(mouseEvent);

		}




	}




}

void Quad::ContentItemUiEntity::HandleLButtonDown(Event* pEvent)
{
	
	MouseEvent* mouseEvnet = (MouseEvent*)pEvent;

	StateComponent* stateComponent = GetStateComponent();


	//만약 텍스트박스가 활성화된상태라면

	//지금 텍스트박스는 활성회되어서 그영역이 itemUi 일부분을 막음으로써 hover가 false상태일것이다
	//또한 아예외부일떄도 false상태이다.
	//정확한 판정을위해서 ray를 이용한다.




	if (stateComponent->GetHoverState())
	{
		if (mouseEvnet->GetGlobalFlag())			
			return;	//global적으로 온것은 받지않는다.	//정확히 클릭됬을때만받는다.

		stateComponent->SetLButtonDownState(true);

		OutputDebugString(L"LButtonHover item\n");

		if (!stateComponent->GetSelectState() )
		{
			//아직선택하지않은 , 처음 클릭한것일때
			

			SetCurrentTexture(GetHoverSelectTexture());
			stateComponent->SetSelectState(true);
	

		}
		else
		{
			TextBox* contentItemNameText = GetContentItemNameText();

			if (mContentItemNameTextSelectState == true)
			{
				//이건 이미 텍스트박스가 활성화된상태에서 마우스클릭


				//텍스트박스 lbutton down되면  - mouse를 캡처한다 
				//따라서 미리 여기서  Panel의 lbuttonDown상태를 리셋한다
				
				stateComponent->SetLButtonDownState(false);
				contentItemNameText->OnEvent(pEvent);
				

			}else 
			if (contentItemNameText->GetStateComponent()->GetHoverState())
			{
				mContentItemNameTextSelectState = true;

			}
			else
			{
				contentItemNameText->SetTexture("Transparent.png");
				contentItemNameText->SetTextEditMode(false);
				contentItemNameText->ResetVisibleLine();
				KeyBoard::ReleaseCaptureObject(contentItemNameText);
				mContentItemNameTextSelectState = false;

			}
		}
	}
	else
	{


		OutputDebugString(L"LButtonNotHover item\n");
		if (stateComponent->GetSelectState())
		{
			TextBox* contentItemNameText = GetContentItemNameText();
			SetCurrentTexture(GetDefaultTexture());
			stateComponent->SetSelectState(false);
			//mContentItemNameText->SetCharacterColorIntensity({ 1.0f,1.0f,1.0f });

			contentItemNameText->SetTexture("Transparent.png");
			contentItemNameText->SetTextColor({ 1.0f,1.0f,1.0f });

			if (mContentItemNameTextSelectState)
			{
				contentItemNameText->SetTextEditMode(false);
				contentItemNameText->ResetVisibleLine();
				KeyBoard::ReleaseCaptureObject(contentItemNameText);
				mContentItemNameTextSelectState = false;
			}

		}


	}





}

void Quad::ContentItemUiEntity::HandleLButtonUp(Event* pEvent)
{
	//uientity에서 하면안되고 uientity 별로 다 다르게 처리할수있어야돼
	//아니면 virtual 로 처리하게해야돼



	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetLButtonDownState())
	{


		MouseEvent* mouseEvent = (MouseEvent*)pEvent;
		StateComponent* stateComponent = GetStateComponent();

		stateComponent->SetLButtonDownState(false);


		if (mContentItemNameTextSelectState /*&& mContentItemNameText->GetStateComponent()->GetHoverState()*/)
		{
			TextBox* contentItemNameText = GetContentItemNameText();

			//움직이지않고 up했다면(텍스트박스 위에서)
			//텍스트박스를 활성화한다.

			//stateComponent->SetSelectState(false);
			contentItemNameText->SetTexture("TextBox.png");
			contentItemNameText->SetTextColor({ 0.0f,0.0f,0.0f });
			contentItemNameText->SetTextEditMode(true);

			KeyBoard::CaptureObject(contentItemNameText);
			//KeyBoard::SetTextBoxActiveFlag(true);









		}
		else
		{




			//ReleaseCapture();
			Mouse::ReleaseCaptureObject(this);
			NotifyDragAndDropEvent* notifyDragAndDropEvent = new NotifyDragAndDropEvent();
			notifyDragAndDropEvent->SetContentItem(GetContentItem());
			notifyDragAndDropEvent->SetScreenPosX(mouseEvent->GetScreenPositionX());
			notifyDragAndDropEvent->SetScreenPosY(mouseEvent->GetScreenPositionY());
			notifyDragAndDropEvent->SetLButtonUpState(true);

			EventDispatcher::SendEvent(notifyDragAndDropEvent, ESystemID::eGamePlayMainSystem);
			
			
			NotifyDragAndDropEvent* notifyDragAndDropEventAttr = new NotifyDragAndDropEvent();
			notifyDragAndDropEventAttr->SetContentItem(GetContentItem());
			notifyDragAndDropEventAttr->SetScreenPosX(mouseEvent->GetScreenPositionX());
			notifyDragAndDropEventAttr->SetScreenPosY(mouseEvent->GetScreenPositionY());
			notifyDragAndDropEventAttr->SetLButtonUpState(true);
			EventDispatcher::SendEvent(notifyDragAndDropEventAttr, ESystemID::eAttributeMainSystem);


			DragAndDropWindowController::EndWindow();
		}

	}
	

}

void Quad::ContentItemUiEntity::HandleLButtonDownDouble(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	if (stateComponent->GetHoverState())
	{
		switch (GetContentItem()->GetEContentItemType())
		{
		case EContentItemType::eLogicalFolder:
		{


			FileUiUiSystem* system = (FileUiUiSystem*)GetSystem();
			system->SetCurrentFolder((LogicalContentItemUiEntityFolder*)GetContentItem());

		}
		break;
		}

	}







}

void Quad::ContentItemUiEntity::InitCreating(ContentItem* item, EType type)
{
	ContentItemBaseUiEntity::InitCreating(item, type);


	//SetEffect("DefaultUi.effect");
	//SetTexture("Black.png", EUiEntityMouseState::eDefault);
	//SetTexture("Gray.png", EUiEntityMouseState::eHover);
	//SetTexture("Blue.png", EUiEntityMouseState::eHoverSelect);
	//SetTexture("Black.png", EUiEntityMouseState::eNotHoverSelect);

	//Map* map = GetMap();
	//int mapLayerIndex = GetMapLayerID();

	//mContentItemImageUiEntity = ContentItemImageUiEntity::Create(map, mapLayerIndex, item);
	//mUiEntityType = type;


	//if (item->GetEContentItemType() == EContentItemType::eLogicalFolder)
	//{
	//	SetTexture("Transparent.png");
	//}

	////mContentItemImageUiEntity->Initialize(item);

	//mContentItemImageUiEntity->GetTransform().SetIndependentScaleFlag(true);



	//mContentItemImageUiEntity->SetSelectBlockFlag(false);
	//mContentItemImageUiEntity->SetSelectAvailableFlag(false);




	//int nameLength = item->GetName().size();
	////int fontSize = entity->mContentItemNameText->GetFontSize();
	//int	textBoxSize = 200;//	 nameLength* fontSize;
	////int	textBoxSize = 80;//	 nameLength* fontSize;

	//SetName(item->GetName());
	//switch (type)
	//{
	//case EType::eFileUiVersion:
	//{
	//	mContentItemNameText = TextBox::Create(map, mapLayerIndex, 100);


	//	SetSize(100, 140);
	//	SetPosition(0, 0, 10);
	//	mContentItemImageUiEntity->SetSize(90, 90);
	//	mContentItemImageUiEntity->SetPosition(0, 20, 9);


	//	//entity->mContentItemNameText->Initialize(50, 40,true);
	//	//mContentItemNameText->Initialize(100);
	//	mContentItemNameText->SetName(GetName() + "_textBox");
	//	mContentItemNameText->SetSize(100, 40);
	//	mContentItemNameText->SetPosition(0, -50.0f, 8.0f);


	//	//entity->mContentItemNameText->SetCharacterInsertTextBoxMaxWIdth(140);
	//	mContentItemNameText->SetAlignmentDirection(ETextAlignmentDirection::eCenter);
	//	mContentItemNameText->SetTexture("Transparent.png");
	//	//
	//	mContentItemNameText->SetText(item->GetName());
	//	mContentItemNameText->SetDrawFlag(true);
	//	mContentItemNameText->SetTextDrawFlag(true);
	//	mContentItemNameText->SetPosition(0, -50.0f, 8.0f);
	//	mContentItemNameText->GetTransform().SetIndependentScaleFlag(true);
	//	mContentItemNameText->SetSelectBlockFlag(false);
	//	mContentItemNameText->SetSelectAvailableFlag(false);
	//	mContentItemNameText->SetEntireSelectAvailableFlag(false);

	//	//entity->mContentItemNameText->OnOffReSizeFlag(false);
	//	//entity->mContentItemNameText->OnOffTextModifyFlag(false);


	//}
	//break;
	//case EType::eDragAndDropVersion:
	//{
	//	mContentItemNameText = TextBox::Create(map, mapLayerIndex, 255);


	//	SetSize(100 + textBoxSize, 100);
	//	SetPosition(0, 0, 10);
	//	mContentItemImageUiEntity->SetSize(90, 90);
	//	mContentItemImageUiEntity->SetPosition(-150, 0, 9);
	//	mContentItemImageUiEntity->SetPosition(50 - (100 + textBoxSize) / 2.0f, 0, 9);


	////	mContentItemNameText->Initialize(255);
	//	mContentItemNameText->SetSize(textBoxSize, 40);
	//	mContentItemNameText->SetTexture("Transparent.png");
	//	mContentItemNameText->SetText(item->GetName());
	//	mContentItemNameText->SetPosition(100 - (100 + textBoxSize) / 2.0f + textBoxSize / 2.0f, 0.0f, 9.0f);
	//	mContentItemNameText->SetPosition(50, 0.0f, 9.0f);
	//	mContentItemNameText->GetTransform().SetIndependentScaleFlag(true);
	//	mContentItemNameText->SetSelectBlockFlag(false);
	//	mContentItemNameText->SetSelectAvailableFlag(false);
	//	mContentItemNameText->SetDrawFlag(true);
	//	mContentItemNameText->SetTextDrawFlag(true);

	//}
	//break;
	//}


	//AddChildPanelUiEntity(mContentItemNameText);
	//AddChildPanelUiEntity(mContentItemImageUiEntity);
	////entity->AddChildObject(entity->mContentItemImageUiEntity);
	////entity->AddChildObject(entity->mContentItemNameText);

	//mContentItem = item;
}


//void Quad::ContentItemUiEntity::ChangeContentItem(ContentItem* item)
//{
//
//	mContentItem = item;
//	mContentItemImageUiEntity->SetTexture(item);
//	mContentItemNameText->SetText(item->GetName());
//
//	switch (mUiEntityType)
//	{
//		case EType::eFileUiVersion:
//		{
//
//		}
//		break;
//
//		case EType::eDragAndDropVersion:
//		{
//
//
//		}
//		break;
//	}
//
//
//}
//


void Quad::ContentItemUiEntity::Update(float deltaTime)
{
	ContentItemBaseUiEntity::Update(deltaTime);
}

void Quad::ContentItemUiEntity::OnEvent(Event* event)
{
	ContentItemBaseUiEntity::OnEvent(event);

	const std::string& eventName = event->GetName();
	

	//if (eventName == "MouseMove")
	//{
	//	HandleMouseMoveEvent(event);
	//}
	if (eventName == "LButtonDownDouble")
	{

		HandleLButtonDownDouble(event);
	}





}

//Quad::ContentItem* Quad::ContentItemUiEntity::GetAsset() const
//{
//	return mContentItem;
//}

//void Quad::ContentItemUiEntity::SetOnOff(bool state)
//{
//	mContentItemImageUiEntity->SetDrawFlag(state);
//	mContentItemNameText->SetDrawFlag(state);
//	//mContentItemNameText->SetTextDrawFlag(state);
//	
//	mContentItemNameText->SetActiveFlag(state);
////	mContentItemNameText->SetEntireSelectAvailableFlag(state);
//	//mContentItemNameText->OnOffDrawFlag(state);
//
//	//OnOffDrawFlag에서 true로설정되어도 여기서는 기본적으로 false이여야하니 다시설정해준다.
//	mContentItemNameText->SetSelectAvailableFlag(false);
//	
//	SetDrawFlag(state);
//	//SetSelectAvailableFlag(state);
//	SetActiveFlag(state);
//	if (state == true)
//	{
//		//transform의 더티플래그를 켜서 콜라이더같은것이 정확히 동기화되도록하자.
//		GetTransform().SetDirtyFlag(true);
//		mContentItemImageUiEntity->GetTransform().SetDirtyFlag(true);
//		mContentItemNameText->GetTransform().SetDirtyFlag(true);
//	}
//
//
//}
