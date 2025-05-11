#include "AnimationStateTransitionBlendNodeEditPanel.h"
#include"Map/Map.h"

#include"EditorCommonHelperClass.h"

#include"AnimationStateTransitionBlendNode.h"
#include"AnimationStateNode.h"

#include"ExpandablePanel.h"
#include"ListPanel.h"
#include"MouseEvent.h"
#include"ComboBoxPanel.h"

#include"InputSystemType.h"
#include"ButtonUiEntitIy.h"


Quad::AnimationStateTransitionBlendNodeEditPanel::AnimationStateTransitionBlendNodeEditPanel(const std::string& name)
	:mDestBlendNode(nullptr), mTransitionDurationTextBox(nullptr), mInputTypeTextBoxPanel(nullptr), mTransitionConditionEditKeyboardMainPanel(nullptr), mKeyStateComboBox(nullptr), mKeyValueTextBox(nullptr), mInputDevice(0)

{

}


Quad::AnimationStateTransitionBlendNodeEditPanel::~AnimationStateTransitionBlendNodeEditPanel()
{

}

void Quad::AnimationStateTransitionBlendNodeEditPanel::Initialize()
{
	PanelUiEntity::Initialize();
}

Quad::AnimationStateTransitionBlendNodeEditPanel* Quad::AnimationStateTransitionBlendNodeEditPanel::Create(Map* map, int mapLayerIndex)
{
	AnimationStateTransitionBlendNodeEditPanel* ob = static_cast<AnimationStateTransitionBlendNodeEditPanel*>(map->CreateObject("AnimationStateTransitionBlendNodeEditPanel", mapLayerIndex));

	ob->InitCreating();

	return ob;
}

void Quad::AnimationStateTransitionBlendNodeEditPanel::SetDestBlendNode(AnimationStateTransitionBlendNode* blendNode)
{
	mDestBlendNode = blendNode;
	
	AnimationStateNode* sourceStateNode = blendNode->GetSourceAnimationStateNode();
	AnimationStateNode* destStateNode = blendNode->GetDestAnimationStateNode();


	if (sourceStateNode)
	{
		const std::string sourceStateName = sourceStateNode->GetStateName();
		mSourceStateNameTextBoxPanel->SetText(sourceStateName);

	}else
	{ 
		mSourceStateNameTextBoxPanel->SetText("");
	}


	if (destStateNode)
	{
		const std::string destStateName = destStateNode->GetStateName();
		mDestStateNameTextBoxPanel->SetText(destStateName);

	}
	else
	{
		mDestStateNameTextBoxPanel->SetText("");
	}





	float transitionDuration= blendNode->GetTransitionDuration();
	mTransitionDurationTextBox->SetText(std::to_string(transitionDuration));


	int inputDeviceIndex = mDestBlendNode->GetInputDevice();
	int inputStateIndex = mDestBlendNode->GetInputState();
	int inputKeyValue = mDestBlendNode->GetInputKeyValue();
	
	if (inputDeviceIndex == 0)
	{
		mInputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"없음"));

	}else if (inputDeviceIndex == 1)
	{
		//키보드
		mInputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"키보드"));
		mTransitionConditionEditKeyboardMainPanel->SetDrawFlag(true);
		mKeyStateComboBox->ClickListItem(inputStateIndex);
		std::string keyValue(1, inputKeyValue);
		mKeyValueTextBox->SetText(keyValue);

	}
	else if (inputDeviceIndex == 2)
	{
		mInputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"마우스"));


	}









}

void Quad::AnimationStateTransitionBlendNodeEditPanel::NotifyStateNodeConnect(AnimationStateTransitionBlendNode* blendNode)
{
	auto instance = GetInstance();
	//다르면 무시
	if (instance->mDestBlendNode != blendNode)
		return;

	//panel에 보여지는 DestBlendPanel일경우에만 , 판넬을 업데이트

	AnimationStateNode* destStateNode = blendNode->GetDestAnimationStateNode();
	AnimationStateNode* sourceStateNode = blendNode->GetSourceAnimationStateNode();

	if(destStateNode)
		instance->mDestStateNameTextBoxPanel->SetText(destStateNode->GetStateName());

	if(sourceStateNode)
		instance->mSourceStateNameTextBoxPanel->SetText(sourceStateNode->GetStateName());





}



void Quad::AnimationStateTransitionBlendNodeEditPanel::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);


}

void Quad::AnimationStateTransitionBlendNodeEditPanel::OnEvent(Event* event)
{

	const std::string& eventName = event->GetName();
	if (eventName == "MouseMove")
	{
		int a = 2;
	}
	PanelUiEntity::OnEvent(event);
}

void Quad::AnimationStateTransitionBlendNodeEditPanel::NotifyTransitionDurationToBlendNode()
{

	const std::string durationText = mTransitionDurationTextBox->GetText();
	float durationTime = std::stof(durationText);

	mDestBlendNode->SetTransitionDuration(durationTime);

}

void Quad::AnimationStateTransitionBlendNodeEditPanel::SetTransitionCondition()
{

	if (!mDestBlendNode)
		return;

	AnimationStateTransitionBaseNode * sourceStateNode = mDestBlendNode->GetSourceAnimationStateNode();

	if (!sourceStateNode)
		return;

	AnimationStateTransitionBaseNode* destStateNode = mDestBlendNode->GetSourceAnimationStateNode();

	if (!destStateNode)
		return;

	std::string keyValueString;
	if (mInputDevice != 0)
	{

		keyValueString = mKeyValueTextBox->GetText().c_str();
		if (keyValueString == "")
			return;
	}


	unsigned char keyValue = (unsigned char)keyValueString[0];
	int currSelectedListItemIndex = mKeyStateComboBox->GetCurrentSelectedListItemIndex();


	//blend Node에게 전이조건을 생성 or 수정하라고 알린다.

	//기존의 전이조건 제거

	//새로운전이조건생성
	mDestBlendNode->SetTransitionCondition(mInputDevice,keyValue, EInputState(currSelectedListItemIndex));
	mDestBlendNode->SetTransitionDuration(std::stof(mTransitionDurationTextBox->GetText()));
	//새로운전이조건저장

	mKeyStateComboBox;
	mKeyValueTextBox;







}

void Quad::AnimationStateTransitionBlendNodeEditPanel::InitCreating()
{
	PanelUiEntity::InitCreating();
	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	
	float blendNodeEditPanelWidth = 400;
	float blendNodeEditPanelHeight = 1000;
	SetSize(blendNodeEditPanelWidth, blendNodeEditPanelHeight);

	float margin = 10;
	DirectX::XMFLOAT3  pos;


	float tagTextPanelWidth = 100;
	float tagTextPanelHeight = 20;

	//상태
	TextBoxPanel* sourceStateNameTagTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, tagTextPanelWidth, tagTextPanelHeight);
	EditorCommonHelperClass::SetObjectDefaultInit(sourceStateNameTagTextBoxPanel);
	sourceStateNameTagTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"Source 상태:"));
	AddChildPanelUiEntity(sourceStateNameTagTextBoxPanel);

	sourceStateNameTagTextBoxPanel->SetPosition(-blendNodeEditPanelWidth / 2 + margin + tagTextPanelWidth / 2, blendNodeEditPanelHeight / 2 - margin - tagTextPanelHeight / 2, -1);

	sourceStateNameTagTextBoxPanel->SetTexture("Transparent.png");
	sourceStateNameTagTextBoxPanel->SetTextColor({ 0,0,0 });


	float stateTextBoxPanelWidth = 200;
	float stateTextBoxPanelHeight = 20;
	TextBoxPanel* sourceStateNameTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, stateTextBoxPanelWidth, tagTextPanelHeight);
	EditorCommonHelperClass::SetObjectDefaultInit(sourceStateNameTextBoxPanel);
	AddChildPanelUiEntity(sourceStateNameTextBoxPanel);

	pos= sourceStateNameTagTextBoxPanel->GetTransform().GetPositionLocal();
	pos.x += tagTextPanelWidth / 2 + margin + stateTextBoxPanelWidth/2;
	sourceStateNameTextBoxPanel->SetPosition(pos);

	sourceStateNameTextBoxPanel->SetTexture("Transparent.png");
	sourceStateNameTextBoxPanel->SetTextColor({ 0,0,0 });



	TextBoxPanel* destStateNameTagTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, tagTextPanelWidth, tagTextPanelHeight);
	EditorCommonHelperClass::SetObjectDefaultInit(destStateNameTagTextBoxPanel);
	destStateNameTagTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"Dest 상태:"));
	AddChildPanelUiEntity(destStateNameTagTextBoxPanel);
	pos = sourceStateNameTagTextBoxPanel->GetTransform().GetPositionLocal();
	pos.y -= tagTextPanelHeight / 2 + margin + tagTextPanelHeight / 2;
	destStateNameTagTextBoxPanel->SetPosition(pos);

	destStateNameTagTextBoxPanel->SetTexture("Transparent.png");
	destStateNameTagTextBoxPanel->SetTextColor({ 0,0,0 });


	TextBoxPanel* destStateNameTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, stateTextBoxPanelWidth, tagTextPanelHeight);
	mDestStateNameTextBoxPanel = destStateNameTextBoxPanel;
	EditorCommonHelperClass::SetObjectDefaultInit(destStateNameTextBoxPanel);
	AddChildPanelUiEntity(destStateNameTextBoxPanel);

	pos = destStateNameTagTextBoxPanel->GetTransform().GetPositionLocal();
	pos.x += tagTextPanelWidth / 2 + margin + stateTextBoxPanelWidth / 2;
	destStateNameTextBoxPanel->SetPosition(pos);

	destStateNameTextBoxPanel->SetTexture("Transparent.png");
	destStateNameTextBoxPanel->SetTextColor({ 0,0,0 });























	//전이 시간 텍스트박스

	TextBoxPanel* transitionDurationTagTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, tagTextPanelWidth, tagTextPanelHeight);
	EditorCommonHelperClass::SetObjectDefaultInit(transitionDurationTagTextBoxPanel);
	transitionDurationTagTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"전이 시간:"));
	AddChildPanelUiEntity(transitionDurationTagTextBoxPanel);
	pos = destStateNameTagTextBoxPanel->GetTransform().GetPositionLocal();
	pos.y -= (tagTextPanelHeight / 2 + margin + tagTextPanelHeight / 2);

	transitionDurationTagTextBoxPanel->SetPosition(pos);

	transitionDurationTagTextBoxPanel->SetTexture("Transparent.png");
	transitionDurationTagTextBoxPanel->SetTextColor({ 0,0,0 });



	TextBox* transitionDurationTextBox = TextBox::Create(map, mapLayerIndex, 50, Quad::ETextLineMode::eOneLine);
	EditorCommonHelperClass::SetObjectDefaultInit(transitionDurationTextBox);

	transitionDurationTextBox->SetSize(100, 20);
	AddChildPanelUiEntity(transitionDurationTextBox);
	pos = transitionDurationTagTextBoxPanel->GetTransform().GetPositionLocal();
	pos.x += tagTextPanelWidth / 2 + margin + transitionDurationTextBox->GetWidth() / 2;
	transitionDurationTextBox->SetTextColor({ 0,0,0 });

	transitionDurationTextBox->SetTextFilterMode(ETextFilterMode::eAlphabet);
	transitionDurationTextBox->SetEnterEndFlag(true);
	transitionDurationTextBox->SetEnterCallBack([]() {
		auto blendEditNode = AnimationStateTransitionBlendNodeEditPanel::GetInstance();
		blendEditNode->NotifyTransitionDurationToBlendNode();

		});
	transitionDurationTextBox->SetPosition(pos);



	//전이 조건 제목텍스트박스판넬

	TextBoxPanel* transitionConditionTitleTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, blendNodeEditPanelWidth, 50, 1);
	EditorCommonHelperClass::SetObjectDefaultInit(transitionConditionTitleTextBoxPanel);
	transitionConditionTitleTextBoxPanel->SetTextColor({ 0,0,0 });
	transitionConditionTitleTextBoxPanel->SetTexture("Blue.png");
	AddChildPanelUiEntity(transitionConditionTitleTextBoxPanel);
	transitionConditionTitleTextBoxPanel->SetPosition({0,300,-1});
	transitionConditionTitleTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"전이 조건"));



	float inputTypeExpandablePanelWidth = 100;
	float inputTypeExpandablePanelHeight = 200;
	ExpandablePanel* inputTypeExpandablePanel = ExpandablePanel::Create(map, mapLayerIndex, inputTypeExpandablePanelWidth, inputTypeExpandablePanelHeight);
	EditorCommonHelperClass::SetObjectDefaultInit(inputTypeExpandablePanel);

	AddChildPanelUiEntity(inputTypeExpandablePanel);
	pos = transitionConditionTitleTextBoxPanel->GetTransform().GetPositionLocal();
	pos.x += -blendNodeEditPanelWidth / 2 + margin + inputTypeExpandablePanelWidth /2;
	pos.y -= (50+margin + inputTypeExpandablePanelHeight /2);
	inputTypeExpandablePanel->SetPosition(pos);
	
	inputTypeExpandablePanel->SetTexture("Green.png");
	PanelUiEntity* childTest = PanelUiEntity::Create(map, mapLayerIndex);
	EditorCommonHelperClass::SetObjectDefaultInit(childTest);


	ListPanel* inputTypeListPanel = ListPanel::Create(map, mapLayerIndex, 100, 120, 20);
	EditorCommonHelperClass::SetObjectDefaultInit(inputTypeListPanel);
	


	auto inputTypeTextBoxHoverLamda = [](Event * pEvent, TextBoxPanel * textBoxPanel) {
		
		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
		StateComponent* stateComponent = textBoxPanel->GetStateComponent();
		bool hoverState =mouseEvent->GetHoverState();
		
		if (hoverState)
		{
			textBoxPanel->SetColorItensity({ 3.0,3.0f,3.0f });
		}
		else
		{
			textBoxPanel->SetColorItensity({ 1.0f,1.0f,1.0f });
		}


		};



	TextBoxPanel* noneInputTypeTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 100, 20);
	EditorCommonHelperClass::SetObjectDefaultInit(noneInputTypeTextBoxPanel);
	noneInputTypeTextBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	noneInputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"없음"));
	noneInputTypeTextBoxPanel->SetTextColor({ 0,0,0 });
	noneInputTypeTextBoxPanel->SetSize(100, 20);
	noneInputTypeTextBoxPanel->RegisterEventCallback("Hover", std::bind(inputTypeTextBoxHoverLamda, std::placeholders::_1, noneInputTypeTextBoxPanel));
	
	







	TextBoxPanel* keyboardInputTypeTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 100, 20);
	EditorCommonHelperClass::SetObjectDefaultInit(keyboardInputTypeTextBoxPanel);
	keyboardInputTypeTextBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	keyboardInputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"키보드"));
	keyboardInputTypeTextBoxPanel->SetTextColor({ 0,0,0 });
	keyboardInputTypeTextBoxPanel->SetSize(100, 20);
	keyboardInputTypeTextBoxPanel->RegisterEventCallback("Hover", std::bind(inputTypeTextBoxHoverLamda, std::placeholders::_1, keyboardInputTypeTextBoxPanel));
//	keyboardInputTypeTextBoxPanel->SetTexture("Green.png");



	TextBoxPanel* mouseInputTypeTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 100, 20);
	EditorCommonHelperClass::SetObjectDefaultInit(mouseInputTypeTextBoxPanel);
	mouseInputTypeTextBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	mouseInputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"마우스"));
	mouseInputTypeTextBoxPanel->SetTextColor({ 0,0,0 });
	mouseInputTypeTextBoxPanel->SetSize(100, 20);
	mouseInputTypeTextBoxPanel->RegisterEventCallback("Hover", std::bind(inputTypeTextBoxHoverLamda, std::placeholders::_1, mouseInputTypeTextBoxPanel));

	//childTest->SetSize(100, 30);

	inputTypeListPanel->AddChildPanelEntity(noneInputTypeTextBoxPanel);
	inputTypeListPanel->AddChildPanelEntity(keyboardInputTypeTextBoxPanel);
	inputTypeListPanel->AddChildPanelEntity(mouseInputTypeTextBoxPanel);

	inputTypeExpandablePanel->AddChildPanelUiEntity(inputTypeListPanel,reinterpret_cast<const char*>(u8"인풋 타입"));


	float inputTypeTextBoxPanelWidth = 100;
	float inputTypeTextBoxPanelHeight = 20;
	TextBoxPanel* inputTypeTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, inputTypeTextBoxPanelWidth, inputTypeTextBoxPanelHeight);

	mInputTypeTextBoxPanel = inputTypeTextBoxPanel;
	EditorCommonHelperClass::SetObjectDefaultInit(inputTypeTextBoxPanel);
	AddChildPanelUiEntity(inputTypeTextBoxPanel);

	pos = inputTypeExpandablePanel->GetTransform().GetPositionLocal();
	pos.x += inputTypeExpandablePanelWidth / 2 + margin + inputTypeTextBoxPanelWidth / 2;
	pos.y += (inputTypeExpandablePanelHeight / 2 - inputTypeTextBoxPanelHeight/2 - 5);
	inputTypeTextBoxPanel->SetPosition(pos);
	inputTypeTextBoxPanel->SetTexture("Transparent.png");
	inputTypeTextBoxPanel->SetTextColor({ 0,0,0 });



	PanelUiEntity* transitionConditionEditKeyboardMainPanel = CreateTransitionConditionEditKeyboardPanel();
	mTransitionConditionEditKeyboardMainPanel = transitionConditionEditKeyboardMainPanel;
	pos = inputTypeExpandablePanel->GetTransform().GetPositionLocal();
	pos.x = 0;
	pos.y -= inputTypeExpandablePanelHeight / 2 + margin + transitionConditionEditKeyboardMainPanel->GetHeight
		()/2;
	transitionConditionEditKeyboardMainPanel->SetPosition(pos);

	noneInputTypeTextBoxPanel->RegisterEventCallback("LButtonDown", [inputTypeTextBoxPanel, noneInputTypeTextBoxPanel, transitionConditionEditKeyboardMainPanel, this ](Event* pEvent) {

		
		StateComponent * stateComponent =noneInputTypeTextBoxPanel->GetStateComponent();
		if (stateComponent->GetHoverState())
		{
			inputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"없음"));
			transitionConditionEditKeyboardMainPanel->SetDrawFlag(false);
			mInputDevice = 0;
		}

		});


	mouseInputTypeTextBoxPanel->RegisterEventCallback("LButtonDown", [inputTypeTextBoxPanel, mouseInputTypeTextBoxPanel, transitionConditionEditKeyboardMainPanel, this](Event* pEvent) {


		StateComponent* stateComponent = mouseInputTypeTextBoxPanel->GetStateComponent();
		if (stateComponent->GetHoverState())
		{
			inputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"마우스"));
			transitionConditionEditKeyboardMainPanel->SetDrawFlag(false);
			mInputDevice = 2;
		}

		});



	keyboardInputTypeTextBoxPanel->RegisterEventCallback("LButtonDown", [inputTypeTextBoxPanel, keyboardInputTypeTextBoxPanel, transitionConditionEditKeyboardMainPanel,this](Event* pEvent) {


		StateComponent* stateComponent = keyboardInputTypeTextBoxPanel->GetStateComponent();
		if (stateComponent->GetHoverState())
		{
			inputTypeTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"키보드"));
			transitionConditionEditKeyboardMainPanel->SetDrawFlag(true);
			mInputDevice = 1;
		}

		});


	PanelUiEntity* applyButton = PanelUiEntity::Create(map, mapLayerIndex);
	EditorCommonHelperClass::SetObjectDefaultInit(applyButton);
	applyButton->SetSize(80, 30);
	AddChildPanelUiEntity(applyButton);

	pos = transitionConditionTitleTextBoxPanel->GetTransform().GetPositionLocal();
	pos.x += blendNodeEditPanelWidth / 2 - margin - 40 ; 
	pos.y -= 25 + margin + 15;
	applyButton->SetPosition(pos);

	applyButton->RegisterEventCallback("LButtonDown", [applyButton, blendEditPanel=this](Event* pEvent) {

		StateComponent* stateComponent = applyButton->GetStateComponent();
		if (stateComponent->GetHoverState())
		{
			blendEditPanel->SetTransitionCondition();
			OutputDebugStringA("clickDown\n");
		}
		});





	mSourceStateNameTextBoxPanel = sourceStateNameTextBoxPanel;
	mTransitionDurationTextBox = transitionDurationTextBox;
}

Quad::PanelUiEntity * Quad::AnimationStateTransitionBlendNodeEditPanel::CreateTransitionConditionEditKeyboardPanel()
{

	Map * map =	GetMap();
	int mapLayerIndex = GetMapLayerID();
	float margin = 10;


	PanelUiEntity* keyboardMainPanel = PanelUiEntity::Create(map, mapLayerIndex);
	EditorCommonHelperClass::SetObjectDefaultInit(keyboardMainPanel);
	float keyboardMainPanelWidth = 300;
	float keyboardMainPanelHeight = 300;
	keyboardMainPanel->SetSize(keyboardMainPanelWidth, keyboardMainPanelHeight);
	keyboardMainPanel->SetTexture("DarkGray.png");

	AddChildPanelUiEntity(keyboardMainPanel);
	


	DirectX::XMFLOAT3 pos;

	float keyStateTextBoxPanelWidth = 100;
	float keyStateTextBoxPanelHeight = 20;
	TextBoxPanel* keyStateTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, keyStateTextBoxPanelWidth, keyStateTextBoxPanelHeight);
	EditorCommonHelperClass::SetObjectDefaultInit(keyStateTextBoxPanel);
	keyStateTextBoxPanel->SetTexture("Transparent.png");
	keyStateTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"키 상태:"));

	keyboardMainPanel->AddChildPanelUiEntity(keyStateTextBoxPanel);

	pos.x = -keyboardMainPanelWidth / 2 + margin  + keyStateTextBoxPanelWidth/2;
	pos.y = keyboardMainPanelHeight / 2 - margin - keyStateTextBoxPanelHeight / 2;
	pos.z = -1;
	keyStateTextBoxPanel->SetPosition(pos);


	float keyStateComboBoxWidth = 100;
	ComboBoxPanel* keyStateComboBox = ComboBoxPanel::Create(map, mapLayerIndex, keyStateComboBoxWidth, 3);
	EditorCommonHelperClass::SetObjectDefaultInit(keyStateComboBox);

	keyboardMainPanel->AddChildPanelUiEntity(keyStateComboBox);

	pos = keyStateTextBoxPanel->GetTransform().GetPositionLocal();
	pos.x += keyStateTextBoxPanelWidth + margin + keyStateComboBoxWidth / 2;
	keyStateComboBox->SetPosition(pos);
	keyStateComboBox->SetText(reinterpret_cast<const char*>(u8"키 눌림"),0);
	keyStateComboBox->SetText(reinterpret_cast<const char*>(u8"키 해제"),1);
	keyStateComboBox->SetText(reinterpret_cast<const char*>(u8"키 눌림유지"),2);







	//key value
	TextBoxPanel* keyValueTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, keyStateTextBoxPanelWidth, keyStateTextBoxPanelHeight);
	EditorCommonHelperClass::SetObjectDefaultInit(keyValueTextBoxPanel);
	keyValueTextBoxPanel->SetTexture("Transparent.png");
	keyValueTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"키 값:"));

	keyboardMainPanel->AddChildPanelUiEntity(keyValueTextBoxPanel);

	pos = keyStateTextBoxPanel->GetTransform().GetPositionLocal();
	pos.y -= keyStateTextBoxPanelHeight/2 + margin + keyStateTextBoxPanelHeight / 2;
	keyValueTextBoxPanel->SetPosition(pos);


	
	TextBox* keyValueTextBox = TextBox::Create(map, mapLayerIndex,1 , ETextLineMode::eOneLine);
	EditorCommonHelperClass::SetObjectDefaultInit(keyValueTextBox);
	keyValueTextBox->SetSize(50, 20);

	keyboardMainPanel->AddChildPanelUiEntity(keyValueTextBox);
	keyValueTextBox->SetTextColor({ 0,0,0 });
	//keyValueTextBox->SetTextFilterMode(ETextFil);
	//keyValueTextBox->Set
	keyValueTextBox->SetEnterEndFlag(true);
	keyValueTextBox->SetOnlyEnglishFlag(true);
	keyValueTextBox->SetOnlyUpperCaseFlag(true);

	pos = keyValueTextBoxPanel->GetTransform().GetPositionLocal();
	pos.x += keyStateTextBoxPanelWidth / 2 + margin + 50;
	keyValueTextBox->SetPosition(pos);


	



















	mKeyStateComboBox = keyStateComboBox;
	mKeyValueTextBox = keyValueTextBox;

	return keyboardMainPanel;
}

Quad::PanelUiEntity* Quad::AnimationStateTransitionBlendNodeEditPanel::CreateTransitionConditionEditMousePanel()
{


	return nullptr;
}
