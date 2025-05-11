#include "FolderPanelEntity.h"
#include"System.h"
#include"MouseEvent.h"
#include"Collision/CollisionHelper.h"
#include"FolderListPanelEntity.h"
#include"PopupWindowController.h"
#include"PopupPanelID.h"
#include"LogicalContentItemUiEntityFolder.h"
#include"System/FileUiUiSystem.h"
#include"Component/UiColliderComponent.h"


float Quad::FolderPanelEntity::mFolderPanelExpandButtonWidth = 30.0f;
float Quad::FolderPanelEntity::mFolderPanelExpandButtonHeight = 30.0f;
float Quad::FolderPanelEntity::mFolderPanelIconWidth = 30.0f;
float Quad::FolderPanelEntity::mFolderPanelIconHeight = 30.0f;
float Quad::FolderPanelEntity::mTextWidth;
float Quad::FolderPanelEntity::mTextHeight;






Quad::FolderPanelEntity::FolderPanelEntity(const std::string& name)
	:PanelUiEntity(name+"_FolderPanel"), mChildFolderExpandButton(nullptr), mFolderIcon(nullptr),
	mFolderNameText(nullptr)
{


}

Quad::FolderPanelEntity::~FolderPanelEntity()
{
}

void Quad::FolderPanelEntity::Initialize()
{
	PanelUiEntity::Initialize();	

	System* system = GetSystem();
	system->RegisterEntityToEvent("LButtonDown", this);
	system->RegisterEntityToEvent("MouseMove", this);
	system->RegisterEntityToEvent("LButtonUp", this);
	system->RegisterEntityToEvent("Hover", this);
	system->RegisterEntityToEvent("RButtonDown", this);
	system->RegisterEntityToEvent("RButtonUp", this);

	




}

void Quad::FolderPanelEntity::Start()
{
	//PanelUiEntity::Start();


	//mChildFolderExpandButton->SetLButtonUpCallback([parentPanel = this]()
	//	{

	//		parentPanel->ExpandChildFolderPanel();

	//	});
}

void Quad::FolderPanelEntity::Update(float deltaTime)
{





}

void Quad::FolderPanelEntity::OnEvent(Event* event)
{

	PanelUiEntity::OnEvent(event);

	/*const std::string& eventName = event->GetName();
	if (eventName == "Hover")
	{
		HandleHover(event);
	}
	else if (eventName == "LButtonDown")
	{
		HandleLButtonDown(event);
	}
	else if (eventName == "LButtonUp")
	{
		HandleLButtonUp(event);
	}
	else if (eventName == "MouseMove")
	{
		HandleLButtonUp(event);
	}
	else if (eventName == "RButtonDown")
	{
		HandleRButtonDown(event);
	}
	else if (eventName == "RButtonUp")
	{
		HandleRButtonUp(event);
	}*/




}

int Quad::FolderPanelEntity::GetChildFolderPanelEntityNum() const
{
	return mChildFolderPanelEntityNum;
}

void Quad::FolderPanelEntity::IncreaseChildFolderPanelEntityNum()
{
	mChildFolderPanelEntityNum++;
}

void Quad::FolderPanelEntity::DecreaseChildFolderPanelEntityNum()
{
	mChildFolderPanelEntityNum--;
}

void Quad::FolderPanelEntity::InitCreating(float width, float height, const std::string& folderName, FolderListPanelEntity* folderListPanel)
{
	PanelUiEntity::InitCreating();


	SetSize(width, height);//3000,40
	const DirectX::XMFLOAT3& pos = GetTransform().GetPositionWorld();
	GetTransform().SetIndependentScaleFlag(true);
	SetTexture("DarkGray.png");
	mFolderListPanelEntity = folderListPanel;

	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	//mChildFolderExpandButton = static_cast<ListExpandButton*>(map->CreateObject("ListExpandButton"));

	mChildFolderExpandButton = ListExpandButton::Create(map, mapLayerIndex);

	//mFolderIcon = static_cast<IconEntity*>(map->CreateObject("IconEntity"));
	mFolderIcon = IconEntity::Create(map, mapLayerIndex);
	
	//mFolderNameText = static_cast<TextBox*>(map->CreateObject("TextBox"));
	mFolderNameText = TextBox::Create(map, mapLayerIndex, 255);





	//가장왼쪽에 오는 (panel에서는 중앙이 시작이다) ListExpandButton 


	DirectX::XMFLOAT3 buttonPos;
	//	mChildFolderExpandButton->SetSystem(system);
//	mChildFolderExpandButton->Initialize();
	mChildFolderExpandButton->SetSize(mFolderPanelExpandButtonWidth, mFolderPanelExpandButtonHeight);


	mChildFolderExpandButton->SetSelectAvailableFlag(false);
	//mChildFolderExpandButton.SetSelectAvailableFlag(false);

	

	//mChildFolderExpandButton->SetHoverCallback([parentPanel = this]() {

	//	/*if (parentPanel->GetStateComponent()->GetSelectState())
	//		return;*/
	 

	//	parentPanel->SetColorItensity({ 2.0F,2.0F,2.0F });

	//	});

	//mChildFolderExpandButton->SetReleaseHoverCallback([parentPanel = this]() {

	///*	
	//	if (parentPanel->GetStateComponent()->GetSelectState())
	//		return;*/ 

	//	parentPanel->SetColorItensity({ 1.0F,1.0F,1.0F });

	//	});











	buttonPos.x = pos.x + mFolderPanelExpandButtonWidth / 2;
	buttonPos.y = pos.y;
	buttonPos.z = pos.z - 1;
	mChildFolderExpandButton->GetTransform().SetPositionWorld(buttonPos);


	//그다음에는 폴더 아이콘 ui entity
	//mFolderIcon.SetSystem(GetSystem());
	//mFolderIcon->Initialize();
	mFolderIcon->SetTexture("Folder.png");
	mFolderIcon->SetSize(mFolderPanelIconWidth, mFolderPanelIconHeight);
	mFolderIcon->SetSelectAvailableFlag(false);

	mFolderIcon->GetTransform().SetIndependentScaleFlag(true);
	mFolderIcon->GetTransform().SetIndependentRotationFlag(true);

	DirectX::XMFLOAT3 iconPos = buttonPos;
	iconPos.x = pos.x + mFolderPanelExpandButtonWidth + mFolderPanelIconWidth / 2;
	mFolderIcon->GetTransform().SetPositionWorld(iconPos);
	//mFolderIcon.SetTexture()


	//그다음에 텍스트

	//mFolderNameText->SetSystem(system);
	//mFolderNameText->Initialize(255);
	mFolderNameText->SetSize(width / 2, height);

	DirectX::XMFLOAT3 textPos = iconPos;
	textPos.x = pos.x + mFolderPanelExpandButtonWidth + mFolderPanelIconWidth + width / 4;
	mFolderNameText->GetTransform().SetPositionWorld(textPos);

	mFolderNameText->SetText(folderName);

	mFolderNameText->SetTexture("Transparent.png");
	mFolderNameText->SetDrawFlag(false);
	mFolderNameText->SetTextDrawFlag(true);
	mFolderNameText->SetSelectAvailableFlag(false);
	mFolderNameText->GetTransform().SetIndependentScaleFlag(true);
	mFolderNameText->GetTransform().SetIndependentRotationFlag(true);

	//mFolderNameText.OnOffSelectFlag(false);
	//mFolderNameText.OnOffTextModifyFlag(false);


	AddChildObject(mChildFolderExpandButton);
	AddChildObject(mFolderIcon);
	AddChildObject(mFolderNameText);






	mChildFolderExpandButton->SetLButtonUpCallback([parentPanel = this]()
		{

			parentPanel->ExpandChildFolderPanel();

		});

}


Quad::FolderPanelEntity* Quad::FolderPanelEntity::Create(Map* map, int mapLayerIndex, float width, float height, const std::string& folderName, FolderListPanelEntity* folderListPanel)
{
	FolderPanelEntity* ob = static_cast<FolderPanelEntity*>(map->CreateObject("FolderPanelEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating(width,height,folderName,folderListPanel);
	//ob->Start();
	return ob;
}

void Quad::FolderPanelEntity::HandleHover(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;
	stateComponent->SetHoverState(mouseEvent->GetHoverState());


	if (stateComponent->GetSelectState())
		return;

	if(stateComponent->GetHoverState())
	{
	
		SetColorItensity({ 2.0F,2.0F,2.0F });
	}
	else
	{
		//if (mChildFolderExpandButton.GetStateComponent()->GetHoverState())
			//return;


		mChildFolderExpandButton->OnEvent(pEvent);
		SetColorItensity({ 1.0f,1.0f,1.0f });
	}



}

void Quad::FolderPanelEntity::HandleLButtonDown(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;


	if(stateComponent->GetHoverState())
	{
		if (mouseEvent->GetGlobalFlag())
			return;
		stateComponent->SetLButtonDownState(true);


		mChildFolderExpandButton->OnEvent(pEvent);


	
	}
	else
	{
		//if (stateComponent->GetSelectState() )
		//{
		//	

		//	if (!mChildFolderExpandButton.GetStateComponent()->GetHoverState())
		//	{
		//		stateComponent->SetSelectState(false);
		//		
		//		//다른folder panel을 클릭했을때 처리해줘야하지.
		//	//	SetColorItensity({ 1.0f,1.0f,1.0f });

		//	}
		//}



	}



}

void Quad::FolderPanelEntity::HandleLButtonUp(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();


	if (stateComponent->GetLButtonDownState())
	{
		stateComponent->SetLButtonDownState(false);

		if (stateComponent->GetHoverState())
		{

			//SetColorItensity({ 0.5f,0.5f,0.5f });
			if (mChildFolderExpandButton->GetStateComponent()->GetHoverState())
			{
				mChildFolderExpandButton->OnEvent(pEvent);
			}
			else
			{

				stateComponent->SetSelectState(true);

				FileUiUiSystem* system = (FileUiUiSystem*)GetSystem();

				LogicalContentItemUiEntityFolder* destLogicalFolder = GetLinkedLogicalFolder();

				system->SetCurrentFolder(destLogicalFolder);
			}
		}

		
	}




}

void Quad::FolderPanelEntity::HandleMouseMove(Event* pEvent)
{

	StateComponent* stateComponent = GetStateComponent();

	MouseEvent* mouseEvent = (MouseEvent*)pEvent;






	float t;
	if (CollisionHelper::Intersect(mChildFolderExpandButton->GetModel()->GetUiColliderComponent()->GetCollider(), mouseEvent->GetRay(), t))
	{

		if (mChildFolderExpandButton->GetStateComponent()->GetHoverState() == false)
		{
			MouseEvent hover("Hover");
			mChildFolderExpandButton->OnEvent(&hover);
			SetChildHoverObject(mChildFolderExpandButton);

		}


	}
	else
	{
		MouseEvent hover("Hover");
		hover.SetHoverState(false);
		mChildFolderExpandButton->OnEvent(&hover);
		SetChildHoverObject(nullptr);
	}

	mChildFolderExpandButton->OnEvent(pEvent);







}

void Quad::FolderPanelEntity::HandleRButtonUp(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;

	if (stateComponent->GetRButtonDownState())
	{
		OutputDebugString(L"RButtonUp\n");
		stateComponent->SetRButtonDownState(false);

		int screenPosX = mouseEvent->GetScreenPositionX();
		int screenPosY = mouseEvent->GetScreenPositionY();
		
		PopupWindowController::ActivateWindow(FILEUIPOPUP,screenPosX, screenPosY);
	}

}

void Quad::FolderPanelEntity::HandleRButtonDown(Event* pEvent)
{
	StateComponent* stateComponent = GetStateComponent();
	MouseEvent* mouseEvent = (MouseEvent*)pEvent;


	if (stateComponent->GetHoverState() || mChildFolderExpandButton->GetStateComponent()->GetHoverState())
	{
		//문제 - 그 expand button위에올라갔을때는 hover가 풀려있지 그러니 expand button위에있는지도확인해줘야겠지.
		stateComponent->SetRButtonDownState(true);
		
	}



}

const Quad::FolderListPanelEntity* Quad::FolderPanelEntity::GetFolderListPanelEntity() const
{
	return mFolderListPanelEntity;
}

Quad::FolderListPanelEntity* Quad::FolderPanelEntity::GetFolderListPanelEntity()
{
	return mFolderListPanelEntity;
}

const std::string& Quad::FolderPanelEntity::GetFolderName() const
{

	return mFolderNameText->GetText();

}

void Quad::FolderPanelEntity::ExpandChildFolderPanel()
{

	bool flag = mChildFolderExpandButton->GetExpandFlag();
	float shiftY = 0;
	float panelHeight = GetHeight();


	std::queue<FolderPanelEntity*> folderPanelQueue;

	const std::vector<UiEntity*>& childFolderPanelVector = GetChildPanelUiEntityVector();
	for (int i = 0; i < childFolderPanelVector.size(); ++i)
	{
		folderPanelQueue.push((FolderPanelEntity*)childFolderPanelVector[i]);

	}




	if (flag == true)
	{
		while (!folderPanelQueue.empty())
		{
			FolderPanelEntity* panelEntity = folderPanelQueue.front();
			folderPanelQueue.pop();

			panelEntity->SetFolderPanelValid(flag);

			if (panelEntity->GetChildFolderExpandFlag())
			{
				const std::vector<UiEntity*>& childFolderPanelVector = panelEntity->GetChildPanelUiEntityVector();
				for (int i = 0; i < childFolderPanelVector.size(); ++i)
				{
					folderPanelQueue.push((FolderPanelEntity*)childFolderPanelVector[i]);
				}
			}

		}
	}
	else
	{
		while (!folderPanelQueue.empty())
		{
			FolderPanelEntity* panelEntity = folderPanelQueue.front();
			folderPanelQueue.pop();

			panelEntity->SetFolderPanelValid(flag);

				const std::vector<UiEntity*>& childFolderPanelVector = panelEntity->GetChildPanelUiEntityVector();
				for (int i = 0; i < childFolderPanelVector.size(); ++i)
				{
					folderPanelQueue.push((FolderPanelEntity*)childFolderPanelVector[i]);
				}
			

		}


	}












	//펼쳐져있다면, 자식의개수*height ,
	//그리고 그자식들중 또 펼쳐져있는 자식들에대해서 그 자식들의 자식의개수*height 방식으로 계속 내려간다.
	//shiftY += childFolderPanelVector.size() * panelHeight;

	mFolderListPanelEntity->ReBuildFolderPanelEntities(this,false);
























	//std::queue<FolderPanelEntity*> folderPanelQueue;
	//for (int i = 0; i < childFolderPanelVector.size(); ++i)
	//{
	//	folderPanelQueue.push((FolderPanelEntity*)childFolderPanelVector[i]);
	//}

	////자식들이 펼쳐져있는지 보고 height를 계산한다.
	//while (!folderPanelQueue.empty())
	//{

	//	FolderPanelEntity* folderPanel = folderPanelQueue.front();

	//	int expandFlag = folderPanel->mChildFolderExpandButton.GetExpandFlag();

	//	if (expandFlag)
	//	{
	//		int childPanelNum =	folderPanel->GetChildPanelUiEntityVector().size();

	//		shiftY += childPanelNum * panelHeight;


	//		const std::vector<UiEntity*>& childPanelVector = folderPanel->GetChildPanelUiEntityVector();
	//		for (int i = 0; i < childPanelVector.size(); ++i)
	//		{
	//			folderPanelQueue.push((FolderPanelEntity*)childPanelVector[i]);

	//		}

	//	}


	//}


	//FolderListPanelEntity* folderListPanelEntity = GetFolderListPanelEntity();

	////
	//folderListPanelEntity








}

void Quad::FolderPanelEntity::SetFolderPanelValid(bool state)
{

	SetDrawFlag(state);
	SetActiveFlag(state);
	SetSelectAvailableFlag(state);
	

	mChildFolderExpandButton->SetDrawFlag(state);
	mChildFolderExpandButton->SetActiveFlag(state);
	mChildFolderExpandButton->SetSelectAvailableFlag(state);


	mFolderIcon->SetDrawFlag(state);
	mFolderIcon->SetActiveFlag(state);
	//mFolderIcon.SetSFlag(state);

	mFolderNameText->SetTextDrawFlag(state);
	mFolderNameText->SetDrawFlag(state);
	//mFolderNameText.OnOffTextModifyFlag(false);
	
	//mFolderNameText.OnOffSelectFlag()
	








}

int Quad::FolderPanelEntity::GetChildFolderExpandFlag() const
{
	return mChildFolderExpandButton->GetExpandFlag();
}

void Quad::FolderPanelEntity::SetChildFolderExpandFlag(bool flag)
{
	if (flag == GetChildFolderExpandFlag())
		return;



	mChildFolderExpandButton->LogicalButtonClick();

	////마치 그 버튼이논리적으로 클릭된것으로 취급할수있다.
	//MouseEvent hoverEvent("Hover");
	//hoverEvent.SetHoverState(true);
	//mChildFolderExpandButton.OnEvent(&hoverEvent);

	//MouseEvent lButtonEvent("LButtonDown");
	//mChildFolderExpandButton.OnEvent(&lButtonEvent);

	//MouseEvent lButtonUpEvent("LButtonUp");
	//mChildFolderExpandButton.OnEvent(&lButtonUpEvent);


	


}

void Quad::FolderPanelEntity::SetLogicalFolderEntity(LogicalContentItemUiEntityFolder* destLogicalFolder)
{
	mDestFolderEntity = destLogicalFolder;
}

Quad::LogicalContentItemUiEntityFolder* Quad::FolderPanelEntity::GetLinkedLogicalFolder() const
{
	return mDestFolderEntity;
}

void Quad::FolderPanelEntity::SetSelectState()
{
	StateComponent* stateComponent = GetStateComponent();

	stateComponent->SetSelectState(true);

	SetColorItensity({ 0.5f,0.5f,0.5f });
	LogicalContentItemUiEntityFolder* destLogicalFolder = GetLinkedLogicalFolder();
	destLogicalFolder->ActivateContentItemUiPanelEntity(true);


}

