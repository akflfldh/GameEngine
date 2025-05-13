#include "EditorDirector/FrameWindowMenuDirector.h"
#include"System/WindowChromeSystem.h"
#include"EditorDirector/TextBoxPanel.h"
#include"EditorDirector/TitleBarEntity.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"Core/MouseEvent.h"

#include"EditorDirector/PopupWindowController.h"

#include"System/PopupWindowUiSystem.h"
#include"EditorDirector/FrameWindowController.h"

#include"EditorDirector/FrameWindow.h"
#include"EditorDirector/ListPanel.h"


#include"System/GamePlaySystem.h"
#include"EditorDirector/ProjectDirector.h"

#include"EditorDirector/Project.h"
#include"ButtonUiEntitiy.h"
#include"ObjectManager/ObjectManager.h"

#include"UserBuildFileGenerator.h"

#include"BuildModule.h"


namespace Quad
{
	void ActivePopupMenuWindow(int popupID, Quad::PanelUiEntity* menuPanel)
	{

		Quad::Transform& panelTransform = menuPanel->GetTransform();
		DirectX::XMFLOAT3 posWorld = panelTransform.GetPositionWorld();

		float clientX = posWorld.x - menuPanel->GetWidth() / 2;
		float clientY = 3000 - posWorld.y + menuPanel->GetHeight() / 2;

		Quad::FrameWindowController* frameWindowController = Quad::FrameWindowController::GetInstance();
		HWND frameWindowHandle = frameWindowController->GetWindow()->GetWindowHandle();

		POINT clientPos = { clientX,clientY };
		ClientToScreen(frameWindowHandle, &clientPos);

		Quad::PopupWindowController::ActivateWindow(popupID, clientPos.x, clientPos.y);


	}
}











void Quad::FrameWindowMenuDirector::Initialize(WindowChromeSystem* frameWindowChromeSystem)
{
	mFrameWindowChromeSystem = frameWindowChromeSystem;
}

void Quad::FrameWindowMenuDirector::SetMainScene()
{

	auto instance = GetInstance();

	//main Scene에서의 frame window menu를 구성한다.

	TitleBarEntity * titleBarEntity = instance->mFrameWindowChromeSystem->GetTitleBarEntity();
	float titleBarEntityWidth = titleBarEntity->GetWidth();
	float titleBarEntityHeight = titleBarEntity->GetHeight();


	float menuMargin = 10;

	Texture* titleBarTexture = titleBarEntity->GetModel()->GetMeshComponent()->GetSubMesh()[0].mModelMaterial.GetDiffuseMap();


	Map * currMap =	titleBarEntity->GetMap();
	int mapLayerIndex = titleBarEntity->GetMapLayerID();

	DirectX::XMFLOAT3 menuPos = { -titleBarEntityWidth / 2 + menuMargin,0,-1 };

	TextBoxPanel* fileMenuTextBoxPanel = TextBoxPanel::Create(currMap, mapLayerIndex, 40, 30);
	fileMenuTextBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	fileMenuTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"파일"));
	Transform& fileMenuTextBoxPanelTransform = fileMenuTextBoxPanel->GetTransform();
	fileMenuTextBoxPanelTransform.SetIndependentScaleFlag(true);
	fileMenuTextBoxPanelTransform.SetIndependentRotationFlag(true);
	
	titleBarEntity->AddChildObject(fileMenuTextBoxPanel);

	menuPos.x += fileMenuTextBoxPanel->GetWidth() / 2;
	fileMenuTextBoxPanel->SetPosition(menuPos);
	fileMenuTextBoxPanel->SetTexture(titleBarTexture);
	fileMenuTextBoxPanel->SetTextColor({ 1.0f,1.0f,1.0f });
	menuPos.x+= fileMenuTextBoxPanel->GetWidth() / 2;


	instance->CreateFileMenu(fileMenuTextBoxPanel, titleBarTexture);



	//tools 
	TextBoxPanel* toolsMenuTextBoxPanel = TextBoxPanel::Create(currMap, mapLayerIndex, 40, 30);
	toolsMenuTextBoxPanel->SetTextAlignmentDirection(ETextAlignmentDirection::eCenter);
	toolsMenuTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"도구"));
	Transform& toolsMenuTextBoxPanelTransform = toolsMenuTextBoxPanel->GetTransform();
	toolsMenuTextBoxPanelTransform.SetIndependentScaleFlag(true);
	toolsMenuTextBoxPanelTransform.SetIndependentRotationFlag(true);

	titleBarEntity->AddChildObject(toolsMenuTextBoxPanel);
	menuPos.x += toolsMenuTextBoxPanel->GetWidth() / 2;
	toolsMenuTextBoxPanel->SetPosition(menuPos);
	toolsMenuTextBoxPanel->SetTexture(titleBarTexture);
	toolsMenuTextBoxPanel->SetTextColor({ 1.0f,1.0f,1.0f });


	instance->CreateToolsMenu(toolsMenuTextBoxPanel, titleBarTexture);





}

void Quad::FrameWindowMenuDirector::CreateFileSubMenuChildPanelUiEntities(ListPanel* parentListPanel)
{

	auto instance = GetInstance();

	Map * map =	parentListPanel->GetMap();
	int mapLayerIndex = parentListPanel->GetMapLayerID();

	Texture* darkGrayTexture = TextureManager::GetTexture("DarkGray.png");


	//common callback 
	std::function<void(Event* pEvent, PanelUiEntity* panel)> childPanelHoverCallback = [](Event* pEvent, PanelUiEntity* panel) {

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
		bool hoverState = mouseEvent->GetHoverState();

		if (hoverState)
		{
			panel->SetTexture("MenuActive.png");

		}
		else
		{
			panel->SetTexture("DarkGray.png");
		}

		};




	//ProjectSaveTextBoxPanel
	TextBoxPanel* projectSaveTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 300, 20);
	projectSaveTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"프로젝트 저장"));

	parentListPanel->AddChildPanelEntity(projectSaveTextBoxPanel);
	projectSaveTextBoxPanel->SetTexture(darkGrayTexture);
	

	projectSaveTextBoxPanel->RegisterEventCallback("Hover", std::bind(childPanelHoverCallback, std::placeholders::_1, projectSaveTextBoxPanel));

	projectSaveTextBoxPanel->SetSelectAvailableFlag(false);



	//MapSaveTexctBoxPanel
	TextBoxPanel* mapSaveTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 300, 20);
	mapSaveTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"맵 저장"));
	parentListPanel->AddChildPanelEntity(mapSaveTextBoxPanel);
	mapSaveTextBoxPanel->SetTexture(darkGrayTexture);

	mapSaveTextBoxPanel->RegisterEventCallback("Hover", std::bind(childPanelHoverCallback, std::placeholders::_1, mapSaveTextBoxPanel));
	mapSaveTextBoxPanel->SetSelectAvailableFlag(false);



	mapSaveTextBoxPanel->RegisterEventCallback("LButtonUp", [panel = mapSaveTextBoxPanel](Event* pEvent) {

		StateComponent* stateComponent = panel->GetStateComponent();

		bool hoverState = stateComponent->GetHoverState();
		bool lbuttonDownState = stateComponent->GetLButtonDownState();


		if (hoverState && lbuttonDownState)
		{
			Project *currProject= ProjectDirector::GetCurrentProject();
			currProject->SaveCurrentMap();
		}

		});




	//Load another map TextBoxPanel

	TextBoxPanel* mapLoadTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 300, 20);
	mapLoadTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"맵 불러오기"));
	parentListPanel->AddChildPanelEntity(mapLoadTextBoxPanel);
	mapLoadTextBoxPanel->SetTexture(darkGrayTexture);

	mapLoadTextBoxPanel->RegisterEventCallback("Hover", std::bind(childPanelHoverCallback, std::placeholders::_1, mapLoadTextBoxPanel));
	mapLoadTextBoxPanel->SetSelectAvailableFlag(false);



	//build TextBoxPanel

	TextBoxPanel* buildTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 300, 20);
	buildTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"빌드"));
	parentListPanel->AddChildPanelEntity(buildTextBoxPanel);
	buildTextBoxPanel->SetTexture(darkGrayTexture);

	buildTextBoxPanel->RegisterEventCallback("Hover", std::bind(childPanelHoverCallback, std::placeholders::_1, buildTextBoxPanel));
	buildTextBoxPanel->SetSelectAvailableFlag(false);

	
	int buildPopupPanelID = 230;
	instance->CreateBuildPopupPanel(buildPopupPanelID);
	buildTextBoxPanel->RegisterEventCallback("LButtonUp", [panel = buildTextBoxPanel, buildPopupPanelID](Event * pEvent) {


		StateComponent* stateComponent = panel->GetStateComponent();

		bool hoverState = stateComponent->GetHoverState();
		bool lbuttonDownState = stateComponent->GetLButtonDownState();


		if (hoverState && lbuttonDownState)
		{
			//엔티티클래스 생성 popup창 띄우기
			auto popupWindowController = PopupWindowController::GetInstance();
			popupWindowController->SetKeepVisiblity(true);
			popupWindowController->HandleInActiveMsg();//InActive된것처럼효과를 낸다(단 창이 사라지지않는다)(다른부분들만 영향을받을것

		/*	Quad::FrameWindowController* frameWindowController = Quad::FrameWindowController::GetInstance();
			int clientWidth = frameWindowController->GetLogicalClientWidth();
			int clientHeight = frameWindowController->GetLogicalClientHeight();
			HWND frameWindowHandle = frameWindowController->GetWindow()->GetWindowHandle();

			POINT pos = { clientWidth / 2,clientHeight / 2 };
			ClientToScreen(frameWindowHandle, &pos);

			popupWindowController->ActivateWindow(246, pos.x, pos.y);*/

			PopupWindowController::ActivateWindow(buildPopupPanelID, 1000, 500);
		}


		});









}

void Quad::FrameWindowMenuDirector::CreateToolSubMenuChildPanelUiEntities(ListPanel* parentListPanel)
{

	Map* map = parentListPanel->GetMap();
	int mapLayerIndex = parentListPanel->GetMapLayerID();
	Texture* darkGrayTexture = TextureManager::GetTexture("DarkGray.png");

	std::function<void(Event *, TextBoxPanel*)> hoverCallback = [](Event* pEvent, TextBoxPanel* panel) {
		
		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
			
		bool hoverState = mouseEvent->GetHoverState();
		if (hoverState)
		{
			panel->SetTexture("MenuActive.png");
			panel->SetTextColor({ 0.0f,0.0f,0.0f });
		}
		else
		{
			panel->SetTexture("DarkGray.png");
			panel->SetTextColor({ 1.0f,1.0f,1.0f });
		}
	
	};




	TextBoxPanel* createEntityClassTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, 300, 20);
	createEntityClassTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"엔티티 클래스 생성"));
	createEntityClassTextBoxPanel->SetTexture(darkGrayTexture);

	createEntityClassTextBoxPanel->RegisterEventCallback("Hover", std::bind(hoverCallback, std::placeholders::_1, createEntityClassTextBoxPanel));
	createEntityClassTextBoxPanel->SetSelectAvailableFlag(false);



	CreateCreateEntityClassPopupPanel(246);
	createEntityClassTextBoxPanel->RegisterEventCallback("LButtonUp", [panel= createEntityClassTextBoxPanel](Event* pEvent) {

		StateComponent * stateComponent =	panel->GetStateComponent();
		bool hoverState = stateComponent->GetHoverState();
		
		if (hoverState)
		{
			
			//엔티티클래스 생성 popup창 띄우기
			auto popupWindowController =	PopupWindowController::GetInstance();
			popupWindowController->SetKeepVisiblity(true);
			popupWindowController->HandleInActiveMsg();//InActive된것처럼효과를 낸다(단 창이 사라지지않는다)(다른부분들만 영향을받을것

			Quad::FrameWindowController* frameWindowController = Quad::FrameWindowController::GetInstance();
			int clientWidth =frameWindowController->GetLogicalClientWidth();
			int clientHeight= frameWindowController->GetLogicalClientHeight();
			HWND frameWindowHandle = frameWindowController->GetWindow()->GetWindowHandle();

			POINT pos = { clientWidth/2,clientHeight/2 };
			ClientToScreen(frameWindowHandle, &pos);

			popupWindowController->ActivateWindow(246, pos.x, pos.y);

		}

		});


	parentListPanel->AddChildPanelEntity(createEntityClassTextBoxPanel);
















}

void Quad::FrameWindowMenuDirector::CreateCreateEntityClassPopupPanel(int popupPanelID)
{
	PopupWindowUiSystem* popupWindowUiSystem = PopupWindowUiSystem::GetInstance();
	Map* map = popupWindowUiSystem->GetMap();
	int mapLayerIndex = 0;

	float margin = 10;
	float mainPanelWidth = 400;
	float mainPanelHeight = 100;

	PanelUiEntity* createEntityClassMainPanel = PanelUiEntity::Create(map, mapLayerIndex);
	createEntityClassMainPanel->SetPosition(0, 0, 10);
	createEntityClassMainPanel->SetSize(mainPanelWidth, mainPanelHeight);
	createEntityClassMainPanel->SetDrawFlag(false);
	createEntityClassMainPanel->SetActiveFlag(false);
	createEntityClassMainPanel->SetTexture("Blue.png");

	createEntityClassMainPanel->RegisterAcceptEvent("LButtonDown");
	createEntityClassMainPanel->RegisterAcceptEvent("LButtonUp");
	//createEntityClassMainPanel->RegisterAcceptEvent("KeyDown");
	createEntityClassMainPanel->RegisterAcceptEvent("MouseMove");
	createEntityClassMainPanel->RegisterAcceptEvent("Hover");


	createEntityClassMainPanel->SetSelectAvailableFlag(true);


	popupWindowUiSystem->RegisterRootPanel(popupPanelID, createEntityClassMainPanel);

	float tagPanelWidth = 100;
	float tagPanelHeight = 20;
	TextBoxPanel* tagTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, tagPanelWidth, tagPanelHeight);
	tagTextBoxPanel->GetTransform().SetIndependentScaleFlag(true);
	tagTextBoxPanel->GetTransform().SetIndependentRotationFlag(true);
	tagTextBoxPanel->SetTexture("Transparent.png");
	tagTextBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	tagTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"클래스 이름:"));
	tagTextBoxPanel->SetSelectAvailableFlag(false);


	tagTextBoxPanel->RegisterEventCallback("Hover", [panel =tagTextBoxPanel](Event* pEvent){

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
			bool hover =	mouseEvent->GetHoverState();
			if (hover)
			{
				panel->SetTexture("MenuActive.png");
			}
			else
			{
				panel->SetTexture("Transparent.png");
			}

		
		});


	createEntityClassMainPanel->AddChildPanelUiEntity(tagTextBoxPanel);
	
	
	tagTextBoxPanel->SetPosition(-mainPanelWidth / 2 + margin + tagPanelWidth / 2, 0, -1);

	//TextBox
	float classNameTextBoxWidth = 200;
	float classNameTextBoxHeight = 30;

	TextBox* classNameTextBox = TextBox::Create(map, mapLayerIndex, 255, Quad::ETextLineMode::eOneLine);
	classNameTextBox->GetTransform().SetIndependentScaleFlag(true);
	classNameTextBox->GetTransform().SetIndependentRotationFlag(true);
	classNameTextBox->SetSize(classNameTextBoxWidth, classNameTextBoxHeight);
	createEntityClassMainPanel->AddChildPanelUiEntity(classNameTextBox);

	DirectX::XMFLOAT3 classNameTextBoxPos =	tagTextBoxPanel->GetTransform().GetPositionLocal();
	classNameTextBoxPos.x += tagPanelWidth / 2 + margin + classNameTextBoxWidth/2;
	classNameTextBox->SetPosition(classNameTextBoxPos);
	classNameTextBox->SetTextDrawFlag(true);
	classNameTextBox->SetSelectAvailableFlag(false);
	classNameTextBox->SetTextColor({ 0.0f,0.0f,0.0f });


	//실패결과출력TextBoxPanel
	float failTextBoxPanelWidth = 150;
	float failTextBoxPanelHeight = 20;
	TextBoxPanel* failTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, failTextBoxPanelWidth, failTextBoxPanelHeight);
	failTextBoxPanel->GetTransform().SetIndependentRotationFlag(true);
	failTextBoxPanel->GetTransform().SetIndependentScaleFlag(true);
	failTextBoxPanel->SetSelectAvailableFlag(false);
	failTextBoxPanel->SetTexture("Transparent.png");

	createEntityClassMainPanel->AddChildPanelUiEntity(failTextBoxPanel);
	DirectX::XMFLOAT3 failTextBoxPanelPos = classNameTextBoxPos;
	failTextBoxPanelPos.y += -classNameTextBoxHeight / 2 - margin - failTextBoxPanelHeight / 2;
	failTextBoxPanel->SetPosition(failTextBoxPanelPos);
	failTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"결과"));
	failTextBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });

	

	//창 닫기 버튼
	float closeButtonWidth = 25;
	float closeButtonHeight = 25;
	ButtonUiEntity* closeButton = ButtonUiEntity::Create(map, mapLayerIndex);
	closeButton->SetSize(closeButtonWidth, closeButtonHeight);
	closeButton->SetTexture("CloseButton.png");
	closeButton->GetTransform().SetIndependentRotationFlag(true);
	closeButton->GetTransform().SetIndependentScaleFlag(true);

	createEntityClassMainPanel->AddChildPanelUiEntity(closeButton);
	DirectX::XMFLOAT3 closeButtonPos = { mainPanelWidth /2 - closeButtonWidth/2 ,mainPanelHeight/2 - closeButtonHeight/2,-1};
	closeButton->SetPosition(closeButtonPos);
	closeButton->SetSelectAvailableFlag(false);
	closeButton->SetHoverCallback([button = closeButton]() {

		button->SetColorItensity({ 2.0f,2.0f,2.0f });
	
		});

	closeButton->SetReleaseHoverCallback([button = closeButton]() {
		button->SetColorItensity({ 1.0f,1.0f,1.0f });
		});

	closeButton->SetLButtonDownCallback([button = closeButton]() {
		button->SetColorItensity({ 0.5f,0.5f,0.5f });
		});

	closeButton->SetLButtonUpCallback([button = closeButton,classNameTextBox = classNameTextBox]() {

		classNameTextBox->SetText("");
		auto popupWindowController = PopupWindowController::GetInstance();
		popupWindowController->SetKeepVisiblity(false);
		PopupWindow::SetVisibilityState(false);

		});




	//생성버튼
	float buttonWidth = 50;
	float buttonHeight = 30;
	ButtonUiEntity* createButton = ButtonUiEntity::Create(map, mapLayerIndex);
	createButton->SetSize(buttonWidth, buttonHeight);
	createButton->SetTexture("Button.png");

	
	createButton->GetTransform().SetIndependentRotationFlag(true);
	createButton->GetTransform().SetIndependentScaleFlag(true);

	createEntityClassMainPanel->AddChildPanelUiEntity(createButton);

	DirectX::XMFLOAT3 buttonPos = classNameTextBoxPos;
	buttonPos.x += classNameTextBoxWidth / 2 + margin + buttonWidth / 2;

	createButton->SetPosition(buttonPos);

	createButton->SetHoverCallback([button = createButton]() {


		button->SetColorItensity({ 2.0F,2.0F,2.0F });

		});

	createButton->SetReleaseHoverCallback([button = createButton]() {

		button->SetColorItensity({ 1.0f,1.0f,1.0f });

		});



	createButton->SetLButtonDownCallback([button = createButton]() {

		button->SetColorItensity({ 0.5f,0.5f,0.5f });

		});



	createButton->SetLButtonUpCallback([button = createButton , classNameTextBox = classNameTextBox,failTextBoxPanel = failTextBoxPanel]() {

		std::string  className = classNameTextBox->GetText();
		if (className == "")
			return;

		
		auto editObjectManager= EditObjectManager::GetInstance();

		auto & objectConstructTable = editObjectManager->GetObjectConstructTable();

		if (objectConstructTable.find(className) != objectConstructTable.end())
		{
			//동일 이름 클래스가 이미 존재한다.
			failTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"동일이름 클래스 존재"));
			return;
		}

		//일단은 목록에 추가하는것은 나중에 생각해보고 
		//먼저 cpp,h파일을 자동으로 생성만 구현
		std::string projectName =ProjectDirector::GetCurrentProject()->GetName();
		ProjectMetaData* projectMetaData = ProjectDirector::GetProjectMetaData(projectName);
		
		//상대경로로 수정해야함
		UserBuildFileGenerator::CreateClassFile(className, "C:/Users/dongd/gitproject/GameEngine/EditorTemplate", projectMetaData->GetProjectFolderPath());
		OutputDebugStringW(L"클래스파일생성 상대경로로 수정할것 !\n");


		//popup창 종료
		classNameTextBox->SetText("");
		auto popupWindowController = PopupWindowController::GetInstance();
		popupWindowController->SetKeepVisiblity(false);
		PopupWindow::SetVisibilityState(false);


		});




}

void Quad::FrameWindowMenuDirector::CreateBuildPopupPanel(int popupPanelID)
{

	auto popupWindowUiSystem  =	PopupWindowUiSystem::GetInstance();

	Map* map = popupWindowUiSystem->GetMap();
	int mapLayerIndex = 0;
	int margin = 10;

	


	PanelUiEntity* buildMainPanel = PanelUiEntity::Create(map, mapLayerIndex);
	float buildMainPanelWidth = 500;
	float buildMainPanelHeight = 100;


	buildMainPanel->SetSize(buildMainPanelWidth, buildMainPanelHeight);
	buildMainPanel->SetDrawFlag(false);
	buildMainPanel->SetActiveFlag(false);
	buildMainPanel->SetTexture("DarkGray.png");
	buildMainPanel->SetPosition({ 0,0,10 });
	buildMainPanel->RegisterAcceptEvent("LButtonDown");
	buildMainPanel->RegisterAcceptEvent("LButtonUp");
	buildMainPanel->RegisterAcceptEvent("MouseMove");
	












	popupWindowUiSystem->RegisterRootPanel(popupPanelID, buildMainPanel);



	//outputPathTextBoxPanel
	float outputPathTextBoxPanelWidth = 100;
	float outputPathTextBoxPanelHeight= 20;
	TextBoxPanel* outputPathTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, outputPathTextBoxPanelWidth, outputPathTextBoxPanelHeight);



	Transform & outputTextBoxPanelTransform = outputPathTextBoxPanel->GetTransform();
	outputTextBoxPanelTransform.SetIndependentRotationFlag(true);
	outputTextBoxPanelTransform.SetIndependentScaleFlag(true);
	outputPathTextBoxPanel->SetSelectAvailableFlag(false);
	outputPathTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"출력경로:"));
	outputPathTextBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	outputPathTextBoxPanel->SetDrawFlag(false);

	buildMainPanel->AddChildPanelUiEntity(outputPathTextBoxPanel);

	outputPathTextBoxPanel->SetPosition({ -buildMainPanelWidth / 2 + margin + outputPathTextBoxPanelWidth / 2  , buildMainPanelHeight / 2 - margin - outputPathTextBoxPanelHeight / 2 ,-1 });
	

	//outputPathTextBox
	TextBox* outputPathTextBox = TextBox::Create(map, mapLayerIndex, MAX_PATH, Quad::ETextLineMode::eOneLine);
	Transform &	outputPathTextBoxTransform  = outputPathTextBox->GetTransform();
	
	outputPathTextBoxTransform.SetIndependentRotationFlag(true);
	outputPathTextBoxTransform.SetIndependentScaleFlag(true);
	outputPathTextBox->SetDrawFlag(false);

	float outputPathTextBoxWidth = 300;
	float outputPathTextBoxHeight = 20;
	outputPathTextBox->SetSize(outputPathTextBoxWidth, outputPathTextBoxHeight);
	outputPathTextBox->SetSelectAvailableFlag(false);
	outputPathTextBox->SetTextColor({ 0.0f,0.0f,0.0f });

	buildMainPanel->AddChildPanelUiEntity(outputPathTextBox);

	DirectX::XMFLOAT3 outputPathTextBoxPos = { outputTextBoxPanelTransform.GetPositionLocal().x + outputPathTextBoxPanelWidth /2+ margin + outputPathTextBoxWidth / 2,
		outputTextBoxPanelTransform.GetPositionLocal().y,-1 };
	outputPathTextBox->SetPosition(outputPathTextBoxPos);





	//programNameTextBoxPanel 
	float programNameTextBoxPanelWidth = 100;
	float programNameTextBoxPanelHeight = 20;
	TextBoxPanel* programNameTextBoxPanel = TextBoxPanel::Create(map, mapLayerIndex, programNameTextBoxPanelWidth, programNameTextBoxPanelHeight);
	programNameTextBoxPanel->SetDrawFlag(false);


	Transform& programTextBoxPanelTransform = programNameTextBoxPanel->GetTransform();
	programTextBoxPanelTransform.SetIndependentRotationFlag(true);
	programTextBoxPanelTransform.SetIndependentScaleFlag(true);
	programNameTextBoxPanel->SetSelectAvailableFlag(false);
	programNameTextBoxPanel->SetText(reinterpret_cast<const char*>(u8"프로그램이름:"));
	programNameTextBoxPanel->SetTextColor({ 0.0f,0.0f,0.0f });


	buildMainPanel->AddChildPanelUiEntity(programNameTextBoxPanel);

	programNameTextBoxPanel->SetPosition({ -buildMainPanelWidth / 2 + margin + programNameTextBoxPanelWidth / 2  ,
		outputTextBoxPanelTransform.GetPositionLocal().y - outputPathTextBoxPanelHeight/2 - margin - programNameTextBoxPanelHeight/2 ,-1});



	//programNameTextBox


	TextBox* programNameTextBox = TextBox::Create(map, mapLayerIndex, MAX_PATH, Quad::ETextLineMode::eOneLine);
	Transform& programNameTextBoxTransform = programNameTextBox->GetTransform();

	programNameTextBoxTransform.SetIndependentRotationFlag(true);
	programNameTextBoxTransform.SetIndependentScaleFlag(true);
	programNameTextBox->SetDrawFlag(false);

	float programNameTextBoxWidth = 300;
	float programNameTextBoxHeight = 20;
	programNameTextBox->SetSize(programNameTextBoxWidth, programNameTextBoxHeight);
	programNameTextBox->SetSelectAvailableFlag(false);
	programNameTextBox->SetTextColor({ 0.0f,0.0f,0.0f });

	buildMainPanel->AddChildPanelUiEntity(programNameTextBox);

	DirectX::XMFLOAT3 programNameTextBoxPos = { programTextBoxPanelTransform.GetPositionLocal().x + programNameTextBoxPanelWidth / 2 + margin + outputPathTextBoxWidth / 2,
		programTextBoxPanelTransform.GetPositionLocal().y,-1 };
	programNameTextBox->SetPosition(programNameTextBoxPos);


	//build Button


	ButtonUiEntity* buildButton = ButtonUiEntity::Create(map, mapLayerIndex);
	Transform &	buildButtonTransform  = buildButton->GetTransform();
	buildButtonTransform.SetIndependentRotationFlag(true);
	buildButtonTransform.SetIndependentScaleFlag(true);
	buildButton->SetDrawFlag(false);
	buildButton->SetSelectAvailableFlag(false);
	float buildButtonWidth = 30;
	float buildButtonHeight = 20;

	buildButton->SetSize(buildButtonWidth, buildButtonHeight);

	buildMainPanel->AddChildPanelUiEntity(buildButton);

	buildButton->SetPosition({ buildMainPanelWidth / 2 - margin - buildButtonWidth / 2 , -buildMainPanelHeight / 2 + margin + buildButtonHeight / 2, -1 });


	buildButton->SetLButtonUpCallback([outputPathTextBox, programNameTextBox]() {


		//build module 실행 
		BuildModule::Build(outputPathTextBox->GetText(), programNameTextBox->GetText());
		auto popupWindowController = PopupWindowController::GetInstance();
		popupWindowController->SetKeepVisiblity(false);
		PopupWindow::SetVisibilityState(false);


		});







	//"C:\Users\dongd\Desktop\Output"



}

void Quad::FrameWindowMenuDirector::CreateFileMenu(TextBoxPanel* fileMenuTextBoxPanel, Texture* titleBarTexture)
{

	fileMenuTextBoxPanel->RegisterAcceptEvent("WindowActive");


	//	fileMenuTextBoxPanel->RegisterAcceptEvent("LButtonDown");

	fileMenuTextBoxPanel->RegisterEventCallback("Hover", [panel = fileMenuTextBoxPanel, defaultTexture = titleBarTexture](Event* pEvent) {

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
		bool hoverState = mouseEvent->GetHoverState();

		if (hoverState == true)
		{
			panel->SetTexture("MenuActive.png");
		}
		else
		{
			panel->SetTexture(defaultTexture);
		}

		});



	int	fileMenuTextBoxPanelPopupInActiveCallbackIndex = PopupWindowController::RegisterWindowInActiveCallback([Panel = fileMenuTextBoxPanel]() {

		MouseEvent virtualLButtonDownEvent("LButtonDown");
		Panel->OnEvent(&virtualLButtonDownEvent);


		});


	fileMenuTextBoxPanel->RegisterEventCallback("LButtonDown", [panel = fileMenuTextBoxPanel, fileMenuTextBoxPanelPopupInActiveCallbackIndex](Event* pEvent) {

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

		StateComponent* stateComponent = panel->GetStateComponent();

		bool hoverState = stateComponent->GetHoverState();


		static bool subMenuExpandState = false;
		static char checkDuplication = 0;
		if (hoverState)
		{
			if (subMenuExpandState)
			{
				//subMenu를 닫는다.

				//이경우에는 menu자체 클릭과 , popup에서의 콜백으로인해 LButtonDown메세지가 두번전달된다.
				//그것을 방지한다.

				if (checkDuplication == 1)
				{
					subMenuExpandState = false;

					PopupWindowController::SetStateWindowInActiveCallback(fileMenuTextBoxPanelPopupInActiveCallbackIndex, false);

					checkDuplication = 0;
				}
				else
					checkDuplication++;

			}
			else
			{
				//subMenu를 펼친다.
				//마우스커서의 위치에 popup창이 뜨는게아니라 정확히 파일메뉴밑에 위치해야한다.
				//popup창의 위치를 계산한다.
				ActivePopupMenuWindow(244, panel);
				subMenuExpandState = true;
				PopupWindowController::SetStateWindowInActiveCallback(fileMenuTextBoxPanelPopupInActiveCallbackIndex, true);

			}
		}
		else
		{
			if (subMenuExpandState)
			{
				//subMenu를 닫는다.
				subMenuExpandState = false;
				PopupWindowController::SetStateWindowInActiveCallback(fileMenuTextBoxPanelPopupInActiveCallbackIndex, false);
			}
		}



		});






	PopupWindowUiSystem* popupWindowUiSystem = PopupWindowUiSystem::GetInstance();

	Map* popupWindowUiMap = popupWindowUiSystem->GetMap();
	/*PanelUiEntity * fileSubMenuPanelUiEntity =	static_cast<PanelUiEntity*>(popupWindowUiMap->CreateObject("PanelUiEntity", 0));*/

	ListPanel* fileSubMenuListPanelUiEntity = ListPanel::Create(popupWindowUiMap, 0, 300, 800, 20);
	fileSubMenuListPanelUiEntity->SetTexture("DarkGray.png");

	//fileSubMenuListPanelUiEntity->SetSize(300, 800);
	fileSubMenuListPanelUiEntity->SetPosition(0, 0, 8);
	fileSubMenuListPanelUiEntity->SetTexture("Blue.png");

	popupWindowUiSystem->RegisterRootPanel(244, fileSubMenuListPanelUiEntity);
	fileSubMenuListPanelUiEntity->SetDrawFlag(false);
	fileSubMenuListPanelUiEntity->SetActiveFlag(false);


	fileSubMenuListPanelUiEntity->RegisterAcceptEvent("MouseMove");
	fileSubMenuListPanelUiEntity->RegisterAcceptEvent("Hover");
	fileSubMenuListPanelUiEntity->RegisterAcceptEvent("LButtonDown");
	fileSubMenuListPanelUiEntity->RegisterAcceptEvent("LButtonUp");




	CreateFileSubMenuChildPanelUiEntities(fileSubMenuListPanelUiEntity);

}

void Quad::FrameWindowMenuDirector::CreateToolsMenu(TextBoxPanel* toolsMenu, Texture* titleBarTexture)
{
	toolsMenu->RegisterEventCallback("Hover", [menu = toolsMenu , defaultTexture= titleBarTexture](Event* pEvent) {

		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);
		StateComponent* stateComponent = menu->GetStateComponent();

		bool hoverState = mouseEvent->GetHoverState();
		stateComponent->SetHoverState(hoverState);

		if (hoverState)
		{
			menu->SetTexture("MenuActive.png");
		}
		else
		{
			menu->SetTexture(defaultTexture);

		}
	});


	int	fileMenuTextBoxPanelPopupInActiveCallbackIndex = PopupWindowController::RegisterWindowInActiveCallback([Panel = toolsMenu]() {

		MouseEvent virtualLButtonDownEvent("LButtonDown");
		Panel->OnEvent(&virtualLButtonDownEvent);


		});


	
	toolsMenu->RegisterEventCallback("LButtonDown", [panel = toolsMenu, fileMenuTextBoxPanelPopupInActiveCallbackIndex ](Event* pEvent) {


		MouseEvent* mouseEvent = static_cast<MouseEvent*>(pEvent);

		StateComponent* stateComponent = panel->GetStateComponent();

		bool hoverState = stateComponent->GetHoverState();


		static bool subMenuExpandState = false;
		static char checkDuplication = 0;
		if (hoverState)
		{

			if (subMenuExpandState)
			{

				if (checkDuplication == 1)
				{
					subMenuExpandState = false;

					PopupWindowController::SetStateWindowInActiveCallback(fileMenuTextBoxPanelPopupInActiveCallbackIndex, false);

					checkDuplication = 0;
				}
				else
					checkDuplication++;

			}
			else
			{
				ActivePopupMenuWindow(245, panel);
				subMenuExpandState = true;
				PopupWindowController::SetStateWindowInActiveCallback(fileMenuTextBoxPanelPopupInActiveCallbackIndex, true);
			}
		}
		else
		{
			if (subMenuExpandState)
			{
				//subMenu를 닫는다.
				subMenuExpandState = false;
				PopupWindowController::SetStateWindowInActiveCallback(fileMenuTextBoxPanelPopupInActiveCallbackIndex, false);
			}
		}



		});



	PopupWindowUiSystem* popupWindowUiSystem = PopupWindowUiSystem::GetInstance();

	Map* popupWindowUiMap = popupWindowUiSystem->GetMap();
	/*PanelUiEntity * fileSubMenuPanelUiEntity =	static_cast<PanelUiEntity*>(popupWindowUiMap->CreateObject("PanelUiEntity", 0));*/

	ListPanel* toolSubMenuListPanelUiEntity = ListPanel::Create(popupWindowUiMap, 0, 300, 500, 20);
	toolSubMenuListPanelUiEntity->SetTexture("DarkGray.png");

	//fileSubMenuListPanelUiEntity->SetSize(300, 800);
	toolSubMenuListPanelUiEntity->SetPosition(0, 0, 8);

	popupWindowUiSystem->RegisterRootPanel(245, toolSubMenuListPanelUiEntity);
	toolSubMenuListPanelUiEntity->SetDrawFlag(false);
	toolSubMenuListPanelUiEntity->SetActiveFlag(false);


	toolSubMenuListPanelUiEntity->RegisterAcceptEvent("MouseMove");
	toolSubMenuListPanelUiEntity->RegisterAcceptEvent("Hover");
	toolSubMenuListPanelUiEntity->RegisterAcceptEvent("LButtonDown");
	toolSubMenuListPanelUiEntity->RegisterAcceptEvent("LButtonUp");



	CreateToolSubMenuChildPanelUiEntities(toolSubMenuListPanelUiEntity);



}



