#include "ProjectSelectSceneDirector.h"
#include "FrameWindowMainSceneDirector.h"

#include"PanelUiEntity.h"
#include"System/FrameWindowUiSystem.h"
#include"ContentItemUiPanelEntity.h"
#include"ContentIProjecttemUiEntity.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"ButtonUiEntitIy.h"
#include"TextBox.h"
#include"TextBoxPanel.h"

#include"ProjectDirector.h"


Quad::ProjectSelectSceneDirector::ProjectSelectSceneDirector()
	:mFrameWindowUiSystem(nullptr), mMainScenePanel(nullptr)
{


}

void Quad::ProjectSelectSceneDirector::Initialize(FrameWindowUiSystem* frameWindowUiSystem)
{

	mFrameWindowUiSystem = frameWindowUiSystem;

	Map* currMap = mFrameWindowUiSystem->GetMap();

	float clientWidth = mFrameWindowUiSystem->GetClientWidth();
	float clientHeight = mFrameWindowUiSystem->GetClientHeight();


	float mainScenePanelWidth = 3000;
	float mainScenePanelHeight = 10000;
	PanelUiEntity* mainScenePanel = PanelUiEntity::Create(currMap, 0);
	mainScenePanel->SetTexture(TextureManager::GetTexture("Red.png"));
	mainScenePanel->SetSize(mainScenePanelWidth, mainScenePanelHeight);

	mainScenePanel->SetPosition(clientWidth / 2, -mainScenePanelHeight / 2, 10);

	mainScenePanel->GetTransform().SetIndependentScaleFlag(true);
	mainScenePanel->GetTransform().SetIndependentRotationFlag(true);

	mainScenePanel->RegisterAcceptEvent("Hover");
	mainScenePanel->RegisterAcceptEvent("MouseMove");
	mainScenePanel->RegisterAcceptEvent("LButtonDown");
	mainScenePanel->RegisterAcceptEvent("LButtonUp");
	mainScenePanel->RegisterAcceptEvent("LButtonDownDouble");




	//float titleBarHeight =	windowController->GetTitleBarHeight();
	float contentItemUiPanelWidth = 3000;
	float contentItemUiPanelHeight = 10000;
	ContentItemUiPanelEntity* contentItemUiPanel = static_cast<ContentItemUiPanelEntity*>(ContentItemUiPanelEntity::Create(currMap, 0, contentItemUiPanelWidth, contentItemUiPanelHeight, 0, 0, 9, clientWidth, clientHeight - 100));


	contentItemUiPanel->GetTransform().SetIndependentRotationFlag(true);
	contentItemUiPanel->GetTransform().SetIndependentScaleFlag(true);
	contentItemUiPanel->GetTransform().SetIndependentTransitionFlag(true);

	mainScenePanel->AddChildPanelUiEntity(contentItemUiPanel);
	contentItemUiPanel->SetSelectAvailableFlag(false);
	//contentItemUiPanel->SetPosition({ contentItemUiPanelWidth / 2.0f, -1.0f * contentItemUiPanelHeight / 2  ,-1 });
	//contentItemUiPanel->SetPosition({0.0f,	mainScenePanelHeight/2 - 1.0f * contentItemUiPanelHeight / 2  ,-1 });

	//contentItemUiPanel->SetPosition((float)contentItemUiPanel->GetWidth() / 2, (float)-1 * contentItemUiPanel->GetHeight() / 2, 20);
	//contentItemUiPanel->SetPosition((float)contentItemUiPanel->GetWidth() / 2 - 130, (float)-1 * contentItemUiPanel->GetHeight() / 2, 20);
	contentItemUiPanel->SetTexture(TextureManager::GetTexture("Blue.png"));


	//ProjectMetaData* projectData = new ProjectMetaData("sample");
	////	ContentItemUiEntity * contentItemUiEntity = ContentItemUiEntity::Create(currMap, 0, projectData, ContentItemUiEntity::EType::eFileUiVersion);
	//ContentIProjecttemUiEntity* contentItemUiEntity = ContentIProjecttemUiEntity::Create(currMap, 0, projectData);
	//contentItemUiPanel->AddContentItemUiEntity(contentItemUiEntity);

	const auto& projectMetaDataVector = ProjectDirector::GetProjectMetaDataVector();

	for (auto& projectMetaData : projectMetaDataVector)
	{
		ContentIProjecttemUiEntity* contentItemUiEntity = ContentIProjecttemUiEntity::Create(currMap, 0, projectMetaData);
		contentItemUiPanel->AddContentItemUiEntity(contentItemUiEntity);
	}







	float margin = 10;

	PanelUiEntity* ProjectCreatePanel = PanelUiEntity::Create(currMap, 0);
	ProjectCreatePanel->GetTransform().SetIndependentRotationFlag(true);
	ProjectCreatePanel->GetTransform().SetIndependentScaleFlag(true);
	ProjectCreatePanel->SetSelectAvailableFlag(false);
	ProjectCreatePanel->SetSize(clientWidth, 160);

	mainScenePanel->AddChildPanelUiEntity(ProjectCreatePanel);

	/*ProjectCreatePanel->SetPosition((float)clientWidth / 2, (float)-1 * clientHeight + ProjectCreatePanel->GetHeight() / 2, 10);*/
	ProjectCreatePanel->SetPosition(0.0f, mainScenePanelHeight / 2 - (float)clientHeight + ProjectCreatePanel->GetHeight() / 2, -2);
	Texture* tex = TextureManager::GetTexture("Red.png");
	ProjectCreatePanel->SetTexture(tex);

	ProjectCreatePanel->RegisterAcceptEvent("LButtonDown");


	ButtonUiEntity* creatingProjectButton = ButtonUiEntity::Create(currMap, 0);
	creatingProjectButton->SetSelectAvailableFlag(false);

	TextBoxPanel* projectNameTextBoxTagPanel = TextBoxPanel::Create(currMap, 0, 100, 60);	//태그 '프로젝트이름:' 을 나타내는 텍스트박스판넬 
	projectNameTextBoxTagPanel->SetSelectAvailableFlag(false);
	TextBoxPanel* projectPathTextBoxTagPanel = TextBoxPanel::Create(currMap, 0, 100, 60);//태그 '프로젝트경로:'을 나타내는 텍스트박스 판넬
	projectPathTextBoxTagPanel->SetSelectAvailableFlag(false);

	TextBox* ProjectNameTextBox = TextBox::Create(currMap, 0, 255, Quad::ETextLineMode::eOneLine, 1);
	ProjectNameTextBox->SetSelectAvailableFlag(false);

	TextBox* projectPathSearchTextBox = TextBox::Create(currMap, 0, 255, Quad::ETextLineMode::eOneLine, 1);
	projectPathSearchTextBox->SetSelectAvailableFlag(false);
	//search path button
	ButtonUiEntity* projectPathSearchButton = ButtonUiEntity::Create(currMap, 0);
	projectPathSearchButton->SetSelectAvailableFlag(false);




	//CreatingTextBoxPanel
	Transform& projectNameTextBoxTagPanelTransform = projectNameTextBoxTagPanel->GetTransform();

	projectNameTextBoxTagPanelTransform.SetIndependentRotationFlag(true);
	projectNameTextBoxTagPanelTransform.SetIndependentScaleFlag(true);

	ProjectCreatePanel->AddChildPanelUiEntity(projectNameTextBoxTagPanel);

	projectNameTextBoxTagPanel->SetText(reinterpret_cast<const char*>(u8"프로젝트 이름: "));
	projectNameTextBoxTagPanel->SetPosition(-ProjectCreatePanel->GetWidth() / 2 + margin + projectNameTextBoxTagPanel->GetWidth() / 2, -ProjectCreatePanel->GetHeight() / 2 + margin + projectNameTextBoxTagPanel->GetHeight() / 2, -1);
	//creatingTextBoxTagPanel
	projectNameTextBoxTagPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	//	projectNameTextBoxTagPanel->SetTexture("Transparent.png");


		//ProjectPathTextBoxPanel
	Transform& projectPathTextBoxTagPanelTransform = projectPathTextBoxTagPanel->GetTransform();
	projectPathTextBoxTagPanelTransform.SetIndependentScaleFlag(true);
	projectPathTextBoxTagPanelTransform.SetIndependentRotationFlag(true);

	ProjectCreatePanel->AddChildPanelUiEntity(projectPathTextBoxTagPanel);

	projectPathTextBoxTagPanel->SetText(reinterpret_cast<const char*>(u8"프로젝트 경로: "));
	projectPathTextBoxTagPanel->SetTextColor({ 0.0f,0.0f,0.0f });
	//projectPathTextBoxTagPanel->SetTexture("Transparent.png");
	projectPathTextBoxTagPanel->SetPosition(-ProjectCreatePanel->GetWidth() / 2 + margin + projectPathTextBoxTagPanel->GetWidth() / 2, ProjectCreatePanel->GetHeight() / 2 - margin - projectPathTextBoxTagPanel->GetHeight() / 2, -1);


	//creating button
	creatingProjectButton->SetSize(60, 60);
	Transform& creatingProjectButtonTransform = creatingProjectButton->GetTransform();
	creatingProjectButtonTransform.SetIndependentScaleFlag(true);
	creatingProjectButtonTransform.SetIndependentRotationFlag(true);

	ProjectCreatePanel->AddChildPanelUiEntity(creatingProjectButton);

	creatingProjectButton->SetPosition(ProjectCreatePanel->GetWidth() / 2 - margin - creatingProjectButton->GetWidth() / 2, -ProjectCreatePanel->GetHeight() / 2 + margin + creatingProjectButton->GetHeight() / 2, -1);
	creatingProjectButton->SetTexture(reinterpret_cast<const char*>(u8"버튼21.png"));


	creatingProjectButton->SetLButtonUpCallback([pathTextBox = projectPathSearchTextBox, nameTextBox = ProjectNameTextBox]() {

		std::string projectName = nameTextBox->GetText();
		if (projectName == "")
			return;

		std::string projectParentFolderPath = pathTextBox->GetText();
		if (Utility::CheckFolderPath(projectParentFolderPath))
		{
			ProjectDirector::CreateProject(projectName, projectParentFolderPath);

		}
		else
		{
			MessageBox(nullptr, L"유효한 경로가 아닙니다.", L"에러", 0);
		}


		});



	//search path text box 

	Transform& projectPathSearchTextBoxTransform = projectPathSearchTextBox->GetTransform();
	projectPathSearchTextBoxTransform.SetIndependentScaleFlag(true);
	projectPathSearchTextBoxTransform.SetIndependentRotationFlag(true);
	projectPathSearchTextBox->SetSize(1000, 60);

	ProjectCreatePanel->AddChildPanelUiEntity(projectPathSearchTextBox);

	DirectX::XMFLOAT3 projectPathSerachTextBoxPosLocal = projectPathTextBoxTagPanelTransform.GetPositionLocal();
	projectPathSerachTextBoxPosLocal.x += projectPathTextBoxTagPanel->GetWidth() / 2 + margin + projectPathSearchTextBox->GetWidth() / 2;
	projectPathSerachTextBoxPosLocal.x = 20;



	projectPathSearchTextBoxTransform.SetPositionLocal(projectPathSerachTextBoxPosLocal);
	//CreatingTextBox->SetTextColor({ 0.0f,0.0f,0.0f });
	projectPathSearchTextBox->SetTexture("DarkGray.png");



	//
	//
		//path search button
	Transform& projectPathSearchButtonTransform = projectPathSearchButton->GetTransform();
	//
	//
	projectPathSearchButton->SetSize(30, 60);
	projectPathSearchButtonTransform.SetIndependentScaleFlag(true);
	projectPathSearchButtonTransform.SetIndependentRotationFlag(true);

	ProjectCreatePanel->AddChildPanelUiEntity(projectPathSearchButton);
	//
	DirectX::XMFLOAT3 serachPathButtonLocal = { ProjectCreatePanel->GetWidth() / 2 - margin - projectPathSearchButton->GetWidth() / 2,ProjectCreatePanel->GetHeight() / 2 - margin - projectPathSearchButton->GetHeight() / 2, -2 };
	projectPathSearchButtonTransform.SetPositionLocal(serachPathButtonLocal);
	//
	//
	projectPathSearchButton->SetTexture(TextureManager::GetTexture("PathSearchButton.png"));
	//
	//	
	//
	//
	//


	projectPathSearchButton->SetHoverCallback([button = projectPathSearchButton]() {

		button->SetColorItensity({ 0.5f,0.5f,0.5f });
		});

	projectPathSearchButton->SetReleaseHoverCallback([button = projectPathSearchButton]() {


		button->SetColorItensity({ 1.0f,1.0f,1.0f });
		});
	//	creatingProjectSearchPathButton->GetLButtonUpCallback([]() {})


	projectPathSearchButton->SetLButtonUpCallback([textBox = projectPathSearchTextBox]() {

		//	MessageBox(nullptr, L"탐색버튼클릭LButtonUp", L"알람", 0);

			//대화상자띄우기 
			//결과 파일이름을 TEXTBox에게 전달//
			//폴더가선택되지않거나 취소되었다면 빈문자열 전달
		const std::string folderPath = Utility::GetFolderPathFromPickingDialog();

		//OutputDebugStringA(folderPath.c_str());

		textBox->SetText(folderPath);
		});




	//project name textbox


	Transform& proejctTextBoxTransform = ProjectNameTextBox->GetTransform();
	proejctTextBoxTransform.SetIndependentScaleFlag(true);
	proejctTextBoxTransform.SetIndependentRotationFlag(true);
	ProjectNameTextBox->SetSize(1000, 60);

	ProjectCreatePanel->AddChildPanelUiEntity(ProjectNameTextBox);

	DirectX::XMFLOAT3 projectNameTextBoxLocalPos = projectNameTextBoxTagPanelTransform.GetPositionLocal();
	projectNameTextBoxLocalPos.x += projectNameTextBoxTagPanel->GetWidth() / 2 + margin + ProjectNameTextBox->GetWidth() / 2;
	//projectNameTextBoxLocalPos.x = 20;
	proejctTextBoxTransform.SetPositionLocal(projectNameTextBoxLocalPos);

	/*ProjectNameTextBox->SetPosition(-ProjectCreatePanel->GetWidth() / 2 + margin + ProjectNameTextBox->GetWidth() / 2, -ProjectCreatePanel->GetHeight() / 2 + margin + CreatingTextBox->GetHeight() / 2, -1);*/
	//CreatingTextBox->SetTextColor({ 0.0f,0.0f,0.0f });
	ProjectNameTextBox->SetTexture("DarkGray.png");






	mainScenePanel->SetDrawFlag(false);
	//mainScenePanel->SetActiveFlag(true);
	mainScenePanel->SetSelectAvailableFlag(false);

	mMainScenePanel = mainScenePanel;

}

void Quad::ProjectSelectSceneDirector::ActiveScene()
{

	mMainScenePanel->SetDrawFlag(true);
	mMainScenePanel->SetActiveFlag(true);
	mMainScenePanel->SetSelectAvailableFlag(true);


}

void Quad::ProjectSelectSceneDirector::InActiveScene()
{
	mMainScenePanel->SetDrawFlag(false);
//	mMainScenePanel->SetActiveFlag(true);
	mMainScenePanel->SetSelectAvailableFlag(false);



}

Quad::ProjectSelectSceneDirector::~ProjectSelectSceneDirector()
{
}
