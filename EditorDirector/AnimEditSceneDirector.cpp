#include "AnimEditSceneDirector.h"

#include"System/FrameWindowUiSystem.h"
#include"ScrollListPanel.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"TextBoxPanel.h"
#include"AnimationStateTransitionGraph.h"
#include"AnimationStateTransitionGraphBasePanel.h"
#include"ButtonUiEntitIy.h"
#include"EditorDirector/EditorDirector.h"
#include"ProjectDirector.h"
#include"System/PopupWindowUiSystem.h"
#include"PopupWindow.h"

#include"ListPanel.h"
#include"MouseEvent.h"

#include"AnimationStateTransitionNodeEditDetailMainPanel.h"
#include"Object/Camera/Camera.h"


Quad::AnimEditSceneDirector::AnimEditSceneDirector()
	:mMainPanel(nullptr), mStateTransitionGraphBasePanel(nullptr), mPopupCreatingNodePanelNum(50)
{
}

Quad::AnimEditSceneDirector::~AnimEditSceneDirector()
{
}

void Quad::AnimEditSceneDirector::Initialize(FrameWindowUiSystem* frameWindowUiSystem)
{
	
	mFrameWindowUiSystem = frameWindowUiSystem;

	CreateAnimStateEditSecene();

}

bool Quad::AnimEditSceneDirector::ActiveScene(Object* object)
{
	AnimationComponent* animComponent = static_cast<AnimationComponent*>(object->GetModel()->GetComponent(Quad::EComponentType::eAnimationComponent));


	if (animComponent == nullptr)
		return false;
	mDestObject = object;
	//mStateTransitionGraphBasePanel->SetDestObject(object);


	mMainPanel->SetDrawFlag(true);
	mMainPanel->SetActiveFlag(true);
	mMainPanel->SetSelectAvailableFlag(true);

	const auto & stateTable =	animComponent->GetAnimationStateTable();

	size_t stateNum = stateTable.size();
	
	//MAX개수 체크
	auto& scrollChildPanelVetor = mStateScrollListPanel->GetChildPanelUiEntityVector();
	int scrollChildPanelIndex = 1;
	//for(auto & stateElement : stateTable)
	//{
	//	TextBoxPanel * textBoxPanel =	 static_cast<TextBoxPanel*>(scrollChildPanelVetor[scrollChildPanelIndex]);
	//	textBoxPanel->SetText(stateElement.second->GetName());
	//	scrollChildPanelIndex++;
	//}

	//mStateScrollListPanel->ActivateChildListPanel(stateNum);

	AnimationStateTransitionGraph * animStateTransitionGraph =	mStateTransitionGraphBasePanel->GetCurrentAnimStateTransitionGraph();



//	animStateTransitionGraph->CreateStateNode("AA");












	return true;


}

void Quad::AnimEditSceneDirector::InActiveScene()
{
	mMainPanel->SetDrawFlag(false);
	//mMainPanel->SetActiveFlag(false);
	mMainPanel->SetSelectAvailableFlag(false);
	mStateTransitionGraphBasePanel->InActive();

	mDestObject = nullptr;

}

void Quad::AnimEditSceneDirector::CreateAnimStateEditSecene()
{
	Map* map = mFrameWindowUiSystem->GetMap();

	PanelUiEntity* mainScenePanel = PanelUiEntity::Create(map);
	float mainScenePanelWidth = 10000;
	float mainScenePanelHeight = 10000;

	mainScenePanel->SetSize(mainScenePanelWidth, mainScenePanelHeight);
	mainScenePanel->SetPosition(mainScenePanelWidth / 2, -mainScenePanelHeight / 2, 100);

	mainScenePanel->SetTexture(TextureManager::GetTexture("DarkGray.png"));


	mainScenePanel->RegisterAcceptEvent("LButtonDown");
	mainScenePanel->RegisterAcceptEvent("LButtonUp");
	mainScenePanel->RegisterAcceptEvent("MouseMove");
	mainScenePanel->RegisterAcceptEvent("Hover");

	mainScenePanel->RegisterAcceptEvent("RButtonDown");
	mainScenePanel->RegisterAcceptEvent("RButtonUp");


	mainScenePanel->RegisterEventCallback("RButtonDown", [](Event* pEvent) {

		PopupWindow::SetVisibilityState(false);
		});
	





	//mainScenePanel->RegisterAcceptEvent("");




	float stateScrollListPanelWidth = 300;
	float stateScrollListPanelHeight = 800;
	ScrollListPanel* stateScrollListPanel = ScrollListPanel::Create(map, 0, stateScrollListPanelWidth, stateScrollListPanelHeight, 150);
	stateScrollListPanel->GetTransform().SetIndependentRotationFlag(true);
	stateScrollListPanel->GetTransform().SetIndependentScaleFlag(true);
	stateScrollListPanel->SetSelectAvailableFlag(false);


	mainScenePanel->AddChildPanelUiEntity(stateScrollListPanel);
	stateScrollListPanel->SetPosition(-mainScenePanelWidth / 2 + stateScrollListPanelWidth / 2, mainScenePanelHeight / 2 - stateScrollListPanelHeight / 2, -2);

	//stateScrollListPanel->SetPosition(-mainPanelSceneWidth / 2, mainPanelSceneHeight / 2, -1);
	stateScrollListPanel->SetDrawFlag(true);
	stateScrollListPanel->SetTexture("Red.png");

	mStateScrollListPanel = stateScrollListPanel;


	TextBoxPanel* textBoxPanel = TextBoxPanel::Create(map, 0, 280, 20);
	textBoxPanel->SetText(reinterpret_cast<const char*>(u8"상태1"));
	textBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	stateScrollListPanel->AddChildPanelUiEntity(textBoxPanel);
	textBoxPanel->SetSelectAvailableFlag(false);


	//돌아가기 버튼(종료버튼)
	ButtonUiEntity* exitButton = ButtonUiEntity::Create(map, 0);

	exitButton->GetTransform().SetIndependentRotationFlag(true);
	exitButton->GetTransform().SetIndependentScaleFlag(true);
	exitButton->SetSelectAvailableFlag(false);
	float exitButtonWidth = stateScrollListPanelWidth;
	float exitButtonHeight = 100;
	exitButton->SetSize(exitButtonWidth, exitButtonHeight);
	exitButton->SetTexture(reinterpret_cast<const char*>(u8"버튼21.png"));

	mainScenePanel->AddChildPanelUiEntity(exitButton);
	exitButton->SetPosition(-mainScenePanelWidth / 2 + exitButtonWidth / 2, stateScrollListPanel->GetTransform().GetPositionLocal().y - stateScrollListPanelHeight / 2 - exitButtonHeight / 2, -1);

	exitButton->SetLButtonUpCallback([]() {

		auto editorDirector = EditorDirector::GetInstance();

		editorDirector->SetSwitchWindowSceneModeFlag(2);
		
		});




	AnimationStateTransitionGraphBasePanel * stateTransitionGraphBasePanel =		AnimationStateTransitionGraphBasePanel::Create(map, 0);
	float stateTransitionGraphBasePanelWidth = 1000;
	float stateTransitionGraphBasePanelHeight = 1000;
	stateTransitionGraphBasePanel->SetSize(stateTransitionGraphBasePanelWidth, stateTransitionGraphBasePanelHeight);
	stateTransitionGraphBasePanel->GetTransform().SetIndependentRotationFlag(true);
	stateTransitionGraphBasePanel->GetTransform().SetIndependentScaleFlag(true);
	mainScenePanel->AddChildPanelUiEntity(stateTransitionGraphBasePanel);

	stateTransitionGraphBasePanel->SetSelectAvailableFlag(false);

	stateTransitionGraphBasePanel->SetPosition(-mainScenePanelWidth / 2 + stateScrollListPanelWidth + stateTransitionGraphBasePanelWidth / 2, mainScenePanelHeight / 2 - stateTransitionGraphBasePanelHeight / 2, -1);

	mStateTransitionGraphBasePanel = stateTransitionGraphBasePanel;



//	stateTransitionGraphBasePanel->RegisterEventCallback("RButtonUp", [](Event * pEvent) {})




	//AnimNode detail edit panel 

	AnimationStateTransitionNodeEditDetailMainPanel* nodeEditDetailMainPanel = AnimationStateTransitionNodeEditDetailMainPanel::Create(map, 0);




	Transform & nodeEditDetailMainPanelTransform  =  nodeEditDetailMainPanel->GetTransform();
	nodeEditDetailMainPanelTransform.SetIndependentRotationFlag(true);
	nodeEditDetailMainPanelTransform.SetIndependentScaleFlag(true);
	nodeEditDetailMainPanel->SetSelectAvailableFlag(false);
	//nodeEditDetailMainPanel->SetSize(nodeEditDetailMainPanelWidth, nodeEditDetailMainPanelHeight);


	float  nodeEditDetailMainPanelWidth = nodeEditDetailMainPanel->GetWidth();
	float  nodeEditDetailMainPanelHeight = nodeEditDetailMainPanel->GetHeight();
	nodeEditDetailMainPanel->SetTexture("DarkGray.png");

	mainScenePanel->AddChildPanelUiEntity(nodeEditDetailMainPanel);

	Camera * mainCamera = map->GetMainCamera(0);
	D3D12_VIEWPORT viewport= map->GetViewPort(0);
	DirectX::XMFLOAT3 cameraPosW=  mainCamera->GetTransform().GetPositionWorld();
	
	nodeEditDetailMainPanelTransform.SetPositionWorld({ viewport.Width - nodeEditDetailMainPanelWidth / 2, -nodeEditDetailMainPanelHeight / 2,mainScenePanel->GetTransform().GetPositionWorld().z - 5 });









	//animationPanel popup panel, 우클릭시 popup창이 띄워지고 여러가지 노드들을 생성할수있다.
	CreateAnimNodePopupPanel();













	mainScenePanel->SetDrawFlag(false);
	mainScenePanel->SetActiveFlag(true);
	mainScenePanel->SetSelectAvailableFlag(false);
	mMainPanel = mainScenePanel;

}

void Quad::AnimEditSceneDirector::CreateAnimNodePopupPanel()
{
	auto popupWindowUiSystem = PopupWindowUiSystem::GetInstance();

	Map * map = popupWindowUiSystem->GetMap();
	int mapLayerIndex = 0; 
	float width = 300;
	float listChildPanelHeight = 20;

	ListPanel* mainPanel = ListPanel::Create(map, mapLayerIndex, width,600, listChildPanelHeight);

	mainPanel->RegisterAcceptEvent("Hover");
	mainPanel->RegisterAcceptEvent("MouseMove");
	mainPanel->RegisterAcceptEvent("LButtonDown");
	mainPanel->RegisterAcceptEvent("LButtonUp");
	mainPanel->SetSelectAvailableFlag(true);

	//PanelUiEntity* mainPanel = PanelUiEntity::Create(map, mapLayerIndex);
	//mainPanel->SetSize(300, 600);

	TextBoxPanel* createBlendNodeTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, width, listChildPanelHeight);
	createBlendNodeTextBoxPanel->GetTransform().SetIndependentRotationFlag(true);
	createBlendNodeTextBoxPanel->GetTransform().SetIndependentScaleFlag(true);
	createBlendNodeTextBoxPanel->SetSelectAvailableFlag(false);
	createBlendNodeTextBoxPanel->SetTextColor({ 1.0f,1.0f,1.0f });
	createBlendNodeTextBoxPanel->SetTexture("DarkGray.png");
	createBlendNodeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"블렌드 노드생성"));



	createBlendNodeTextBoxPanel->RegisterEventCallback("Hover", [panel = createBlendNodeTextBoxPanel](Event* pEvent) {

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

		bool hoverState = mouseEvent->GetHoverState();

		StateComponent* stateComponent = panel->GetStateComponent();
		stateComponent->SetHoverState(hoverState);

		if (hoverState)
		{
			panel->SetColorItensity({ 3.0f,3.0f,3.0f });
		}
		else
		{
			panel->SetColorItensity({ 1.0f,1.0f,1.0f });
		}


		});


	createBlendNodeTextBoxPanel->RegisterEventCallback("LButtonUp", [panel = createBlendNodeTextBoxPanel](Event* pEvent) {

		StateComponent * stateComponent =  panel->GetStateComponent();

		if (stateComponent->GetHoverState() && stateComponent->GetLButtonDownState())
		{ 
			auto animationStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();
			AnimationStateTransitionGraph * currAnimationStateTransitionGraph = animationStateTransitionGraphBasePanel->GetCurrentAnimStateTransitionGraph();

			currAnimationStateTransitionGraph->CreateBlendNode();

		}
		stateComponent->SetLButtonDownState(false);

		});



	mainPanel->AddChildPanelEntity(createBlendNodeTextBoxPanel);









	//조건노드생성텍스트박스
	TextBoxPanel* createConditionNodeTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, width, listChildPanelHeight);
	createConditionNodeTextBoxPanel->GetTransform().SetIndependentRotationFlag(true);
	createConditionNodeTextBoxPanel->GetTransform().SetIndependentScaleFlag(true);
	createConditionNodeTextBoxPanel->SetSelectAvailableFlag(false);
	createConditionNodeTextBoxPanel->SetTextColor({ 1.0f,1.0f,1.0f });
	createConditionNodeTextBoxPanel->SetTexture("DarkGray.png");
	createConditionNodeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"조건 노드생성"));


	createConditionNodeTextBoxPanel->RegisterEventCallback("Hover", [panel = createConditionNodeTextBoxPanel](Event* pEvent) {

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

		bool hoverState = mouseEvent->GetHoverState();

		StateComponent* stateComponent = panel->GetStateComponent();
		stateComponent->SetHoverState(hoverState);

		if (hoverState)
		{
			panel->SetColorItensity({ 3.0f,3.0f,3.0f });
		}
		else
		{
			panel->SetColorItensity({ 1.0f,1.0f,1.0f });
		}


		});


	createConditionNodeTextBoxPanel->RegisterEventCallback("LButtonUp", [panel = createConditionNodeTextBoxPanel](Event* pEvent) {

		StateComponent* stateComponent = panel->GetStateComponent();

		if (stateComponent->GetHoverState() && stateComponent->GetLButtonDownState())
		{
			auto animationStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();
			AnimationStateTransitionGraph* currAnimationStateTransitionGraph = animationStateTransitionGraphBasePanel->GetCurrentAnimStateTransitionGraph();

			currAnimationStateTransitionGraph->CreateConditionNode();

		}
		stateComponent->SetLButtonDownState(false);

		});



	mainPanel->AddChildPanelEntity(createConditionNodeTextBoxPanel);

	popupWindowUiSystem->RegisterRootPanel(mPopupCreatingNodePanelNum, mainPanel);

	mainPanel->SetPosition({ 0,0,10 });

	mainPanel->SetDrawFlag(true);
	mainPanel->SetTexture("DarkGray.png");












}
