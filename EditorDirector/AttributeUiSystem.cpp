#include "System/AttributeUiSystem.h"
#include"EditorDirector/DefaultState.h"
#include"Core/SystemState.h"
#include"EditorDirector/DockingWindowController.h"

#include"Core/PanelUiEntity.h"
#include"Object/Camera/Camera.h"
#include"EditorDirector/TransformPropertyPanel.h"

#include"EditorDirector/AssetImageFrameEntity.h"
#include"EditorDirector/GameObjectSelectEvent.h"
#include"Object/Entity.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"Object/Line/Line.h"
#include"ObjectFactory/LineFactory.h"
#include"EditorDirector/NotifyDragAndDropEvent.h"
#include"Utility/MathHelper.h"
#include"Collision/CollisionHelper.h"
#include"EditorDirector/Window.h"


#include"EditorDirector/ScrollListPanel.h"
#include"EditorDirector/TextBoxPanel.H"
#include"EditorDirector/SubMeshListPanel.h"
#include"Core/EventDispatcher.h"

#include"EditorDirector/AttrUiSystemSelectObjectComponent.h"
#include"EditorDirector/IconEntity.h"
#include"EditorDirector/ExpandablePanel.h"
#include"Asset/Skeleton/Skeleton.h"
#include"Component/AnimationComponent.h"
#include"Asset/Animation/AnimationClip.h"
#include"ResourceManager/MeshManager.h"

#include"Object/Light/Light.h"
#include"ResourceFactory/TextureFactory/TextureFactory.h"
#include"ResourceManager/AnimationClipManager.h"
#include"ResourceManager/SkeletonManager.h"
#include"Core/MouseEvent.h"
#include"Core/Mouse.h"

#include"EditorDirector/ButtonUiEntitiy.h"
#include"Core/AnimationClipSplitter.h"

#include"Core/EventDispatcher.h"
#include"EditorDirector/LoadFileResponseEvent.h"

#include<cwctype>
#include"Component/UiColliderComponent.h"

#include"Object/Camera/OrthogoanlCamera.h"

#include"Object/Camera/CameraHelper.h"

#include"Core/ComponentIDGenerator.h"
#include"Core/TextBox.h"
#include"ClassListItemPanelEntity.h"

#include"System/GamePlaySystem.h"

#include"ObjectManager/ObjectManager.h"

#include"RegisterObjectHelper.h"
#include"System/FileUiUiSystem.h"
#include"FrameWindowMainSceneDirector.h"

#include"FrameWindow.h"
#include"ProjectDirector.h"

#include"EditorDirector/EditorDirector.h"


#include"AnimationStateTransitionGraphBasePanel.h"


Quad::AttributeUiSystem::AttributeUiSystem()
	:UiSystem(ESystemID::eAttributeMainSystem,"AttributeUiSystem"), mRootPanel(nullptr), mPositionPanel(nullptr), mScalePanel(nullptr), mRotationPanel(nullptr)
	,mMaterialPanel(nullptr),mSelectedObject(nullptr),mScrollClassListPanel(nullptr)
{


}

void Quad::AttributeUiSystem::Initialize(UINT clientWidth, UINT clientHeight, Quad::Map* map)
{
	UiSystem::Initialize(clientWidth, clientHeight, map);
//	SetPlayModeState(true);

	SetSystemState(new DefaultState("Default"));
	OnRuntimeMode();
	
	Map* currMap = GetMap();
	
	currMap->GetMapLayer(1).mDepthStencilBuffer = TextureManager::GetTexture("AnimationEdit3DPanelDepthStencilBuffer");
	currMap->GetMapLayer(1).mRenderTarget = static_cast<RenderTargetTexture*>(TextureManager::GetTexture("AnimationEdit3DPanelTexture"));





	int mapLayerIndex = 0;
	//mRootPanel = new ExpandablePanel("SystemRootPanel");
	//mRootPanel = static_cast<ExpandablePanel*>(currMap->CreateObject("ExpandablePanel"));
	
	mRootPanel = ExpandablePanel::Create(currMap, mapLayerIndex, 5000, 5000);
	mRootPanel->SetTexture("Gray.png");
	//mRootPanel->SetSize(5000, 5000);
	mRootPanel->SetPosition(2500, -1.0  * 2500 , 500);
	mRootPanel->RegisterAcceptEvent("MouseMove");
	mRootPanel->RegisterAcceptEvent("LButtonDown");
	mRootPanel->RegisterAcceptEvent("LButtonUp");
	mRootPanel->RegisterAcceptEvent("GameObjectSelect");
	mRootPanel->RegisterAcceptEvent("NotifyDragAndDrop");
	mRootPanel->RegisterAcceptEvent("Hover");

	mRootPanel->RegisterEventCallback("GameObjectSelect", [panel = mRootPanel](Event* pEvent)
		{
			GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;
			selectEvent->GetSelectObject()->GetTransform().SetDirtyFlag(true);	//��Ƽ�÷��׸� �Ѿ� �Ӽ��ǳڵ��� ������Ʈ�Ѵ�.

			const std::vector<UiEntity*>& childUiEntityVector = panel->GetChildPanelUiEntityVector();
			std::for_each(childUiEntityVector.begin(), childUiEntityVector.end(), [pEvent = pEvent](UiEntity* child) {
				child->OnEvent(pEvent);
				});






		});

	mRootPanel->RegisterEventCallback("NotifyDragAndDrop", [panel = mRootPanel](Event* pEvent)
		{
			NotifyDragAndDropEvent* event = (NotifyDragAndDropEvent*)pEvent;
			
			System* system = panel->GetSystem();
			Camera* camera = system->GetMap()->GetMainCamera();

			Ray ray;
			//Ŭ���̾�Ʈ�������� ���� �ٲ����
			
			POINT clientPos = { event->GetScreenPosX(),event->GetScreenPosY() };
			ScreenToClient(system->GetWindow()->GetWindowHandle(), &clientPos);

		
			CameraHelper::CalculateRay({ (float)clientPos.x,(float)clientPos.y }, system->GetViewPort(),
				*camera, true, ray);

			event->SetRay(ray);

			const std::vector<UiEntity*>& childUiEntityVector = panel->GetChildPanelUiEntityVector();
			std::for_each(childUiEntityVector.begin(), childUiEntityVector.end(), [pEvent = pEvent](UiEntity* child) {
				child->OnEvent(pEvent);
				});
		});












	//mRootPanel->SetDrawFlag(false);
	//AddEntity(mRootPanel);


	//create  GenerateClassEntity Panel
	CreateGenerateEntityPanel();
	//AddClassListItem("");

	//create transform panel 
	CreateTransformPanel();

	//create material panel ;
	CreateMaterialPanel();

	CreateAnimationPanel();


	//Camera* camera = GetMap()->GetMainCamera();
	//camera->GetTransform().SetPositionLocal({ (float)clientWidth / 2,-1.0f * (float)clientHeight / 2 , 0 });














}



void Quad::AttributeUiSystem::Update(float deltaTime, bool playMode)
{
	UiSystem::Update(deltaTime,playMode);

}

void Quad::AttributeUiSystem::OnResize(UINT clientWidth, UINT clientHeight)
{
	UiSystem::OnResize(clientWidth, clientHeight);
	D3D12_VIEWPORT viewport = GetViewPort();
	viewport.TopLeftY = ((DockingWindowController*)GetController())->GetTitleBarHeight();

	Camera *camera = GetMap()->GetMainCamera();
	OrthogoanlCamera* mainCamera = static_cast<OrthogoanlCamera*>(camera);
	
	
	SetViewPort(viewport.TopLeftX, viewport.TopLeftY , viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth);

	mainCamera->GetTransform().SetPositionLocal({ (float)clientWidth / 2,-1.0f * (float)clientHeight / 2 , 0 });
	mainCamera->SetViewWidthAndHeight(clientWidth, clientHeight);
	
}



void Quad::AttributeUiSystem::AddEntity(Object* entity,int mapLayerID, bool engineEditObject)
{
	Map* map = GetMap();
	//map->AddObject((Object*)entity,mapLayerID);

}

//void Quad::AttributeUiSystem::GetEntity(std::vector<Object*>& oUiEntityVector)
//{
//
//	//Map* map = GetMap();
//	////ui�ý��ۿ����� ������ �׳� ��Ʈ�ڽ� ��ƼƼ���� ��� �����´�
//	//map->GetObjectVectorInViewFrustum(oUiEntityVector);
//
//}

//const std::vector<Quad::MapLayer>& Quad::AttributeUiSystem::GetEntity()
//{
//	// TODO: ���⿡ return ���� �����մϴ�.
//}

void Quad::AttributeUiSystem::AddClassListItem(const std::string& className)
{
	Map* map = GetMap();

	
		ClassListItemPanelEntity* testPanel = ClassListItemPanelEntity::Create(map, 0, className, 400, 30);
		testPanel->SetDrawFlag(false);

		mScrollClassListPanel->AddChildPanelUiEntity(testPanel);
	
	mScrollClassListPanel->ActivateChildListPanel(mScrollClassListPanel->GetActiveChildPanelNum());

}


void Quad::AttributeUiSystem::SetSelectedObject(Object* object)
{
	mSelectedObject = object;


	//transform 
	const DirectX::XMFLOAT3 & pos = mSelectedObject->GetTransform().GetPositionWorld();

	const DirectX::XMFLOAT3& scale = mSelectedObject->GetTransform().GetScaleWorld();

	const DirectX::XMFLOAT3& rotation = mSelectedObject->GetTransform().GetRotationWorld();

	mPositionPanel->SetSelectedObject(object);
	mScalePanel->SetSelectedObject(object);
	mRotationPanel->SetSelectedObject(object);



	mPositionPanel->SetXYZ(pos);
	mScalePanel->SetXYZ(scale);
	mRotationPanel->SetXYZ(rotation);

	


	//material 












	//









}

Quad::Object* Quad::AttributeUiSystem::GetSelectedObject() const
{
	return mSelectedObject;
}

void Quad::AttributeUiSystem::CreateGenerateEntityPanel()
{

 	Map* map = GetMap();

	int mapLayerIndex = 0;
	PanelUiEntity* generateEntityPanel = PanelUiEntity::Create(map, mapLayerIndex);
	generateEntityPanel->SetTexture("Gray.png");
	generateEntityPanel->SetSize(5000, 250);
	generateEntityPanel->GetTransform().SetIndependentScaleFlag(true);
	generateEntityPanel->SetSelectAvailableFlag(false);
	mRootPanel->AddChildPanelUiEntity(generateEntityPanel, reinterpret_cast<const char*>(u8"엔티티생성"));

	float generateEntityPanelWidth = generateEntityPanel->GetWidth();
	float generateEntityPanelHeight = generateEntityPanel->GetHeight();
	float margin = 10;

	ScrollListPanel* scrollClassListPanel = ScrollListPanel::Create(map, mapLayerIndex, 600, 200, 100);
	mScrollClassListPanel = scrollClassListPanel;

	float scrollClassListPanelWidth = scrollClassListPanel->GetWidth();
	float scrollClassListPanelHeight = scrollClassListPanel->GetHeight();

	scrollClassListPanel->GetTransform().SetIndependentScaleFlag(true);
	scrollClassListPanel->GetTransform().SetIndependentRotationFlag(true);
	scrollClassListPanel->SetTexture("33343c.png");
	scrollClassListPanel->SetSelectAvailableFlag(false);
	generateEntityPanel->AddChildPanelUiEntity(scrollClassListPanel);



	DirectX::XMFLOAT3 scrollClassListPanelPos = { -generateEntityPanelWidth/2 + scrollClassListPanelWidth/2 + margin  ,  scrollClassListPanelHeight/2 - margin - scrollClassListPanelHeight/2 ,-1 };
	scrollClassListPanel->GetTransform().SetPositionLocal(scrollClassListPanelPos);


	float scrollClassListTagPanelWidth = 200;
	float scrollClassListTagPanelHeight = 30;

	TextBoxPanel* scrollClassListTagPanel = TextBoxPanel::Create(map, mapLayerIndex, scrollClassListTagPanelWidth, scrollClassListTagPanelHeight);
	scrollClassListTagPanel->GetTransform().SetIndependentScaleFlag(true);
	scrollClassListTagPanel->GetTransform().SetIndependentRotationFlag(true);
	scrollClassListTagPanel->SetSelectAvailableFlag(false);
	scrollClassListTagPanel->SetTexture("Black.png");
	scrollClassListTagPanel->SetText(reinterpret_cast<const char*>(u8"엔티티 클래스 목록"));
	//scrollClassListTagPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	

	generateEntityPanel->AddChildPanelUiEntity(scrollClassListTagPanel);
	DirectX::XMFLOAT3 scrollClassListTagPanelPos = { scrollClassListPanelPos.x - scrollClassListPanelWidth / 2 + scrollClassListTagPanelWidth / 2,
	scrollClassListPanelPos.y + scrollClassListPanelHeight / 2 + scrollClassListTagPanelHeight / 2,-1 };
	scrollClassListTagPanel->SetPosition(scrollClassListTagPanelPos);



	ButtonUiEntity* createEntityButton = ButtonUiEntity::Create(map, mapLayerIndex);
	//PanelUiEntity* createEntityButton = PanelUiEntity::Create(map, mapLayerIndex);
	createEntityButton->GetTransform().SetIndependentRotationFlag(true);
	createEntityButton->GetTransform().SetIndependentScaleFlag(true);
	float createEntityButtonWidth = 60;
	float createEntityButtonHeight =30;
	createEntityButton->SetSize(createEntityButtonWidth, createEntityButtonHeight);
	createEntityButton->SetTexture(reinterpret_cast<const char*>(u8"버튼21.png"));
	createEntityButton->SetEntireSelectAvailableFlag(false);

	generateEntityPanel->AddChildPanelUiEntity(createEntityButton);


	DirectX::XMFLOAT3 createEntityButtonPos = { scrollClassListPanelPos.x + scrollClassListPanelWidth/2 +margin + createEntityButtonWidth/2 , 
		scrollClassListPanelPos.y + scrollClassListPanelHeight/2 - createEntityButtonHeight/2 , -1 };
	createEntityButton->SetPosition(createEntityButtonPos);

	



	createEntityButton->SetLButtonUpCallback([classListPanel = scrollClassListPanel]() {

		ClassListItemPanelEntity* currentSelectedClassPanel = static_cast<ClassListItemPanelEntity*>(classListPanel->GetCurrentSelectedChildPanel());
		const std::string& className = currentSelectedClassPanel->GetEntityClassName();

		GamePlaySystem* gamePlay3DSystem = GamePlaySystem::GetInstance();

		Map* currentMap = gamePlay3DSystem->GetMap();


		//현재 바라보는 위치에 생성 , 적절한 초기화필요

		

		Object* object = currentMap->CreateObject(className, 0);
		object->Initialize();
		object->DefaultCreatingInitialize();
		
		});



	


	//EditObjectManager *	editObjectManager =	EditObjectManager::GetInstance();
	//const auto & objectConstructorTable = editObjectManager->GetObjectConstructTable();


	//이시점에서는 기본적인 엔티티들이 등록되는것인데 이때 하지말고, project를 로드할때 해야된다..
	//for (const auto& objectName : ObjectTable::GetObjectTable())
	//{
	//	AddClassListItem(objectName);
	//}


}

void Quad::AttributeUiSystem::CreateTransformPanel()
{
	Map* map = GetMap();

	int mapLayerIndex = 0;

	//PanelUiEntity* transformPanel = new PanelUiEntity("SystemTransformPanel");
//	PanelUiEntity* transformPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* transformPanel = PanelUiEntity::Create(map, mapLayerIndex);

	transformPanel->SetTexture("Gray.png");
	transformPanel->SetSize(5000, 200);
	transformPanel->GetTransform().SetIndependentScaleFlag(true);
	transformPanel->SetSelectAvailableFlag(false);
	mRootPanel->AddChildPanelUiEntity(transformPanel,reinterpret_cast<const char*>(u8"트랜스폼"));

	//transformPanel->SetPosition(0, mRootPanel->GetHeight() / 2 - transformPanel->GetHeight() / 2, -1);
	transformPanel->RegisterEventCallback("GameObjectSelect", [panel =transformPanel](Event * pEvent)
		{
			const std::vector<UiEntity*>& childPanelUiEntityVector = panel->GetChildPanelUiEntityVector();
			
			for (int i = 0; i < childPanelUiEntityVector.size(); ++i)
			{
				childPanelUiEntityVector[i]->OnEvent(pEvent);
			}
		});












	float propertypanelLocalY = transformPanel->GetHeight() / 2;

//TransformPropertyPanel* positionPanel = static_cast<TransformPropertyPanel*>(map->CreateObject("TransformPropertyPanel"));
	TransformPropertyPanel* positionPanel = TransformPropertyPanel::Create(map, mapLayerIndex, 5000, 60, reinterpret_cast<const char*>(u8"위치"));

	mPositionPanel = positionPanel;

	//positionPanel->SetSystem(this);
	//positionPanel->Initialize(5000,60,"위치");
	positionPanel->SetTexture("DarkGray.png");
	//positionPanel->SetSize(5000, 60);
	positionPanel->GetTransform().SetIndependentScaleFlag(true);
	transformPanel->AddChildPanelUiEntity(positionPanel);
	positionPanel->SetSelectAvailableFlag(false);
	propertypanelLocalY -= positionPanel->GetHeight() / 2;
	positionPanel->SetPosition(0, propertypanelLocalY, -1);

	//positionPanel->SetDrawFlag(false);
	//select�ɋ� �� ������Ʈ�� Ʈ�������� �ݹ��� ����ؾ��Ѵ�.
	//
	QHANDLE callbackHandle = 0;
	auto transformChangeCallbackLamda = [callbackHandle](Event* pEvent,
		TransformPropertyPanel* panel, int propertyIndex) mutable
	{
		GameObjectSelectEvent* event = (GameObjectSelectEvent*)pEvent;

		Object* selectedObject = panel->GetSelectedObject();
		if (selectedObject != nullptr)
		{
			selectedObject->GetTransform().RemoveTransformChangeCallback(callbackHandle);
		}

		selectedObject = event->GetSelectObject();

		if (selectedObject != nullptr)
		{
			panel->SetSelectedObject(selectedObject);
			DirectX::XMFLOAT3 xyz;
			switch(propertyIndex)
			{
			case 0://��ġ
				xyz = selectedObject->GetTransform().GetPositionWorld();
				

				break;
			case 1: //ȸ��
				xyz = selectedObject->GetTransform().GetRotationWorld();

				
				break;
			case 2: //������
				xyz = selectedObject->GetTransform().GetScaleWorld();
				break;
			}

			panel->SetXYZ(xyz);



			callbackHandle = selectedObject->GetTransform().RegisterTransformChangeCallback([panel, propertyIndex]() {

				Object* selectedObject = panel->GetSelectedObject();

				DirectX::XMFLOAT3  xyz;

				switch (propertyIndex)
				{
				case 0:
				{
					xyz =selectedObject->GetTransform().GetPositionWorld();
				}
				break;
				case 1:
				{
					xyz=selectedObject->GetTransform().GetRotationWorld();

				}
				break;
				case 2:
				{
					xyz=selectedObject->GetTransform().GetScaleWorld();
				}
				break;
				}

				panel->SetXYZ(xyz);
				});
		}

	};

	positionPanel->RegisterEventCallback("GameObjectSelect", std::bind(transformChangeCallbackLamda, std::placeholders::_1, positionPanel,
		0));

	
	
	




	//positionPanel->SetUpdateCallback([panel = positionPanel](float deltaTime) {

	//		Object * selectedObject = panel->GetSelectedObject();
	//		
	//		if (selectedObject != nullptr && selectedObject->GetTransform().GetDirtyFlag())
	//		{
	//			panel->SetXYZ(selectedObject->GetTransform().GetPositionWorld());
	//		}

	//	});



	positionPanel->SetXTextBoxEnterCallback([panel = positionPanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float x = 0;
			try
			{
				x = std::stof(panel->GetTextX());
			}
			catch (const std::invalid_argument & invalidArgument)
			{//����
				return;
			}
			DirectX::XMFLOAT3 pos = selectedObject->GetTransform().GetPositionWorld();
			
			pos.x = x;
			selectedObject->GetTransform().SetPositionWorld(pos);

		});

	positionPanel->SetYTextBoxEnterCallback([panel = positionPanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float y = 0;
			try
			{
				y = std::stof(panel->GetTextY());
			}
			catch (const std::invalid_argument& invalidArgument)
			{//����
				return;
			}
			DirectX::XMFLOAT3 pos = selectedObject->GetTransform().GetPositionWorld();
			pos.y = y;
			selectedObject->GetTransform().SetPositionWorld(pos);

		});

	positionPanel->SetZTextBoxEnterCallback([panel = positionPanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float z = 0;
			try
			{
				z = std::stof(panel->GetTextZ());
			}
			catch (const std::invalid_argument& invalidArgument)
			{//����
				return;
			}
			DirectX::XMFLOAT3 pos = selectedObject->GetTransform().GetPositionWorld();
			pos.z = z;
			selectedObject->GetTransform().SetPositionWorld(pos);

		});













//	TransformPropertyPanel* scalePanel = new TransformPropertyPanel("SystemTransformScalePanel");
	//TransformPropertyPanel* scalePanel = static_cast<TransformPropertyPanel*>(map->CreateObject("TransformPropertyPanel"));
	TransformPropertyPanel* scalePanel = TransformPropertyPanel::Create(map, mapLayerIndex, 5000, 60, reinterpret_cast<const char*>(u8"스케일"));

	mScalePanel = scalePanel;

	//scalePanel->SetSystem(this);
	//scalePanel->Initialize(5000, 60 ,"스케일");
	scalePanel->SetTexture("DarkGray.png");
	scalePanel->SetSize(5000, 60);
	scalePanel->GetTransform().SetIndependentScaleFlag(true);
	scalePanel->SetSelectAvailableFlag(false);
	transformPanel->AddChildPanelUiEntity(scalePanel);
	propertypanelLocalY -= positionPanel->GetHeight();
	scalePanel->SetPosition(0, propertypanelLocalY, -1);

	scalePanel->RegisterEventCallback("GameObjectSelect", std::bind(transformChangeCallbackLamda, std::placeholders::_1, scalePanel,
		2));











	scalePanel->SetXTextBoxEnterCallback([ panel = scalePanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float x = 0;
			try
			{
				x = std::stof(panel->GetTextX());
			}
			catch (const std::invalid_argument& invalidArgument)
			{//����
				return;
			}
			DirectX::XMFLOAT3 scale = selectedObject->GetTransform().GetScaleWorld();
			scale.x = x;
			selectedObject->GetTransform().SetScaleWorld(scale);

		});

	scalePanel->SetYTextBoxEnterCallback([panel = scalePanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float y = 0;
			try
			{
				y = std::stof(panel->GetTextY());
			}
			catch (const std::invalid_argument& invalidArgument)
			{
				return;
			}
			DirectX::XMFLOAT3 scale = selectedObject->GetTransform().GetScaleWorld();
			scale.y = y;
			selectedObject->GetTransform().SetScaleWorld(scale);

		});

	scalePanel->SetZTextBoxEnterCallback([panel = scalePanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float z = 0;
			try
			{
				z = std::stof(panel->GetTextZ());
			}
			catch (const std::invalid_argument& invalidArgument)
			{
				return;
			}
			DirectX::XMFLOAT3 scale = selectedObject->GetTransform().GetScaleWorld();
			scale.z = z;
			selectedObject->GetTransform().SetScaleWorld(scale);

		});






























//	TransformPropertyPanel* rotationPanel = new TransformPropertyPanel("SystemTransformRotationPanel");
	//TransformPropertyPanel* rotationPanel = static_cast<TransformPropertyPanel*>(map->CreateObject("TransformPropertyPanel"));
	TransformPropertyPanel* rotationPanel = TransformPropertyPanel::Create(map, mapLayerIndex, 5000, 60, reinterpret_cast<const char*>(u8"회전"));
	mRotationPanel = rotationPanel;

//	rotationPanel->SetSystem(this);
	//rotationPanel->Initialize(5000, 60,"회전");
	rotationPanel->SetTexture("DarkGray.png");
	//rotationPanel->SetSize(5000, 60);
	rotationPanel->GetTransform().SetIndependentScaleFlag(true);
	rotationPanel->SetSelectAvailableFlag(false);
	transformPanel->AddChildPanelUiEntity(rotationPanel);

	propertypanelLocalY -= positionPanel->GetHeight();
	rotationPanel->SetPosition(0, propertypanelLocalY, -1);
	rotationPanel->RegisterEventCallback("GameObjectSelect", std::bind(transformChangeCallbackLamda, std::placeholders::_1, rotationPanel,
		1));


	


	rotationPanel->SetXTextBoxEnterCallback([panel = rotationPanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float x = 0;
			try
			{
				x = std::stof(panel->GetTextX());
			}
			catch (const std::invalid_argument& invalidArgument)
			{
				return;
			}
			DirectX::XMFLOAT3 rotation = selectedObject->GetTransform().GetRotationWorld();
			rotation.x= x;
			selectedObject->GetTransform().SetRotationWorld(rotation);

		});

	rotationPanel->SetYTextBoxEnterCallback([panel = rotationPanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float y = 0;
			try
			{
				y = std::stof(panel->GetTextY());
			}
			catch (const std::invalid_argument& invalidArgument)
			{//����
				return;
			}
			DirectX::XMFLOAT3 rotation = selectedObject->GetTransform().GetRotationWorld();
			rotation.y = y;
			selectedObject->GetTransform().SetRotationWorld(rotation);

		});


	rotationPanel->SetZTextBoxEnterCallback([panel = rotationPanel]()
		{

			Object* selectedObject = panel->GetSelectedObject();
			if (selectedObject == nullptr)
				return;

			float z = 0;
			try
			{
				z = std::stof(panel->GetTextZ());
			}
			catch (const std::invalid_argument& invalidArgument)
			{//����
				return;
			}
			DirectX::XMFLOAT3 rotation = selectedObject->GetTransform().GetRotationWorld();
			rotation.z = z;
			selectedObject->GetTransform().SetRotationWorld(rotation);

		});














}

void Quad::AttributeUiSystem::CreateMaterialPanel()
{

	Map* map = GetMap();
	int mapLayerIndex = 0;
	//mMaterialPanel = new PanelUiEntity("SystemMaterialPanel");
	//mMaterialPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	mMaterialPanel = PanelUiEntity::Create(map, mapLayerIndex);

	//mMaterialPanel->SetSystem(this);
	//mMaterialPanel->Initialize();
	mMaterialPanel->SetTexture("DarkGray.png");
	mMaterialPanel->SetSize(5000, 1000);
	mMaterialPanel->GetTransform().SetIndependentScaleFlag(true);
	mMaterialPanel->SetSelectAvailableFlag(false);
	mRootPanel->AddChildPanelUiEntity(mMaterialPanel, reinterpret_cast<const char*>(u8"머터리얼"));
//	mMaterialPanel->SetPosition(0, mRootPanel->GetHeight() / 2 - 200 - mMaterialPanel->GetHeight() / 2, -1);


	auto sendEventToChildLamda = [](PanelUiEntity* panel, Event* pEvent) {

		const std::vector<UiEntity*>& childPanelUiEntityVector = panel->GetChildPanelUiEntityVector();
		for (int i = 0; i < childPanelUiEntityVector.size(); ++i)
		{
			childPanelUiEntityVector[i]->OnEvent(pEvent);
		}
	};

	mMaterialPanel->RegisterEventCallback("GameObjectSelect", std::bind(sendEventToChildLamda, mMaterialPanel, std::placeholders::_1));
	mMaterialPanel->RegisterEventCallback("NotifyDragAndDrop", std::bind(sendEventToChildLamda, mMaterialPanel, std::placeholders::_1));







	//TextBox* materialTagTextBox = new TextBox(L"MaterialPanelTagTextBox");
	//materialTagTextBox->SetSystem(this);
	//materialTagTextBox->Initialize(4);
	//materialTagTextBox->SetTexture(L"Black.png");
	//materialTagTextBox->SetSize(60, 20);
	//materialTagTextBox->SetText(L"���͸���");
	//materialTagTextBox->SetTextColor({ 1.0f,1.0f,1.0f });
	//materialTagTextBox->SetSelectAvailableFlag(false);
	//materialTagTextBox->GetTransform().SetIndependentScaleFlag(true);

	//mMaterialPanel->AddChildPanelUiEntity(materialTagTextBox);
	//materialTagTextBox->SetPosition(-1 * mMaterialPanel->GetWidth() / 2 + materialTagTextBox->GetWidth() / 2, mMaterialPanel->GetHeight() / 2 - materialTagTextBox->GetHeight() / 2, -1);


	DirectX::XMFLOAT3 pos;



	//main panel 
	//materialAsset panel,submesh list panel �� �ڽ����� ������.
	//PanelUiEntity* materialMainPanel = new PanelUiEntity("MaterialMainPanel");
	//PanelUiEntity* materialMainPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* materialMainPanel = PanelUiEntity::Create(map, mapLayerIndex);

	
	materialMainPanel->SetTexture("DarkGray.png");
	materialMainPanel->SetSize(mMaterialPanel->GetWidth(), 200);
	materialMainPanel->SetSelectAvailableFlag(false);
	materialMainPanel->GetTransform().SetIndependentScaleFlag(true);
	materialMainPanel->GetTransform().SetIndependentRotationFlag(true);
	mMaterialPanel->AddChildPanelUiEntity(materialMainPanel);
	pos.x = 0;
	pos.y = mMaterialPanel->GetHeight() / 2 - materialMainPanel->GetHeight() / 2;
	pos.z = -1;
	materialMainPanel->GetTransform().SetPositionLocal(pos);
	materialMainPanel->RegisterEventCallback("GameObjectSelect", std::bind(sendEventToChildLamda, materialMainPanel, std::placeholders::_1));
	materialMainPanel->RegisterEventCallback("NotifyDragAndDrop", std::bind(sendEventToChildLamda, materialMainPanel, std::placeholders::_1));










	//AssetImageFrameEntity* materialAssetFrameEntity = nullptr;
	IconEntity* materialAssetFrameEntity = nullptr;
	PanelUiEntity* materialAssetPanel = nullptr;

	CreateMaterialAssetPanel(materialMainPanel, "MaterialAssetPanel", "Material", materialAssetPanel, materialAssetFrameEntity);
	pos.x = -materialMainPanel->GetWidth() / 2 + 50 + materialAssetPanel->GetWidth() / 2;
	pos.y = 0;
	pos.z = -1;
	materialAssetPanel->GetTransform().SetPositionLocal(pos);

	QHANDLE matDirtyCallbackHandle = 0;
	int textureMapTypeTempNum = 0;
	auto textureMapGameObjectSelectLamda = [handle = matDirtyCallbackHandle](Event* pEvent, IconEntity* frameEntity, int textureMapTypeNum)  mutable {


		AttrUiSystemSelectObjectComponent * selectComponent =(AttrUiSystemSelectObjectComponent*)frameEntity->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());


		//���͸��� ����� �ݹ��Լ��� �ڵ��� ���ٰ� �����ؾ��Ѵ�.

		//���� select object���� �ݹ��� ����
		Object* preSelectedObject = selectComponent->GetSelectedObject();
		int subMeshIndex = selectComponent->GetSubMeshIndex();

		if (preSelectedObject != nullptr && preSelectedObject->GetObjectType() == EObjectType::eEntity)
		{
			Entity* preEntity = (Entity*)preSelectedObject;
			MeshComponent * meshComponent = static_cast<MeshComponent*>(preEntity->GetModel()->GetComponent(EComponentType::eMeshComponent));
			meshComponent->GetSubMesh()[subMeshIndex].mModelMaterial.RemoveDirtyCallback(handle);
		}

		handle = 0;

		GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;
		Object* selectedObject = selectEvent->GetSelectObject();
		subMeshIndex = selectEvent->GetCurrentSubMeshIndex();
		selectComponent->SetSelectedObject(selectedObject);
		selectComponent->SetSubMeshIndex(subMeshIndex);
		switch (selectedObject->GetObjectType())
		{
		case EObjectType::eEntity:
		{

			Entity* entity = (Entity*)selectedObject;

			Material* mat = &entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial;

			handle = mat->AddDirtyCallback([mat, frameEntity, textureMapTypeNum]() {
				Texture* texture = nullptr;
				switch (textureMapTypeNum)
				{
				case 0: //material
					texture = mat->GetDiffuseMap();
					if (texture == nullptr)
						texture = TextureManager::GetTexture("MaterialAsset.png");
					break;
				case 1: //diffuse
					texture = mat->GetDiffuseMap();

					break;

				case 2:		//normal

					texture = mat->GetNormalMap();

					break;
				}
				if (texture != nullptr)
					frameEntity->SetTexture(texture);
				});





			Texture* texture = nullptr;
			switch (textureMapTypeNum)
			{
			case 0: //material
				texture = entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial.GetDiffuseMap();
				if (texture == nullptr)
					texture = TextureManager::GetTexture("MaterialAsset.png");

				break;
			case 1:	//diffuse

				texture = entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial.GetDiffuseMap();
				break;

			case 2: //normal

				texture = entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial.GetNormalMap();
				break;
			}

			if (texture != nullptr)
				frameEntity->SetTexture(texture);
		}
		break;
		}



	};


	materialAssetFrameEntity->RegisterEventCallback("GameObjectSelect",std::bind(textureMapGameObjectSelectLamda, std::placeholders::_1,
		materialAssetFrameEntity,0));






	materialAssetFrameEntity->RegisterEventCallback("NotifyDragAndDrop", [frame = materialAssetFrameEntity](Event* pEvent)
		{
			AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*)frame->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());




			Object* selectedObject = selectComponent->GetSelectedObject();
			if (selectedObject == nullptr || selectedObject->GetObjectType() != EObjectType::eEntity)
				return;
			Entity* selectedEntity = (Entity*)selectedObject;


			NotifyDragAndDropEvent* event = (NotifyDragAndDropEvent*)pEvent;
			const Ray& ray = event->GetRay();
			float t = FLT_MAX;
			if (!CollisionHelper::Intersect(frame->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t))
			{
				DirectX::XMFLOAT3 pos = ray.GetOrigin();
				frame->SetColorItensity({ 1.0f,1.0f,1.0f });
				return;
			}


			ContentItem* contentItem = event->GetContentItem();
			if (contentItem->GetEContentItemType() == EContentItemType::eAsset)
			{
				Asset* asset = (Asset*)contentItem;
				if (asset->GetAssetType() == EAssetType::eMaterial)
				{
					Material* mat = (Material*)asset;
					frame->SetColorItensity({ 2.0f,2.0f,2.0f });

					if (event->GetLButtonUpState())
					{
						int subMeshIndex = selectComponent->GetSubMeshIndex();
					
						selectedEntity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial = *mat;
						frame->SetColorItensity({ 1.0f,1.0f,1.0f });
					}
				}
			}

		});


	materialAssetPanel->RegisterEventCallback("NotifyDragAndDrop", std::bind(sendEventToChildLamda, materialAssetPanel,
		std::placeholders::_1));























	//main panel - submesh scroll list panel 

	//ScrollListPanel* subMeshScrollListPanel = new ScrollListPanel("SubMeshScrollListPanel");
	//ScrollListPanel* subMeshScrollListPanel = static_cast<ScrollListPanel*>(map->CreateObject("ScrollListPanel"));
	ScrollListPanel* subMeshScrollListPanel = ScrollListPanel::Create(map, mapLayerIndex, 400, materialMainPanel->GetHeight(), 100);


//	subMeshScrollListPanel->SetSystem(this);
	//subMeshScrollListPanel->Initialize(400, materialMainPanel->GetHeight() ,100);
	subMeshScrollListPanel->GetTransform().SetIndependentScaleFlag(true);
	subMeshScrollListPanel->GetTransform().SetIndependentRotationFlag(true);
	subMeshScrollListPanel->SetTexture("33343c.png");
	subMeshScrollListPanel->SetSelectAvailableFlag(false);
	materialMainPanel->AddChildPanelUiEntity(subMeshScrollListPanel);
	pos = materialAssetPanel->GetTransform().GetPositionLocal();
	pos.x += materialAssetPanel->GetWidth()/2 + 50  + subMeshScrollListPanel->GetWidth()/2 ;
	pos.y = 0;
	pos.z = -1;
	subMeshScrollListPanel->GetTransform().SetPositionLocal(pos);
	subMeshScrollListPanel->RegisterEventCallback("GameObjectSelect", [panel = subMeshScrollListPanel](Event* pEvent) {



		GameObjectSelectEvent* event = (GameObjectSelectEvent*)pEvent;
		Object * selectedObject = event->GetSelectObject();


		/*if (!panel->GetDrawFlag())
			return;*/

		if (selectedObject->GetObjectType() == EObjectType::eEntity)
		{
			panel->SetSelectedObject(selectedObject);
			Entity* entity = (Entity*)selectedObject;


			const std::vector<ModelSubMesh> & subMeshVector = entity->GetModel()->GetMeshComponent()->GetSubMesh();
			///activate subMeshVector.size()������ ����Ʈ�� Ȱ��ȭ

			panel->ActivateChildListPanel(subMeshVector.size());


			const std::vector<UiEntity*>& childPanelVector = panel->GetChildPanelUiEntityVector();

			for (int i = 1; i <=subMeshVector.size(); ++i)
			{
				//SCROLL BAR�ϳ��� 0���������� . 
				childPanelVector[i]->OnEvent(pEvent);
			}
		}
		});




	for (int i = 0; i < 100; ++i)
	{
		

	//	SubMeshListPanel* subMeshPanel1 = new SubMeshListPanel("SubMeshListPanel" + std::to_string(i));
		//SubMeshListPanel* subMeshPanel1 = static_cast<SubMeshListPanel*>(map->CreateObject("SubMeshListPanel"));
		SubMeshListPanel* subMeshPanel1 = SubMeshListPanel::Create(map, mapLayerIndex, 100, 20);
	//	subMeshPanel1->SetSystem(this);
	//	subMeshPanel1->Initialize(100, 20);
		subMeshPanel1->SetListNum(i);
		subMeshPanel1->GetTransform().SetIndependentScaleFlag(true);
		subMeshPanel1->GetTransform().SetIndependentRotationFlag(true);
		subMeshPanel1->SetTexture("Transparent.png");
		subMeshPanel1->SetSelectAvailableFlag(false);
		subMeshPanel1->SetDrawFlag(false);
		//subMeshPanel1->SetActiveFlag(false);
		subMeshScrollListPanel->AddChildPanelUiEntity(subMeshPanel1);
	}

	subMeshScrollListPanel->ActivateChildListPanel(0);
























	//subpanel1 


	//PanelUiEntity* materialAssetMapPanel = new PanelUiEntity("MaterialAssetMapPanel");
//	PanelUiEntity* materialAssetMapPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* materialAssetMapPanel = PanelUiEntity::Create(map, mapLayerIndex);

	materialAssetMapPanel->SetTexture("08252b.png");
	materialAssetMapPanel->SetSelectAvailableFlag(false);
	materialAssetMapPanel->GetTransform().SetIndependentScaleFlag(true);
	materialAssetMapPanel->GetTransform().SetIndependentRotationFlag(true);
	materialAssetMapPanel->SetSize(200, 2000);
	materialAssetMapPanel->RegisterEventCallback("GameObjectSelect", std::bind(sendEventToChildLamda, materialAssetMapPanel, std::placeholders::_1));
	materialAssetMapPanel->RegisterEventCallback("NotifyDragAndDrop", std::bind(sendEventToChildLamda, materialAssetMapPanel, std::placeholders::_1));







	mMaterialPanel->AddChildPanelUiEntity(materialAssetMapPanel);
	pos = materialAssetPanel->GetTransform().GetPositionLocal();
	pos.x = -mMaterialPanel->GetWidth() / 2 + materialAssetMapPanel->GetWidth() / 2;
	pos.y = mMaterialPanel->GetHeight() / 2 - materialMainPanel->GetHeight() - materialAssetMapPanel->GetHeight() / 2;
	pos.z = -1;
	materialAssetMapPanel->GetTransform().SetPositionLocal(pos);





	IconEntity* diffuseMapFrameEntity = nullptr;
	PanelUiEntity* diffuseMapPanel = nullptr;
	CreateMaterialAssetPanel(materialAssetMapPanel, "DiffuseMapPanel", "DiffuseMap", diffuseMapPanel, diffuseMapFrameEntity);

	pos.x = 0;
	pos.y = materialAssetMapPanel->GetHeight() / 2 - 50 - diffuseMapPanel->GetHeight() / 2;
	pos.z = -1;
	diffuseMapPanel->GetTransform().SetPositionLocal(pos);


	textureMapTypeTempNum = 0;
	diffuseMapFrameEntity->RegisterEventCallback("GameObjectSelect", [textureMapGameObjectSelectLamda, frameEntity = diffuseMapFrameEntity,
		textureMapTypeIndex=1](Event* pEvent) mutable {
		textureMapGameObjectSelectLamda(pEvent, frameEntity, textureMapTypeIndex);
		});


	IconEntity* normalMapFrameEntity = nullptr;
	PanelUiEntity* normalMapPanel = nullptr;
	CreateMaterialAssetPanel(materialAssetMapPanel,"NormalMapPanel", "NormalMap", normalMapPanel, normalMapFrameEntity);
	pos.y -= (50 + diffuseMapPanel->GetHeight() / 2 + normalMapPanel->GetHeight() / 2);
	normalMapPanel->GetTransform().SetPositionLocal(pos);

	textureMapTypeTempNum = 0;
	

	normalMapFrameEntity->RegisterEventCallback("GameObjectSelect", [textureMapGameObjectSelectLamda, frameEntity = normalMapFrameEntity,
		textureMapTypeIndex = 2](Event* pEvent) mutable {
		textureMapGameObjectSelectLamda(pEvent, frameEntity, textureMapTypeIndex);
		});




	//subpanel 2


//	PanelUiEntity* subPanel2Entity = new PanelUiEntity("MaterialSubPanel2");
	//PanelUiEntity* subPanel2Entity = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* subPanel2Entity = PanelUiEntity::Create(map, mapLayerIndex);


	subPanel2Entity->GetTransform().SetIndependentScaleFlag(true);
	subPanel2Entity->GetTransform().SetIndependentRotationFlag(true);
	subPanel2Entity->SetSelectAvailableFlag(false);
	subPanel2Entity->SetSize(mMaterialPanel->GetWidth(), mMaterialPanel->GetHeight());
	subPanel2Entity->SetTexture("33343c.png");
	mMaterialPanel->AddChildPanelUiEntity(subPanel2Entity);
	pos.x = -mMaterialPanel->GetWidth() / 2 + materialAssetMapPanel->GetWidth() + subPanel2Entity->GetWidth() / 2;
	pos.y = mMaterialPanel->GetHeight() / 2 - materialMainPanel->GetHeight() - subPanel2Entity->GetHeight() / 2;
	pos.z = -1;
	subPanel2Entity->GetTransform().SetPositionLocal(pos);
	subPanel2Entity->RegisterEventCallback("GameObjectSelect", std::bind(sendEventToChildLamda,subPanel2Entity,std::placeholders::_1));






	CreateMaterialSubPanel2(subPanel2Entity);











}

void Quad::AttributeUiSystem::CreateMaterialSubPanel2(PanelUiEntity* subPanel2)
{

	Map* map = GetMap();
	int mapLayerIndex = 0;
	DirectX::XMFLOAT3 pos;

	//fresnelR0
//	TransformPropertyPanel* fresnelR0Panel = new TransformPropertyPanel("fresnelR0Panel");
//	TransformPropertyPanel* fresnelR0Panel = static_cast<TransformPropertyPanel*>(map->CreateObject("TransformPropertyPanel"));
	TransformPropertyPanel* fresnelR0Panel = TransformPropertyPanel::Create(map, mapLayerIndex, 2000, 50, "fresnelR0",3);


	//fresnelR0Panel->SetSystem(this);
//	fresnelR0Panel->Initialize(2000, 50, "fresnelR0", 3);
	fresnelR0Panel->GetTransform().SetIndependentScaleFlag(true);
	fresnelR0Panel->GetTransform().SetIndependentRotationFlag(true);
	fresnelR0Panel->SetSelectAvailableFlag(false);
	fresnelR0Panel->SetTexture("Gray.png");

	subPanel2->AddChildPanelUiEntity(fresnelR0Panel);
	pos.x = -subPanel2->GetWidth() / 2 + fresnelR0Panel->GetWidth() / 2;
	pos.y = subPanel2->GetHeight() / 2 - fresnelR0Panel->GetHeight() / 2;
	pos.z = -1;
	fresnelR0Panel->GetTransform().SetPositionLocal(pos);

	AttrUiSystemSelectObjectComponent* selectObjectComponent = new AttrUiSystemSelectObjectComponent;
	fresnelR0Panel->AddComponent(selectObjectComponent);

	int subMeshIndex = 0;
	fresnelR0Panel->RegisterEventCallback("GameObjectSelect", [panel = fresnelR0Panel](Event* pEvent)
		{
			GameObjectSelectEvent* event = (GameObjectSelectEvent*)pEvent;
			Object* selectedObject = event->GetSelectObject();
			int subMeshIndex = event->GetCurrentSubMeshIndex();


			if (selectedObject->GetObjectType() == EObjectType::eEntity)
			{
				AttrUiSystemSelectObjectComponent * selectObjectComponent =(AttrUiSystemSelectObjectComponent * )panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());


				selectObjectComponent->SetSelectedObject(selectedObject);
				selectObjectComponent->SetSubMeshIndex(subMeshIndex);
			//	panel->SetSelectedObject(selectedObject);

				Entity* entity = (Entity*)selectedObject;
				DirectX::XMFLOAT3 fresnelR0 = entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial.GetFresnelR0();
				panel->SetXYZ(fresnelR0);
			}
		});

	auto fresnelROTextLamda = [panel = fresnelR0Panel, subMeshIndex](int xyzIndex) {



		AttrUiSystemSelectObjectComponent* selectObjectComponent = (AttrUiSystemSelectObjectComponent*)panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());

		Object* object = selectObjectComponent->GetSelectedObject();
		int subMeshIndex = selectObjectComponent->GetSubMeshIndex();
		if (object == nullptr)
			return;

		if (object->GetObjectType() == EObjectType::eEntity)
		{
			Entity* entity = (Entity*)object;
			float value = 0.0f;
			DirectX::XMFLOAT3 fresnelR0 = entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial.GetFresnelR0();
			switch (xyzIndex)
			{
			case 0:
			{
				try
				{
					value = std::stof(panel->GetTextX());
				}
				catch (const std::invalid_argument& invalidArgument)
				{
					return;
				}
				fresnelR0.x = value;
			}
			break;

			case 1:
				try
				{
					value = std::stof(panel->GetTextY());
				}
				catch (const std::invalid_argument& invalidArgument)
				{
					return;
				}
				fresnelR0.y = value;
				break;

			case 2:
				try
				{
					value = std::stof(panel->GetTextZ());
				}
				catch (const std::invalid_argument& invalidArgument)
				{
					return;
				}
				fresnelR0.z = value;
				break;
			}
			entity->GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.SetFresnelRO(fresnelR0);
		}
	};

	fresnelR0Panel->SetXTextBoxEnterCallback(std::bind(fresnelROTextLamda, 0));
	fresnelR0Panel->SetYTextBoxEnterCallback(std::bind(fresnelROTextLamda, 1));
	fresnelR0Panel->SetZTextBoxEnterCallback(std::bind(fresnelROTextLamda, 2));


	//shiness

	TransformPropertyPanel* shinessPanel = CreatePropertyPanel("ShinessPanel", "Shiness", 2000, 50, 1, "Gray.png");
	subPanel2->AddChildPanelUiEntity(shinessPanel);
	pos.x = -subPanel2->GetWidth() / 2 + shinessPanel->GetWidth() / 2;
	pos.y = subPanel2->GetHeight() / 2 - fresnelR0Panel->GetHeight() - shinessPanel->GetHeight() / 2;
	pos.z = -1;
	shinessPanel->GetTransform().SetPositionLocal(pos);

	selectObjectComponent = new AttrUiSystemSelectObjectComponent;
	shinessPanel->AddComponent(selectObjectComponent);


	QHANDLE callbackHandle;
	shinessPanel->RegisterEventCallback("GameObjectSelect", [panel = shinessPanel, callbackHandle = 0](Event* pEvent) mutable {

		AttrUiSystemSelectObjectComponent* selectObjectComponent = (AttrUiSystemSelectObjectComponent*)panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());



		GameObjectSelectEvent* event = (GameObjectSelectEvent*)pEvent;
		Object* selectedObject = selectObjectComponent->GetSelectedObject();
		int subMeshIndex = selectObjectComponent->GetSubMeshIndex();

		if (selectedObject != nullptr && selectedObject->GetObjectType() == EObjectType::eEntity)
		{
			//�� ��ƼƼ�� ���͸��� ����ߴ� ��Ƽ�ݹ��� �����Ѵ�.
			Entity* entity = (Entity*)selectedObject;
			entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial.RemoveDirtyCallback(callbackHandle);
			callbackHandle = 0;
		}
		selectedObject = event->GetSelectObject();
		subMeshIndex = event->GetCurrentSubMeshIndex();
		selectObjectComponent->SetSelectedObject(selectedObject);
		selectObjectComponent->SetSubMeshIndex(subMeshIndex);


		if (selectedObject->GetObjectType() == EObjectType::eEntity)
		{
			Entity* entity = (Entity*)selectedObject;
			Material* mat = &entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial;
			float shiness = mat->GetShiness();
			panel->SetXYZ({ shiness,0,0 });

			callbackHandle = mat->AddDirtyCallback([mat = mat, panel]() {

				float shiness = mat->GetShiness();
				panel->SetXYZ({ shiness, 0, 0 });
				});
		}
	});


	shinessPanel->SetXTextBoxEnterCallback([panel = shinessPanel]() {
		float shiness = std::stof(panel->GetTextX());

		AttrUiSystemSelectObjectComponent* selectObjectComponent = (AttrUiSystemSelectObjectComponent*)panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());

		Object* object = selectObjectComponent->GetSelectedObject();
		int subMehsIndex = selectObjectComponent->GetSubMeshIndex();

		if (object->GetObjectType() == EObjectType::eEntity)
		{
			Entity* entity = (Entity*)object;
			entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMehsIndex].mModelMaterial.SetShiness(shiness);
		}
	});






	//specular

	TransformPropertyPanel* specularPanel = CreatePropertyPanel("SpecularPanel", "Specular", 2000, 50, 3, "Gray.png");
	subPanel2->AddChildPanelUiEntity(specularPanel);
	pos = shinessPanel->GetTransform().GetPositionLocal();
	pos.x = -subPanel2->GetWidth() / 2 + specularPanel->GetWidth() / 2;
	pos.y -= (shinessPanel->GetHeight()/2  + specularPanel->GetHeight()/2);
	pos.z = -1;
	specularPanel->GetTransform().SetPositionLocal(pos);


	selectObjectComponent = new AttrUiSystemSelectObjectComponent;
	specularPanel->AddComponent(selectObjectComponent);


	specularPanel->RegisterEventCallback("GameObjectSelect", [panel = specularPanel, callbackHandle = 0](Event* pEvent) mutable {


		AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*) panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());


		GameObjectSelectEvent* event = (GameObjectSelectEvent*)pEvent;

		Object *selectedObject= selectComponent->GetSelectedObject();
		int subMeshIndex = selectComponent->GetSubMeshIndex();
		if (selectedObject != nullptr && selectedObject->GetObjectType() == EObjectType::eEntity)
		{
			Entity* entity = (Entity*)selectedObject;
		
			Material* mat = &entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial;
			mat->RemoveDirtyCallback(callbackHandle);
		}
		selectedObject = event->GetSelectObject();
		subMeshIndex = event->GetCurrentSubMeshIndex();
		selectComponent->SetSelectedObject(selectedObject);
		selectComponent->SetSubMeshIndex(subMeshIndex);


		if (selectedObject->GetObjectType() == EObjectType::eEntity)
		{
			Entity* entity = (Entity*)selectedObject;
			Material* mat = &entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial;
			callbackHandle = mat->AddDirtyCallback([panel, mat]() {

				DirectX::XMFLOAT3 xyz = mat->GetSpecular();
				panel->SetXYZ(xyz);
				});

			DirectX::XMFLOAT3 xyz = mat->GetSpecular();
			panel->SetXYZ(xyz);
		}

	});
	
	auto specularTextEnterLamda = [panel = specularPanel](int xyzIndex) {

		AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*)panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());

		Object* selectedObject = selectComponent->GetSelectedObject();
		int subMeshIndex = selectComponent->GetSubMeshIndex();
		if(selectedObject == nullptr || selectedObject->GetObjectType() != EObjectType::eEntity)
		{ 
			return;
		}

		Entity* entity = (Entity*)selectedObject;
		Material* mat = &entity->GetModel()->GetMeshComponent()->GetSubMesh()[subMeshIndex].mModelMaterial;
		DirectX::XMFLOAT3 xyz = mat->GetSpecular();
		switch (xyzIndex)
		{
		case 0:
		{
			float x = 0.0F;
			try
			{
				x = std::stof(panel->GetTextX());
			}
			catch (const std::invalid_argument& invalidArgument)
			{
				return;
			}
			xyz.x = x;

		}
		break;
		case 1:
		{
			float y = 0.0F;
			try
			{
				y = std::stof(panel->GetTextY());
			}
			catch (const std::invalid_argument& invalidArgument)
			{
				return;
			}
			xyz.y = y;

		}
		break;
		case 2:
		{
			float z = 0.0F;
			try
			{
				z = std::stof(panel->GetTextZ());
			}
			catch (const std::invalid_argument& invalidArgument)
			{
				return;
			}
			xyz.z = z;
		}
		break;
		}
		mat->SetSpecular(xyz);
	};

	specularPanel->SetXTextBoxEnterCallback(std::bind(specularTextEnterLamda,0));
	specularPanel->SetYTextBoxEnterCallback(std::bind(specularTextEnterLamda,1));
	specularPanel->SetZTextBoxEnterCallback(std::bind(specularTextEnterLamda,2));





















}

void Quad::AttributeUiSystem::CreateMaterialAssetPanel(	PanelUiEntity * parentPanel, const std::string& panelName, const std::string& text ,
	PanelUiEntity* & oAssetPanel , IconEntity* & oAssetImageFramePanel)
{

	Map* map = GetMap();
	int mapLayerIndex = 0;
//	PanelUiEntity* assetPanel = new PanelUiEntity(panelName);
//	PanelUiEntity* assetPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* assetPanel = PanelUiEntity::Create(map, mapLayerIndex);
	oAssetPanel = assetPanel;
//	assetPanel->SetSystem(this);
	//assetPanel->Initialize();
	assetPanel->SetTexture("White.png");
	assetPanel->SetSize(100, 130);
	assetPanel->SetSelectAvailableFlag(false);
	assetPanel->GetTransform().SetIndependentScaleFlag(true);
	assetPanel->GetTransform().SetIndependentRotationFlag(true);
	parentPanel->AddChildPanelUiEntity(assetPanel);


	assetPanel->RegisterEventCallback("GameObjectSelect", [panel = assetPanel](Event* pEvent)
		{
			const std::vector<UiEntity*>& childPanelUiEntityVector = panel->GetChildPanelUiEntityVector();
			for (int i = 0; i < childPanelUiEntityVector.size(); ++i)
			{
				if (static_cast<AssetImageFrameEntity*>(childPanelUiEntityVector[i]))
					childPanelUiEntityVector[i]->OnEvent(pEvent);
			}
		});


//	IconEntity* assetImageFrameEntity = new IconEntity(panelName+"_AssetImageFrameEntity");
	//IconEntity* assetImageFrameEntity = static_cast<IconEntity*>(map->CreateObject("IconEntity"));
	IconEntity* assetImageFrameEntity = IconEntity::Create(map, mapLayerIndex);
	oAssetImageFramePanel = assetImageFrameEntity;
	//assetImageFrameEntity->Initialize();
	assetImageFrameEntity->SetTexture("Gray.png");
	assetImageFrameEntity->SetSize(100, 100);
	assetImageFrameEntity->SetSelectAvailableFlag(false);
	assetImageFrameEntity->GetTransform().SetIndependentScaleFlag(true);
	assetImageFrameEntity->GetTransform().SetIndependentRotationFlag(true);

	AttrUiSystemSelectObjectComponent* selectComponent = new AttrUiSystemSelectObjectComponent;
	assetImageFrameEntity->AddComponent(selectComponent);

	assetPanel->AddChildPanelUiEntity(assetImageFrameEntity);
	assetImageFrameEntity->SetPosition(0, assetImageFrameEntity->GetHeight() / 2 - assetPanel->GetHeight() / 2, -1);


	//TextBoxPanel* textBoxPanel  = new TextBoxPanel(panelName + "TextBoxPanel");
	//TextBoxPanel* textBoxPanel  = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	TextBoxPanel* textBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 100, 30);
	//TextBox* textBox = new TextBox(panelName+L"_TextBox");
	//textBox->SetSystem(this);
	//textBox->Initialize(100);
	//textBox->SetTextColor({ 0.0f,0.0f,0.0f });
	//textBox->SetSize(100, 30);
	//textBox->SetSelectAvailableFlag(false);
	//textBox->SetAlignmentDirection(ETextAlignmentDirection::eCenter);
	//textBox->SetTexture(L"Gray.png");
	//textBox->SetText(text);

//	textBoxPanel->SetSystem(this);
//	textBoxPanel->Initialize(100, 30);
	textBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	//textBoxPanel->SetSize(100, 30);
	textBoxPanel->SetSelectAvailableFlag(false);
	textBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	textBoxPanel->SetTexture("Gray.png");
	textBoxPanel->SetText(text);








	//diffuseMapTextBox->SetTextEditMode(false);

	textBoxPanel->GetTransform().SetIndependentScaleFlag(true);
	textBoxPanel->GetTransform().SetIndependentRotationFlag(true);

	assetPanel->AddChildPanelUiEntity(textBoxPanel);
	textBoxPanel->SetPosition(0, assetPanel->GetHeight() / 2 - textBoxPanel->GetHeight() / 2, -1);


	



}

void Quad::AttributeUiSystem::CreateAnimationPanel()
{
	Map* map = GetMap();
	int mapLayerIndex = 0;

	auto sendEventCallbackLamda = [](Event* pEvent, PanelUiEntity* panel) {

		const std::vector<UiEntity*>& childPanelVector = panel->GetChildPanelUiEntityVector();
		std::for_each(childPanelVector.begin(), childPanelVector.end(), [pEvent](UiEntity* childPanel) {
			childPanel->OnEvent(pEvent);
			});
			
	};







//	PanelUiEntity* animMainPanel = new PanelUiEntity("AnimationMainPanel");
	//PanelUiEntity* animMainPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* animMainPanel = PanelUiEntity::Create(map, mapLayerIndex);
	SetDefaultInitSetting(animMainPanel);
	//animMainPanel->Initialize();
	animMainPanel->SetSize(500, 800);
	animMainPanel->SetTexture("DarkGray.png");
	mRootPanel->AddChildPanelUiEntity(animMainPanel, reinterpret_cast<const char*>(u8"에니메이션"));
	animMainPanel->RegisterEventCallback("GameObjectSelect",std::bind(sendEventCallbackLamda,std::placeholders::_1,animMainPanel));
	animMainPanel->RegisterEventCallback("NotifyDragAndDrop",std::bind(sendEventCallbackLamda,std::placeholders::_1,animMainPanel));
	animMainPanel->SetDrawFlag(true);
//	animMainPanel->RegisterEventCallback("MouseMove",std::bind(sendEventCallbackLamda,std::placeholders::_1,animMainPanel));
	//animMainPanel->RegisterEventCallback("LButtonDown",std::bind(sendEventCallbackLamda,std::placeholders::_1,animMainPanel));







	//skeleton tag panel 
	DirectX::XMFLOAT3 pos;
	//TextBoxPanel* skeletonTagPanel = new TextBoxPanel("SkeletonTagPanel");
	//TextBoxPanel* skeletonTagPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	TextBoxPanel* skeletonTagPanel = TextBoxPanel::Create(map, mapLayerIndex,180,20);
	SetDefaultInitSetting(skeletonTagPanel);
//	skeletonTagPanel->Initialize(180, 20);
	skeletonTagPanel->SetTextColor({ 5.0f,5.0f,5.0f });
	skeletonTagPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	skeletonTagPanel->SetText(reinterpret_cast<const char*>(u8"스켈레톤"));
	//stateClipTagPanel->SetDrawFlag(true);
	skeletonTagPanel->SetTexture("DarkGray.png");
	animMainPanel->AddChildPanelUiEntity(skeletonTagPanel);

	pos.x = -animMainPanel->GetWidth() / 2 +	skeletonTagPanel->GetWidth()/2+	10;
	pos.y = animMainPanel->GetHeight() / 2 - 5 - skeletonTagPanel->GetHeight() / 2;
	pos.z = -1;
	skeletonTagPanel->GetTransform().SetPositionLocal(pos);


	//skeleton image panel 


//	PanelUiEntity* skeletonAssetPanel = new PanelUiEntity("SkeletonAssetPanel");
//	PanelUiEntity* skeletonAssetPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* skeletonAssetPanel = PanelUiEntity::Create(map, mapLayerIndex);






	SetDefaultInitSetting(skeletonAssetPanel);
//	skeletonAssetPanel->Initialize();
	skeletonAssetPanel->SetTexture("DarkGray.png");
	skeletonAssetPanel->SetSize(skeletonTagPanel->GetWidth(), 200);

	animMainPanel->AddChildPanelUiEntity(skeletonAssetPanel);
	pos = skeletonTagPanel->GetTransform().GetPositionLocal();
	//pos.x = -animMainPanel->GetWidth() / 2 + skeletonAssetPanel->GetWidth() / 2 + 10;
	pos.y -= (skeletonTagPanel->GetHeight()/2+10 + skeletonAssetPanel->GetHeight() / 2);
	skeletonAssetPanel->GetTransform().SetPositionLocal(pos);



	skeletonAssetPanel->RegisterEventCallback("GameObjectSelect", std::bind(sendEventCallbackLamda, std::placeholders::_1, skeletonAssetPanel));
	skeletonAssetPanel->RegisterEventCallback("NotifyDragAndDrop", std::bind(sendEventCallbackLamda, std::placeholders::_1, skeletonAssetPanel));

	//IconEntity* skeletonImagePanel = new IconEntity("SkeletonImagePanel");
	//IconEntity* skeletonImagePanel = static_cast<IconEntity*>(map->CreateObject("IconEntity"));
	IconEntity* skeletonImagePanel = IconEntity::Create(map, mapLayerIndex);
	TextBoxPanel* skeletonNameTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 180, 20);




//	skeletonImagePanel->Initialize();
	skeletonImagePanel->GetTransform().SetIndependentScaleFlag(true);
	skeletonImagePanel->GetTransform().SetIndependentRotationFlag(true);
	skeletonImagePanel->SetSelectAvailableFlag(false);
	skeletonImagePanel->SetSize(180, 180);
	skeletonImagePanel->SetTexture("None.png");
	skeletonAssetPanel->AddChildPanelUiEntity(skeletonImagePanel);
	pos.x = 0;
	pos.y = (skeletonAssetPanel->GetHeight() - skeletonImagePanel->GetHeight())/2;
	pos.z = -1;
	skeletonImagePanel->GetTransform().SetPositionLocal(pos);

	AttrUiSystemSelectObjectComponent* selectComponent = new AttrUiSystemSelectObjectComponent;
	skeletonImagePanel->AddComponent(selectComponent);

	skeletonImagePanel->RegisterEventCallback("GameObjectSelect", [panel = skeletonImagePanel](Event* pEvent) {


		AttrUiSystemSelectObjectComponent * selectComponent = (AttrUiSystemSelectObjectComponent*)panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());
		GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;


		Object* selectedObject = selectComponent->GetSelectedObject();
		
		
		selectedObject = selectEvent->GetSelectObject();
		selectComponent->SetSelectedObject(selectedObject);
		if (selectedObject->GetObjectType() == EObjectType::eEntity)
		{

			Entity* entity = (Entity*)selectedObject;
			AnimationComponent * animationComponent = entity->GetModel()->GetAnimationComponent();
			if (animationComponent == nullptr)
			{
				panel->SetTexture("None.png");
				return;
			}

			Skeleton * skeleton =  animationComponent->GetSkeleton();
			
			if (skeleton != nullptr)
				panel->SetTexture("SkeletonAsset.png");
			else
				panel->SetTexture("None.png");
		}
		else
		{
			panel->SetTexture("None.png");
		}	
			
		});
	skeletonImagePanel->RegisterEventCallback("NotifyDragAndDrop", [panel =skeletonImagePanel,NamePanel = skeletonNameTextBoxPanel](Event *pEvent) {


		NotifyDragAndDropEvent* dragAndDropEvent = (NotifyDragAndDropEvent*)pEvent;
		Ray ray =dragAndDropEvent->GetRay();

		float t = FLT_MAX;
		if (!CollisionHelper::Intersect(panel->GetModel()->GetUiColliderComponent()->GetCollider(), ray, t))
		{
			panel->SetColorItensity({ 1.0f,1.0f,1.0f });
			return;
		}

		if (!dragAndDropEvent->GetLButtonUpState())
		{
			panel->SetColorItensity({ 2.0f,1.0f,1.0f });
			return;
		}
		else
		{
			panel->SetColorItensity({ 1.0f,1.0f,1.0f });
		}


		AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*)panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());
		Object * selectedObject = selectComponent->GetSelectedObject();

		if (selectedObject != nullptr && selectedObject->GetObjectType() == EObjectType::eEntity)
		{
			Entity* entity = (Entity*)selectedObject;
			AnimationComponent* animationComponent = entity->GetModel()->GetAnimationComponent();
			if (animationComponent == nullptr)
				return;
			
			
			ContentItem* contentItem = dragAndDropEvent->GetContentItem();

			if (contentItem->GetEContentItemType() == EContentItemType::eAsset)
			{
				Asset* asset = (Asset*)contentItem;
				if (asset->GetAssetType() == EAssetType::eSkeleton)
				{
					
					animationComponent->SetSkeleton((Skeleton*)asset);
					panel->SetTexture("SkeletonAsset.png");
					NamePanel->SetText(asset->GetName());
				}
			}
		}


		});







////


	
	SetDefaultInitSetting(skeletonNameTextBoxPanel);
	//skeletonNameTextBoxPanel->Initialize(180, 20);
	skeletonNameTextBoxPanel->SetTexture("DarkGray.png");
	skeletonNameTextBoxPanel->SetDrawFlag(true);
	skeletonAssetPanel->AddChildPanelUiEntity(skeletonNameTextBoxPanel);
	pos.y = -skeletonAssetPanel->GetHeight() / 2 + skeletonNameTextBoxPanel->GetHeight() / 2;
	skeletonNameTextBoxPanel->GetTransform().SetPositionLocal(pos);
	skeletonNameTextBoxPanel->RegisterEventCallback("GameObjectSelect", [panel =skeletonNameTextBoxPanel](Event * pEvent) 
		{
			GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;

			Object* selectedObject = selectEvent->GetSelectObject();
			if (selectedObject->GetObjectType() == EObjectType::eEntity)
			{
				Entity* entity = (Entity*)selectedObject;
				AnimationComponent* animationComponent =entity->GetModel()->GetAnimationComponent();
				if (animationComponent != nullptr)
				{
					Skeleton* skeleton = animationComponent->GetSkeleton();
					if (skeleton != nullptr)
					{

						const std::string& skeletonName = skeleton->GetName();
						panel->SetText(skeletonName);
						return;
					}
				}

			}

			panel->SetText("");


		});
	



//
//
//
//
//
//
//
//
//
//
////
////
////
////
////
////
////
////	
////
////
////
//

	//state - clip  panel 


//	TextBoxPanel* stateClipTagPanel = new TextBoxPanel("AnimationClipTagPanel");
	//TextBoxPanel* stateClipTagPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	TextBoxPanel* stateClipTagPanel = TextBoxPanel::Create(map, mapLayerIndex, 500, 20);
	SetDefaultInitSetting(stateClipTagPanel);

	stateClipTagPanel->SetTextColor({ 5.0f,5.0f,5.0f });
	stateClipTagPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	stateClipTagPanel->SetText(reinterpret_cast<const char*>(u8"상태:클립"));
	//stateClipTagPanel->SetDrawFlag(true);
	stateClipTagPanel->SetTexture("DarkGray.png");
	animMainPanel->AddChildPanelUiEntity(stateClipTagPanel);

	pos.x = -animMainPanel->GetWidth() / 2 + stateClipTagPanel->GetWidth() / 2 + 10 + 200;
	pos.y = ( animMainPanel->GetHeight() / 2 - stateClipTagPanel->GetHeight() / 2 - 5);
	pos.z = -1;
	stateClipTagPanel->GetTransform().SetPositionLocal(pos);



//
//
//

	 pos = stateClipTagPanel->GetTransform().GetPositionLocal();

	int listItemMaxNum = 100;
//	ScrollListPanel* stateClipScrollListPanel = new ScrollListPanel("AnimationStateClipScrollPanel");
	//ScrollListPanel* stateClipScrollListPanel = static_cast<ScrollListPanel*>(map->CreateObject("ScrollListPanel"));
	ScrollListPanel* stateClipScrollListPanel = ScrollListPanel::Create(map, mapLayerIndex, 500, 200, listItemMaxNum);
	SetDefaultInitSetting(stateClipScrollListPanel);
//	stateClipScrollListPanel->Initialize(500, 200, listItemMaxNum);
	stateClipScrollListPanel->SetTexture("33343c.png");

	animMainPanel->AddChildPanelUiEntity(stateClipScrollListPanel);
	pos = stateClipTagPanel->GetTransform().GetPositionLocal();
	//pos.x = -animMainPanel->GetWidth() / 2 + stateClipScrollListPanel->GetWidth() / 2 + 10;
	pos.y -= (stateClipTagPanel->GetHeight() / 2 + 10 + stateClipScrollListPanel->GetHeight() / 2);
	stateClipScrollListPanel->GetTransform().SetPositionLocal(pos);


	selectComponent = new AttrUiSystemSelectObjectComponent;
	stateClipScrollListPanel->AddComponent(selectComponent);
	stateClipScrollListPanel->RegisterEventCallback("GameObjectSelect", [panel = stateClipScrollListPanel](Event* pEvent) {


		GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;

		AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*)panel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());

		Object* selectedObject = selectEvent->GetSelectObject();

		selectComponent->SetSelectedObject(selectedObject);

		if (selectedObject->GetObjectType() == EObjectType::eEntity)
		{
			Entity* entity = (Entity*)selectedObject;

			AnimationComponent* animationComponent = entity->GetModel()->GetAnimationComponent();
			if (animationComponent == nullptr)
			{
				panel->ActivateChildListPanel(0);
				return;
			}

			const std::unordered_map<std::string, AnimationState*>& animationStateTable = animationComponent->GetAnimationStateTable();

			int stateNum = animationStateTable.size();
			panel->ActivateChildListPanel(stateNum);
			panel->ResetLScrollBar();
			const std::vector<UiEntity*>& childPaenlVector = panel->GetChildPanelUiEntityVector();


			std::unordered_map<std::string, AnimationState*>::const_iterator tableIterator = animationStateTable.cbegin();



			for (int i = 1; i <= stateNum; ++i)
			{

				const PanelUiEntity* listPanel = (PanelUiEntity*)childPaenlVector[i];

				TextBoxPanel* stateTextBoxPanel = (TextBoxPanel*)(listPanel->GetChildObjectVector()[0].GetPointer());
				TextBoxPanel* clipTextBoxPanel = (TextBoxPanel*)(listPanel->GetChildObjectVector()[1].GetPointer());


				AnimationState* state = tableIterator->second;
				stateTextBoxPanel->SetText(tableIterator->first);
				clipTextBoxPanel->SetText(state->GetAnimationClip()->GetName());

				tableIterator++;
			}




		}
		else
		{
			panel->ActivateChildListPanel(0);
		}



		});
	

//
	PanelUiEntity* stateClipListItemPanel;
	TextBoxPanel* stateTextBoxPanel;
	TextBoxPanel* clipTextBoxPanel;

	for (int i = 0; i < listItemMaxNum; ++i)
	{
	//	stateClipListItemPanel = new PanelUiEntity("stateClipListItemPanel" + std::to_string(i));
		//stateClipListItemPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
		stateClipListItemPanel = PanelUiEntity::Create(map, mapLayerIndex);
		SetDefaultInitSetting(stateClipListItemPanel);
		//stateClipListItemPanel->Initialize();
		stateClipListItemPanel->SetTexture("Blue.png");
		stateClipListItemPanel->SetSize(100, 30);
		float colorIntensity = 1.0f * i;
	//	stateClipListItemPanel->SetColorItensity({ colorIntensity,colorIntensity,colorIntensity });


		stateClipScrollListPanel->AddChildPanelUiEntity(stateClipListItemPanel);




		float width = stateClipListItemPanel->GetWidth();
		//stateTextBoxPanel = new TextBoxPanel("stateListItemPanel" + std::to_string(i));
		//stateTextBoxPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
		stateTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, width / 2, 20);
	//	clipTextBoxPanel = new TextBoxPanel("clipListItemPanel" + std::to_string(i));
		//clipTextBoxPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
		clipTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, width / 2, 20);


		SetDefaultInitSetting(stateTextBoxPanel);
		SetDefaultInitSetting(clipTextBoxPanel);
		//stateTextBoxPanel->Initialize(width/2, 20);
		//clipTextBoxPanel->Initialize(width/2, 20);
		//stateTextBoxPanel->SetSize(width / 2, 30);

		stateClipListItemPanel->AddChildPanelUiEntity(stateTextBoxPanel);
		stateClipListItemPanel->AddChildPanelUiEntity(clipTextBoxPanel);
		stateTextBoxPanel->SetTexture("DarkGray.png");
		stateTextBoxPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
	//	stateTextBoxPanel->SetText(L"ASD");
	//	stateTextBoxPanel->SetEntireDrawFlag(true);
	//	clipTextBoxPanel->SetText(L"ASD");
		clipTextBoxPanel->SetTexture("DarkGray.png");

		stateTextBoxPanel->SetTextColor({ 5.0f,5.0f,5.0f });
		clipTextBoxPanel->SetTextColor({ 5.0f,5.0f,5.0f });


		pos.x = -width / 2 + stateTextBoxPanel->GetWidth() / 2;
		pos.y = 0;
		pos.z = -1;
		stateTextBoxPanel->GetTransform().SetPositionLocal(pos);
		pos.x += stateTextBoxPanel->GetWidth() / 2 + clipTextBoxPanel->GetWidth() / 2;


		clipTextBoxPanel->GetTransform().SetPositionLocal(pos);

	}

	stateClipScrollListPanel->ActivateChildListPanel(0);







//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

	//edit3dMainSceneLight
	//Light* animationEdit3DPanelLight = new Light;
	Light* animationEdit3DPanelLight = static_cast<Light*>(map->CreateObject("Light",1));
	//animationEdit3DPanelLight->SetSystem(this);
	animationEdit3DPanelLight->SetName("AnimationEdit3DLight");
	animationEdit3DPanelLight->SetLightType(ELightType::eDirect);
//	AddEntity(animationEdit3DPanelLight, 1);


//
	int animMapLayerIndex = 1;
	//animationEdit3DMainPanel

	PanelUiEntity* animationEdit3DPanel = PanelUiEntity::Create(map, mapLayerIndex);



	PanelUiEntity* animationEdit3DFrameControlPanel = PanelUiEntity::Create(map, mapLayerIndex);


	PanelUiEntity* animationEdit3DStickControlPanel = PanelUiEntity::Create(map, mapLayerIndex);
	
	TextBoxPanel* animEdit3DFrameCountTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 300, 20);


	TextBoxPanel* animEdit3DSecondTimeTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 300, 20);

	
	PanelUiEntity* animationEdit3DFrameControlStickPanel = PanelUiEntity::Create(map, mapLayerIndex);

	PanelUiEntity* animationEdit3DFrameControlStartStickPanel = PanelUiEntity::Create(map, mapLayerIndex);

	PanelUiEntity* animationEdit3DFrameControlEndStickPanel = PanelUiEntity::Create(map, mapLayerIndex);


	ButtonUiEntity* animationPlayButton = ButtonUiEntity::Create(map, mapLayerIndex);




//
//
//

	SetDefaultInitSetting(animationEdit3DPanel);
	//animationEdit3DPanel->Initialize();
	animationEdit3DPanel->SetSize(300, 400);
	animationEdit3DPanel->SetTexture("Blue.png");
	selectComponent = new AttrUiSystemSelectObjectComponent;
	animationEdit3DPanel->AddComponent(selectComponent);


//	Entity* animation3DModelEntity = new Entity;
	//Entity* animation3DModelEntity = static_cast<Entity*>(map->CreateObject("Entity", 1));
	Entity* animation3DModelEntity = Entity::Create(map, animMapLayerIndex);
	//animation3DModelEntity->SetSystem(this);
	//animation3DModelEntity->Initialize(MeshManager::GetMesh("black_Bison"));
	//animation3DModelEntity->GetModel()->GetMeshComponent()->SetMesh(MeshManager::GetMesh("black_Bison"));

	animation3DModelEntity->SetDrawFlag(false);
	//AddEntity(animation3DModelEntity,1);
	animation3DModelEntity->GetTransform().SetPositionLocal({ 0,0,20.0F });



	animMainPanel->AddChildPanelUiEntity(animationEdit3DPanel);
	pos = stateClipScrollListPanel->GetTransform().GetPositionLocal();
	pos.x = -animMainPanel->GetWidth() / 2 + 150 +10;
	pos.y -= (stateClipScrollListPanel->GetHeight() / 2 + 10 + animationEdit3DPanel->GetHeight() / 2);
	pos.z = -1;
	animationEdit3DPanel->GetTransform().SetPositionLocal(pos);
	animationEdit3DPanel->RegisterEventCallback("NotifyDragAndDrop", std::bind(sendEventCallbackLamda, std::placeholders::_1, animationEdit3DPanel));





	//3d image
//	PanelUiEntity* animationEdit3DImagePanel = new PanelUiEntity("AnimationEdit3DImagePanel");
	//PanelUiEntity* animationEdit3DImagePanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity", 1));
	PanelUiEntity* animationEdit3DImagePanel = PanelUiEntity::Create(map, mapLayerIndex);
	SetDefaultInitSetting(animationEdit3DImagePanel);
	//animationEdit3DImagePanel->Initialize();
	animationEdit3DImagePanel->SetSize(300, 300);
	//animationEdit3DImagePanel->SetTexture("AnimationEdit3DPanelTexture");	//지금현재 이 렌더타켓텍스처는 {0,0,0,0}값이고 shader에서 w가 0이라서 clip될것이다.따라서 보이지않는다.
	animationEdit3DImagePanel->SetTexture("AnimationEdit3DPanelTexture");	//지금현재 이 렌더타켓텍스처는 {0,0,0,0}값이고 shader에서 w가 0이라서 clip될것이다.따라서 보이지않는다.
//	animationEdit3DImagePanel->SetTexture("Red.png");
	animationEdit3DPanel->AddChildPanelUiEntity(animationEdit3DImagePanel);


	pos = stateClipScrollListPanel->GetTransform().GetPositionLocal();
	pos.x =0;
	pos.y = 50.0f;
	pos.z = -1;
	animationEdit3DImagePanel->GetTransform().SetPositionLocal(pos);


	std::shared_ptr<int> animationPlayState(new int(0));



	animationEdit3DImagePanel->RegisterEventCallback("NotifyDragAndDrop", [panel = animationEdit3DImagePanel , entity = animation3DModelEntity, button = animationPlayButton, playState = animationPlayState,
		startStick = animationEdit3DFrameControlStartStickPanel, endStick = animationEdit3DFrameControlEndStickPanel, controlStick = animationEdit3DFrameControlStickPanel](Event *pEvent) {
		
		NotifyDragAndDropEvent* dragAndDropEvent = static_cast<NotifyDragAndDropEvent*>(pEvent);
		
		if (!dragAndDropEvent->GetLButtonUpState())
		{
			return;
		}

		const Ray & ray = dragAndDropEvent->GetRay();
			
		float minT = FLT_MAX;
		if (!CollisionHelper::Intersect(panel->GetModel()->GetUiColliderComponent()->GetCollider(), ray, minT))
		{
			//panel->SetColorItensity({ 1.0f,1.0f,1.0f });
			return;
		}


		ContentItem * contentItem =	dragAndDropEvent->GetContentItem();
		if (contentItem->GetEContentItemType() == EContentItemType::eAsset)
		{
			Asset* asset = static_cast<Asset*>(contentItem);

			if (asset->GetAssetType() == EAssetType::eAnimationClip)
			{

				if (dragAndDropEvent->GetLButtonUpState())
				{

				AnimationClip* animationClip = static_cast<AnimationClip*>(asset);
				entity->GetModel()->GetMeshComponent()->SetMesh(animationClip->GetDefaultMesh());
				AnimationComponent& animationComponent = *entity->GetModel()->GetAnimationComponent();
				animationComponent.SetSkeleton(animationClip->GetDefaultSkeleton());

				animationComponent.RemoveState("Default");

				animationComponent.AddState("Default", animationClip);
				animationComponent.SetCurrentAnimationState("Default");
				entity->SetDrawFlag(true);
				animationComponent.SetUpdateFlag(false);


				//����
				button->SetTexture("gamePlayButtonOff.png");
				*playState = 0;
				startStick->GetStateComponent()->SetSelectState(true);
				endStick->GetStateComponent()->SetSelectState(true);
				controlStick->GetStateComponent()->SetSelectState(true);

				startStick->GetTransform().SetPositionLocal({ -100.0f,0,-1 });
				endStick->GetTransform().SetPositionLocal({ 100.0f,0,-1 });
				controlStick->GetTransform().SetPositionLocal({ -100.0f,0,-1 });

				return;
				}
			}
		}

		
		});











	//animationEdit3DFrameControlPanel
	//PanelUiEntity* animationEdit3DFrameControlPanel = new PanelUiEntity(L"AnimationEidt3DFrameControlPanel");
	SetDefaultInitSetting(animationEdit3DFrameControlPanel);
//	animationEdit3DFrameControlPanel->Initialize();
	animationEdit3DFrameControlPanel->SetSize(300, 100);
	animationEdit3DFrameControlPanel->SetTexture("33343c.png");
	animationEdit3DPanel->AddChildPanelUiEntity(animationEdit3DFrameControlPanel);

	pos.x = 0;
	pos.y = -animationEdit3DPanel->GetHeight()/2 + animationEdit3DFrameControlPanel->GetHeight()/2;
	
	pos.z = -1.0f;
	animationEdit3DFrameControlPanel->GetTransform().SetPositionLocal(pos);








	//animationStickControlPanel 
	//PanelUiEntity* animationEdit3DStickControlPanel = new PanelUiEntity(L"AnimationEidt3DmStickControlPanel");
	SetDefaultInitSetting(animationEdit3DStickControlPanel);
	//animationEdit3DStickControlPanel->Initialize();
	animationEdit3DStickControlPanel->SetSize(220, 50);
	animationEdit3DStickControlPanel->SetTexture("Black.png");
	animationEdit3DFrameControlPanel->AddChildPanelUiEntity(animationEdit3DStickControlPanel);
	pos.x = 0;
	pos.y = -animationEdit3DFrameControlPanel->GetHeight() / 2 + animationEdit3DStickControlPanel->GetHeight() / 2;

	pos.z = -1.0f;
	animationEdit3DStickControlPanel->GetTransform().SetPositionLocal(pos);









	//textboxpanel 
	//frame text boux
	//TextBoxPanel* animEdit3DFrameCountTextBoxPanel = new TextBoxPanel(L"animEdit3DFrameCountTextBoxPanel");
	SetDefaultInitSetting(animEdit3DFrameCountTextBoxPanel);
	//animEdit3DFrameCountTextBoxPanel->Initialize(300, 20);
	animEdit3DFrameCountTextBoxPanel->SetTexture("33343c.png");
	animEdit3DFrameCountTextBoxPanel->SetColorItensity({ 1.0F,1.0F,1.0F });
	animEdit3DFrameCountTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"프레임번호 : 0"));

	animEdit3DFrameCountTextBoxPanel->SetTextColor({ 1.0f,1.0f,1.0f });
	animationEdit3DFrameControlPanel->AddChildPanelUiEntity(animEdit3DFrameCountTextBoxPanel);
	pos.x = 0;// -animationEdit3DFrameControlPanel->GetWidth() / 2;
	pos.y = animationEdit3DFrameControlPanel->GetHeight() / 2 - animEdit3DFrameCountTextBoxPanel->GetHeight() / 2;
	pos.z = -1;
	animEdit3DFrameCountTextBoxPanel->GetTransform().SetPositionLocal(pos);
	

	//second time text box
	//TextBoxPanel* animEdit3DSecondTimeTextBoxPanel = new TextBoxPanel(L"animEdit3DSecondTimeTextBoxPanel");
	SetDefaultInitSetting(animEdit3DSecondTimeTextBoxPanel);
	//animEdit3DSecondTimeTextBoxPanel->Initialize(300, 20);
	animEdit3DSecondTimeTextBoxPanel->SetTexture("33343c.png");
	animEdit3DSecondTimeTextBoxPanel->SetColorItensity({ 1.0F,1.0F,1.0F });
	animEdit3DSecondTimeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"시간(초) : 0.0"));

	animEdit3DSecondTimeTextBoxPanel->SetTextColor({ 1.0f,1.0f,1.0f });
	animationEdit3DFrameControlPanel->AddChildPanelUiEntity(animEdit3DSecondTimeTextBoxPanel);
	pos.x = 0;// -animationEdit3DFrameControlPanel->GetWidth() / 2;
	pos.y = animationEdit3DFrameControlPanel->GetHeight() / 2 - animEdit3DFrameCountTextBoxPanel->GetHeight() - animEdit3DSecondTimeTextBoxPanel->GetHeight()/2;
	pos.z = -1;
	animEdit3DSecondTimeTextBoxPanel->GetTransform().SetPositionLocal(pos);
	












	//stick 


	//PanelUiEntity* animationEdit3DFrameControlStickPanel = new PanelUiEntity(L"AnimationEidt3DFrameControlStickPanel");
	SetDefaultInitSetting(animationEdit3DFrameControlStickPanel);
//	animationEdit3DFrameControlStickPanel->Initialize();
	animationEdit3DFrameControlStickPanel->SetSize(5, 30);
	animationEdit3DFrameControlStickPanel->SetTexture("Gray.png");
	animationEdit3DFrameControlStickPanel->GetStateComponent()->SetSelectState(true);

	animationEdit3DStickControlPanel->AddChildPanelUiEntity(animationEdit3DFrameControlStickPanel);

	pos.x = -(animationEdit3DStickControlPanel->GetWidth()-20) / 2 ;
	pos.y = 0;
	pos.z = -1;

	animationEdit3DFrameControlStickPanel->GetTransform().SetPositionLocal(pos);





	animationEdit3DFrameControlStickPanel->RegisterEventCallback("MouseMove", [panel = animationEdit3DFrameControlStickPanel,frameCountTextBoxPanel = animEdit3DFrameCountTextBoxPanel , secondTimeTextBoxPanel = animEdit3DSecondTimeTextBoxPanel, entity = animation3DModelEntity](Event* pEvent) {

		StateComponent* stateComponent = panel->GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		if (stateComponent->GetLButtonDownState())
		{

				
				AnimationComponent* animationComponent = entity->GetModel()->GetAnimationComponent();

				if (animationComponent==nullptr || !animationComponent->GetAnimationComponentState())
					return;


				int shiftX = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();

				float width = panel->GetWidth();
				DirectX::XMFLOAT3 pos = panel->GetTransform().GetPositionLocal();

				pos.x += shiftX;
				if (pos.x < -100.0f)
					pos.x = -100.0f;

				if (pos.x > 100.0f)
					pos.x = 100.0f;

				
					panel->GetTransform().SetPositionLocal(pos);

					int allFrameCount = animationComponent->GetCurrentAnimationState()->GetAnimationClip()->GetEndFrameCount() + 1;

					float shiftUnitPerFrame = allFrameCount / 200.0f;



					float endTimeSecond = animationComponent->GetCurrentAnimationState()->GetAnimationClip()->GetEndTimeSecond();
					float currentTimeSecond = (pos.x +100) / 200.0f * endTimeSecond;


					//animationComponent.SetLocalTimeFrameUpdate(shiftUnitPerFrame * (pos.x + 150));
					animationComponent->SetLocalTimeSecondUpdate(currentTimeSecond);

					const std::string frameText = reinterpret_cast<const char*>(u8"프레임번호 : ") + std::to_string((int)(shiftUnitPerFrame * (pos.x + 100)));
					

					frameCountTextBoxPanel->SetText(frameText.c_str());




					const std::string secondText = reinterpret_cast<const char*>(u8"시간(초) : ") + std::to_string(currentTimeSecond);
					secondTimeTextBoxPanel->SetText(reinterpret_cast<const char*>(secondText.c_str()));
				
		}
		else
		{
			panel->SetColorItensity({ 1.0F,1.0F,1.0F });
		}

	});

	animationEdit3DFrameControlStickPanel->RegisterEventCallback("LButtonDown", [panel  = animationEdit3DFrameControlStickPanel](Event* pEvent) {
		
		StateComponent* stateComponent = panel->GetStateComponent();
		
		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		if (stateComponent->GetHoverState() && stateComponent->GetSelectState())
		{
			panel->SetColorItensity({ 0.0F,2.0F,0.0F });
			stateComponent->SetLButtonDownState(true);
			Mouse::CaptureObject(panel);

		}
		else
		{
			panel->SetColorItensity({ 1.0F,1.0F,1.0F });
			stateComponent->SetLButtonDownState(false);
		}

		});
	
	animationEdit3DFrameControlStickPanel->RegisterEventCallback("LButtonUp", [panel = animationEdit3DFrameControlStickPanel](Event* pEvent) {

		StateComponent* stateComponent = panel->GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		if (stateComponent->GetLButtonDownState())
		{
			panel->SetColorItensity({ 1.0F,1.0F,1.0F });
			stateComponent->SetLButtonDownState(false);
			Mouse::ReleaseCaptureObject(panel);

		}


		});








	//start frame stick 
	//PanelUiEntity* animationEdit3DFrameControlStartStickPanel = new PanelUiEntity(L"AnimationEidt3DFrameControlStartStickPanel");
	SetDefaultInitSetting(animationEdit3DFrameControlStartStickPanel);
	//animationEdit3DFrameControlStartStickPanel->Initialize();
	animationEdit3DFrameControlStartStickPanel->SetSize(5, 30);
	animationEdit3DFrameControlStartStickPanel->SetTexture("Blue.png");
	animationEdit3DFrameControlStartStickPanel->GetStateComponent()->SetSelectState(true);
	animationEdit3DStickControlPanel->AddChildPanelUiEntity(animationEdit3DFrameControlStartStickPanel);

	pos.x = -(animationEdit3DStickControlPanel->GetWidth() - 20) / 2 ;
	pos.y = 0;
	pos.z = -1;

	animationEdit3DFrameControlStartStickPanel->GetTransform().SetPositionLocal(pos);


	//end frame stick 
//	PanelUiEntity* animationEdit3DFrameControlEndStickPanel = new PanelUiEntity(L"AnimationEidt3DFrameControlEndStickPanel");
	SetDefaultInitSetting(animationEdit3DFrameControlEndStickPanel);
	//animationEdit3DFrameControlEndStickPanel->Initialize();
	animationEdit3DFrameControlEndStickPanel->SetSize(5, 30);
	animationEdit3DFrameControlEndStickPanel->SetTexture("Red.png");
	animationEdit3DFrameControlEndStickPanel->GetStateComponent()->SetSelectState(true);
	animationEdit3DStickControlPanel->AddChildPanelUiEntity(animationEdit3DFrameControlEndStickPanel);

	pos.x = (animationEdit3DStickControlPanel->GetWidth() - 20) / 2;
	pos.y = 0;
	pos.z = -1;

	animationEdit3DFrameControlEndStickPanel->GetTransform().SetPositionLocal(pos);










	animationEdit3DFrameControlStartStickPanel->RegisterEventCallback("LButtonDown", [panel = animationEdit3DFrameControlStartStickPanel](Event* pEvent) {

		StateComponent* stateComponent = panel->GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		if (stateComponent->GetHoverState() && stateComponent->GetSelectState())
		{
			panel->SetColorItensity({ 2.0F,2.0F,2.0F });
			stateComponent->SetLButtonDownState(true);
			Mouse::CaptureObject(panel);

		}
		else
		{
			panel->SetColorItensity({ 1.0F,1.0F,1.0F });
			stateComponent->SetLButtonDownState(false);
		}
		
		

		
		});

	animationEdit3DFrameControlStartStickPanel->RegisterEventCallback("LButtonUp", [panel = animationEdit3DFrameControlStartStickPanel](Event* pEvent) {
		
		StateComponent* stateComponent = panel->GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		if (stateComponent->GetLButtonDownState())
		{
			panel->SetColorItensity({ 1.0F,1.0F,1.0F });
			stateComponent->SetLButtonDownState(false);
			Mouse::ReleaseCaptureObject(panel);

		}
		
	
		
		});
	animationEdit3DFrameControlStartStickPanel->RegisterEventCallback("MouseMove", [startStick = animationEdit3DFrameControlStartStickPanel,
	endStick = animationEdit3DFrameControlEndStickPanel	, entity = animation3DModelEntity](Event* pEvent) mutable {
		
		StateComponent* stateComponent = startStick->GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		if (stateComponent->GetLButtonDownState())
		{
			int shiftX = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();

			float width = startStick->GetWidth();
			
			DirectX::XMFLOAT3 startSitckpos = startStick->GetTransform().GetPositionLocal();
			DirectX::XMFLOAT3 endStickPos = endStick->GetTransform().GetPositionLocal();

			startSitckpos.x += shiftX;
			if (startSitckpos.x < -100.0f)
				startSitckpos.x = -100.0f;


			if (startSitckpos.x >= (-100.0f ) && startSitckpos.x < endStickPos.x)
			{
				
				startStick->GetTransform().SetPositionLocal(startSitckpos);

				AnimationComponent* animationComponent =entity->GetModel()->GetAnimationComponent();
				if (animationComponent==nullptr || !animationComponent->GetAnimationComponentState())
					return;

				float endLocalSecondTime = animationComponent->GetCurrentAnimationState()->GetAnimationClip()->GetEndTimeSecond();
				float startStickSecondTime = (startSitckpos.x + 100) / 200 * endLocalSecondTime;
				animationComponent->GetCurrentAnimationState()->SetLocalStartTime(startStickSecondTime);

			}

		}

		});





	animationEdit3DFrameControlEndStickPanel->RegisterEventCallback("LButtonDown", [panel = animationEdit3DFrameControlEndStickPanel](Event* pEvent) {

		StateComponent* stateComponent = panel->GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		if (stateComponent->GetHoverState() && stateComponent->GetSelectState())
		{
			panel->SetColorItensity({ 2.0F,2.0F,2.0F });
			stateComponent->SetLButtonDownState(true);
			Mouse::CaptureObject(panel);

		}
		else
		{
			panel->SetColorItensity({ 1.0F,1.0F,1.0F });
			stateComponent->SetLButtonDownState(false);
		}




		});

	animationEdit3DFrameControlEndStickPanel->RegisterEventCallback("LButtonUp", [panel = animationEdit3DFrameControlEndStickPanel](Event* pEvent) {

		StateComponent* stateComponent = panel->GetStateComponent();

		MouseEvent* mouseEvent = (MouseEvent*)pEvent;

		if (stateComponent->GetLButtonDownState())
		{
			panel->SetColorItensity({ 1.0F,1.0F,1.0F });
			stateComponent->SetLButtonDownState(false);
			Mouse::ReleaseCaptureObject(panel);

		}



		});
	animationEdit3DFrameControlEndStickPanel->RegisterEventCallback("MouseMove", [startStick = animationEdit3DFrameControlStartStickPanel,
		endStick = animationEdit3DFrameControlEndStickPanel, entity = animation3DModelEntity](Event* pEvent) mutable {

			StateComponent* stateComponent = endStick->GetStateComponent();

			MouseEvent* mouseEvent = (MouseEvent*)pEvent;

			if (stateComponent->GetLButtonDownState())
			{
				int shiftX = mouseEvent->GetClientPositionX() - mouseEvent->GetPreClientPositionX();

				float width = endStick->GetWidth();

				DirectX::XMFLOAT3 startStickpos = startStick->GetTransform().GetPositionLocal();
				DirectX::XMFLOAT3 endStickPos = endStick->GetTransform().GetPositionLocal();

				endStickPos.x += shiftX;
				if (endStickPos.x > 100.0f)
					endStickPos.x = 100.0f;

				if (endStickPos.x <= (100.0f) && endStickPos.x > startStickpos.x)
				{
					endStick->GetTransform().SetPositionLocal(endStickPos);

					AnimationComponent* animationComponent = entity->GetModel()->GetAnimationComponent();
					if (animationComponent==nullptr || !animationComponent->GetAnimationComponentState())
						return;

					float endLocalSecondTime = animationComponent->GetCurrentAnimationState()->GetAnimationClip()->GetEndTimeSecond();
					float endStickSecondTime = (endStickPos.x + 100) / 200 * endLocalSecondTime;
					animationComponent->GetCurrentAnimationState()->SetLocalEndTime(endStickSecondTime);


				}

			}

		});

	//ButtonUiEntitiy* animationPlayButton = new ButtonUiEntitiy(L"");
	SetDefaultInitSetting(animationPlayButton);
	//animationPlayButton->Initialize();
	animationPlayButton->SetSize(20, 20);
	animationPlayButton->SetTexture("gamePlayButtonOff.png");


	animationEdit3DFrameControlPanel->AddChildPanelUiEntity(animationPlayButton);

	pos.x = animationEdit3DFrameControlPanel->GetWidth() / 2 - animationPlayButton->GetWidth() / 2 - 5;
	pos.y = animationEdit3DFrameControlPanel->GetHeight() / 2 - animationPlayButton->GetHeight() / 2;
	pos.z = -2;
	animationPlayButton->GetTransform().SetPositionLocal(pos);



	
	animationPlayButton->SetLButtonDownCallback([button = animationPlayButton, playState = animationPlayState,
		startStick = animationEdit3DFrameControlStartStickPanel,endStick = animationEdit3DFrameControlEndStickPanel , controlStick = animationEdit3DFrameControlStickPanel, entity = animation3DModelEntity]() mutable{



		AnimationComponent* animationComponent = entity->GetModel()->GetAnimationComponent();
		if (animationComponent==nullptr || !animationComponent->GetAnimationComponentState())
			return;


		float endLocalSecondTime = animationComponent->GetCurrentAnimationState()->GetAnimationClip()->GetEndTimeSecond();

		if (*playState)
		{
			button->SetTexture("gamePlayButtonOff.png");
			*playState = 0;
			startStick->GetStateComponent()->SetSelectState(true);
			endStick->GetStateComponent()->SetSelectState(true);
			controlStick->GetStateComponent()->SetSelectState(true);

		//	animationComponent.GetCurrentAnimationState()->SetLocalStartTime(0.0f);
		//	animationComponent.GetCurrentAnimationState()->SetLocalEndTime(endLocalSecondTime);
			animationComponent->SetUpdateFlag(false);


		}
		else
		{
			button->SetTexture("gamePlayButtonOn.png");
			*playState = 1;


			startStick->GetStateComponent()->SetSelectState(false);
			endStick->GetStateComponent()->SetSelectState(false);
			controlStick->GetStateComponent()->SetSelectState(false);


			animationComponent->SetUpdateFlag(true);



		}

	});


	/*auto animaitonPlayButtonResetLamda = [button = animationPlayButton, playState = false,
		startStick = animationEdit3DFrameControlStartStickPanel, endStick = animationEdit3DFrameControlEndStickPanel, controlStick = animationEdit3DFrameControlStickPanel, entity = animation3DModelEntity]() {
		
		}*/





	//split panel 

	// new animation clip name text box panel  , ���� ��ư ,  ����,���� �ؽ�Ʈ�ڽ� �ǳ� 

//	PanelUiEntity* splitPanel = new PanelUiEntity("splitPanel");
//	PanelUiEntity* splitPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* splitPanel = PanelUiEntity::Create(map, mapLayerIndex);
	SetDefaultInitSetting(splitPanel);
	//splitPanel->Initialize();
	splitPanel->SetSize(300, 70);
	splitPanel->SetTexture("33343c.png");

	animMainPanel->AddChildPanelUiEntity(splitPanel);
	pos = animationEdit3DPanel->GetTransform().GetPositionLocal();
	pos.x = -animMainPanel->GetWidth() / 2 + 10 + splitPanel->GetWidth() / 2;
	pos.y -= (animationEdit3DPanel->GetHeight() / 2 +10 +splitPanel->GetHeight()/2);
	pos.z = -1;
	splitPanel->GetTransform().SetPositionLocal(pos);
	

	//ButtonUiEntitiy* splitButton = new ButtonUiEntitiy(L"splitButton");
	//SetDefaultInitSetting(splitButton);
	//splitButton->Initialize();
	//splitButton->SetSize(30, 30);
	//splitButton->SetTexture(L"Green.png");

	//splitPanel->AddChildPanelUiEntity(splitButton);
	//

	//pos.x =splitPanel->GetWidth() / 2 - 5 - splitButton->GetWidth() / 2;
	//pos.y = 25.0f;
	//pos.z = -1;
	//splitButton->GetTransform().SetPositionLocal(pos);

	float horiMargin = 5;
	float verMargin = 5;
//	TextBoxPanel* splitTextBoxPanel = new TextBoxPanel("splitTextBoxPanel");
	//TextBoxPanel* splitTextBoxPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	TextBoxPanel* splitTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 60, 20);
	SetDefaultInitSetting(splitTextBoxPanel);
	//splitTextBoxPanel->Initialize(60, 20);
	splitTextBoxPanel->SetTexture("EngineDefaultButtonFrame.png");
	splitTextBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	splitTextBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	splitTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"분할"));
	
	splitPanel->AddChildPanelUiEntity(splitTextBoxPanel);

	pos.x = splitPanel->GetWidth() / 2 - horiMargin - splitTextBoxPanel->GetWidth() / 2;
	pos.y = splitPanel->GetHeight() / 2 - verMargin - splitTextBoxPanel->GetHeight()/2; 
	pos.z = -1;
	splitTextBoxPanel->GetTransform().SetPositionLocal(pos);
	selectComponent = new AttrUiSystemSelectObjectComponent;
	splitTextBoxPanel->AddComponent(selectComponent);





	

	//TextBoxPanel* splitTextBoxNamePanel = new TextBoxPanel("splitTextBoxNamePanel");
	//TextBoxPanel* splitTextBoxNamePanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	TextBoxPanel* splitTextBoxNamePanel = TextBoxPanel::Create(map, mapLayerIndex, 30, 20);
	SetDefaultInitSetting(splitTextBoxNamePanel);
	//splitTextBoxNamePanel->Initialize(30, 20);
	splitTextBoxNamePanel->SetTexture("Transparent.png");
	splitTextBoxNamePanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	splitTextBoxNamePanel->SetTextColor({ 5.0f,5.0f,5.0f });
	splitTextBoxNamePanel->SetText(reinterpret_cast<const char*>(u8"이름"));

	//splitTextBoxNamePanel->SetDrawFlag(false);

	splitPanel->AddChildPanelUiEntity(splitTextBoxNamePanel);

	pos.x = -splitPanel->GetWidth() / 2 + horiMargin + splitTextBoxNamePanel->GetWidth() / 2;
	pos.y = splitPanel->GetHeight() / 2 - verMargin - splitTextBoxNamePanel->GetHeight() / 2; ;
	pos.z = -1;
	splitTextBoxNamePanel->GetTransform().SetPositionLocal(pos);



	





	

	//TextBox* splitTextBoxNameInput = new TextBox("splitTextBoxNameInput");
//	TextBox* splitTextBoxNameInput = static_cast<TextBox*>(map->CreateObject("TextBox"));
	TextBox* splitTextBoxNameInput = TextBox::Create(map, mapLayerIndex, 255);
	SetDefaultInitSetting(splitTextBoxNameInput);
	//splitTextBoxNameInput->Initialize(255);
	splitTextBoxNameInput->SetSize(180, 20);
	splitTextBoxNameInput->SetTexture("TextBox.png");
	//splitTextBoxNameInputPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	splitTextBoxNameInput->SetTextColor({ 0.1f,0.1f,0.1f });
	splitTextBoxNameInput->SetText(reinterpret_cast<const char*>(u8"에니메이션클립이름입력"));
	

	//splitTextBoxNamePanel->SetDrawFlag(false);

	splitPanel->AddChildPanelUiEntity(splitTextBoxNameInput);

	pos.x = -splitPanel->GetWidth() / 2 + horiMargin + splitTextBoxNamePanel->GetWidth() + horiMargin + splitTextBoxNameInput->GetWidth() / 2;
	pos.y = splitPanel->GetHeight() / 2 - verMargin - splitTextBoxNameInput->GetHeight() / 2; ;
	pos.z = -1;
	splitTextBoxNameInput->GetTransform().SetPositionLocal(pos);

	splitTextBoxNameInput->SetEnterEndFlag(true);















	//result text box panel 
	//TextBoxPanel* splitTextBoxResultPanel = new TextBoxPanel("splitTextBoxResultPanel");
//	TextBoxPanel* splitTextBoxResultPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	TextBoxPanel* splitTextBoxResultPanel = TextBoxPanel::Create(map, mapLayerIndex, 300, 200);
	SetDefaultInitSetting(splitTextBoxResultPanel);
//	splitTextBoxResultPanel->Initialize(300, 20);
	splitTextBoxResultPanel->SetTexture("Transparent.png");
	//splitTextBoxNameInputPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	splitTextBoxResultPanel->SetTextColor({ 5.0f,5.0f,5.0f });
	splitTextBoxResultPanel->SetText(reinterpret_cast<const char*>(u8"결과 : "));

	//splitTextBoxNamePanel->SetDrawFlag(false);

	splitPanel->AddChildPanelUiEntity(splitTextBoxResultPanel);

	pos.x = -splitPanel->GetWidth() / 2 + horiMargin + splitTextBoxResultPanel->GetWidth() / 2;
	pos.y = -splitPanel->GetHeight() / 2 + verMargin + splitTextBoxNameInput->GetHeight() / 2; ;
	pos.z = -1;
	splitTextBoxResultPanel->GetTransform().SetPositionLocal(pos);









	

	splitTextBoxPanel->RegisterEventCallback("Hover", [splitbutton = splitTextBoxPanel](Event* pEvent) {

		StateComponent* stateComponent = splitbutton->GetStateComponent();
			if (stateComponent->GetHoverState())
			{
				splitbutton->SetColorItensity({ 1.3f,1.3f,1.3f });
			}
			else
			{
				splitbutton->SetColorItensity({ 1.0f,1.0f,1.0f });
			}




		});

	splitTextBoxPanel->RegisterEventCallback("LButtonDown", [splitbutton = splitTextBoxPanel, newAnimNameTextBox = splitTextBoxNameInput,
		resultTextBoxPanel = splitTextBoxResultPanel, entity = animation3DModelEntity](Event* pEvent) {


			StateComponent* stateComponent = splitbutton->GetStateComponent();
			if (!stateComponent->GetHoverState())
				return;




			stateComponent->SetLButtonDownState(true);
			splitbutton->SetColorItensity({ 2.0f,2.0f,2.0f });



			const std::string & newAnimationClipName = newAnimNameTextBox->GetText();
			
			if (newAnimationClipName == "")
			{
				resultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 에니메이션클립이름이없습니다."));
				return;
			}


			if (AnimationClipManager::GetAnimationClip(newAnimationClipName))
			{
				
				resultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 에니메이션클립을 찾을수없습니다."));
				return;
			}
		


			AnimationComponent * animationComponent  =entity->GetModel()->GetAnimationComponent();

				
			if (animationComponent==nullptr || !animationComponent->GetAnimationComponentState())
					return;

			AnimationState* animationState = animationComponent->GetCurrentAnimationState();

			int framePerSecond = 0;
			switch (animationState->GetAnimationClip()->GetTimeMode())
			{
			case EAnimationTimeMode::eFPS30:

				framePerSecond = 30;
				break;

			case EAnimationTimeMode::eFPS60:
				framePerSecond = 60;
				break;
			}



			float startSecondTime = animationState->GetLocalStartTime();
			float endSecondTime = animationState->GetLocalEndTime();

			int startFrameCount = startSecondTime * framePerSecond;
			int endFrameCount = endSecondTime * framePerSecond;




			auto fileUiUiSystem = FileUiUiSystem::GetInstance();
	
			std::string logicalFolderPath = fileUiUiSystem->GetLogicalFolderPath(fileUiUiSystem->GetCurrentFolder());
			//logicalFolderPath = logicalFolderPath 


			AnimationClip * newAnimationClip =	AnimationClipSplitter::SplitAnimationClip(animationComponent->GetCurrentAnimationState()->GetAnimationClip(), logicalFolderPath+"/"+newAnimationClipName, startFrameCount, endFrameCount);
				
			if (newAnimationClip)
			{
				resultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 성공"));
				 
				fileUiUiSystem->AddAsset({ newAnimationClip });

				//LoadFileResponseEvent* loadFileResponseEvent = new LoadFileResponseEvent;
				//loadFileResponseEvent->SetAssetVector({ newAnimationClip });
				//
				//EventDispatcher::SendEvent(loadFileResponseEvent, ESystemID::eFileUiUiSystem);
			}





		});


















	//state create panel 
	float stateCreationPanelWidth = 500;
	float stateCreationPanelHeight = 100;

	pos = animationEdit3DPanel->GetTransform().GetPositionLocal();
	pos.x += (animationEdit3DPanel->GetWidth() / 2 + 10 + stateCreationPanelWidth / 2);
	pos.y = stateClipScrollListPanel->GetTransform().GetPositionLocal().y - stateClipScrollListPanel->GetHeight() / 2 - 10 - stateCreationPanelHeight / 2;
	pos.z = -1;
	PanelUiEntity* stateCreationPanel = CreateDefaultPanel("stateCreationPanel", "Gray.png", animMainPanel, pos, stateCreationPanelWidth, stateCreationPanelHeight);


	float stateCreationInputTagTextBoxPanelWidth = 80;
	float stateCreationInputTagTextBoxPanelHeight = 20;
	pos.x = -stateCreationPanelWidth / 2 + 5 + stateCreationInputTagTextBoxPanelWidth / 2;
	pos.y = stateCreationPanelHeight / 2 - 5 - stateCreationInputTagTextBoxPanelHeight / 2;
	TextBoxPanel* stateInputTagTextBoxPanel = CreateDefaultTextBoxPanel("stateCreationInputTagTextBoxPanel", "Transparent.png", stateCreationPanel,
		pos, reinterpret_cast<const char*>(u8"상태 :"), stateCreationInputTagTextBoxPanelWidth, stateCreationInputTagTextBoxPanelHeight, {0.0f,0.0f,0.0f});



	float stateCreationInputTextBoxWidth = 300;
	float stateCreationInputTextBoxHeight = 20;
	pos.x = stateInputTagTextBoxPanel->GetTransform().GetPositionLocal().x + stateCreationInputTagTextBoxPanelWidth / 2 + 5 + stateCreationInputTextBoxWidth / 2;
	pos.y = stateCreationPanelHeight / 2 - 5 - stateCreationInputTextBoxHeight / 2;

	TextBox* stateInputTextBox = CreateDefaultTextBox(255, "stateInputTextBox", "TextBox.png", stateCreationPanel, pos, "", stateCreationInputTextBoxWidth,
		stateCreationInputTextBoxHeight, { 0.0f,0.0f,0.0f });



	float animaitonClipTagTextBoxPanelWidth = 80;
	float animaitonClipTagTextBoxPanelHeight = 20;
	pos.x = -stateCreationPanelWidth / 2 + 5 + animaitonClipTagTextBoxPanelWidth / 2;
	pos.y = stateInputTagTextBoxPanel->GetTransform().GetPositionLocal().y - stateCreationInputTagTextBoxPanelHeight / 2 - 5 - animaitonClipTagTextBoxPanelHeight / 2;

	TextBoxPanel* animationClipTagTextBoxPanel = CreateDefaultTextBoxPanel("animationClipTagTextBoxPanel", "Transparent.png", stateCreationPanel,
		pos, reinterpret_cast<const char*>(u8"클립 :"), animaitonClipTagTextBoxPanelWidth, animaitonClipTagTextBoxPanelHeight, { 0.0f,0.0f,0.0f });


	float animationClipInputPanelWidth = 300;
	float animationClipInputPanelHeight = 20;

	pos.x = animationClipTagTextBoxPanel->GetTransform().GetPositionLocal().x + animaitonClipTagTextBoxPanelWidth / 2 +5 +animationClipInputPanelWidth/2;
	TextBoxPanel* animationClipTextBoxPanel = CreateDefaultTextBoxPanel("animationClipTextBoxPanel", "TextBox.png", stateCreationPanel, pos, "", animationClipInputPanelWidth, animationClipInputPanelHeight, {0,0,0});



	float stateCreationButtonPanelWidth = 40;
	float stateCreationButtonPanelHeight = 20;
	pos.x = stateCreationPanel->GetWidth() / 2 - 5 - stateCreationButtonPanelWidth / 2;
	pos.y = -stateCreationPanel->GetHeight() / 2 + 5 + stateCreationButtonPanelHeight / 2;
	TextBoxPanel* stateCreationButtonPanel = CreateDefaultTextBoxPanel("stateCreationButtonPanel", "EngineDefaultButtonFrame.png", stateCreationPanel,
		pos, reinterpret_cast<const char*>(u8"생성"), stateCreationButtonPanelWidth, stateCreationButtonPanelHeight, { 0.0f,0.0f,0.0f },ETextAlignmentDirection::eCenter);
	selectComponent = new AttrUiSystemSelectObjectComponent;
	stateCreationButtonPanel->AddComponent(selectComponent);

	//remove button panel
	float stateRemoveButtonPanelWidth = 40;
	float stateRemoveButtonPanelHeight = 20;
	pos.x = stateCreationPanel->GetWidth() / 2 - 5 - stateCreationButtonPanelWidth -5 - stateRemoveButtonPanelWidth/2;
	pos.y = -stateCreationPanel->GetHeight() / 2 + 5 + stateRemoveButtonPanelHeight / 2;
	TextBoxPanel* stateRemoveButtonPanel = CreateDefaultTextBoxPanel("stateRemoveButtonPanel", "EngineDefaultButtonFrame.png", stateCreationPanel,
		pos, reinterpret_cast<const char*>(u8"제거"), stateRemoveButtonPanelWidth, stateRemoveButtonPanelHeight, { 0.0f,0.0f,0.0f },ETextAlignmentDirection::eCenter);
	selectComponent = new AttrUiSystemSelectObjectComponent;
	stateRemoveButtonPanel->AddComponent(selectComponent);







	float stateCreationResultTextBoxPanelWidth = 300;
	float stateCreationResultTextBoxPanelHeight = 20;


	pos.x = -stateCreationPanel->GetWidth() / 2 + 5 + stateCreationResultTextBoxPanelWidth / 2;
	pos.y = -stateCreationPanel->GetHeight() / 2 + 5 + stateCreationResultTextBoxPanelHeight / 2;

	TextBoxPanel* stateCreationResultTextBoxPanel = CreateDefaultTextBoxPanel("stateCreationResultTextBoxPanel", "Transparent.png", stateCreationPanel,
		pos, reinterpret_cast<const char*>(u8"결과 : "), stateCreationResultTextBoxPanelWidth, stateCreationResultTextBoxPanelHeight, { 0,0,0 });













	//���»��� �ǳ� ���� .

 
	stateCreationPanel->RegisterEventCallback("GameObjectSelect", std::bind(sendEventCallbackLamda, std::placeholders::_1, stateCreationPanel));
	stateCreationButtonPanel->RegisterEventCallback("GameObjectSelect", [creationButtonPanel = stateCreationButtonPanel](Event* pEvent) {


		AttrUiSystemSelectObjectComponent* selectComponent = static_cast<AttrUiSystemSelectObjectComponent*>(creationButtonPanel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>()));


		GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;
		Object* selectedObject = selectEvent->GetSelectObject();

		selectComponent->SetSelectedObject(selectedObject);


		});
	stateCreationButtonPanel->RegisterEventCallback("LButtonDown", [creationButtonPanel = stateCreationButtonPanel](Event* pEvent) {

		StateComponent* stateComponent = creationButtonPanel->GetStateComponent();
		if (stateComponent->GetHoverState())
		{
			stateComponent->SetLButtonDownState(true);
			creationButtonPanel->SetColorItensity({ 1.5f,1.5f,1.5f });
			Mouse::CaptureObject(creationButtonPanel);
		}

		});
	stateCreationButtonPanel->RegisterEventCallback("LButtonUp", [creationButtonPanel = stateCreationButtonPanel,animationClipNameTextBoxPanel = animationClipTextBoxPanel, stateNameTextBox = stateInputTextBox, creationResultTextBoxPanel  = stateCreationResultTextBoxPanel , stateClipListPanel = stateClipScrollListPanel
	](Event* pEvent) {

		StateComponent* stateComponent = creationButtonPanel->GetStateComponent();



	
		if (stateComponent->GetHoverState() && stateComponent->GetLButtonDownState())
		{

			

			AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*)creationButtonPanel->GetComponent(ComponentIDGenerator::GetID< AttrUiSystemSelectObjectComponent>());

			Object* selectedObject = selectComponent->GetSelectedObject();
			

			if (selectedObject == nullptr || selectedObject->GetObjectType() != EObjectType::eEntity)
			{
				Mouse::ReleaseCaptureObject(creationButtonPanel);
				stateComponent->SetLButtonDownState(false);
				creationButtonPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
				return;
			
			}
			Entity* entity = (Entity*)selectedObject;

			AnimationComponent * animationComponent = entity->GetModel()->GetAnimationComponent();

			

			const std::string& clipName = animationClipNameTextBoxPanel->GetText();
			AnimationClip* animationClip = AnimationClipManager::GetAnimationClip(clipName);
			if (animationClip == nullptr)
			{
				

				creationResultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 실패(클립을 찾을수없습니다)"));
				Mouse::ReleaseCaptureObject(creationButtonPanel);
				creationButtonPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
				stateComponent->SetLButtonDownState(false);
				return;
			}

			const std::string& newStateName = stateNameTextBox->GetText();
			
			//상태이름이없음 실패
			if (newStateName == "")
			{
				creationResultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 실패(상태이름이없습니다)"));
				creationButtonPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
				Mouse::ReleaseCaptureObject(creationButtonPanel);
				stateComponent->SetLButtonDownState(false);
				return;
			}


			
			if (entity->GetModel()->GetMeshComponent()->GetMesh() != animationClip->GetDefaultMesh())
			{
				//에니메이션에맞는 메시가아니다.
				creationResultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 실패(에니메이션클립에 맞지않는메시)"));
				creationButtonPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
				Mouse::ReleaseCaptureObject(creationButtonPanel);
				stateComponent->SetLButtonDownState(false);
				return;
			}
			if (animationComponent->GetSkeleton() != animationClip->GetDefaultSkeleton())
			{
				//에니메이션에맞는 스켈레톤이아니다.
				creationResultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 실패(에니메이션클립에 맞지않는스켈레톤)"));
				creationButtonPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
				Mouse::ReleaseCaptureObject(creationButtonPanel);
				stateComponent->SetLButtonDownState(false);

				return;
			}

			if(!animationComponent->AddState(newStateName, animationClip))
			{
				//새로운상태만들기 실패
				creationResultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 실패(상태만들기실패)"));
				creationButtonPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
				Mouse::ReleaseCaptureObject(creationButtonPanel);
				stateComponent->SetLButtonDownState(false);

				return;
			}
			//����

			creationResultTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과 : 성공"));

			creationButtonPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
			//Mouse::ReleaseCaptureObject(creationButtonPanel);

			animationComponent->SetCurrentAnimationState(newStateName);
			auto AnimationStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();
			AnimationStateTransitionGraphBasePanel->CreateStateNode(newStateName);



			
	
			int stateNum = animationComponent->GetAnimationStateTable().size();

			const std::vector<UiEntity*>& childListPanelVector = stateClipListPanel->GetChildPanelUiEntityVector();
			const PanelUiEntity * listPanel = (PanelUiEntity*)childListPanelVector[stateNum];

			TextBoxPanel* stateTextBoxPanel = (TextBoxPanel*)listPanel->GetChildObjectVector()[0].GetPointer();
			TextBoxPanel* clipTextBoxPanel = (TextBoxPanel*)listPanel->GetChildObjectVector()[1].GetPointer();

			stateTextBoxPanel->SetText(newStateName);
			clipTextBoxPanel->SetText(clipName);

		
			stateClipListPanel->ActivateChildListPanel(stateNum);
			//stateClipListPanel->ActivateChildListPanel(0);
			//stateClipListPanel->ActivateChildListPanel(stateNum);











		}

		stateComponent->SetLButtonDownState(false);
		Mouse::ReleaseCaptureObject(creationButtonPanel);

		});

	stateCreationPanel->RegisterEventCallback("NotifyDragAndDrop", std::bind(sendEventCallbackLamda, std::placeholders::_1, stateCreationPanel));
	animationClipTextBoxPanel->RegisterEventCallback("NotifyDragAndDrop", [panel = animationClipTextBoxPanel](Event *pEvent) {

		NotifyDragAndDropEvent* dragAndDropEvent = (NotifyDragAndDropEvent*)pEvent;
		if (!dragAndDropEvent->GetLButtonUpState())
			return;

		ContentItem * contentItem = dragAndDropEvent->GetContentItem();
		if (contentItem->GetEContentItemType() != EContentItemType::eAsset)
			return;

		Asset* asset = (Asset*)contentItem;

		if (asset->GetAssetType() != EAssetType::eAnimationClip)
			return;

		
		const Ray& ray = dragAndDropEvent->GetRay();
		float minT = FLT_MAX;
		if (CollisionHelper::Intersect(panel->GetModel()->GetUiColliderComponent()->GetCollider(), ray, minT))
		{
			panel->SetText(asset->GetName());
		}

		});





	stateRemoveButtonPanel->RegisterEventCallback("GameObjectSelect", [removeButtonPanel = stateRemoveButtonPanel](Event* pEvent) {


		AttrUiSystemSelectObjectComponent* selectComponent = static_cast<AttrUiSystemSelectObjectComponent*>(removeButtonPanel->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>()));


		GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;
		Object* selectedObject = selectEvent->GetSelectObject();

		selectComponent->SetSelectedObject(selectedObject);


		});
	stateRemoveButtonPanel->RegisterEventCallback("LButtonDown", [removeButtonPanel = stateRemoveButtonPanel](Event* pEvent) {

		StateComponent* stateComponent = removeButtonPanel->GetStateComponent();
		if (stateComponent->GetHoverState())
		{
			stateComponent->SetLButtonDownState(true);
			removeButtonPanel->SetColorItensity({ 1.5f,1.5f,1.5f });
			Mouse::CaptureObject(removeButtonPanel);
		}

		});
	stateRemoveButtonPanel->RegisterEventCallback("LButtonUp", [removeButtonPanel = stateRemoveButtonPanel,  stateNameTextBox = stateInputTextBox, creationResultTextBoxPanel = stateCreationResultTextBoxPanel, stateClipListPanel = stateClipScrollListPanel
	](Event* pEvent) {

			StateComponent* stateComponent = removeButtonPanel->GetStateComponent();

			if (stateComponent->GetLButtonDownState() && stateComponent->GetHoverState())
			{
			

				AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*)removeButtonPanel->GetComponent(
				ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());
				



				Object* selectedObject = selectComponent->GetSelectedObject();
				if (selectedObject == nullptr)
				{
					Mouse::ReleaseCaptureObject(removeButtonPanel);
					return;
				}




				if (selectedObject->GetObjectType() == EObjectType::eEntity)
				{
					Entity* entity = (Entity*)selectedObject;

					AnimationComponent& animationComponent =*entity->GetModel()->GetAnimationComponent();
					const std::string& stateName = stateNameTextBox->GetText();
					

					if (animationComponent.RemoveState(stateName))
					{
						creationResultTextBoxPanel->SetText("��� : ���ż���");



						
						const std::vector<UiEntity*>& childPanelVector = stateClipListPanel->GetChildPanelUiEntityVector();
						int childPanelIndex = -1;
						for (int i = 1; i <= stateClipListPanel->GetActiveChildPanelNum(); ++i)
						{

							TextBoxPanel* stateTextBoxPanel = (TextBoxPanel*)childPanelVector[i]->GetChildObjectVector()[0].GetPointer();
							if (stateTextBoxPanel->GetText() == stateName)
							{
								
								childPanelIndex = i;
								break;
							}
						}

						stateClipListPanel->InActiveChildPanel(childPanelIndex);


						AnimationStateTransitionGraphBasePanel* animationStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();

						animationStateTransitionGraphBasePanel->RemoveStateNode(stateName);

					}
					else
					{
						creationResultTextBoxPanel->SetText("��� : ���Ž���");
					}
					
				}
				removeButtonPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
			}
		
			stateComponent->SetLButtonDownState(false);
			Mouse::ReleaseCaptureObject(removeButtonPanel);
		});
	



	stateClipScrollListPanel->RegisterEventCallback("LButtonDown", [panel = stateClipScrollListPanel, stateInputTextBox](Event* pEvent) {


		static int preSelectedChildPanelIndex = 0;

		if (preSelectedChildPanelIndex != 0)
		{
			std::vector<UiEntity*>& childPanelVector = panel->GetChildPanelUiEntityVector();
			PanelUiEntity* preSelectedChildPanel = (PanelUiEntity*)childPanelVector[preSelectedChildPanelIndex];
			preSelectedChildPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
		}

		PanelUiEntity* currentSelectChildPanel = (PanelUiEntity*)panel->GetCurrentSelectedChildPanel();
		preSelectedChildPanelIndex = panel->GetCurrentSelectedChildPanelIndex();
		if (currentSelectChildPanel == nullptr)
		{
			return;
		}

		TextBoxPanel* stateTextBoxPanel = (TextBoxPanel*)currentSelectChildPanel->GetChildPanelUiEntityVector()[0];
		TextBoxPanel* clipTextBoxPanel= (TextBoxPanel*)currentSelectChildPanel->GetChildPanelUiEntityVector()[1];

		currentSelectChildPanel->SetColorItensity({ 0.0f,2.0f,0.0f });
		const std::string& stateName = stateTextBoxPanel->GetText();


		stateInputTextBox->SetText(stateName);

		});

	 




	//EditAnimationTranslationButton
	//

	ButtonUiEntity* editAnimationTranslationButton = ButtonUiEntity::Create(map, 0);
	float editAnimationTranslationButtonWidth = 100;
	float editAnimationTranslationButtonHeight =50;
	SetDefaultInitSetting(editAnimationTranslationButton);
	editAnimationTranslationButton->SetSize(editAnimationTranslationButtonWidth, editAnimationTranslationButtonHeight);
	animMainPanel->AddChildPanelUiEntity(editAnimationTranslationButton);

	DirectX::XMFLOAT3 editAnimationTranslationButtonPosL =	stateClipScrollListPanel->GetTransform().GetPositionLocal();
	editAnimationTranslationButtonPosL.x += stateClipScrollListPanel->GetWidth() / 2 + 10 +		editAnimationTranslationButtonWidth/2 ;//10은 margin 

	selectComponent = new AttrUiSystemSelectObjectComponent;
	editAnimationTranslationButton->AddComponent(selectComponent);

	editAnimationTranslationButton->RegisterEventCallback("GameObjectSelect", [button = editAnimationTranslationButton](Event* pEvent) {

		AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*)button->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());
		GameObjectSelectEvent* selectEvent = (GameObjectSelectEvent*)pEvent;

		Object * selectObject =	selectEvent->GetSelectObject();
		AnimationComponent* animComponent = selectObject->GetModel()->GetAnimationComponent();


		AnimationStateTransitionGraphBasePanel* animationStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();


		if (animComponent ==nullptr || !animComponent->GetAnimationComponentState())
		{
			selectComponent->SetSelectedObject(nullptr);
			animationStateTransitionGraphBasePanel->SetDestObject(nullptr);

		}
		else
		{
			selectComponent->SetSelectedObject(selectObject);
			animationStateTransitionGraphBasePanel->SetDestObject(selectObject);
		}

		});



	editAnimationTranslationButton->SetPosition(editAnimationTranslationButtonPosL);
	editAnimationTranslationButton->SetLButtonUpCallback([button = editAnimationTranslationButton]() {

		AttrUiSystemSelectObjectComponent* selectComponent = (AttrUiSystemSelectObjectComponent*)button->GetComponent(ComponentIDGenerator::GetID<AttrUiSystemSelectObjectComponent>());
	
		Object* selectedObject = selectComponent->GetSelectedObject();
		if (selectedObject != nullptr)
		{
			auto frameWindowMainSceneDirector = FrameWindowMainSceneDirector::GetInstance();
	

			frameWindowMainSceneDirector->SetAnimStateEditScene(selectedObject);

			EditorDirector::SetSwitchWindowSceneModeFlag(1);
			//auto * frameWindow =FrameWindow::GetInstance();
			//frameWindow->SetProjectSelectSceneFlag(true);

			//ProjectDirector::SetOhterWindowSystemOff();

		}


		});






}

void Quad::AttributeUiSystem::SetDefaultInitSetting(UiEntity* entity)
{
	//entity->SetSystem(this);
	entity->GetTransform().SetIndependentRotationFlag(true);
	entity->GetTransform().SetIndependentScaleFlag(true);
	entity->SetSelectAvailableFlag(false);

}

Quad::TransformPropertyPanel* Quad::AttributeUiSystem::CreatePropertyPanel(const std::string& panelName, const std::string& tagName, float localWidth, float localHeight, int textBoxNum ,  const std::string& textureName)
{
	Map* map = GetMap();
	int mapLayerIndex = 0;
//	TransformPropertyPanel* panel = new TransformPropertyPanel(panelName);
//	TransformPropertyPanel* panel = static_cast<TransformPropertyPanel*>(map->CreateObject("TransformPropertyPanel"));
	TransformPropertyPanel* panel = TransformPropertyPanel::Create(map, mapLayerIndex, localWidth, localHeight, tagName, textBoxNum);

	panel->SetName(panelName);
//	panel->SetSystem(this);
	//panel->Initialize(localWidth, localHeight, tagName, textBoxNum);
	panel->GetTransform().SetIndependentRotationFlag(true);
	panel->GetTransform().SetIndependentScaleFlag(true);
	panel->SetSelectAvailableFlag(false);
	panel->SetTexture(textureName);


	return panel;
}

Quad::PanelUiEntity* Quad::AttributeUiSystem::CreateDefaultPanel(const std::string& panelName, const std::string& textureName, PanelUiEntity* parentPanel, const DirectX::XMFLOAT3& localPosition, float width, float height)
{
	Map* map = GetMap();
	int mapLayerIndex = 0;
//	PanelUiEntity* newPanel = new PanelUiEntity(panelName);
	//PanelUiEntity* newPanel = static_cast<PanelUiEntity*>(map->CreateObject("PanelUiEntity"));
	PanelUiEntity* newPanel = PanelUiEntity::Create(map, mapLayerIndex);
	SetDefaultInitSetting(newPanel);
//	newPanel->Initialize();
	newPanel->SetSize(width, height);
	newPanel->SetTexture(textureName);
	parentPanel->AddChildPanelUiEntity(newPanel);
	newPanel->GetTransform().SetPositionLocal(localPosition);


	return newPanel;
}

Quad::TextBoxPanel* Quad::AttributeUiSystem::CreateDefaultTextBoxPanel(const std::string& panelName, const std::string& textureName, PanelUiEntity* parentPanel, const DirectX::XMFLOAT3& localPosition, const std::string& text, float width, float height,const DirectX::XMFLOAT3 & textColor,ETextAlignmentDirection textAlignmentDirection)
{
	Map* map = GetMap();
	int mapLayerIndex = 0;
	//TextBoxPanel* newTextBoxPanel = new TextBoxPanel(panelName);
	//TextBoxPanel* newTextBoxPanel = static_cast<TextBoxPanel*>(map->CreateObject("TextBoxPanel"));
	TextBoxPanel* newTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, width, height);
	SetDefaultInitSetting(newTextBoxPanel);
	//newTextBoxPanel->Initialize(width, height);
	newTextBoxPanel->SetTexture(textureName);
	newTextBoxPanel->SetTextAlignmentDirection(textAlignmentDirection);

	newTextBoxPanel->SetText(text);
	newTextBoxPanel->SetTextColor(textColor);
	parentPanel->AddChildPanelUiEntity(newTextBoxPanel);
	newTextBoxPanel->GetTransform().SetPositionLocal(localPosition);


	return newTextBoxPanel;
}

Quad::TextBox* Quad::AttributeUiSystem::CreateDefaultTextBox(int maxCharacterNum ,const std::string& textBoxName, const std::string& textureName, PanelUiEntity* parentPanel, const DirectX::XMFLOAT3& localPosition, const std::string& text, float width, float height, const DirectX::XMFLOAT3& textColor)
{
	Map* map = GetMap();
	int mapLayerIndex = 0;
	//TextBox* newTextBox = new TextBox(textBoxName);
//	TextBox* newTextBox = static_cast<TextBox*>(map->CreateObject("TextBox"));
	TextBox* newTextBox = TextBox::Create(map, mapLayerIndex, maxCharacterNum);
	SetDefaultInitSetting(newTextBox);
	//newTextBox->Initialize(maxCharacterNum);
	newTextBox->SetSize(width, height);
	newTextBox->SetTexture(textureName);
	newTextBox->SetTextColor(textColor);
	newTextBox->SetText(text);
	parentPanel->AddChildPanelUiEntity(newTextBox);
	newTextBox->GetTransform().SetPositionLocal(localPosition);

	return newTextBox;
}




