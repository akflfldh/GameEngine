#include "NavigationFolderPanelEntity.h"
#include"System/FileUiUiSystem.h"


Quad::NavigationFolderPanelEntity::NavigationFolderPanelEntity(const std::string& name)
	:PanelUiEntity(name),mBackButton(nullptr),mFowardButton(nullptr),
	mCurrentBackVectorIndex(0),mCurrentFowardVectorIndex(0),mBackFolderCount(0), mFowardFolderCount(0), mVectorMaxCount(5),mFowardFolderEntityVector(5),mBackFolderEntityVector(5)
{

}

Quad::NavigationFolderPanelEntity::~NavigationFolderPanelEntity()
{
}


void Quad::NavigationFolderPanelEntity::SetNewFolder(LogicalContentItemUiEntityFolder* preFolder)
{
	//mBackFolderEntityDeque.push_back(preFolder);
	//mFowardFolderEntityDeque.clear();






	mBackFolderEntityVector[mCurrentBackVectorIndex] = preFolder;
	mCurrentBackVectorIndex = (mCurrentBackVectorIndex + 1) % mVectorMaxCount;

	mBackFolderCount++;
	if (mBackFolderCount > mVectorMaxCount)
		mBackFolderCount = mVectorMaxCount;

	mFowardFolderCount = 0;
	mCurrentFowardVectorIndex = 0;





}

void Quad::NavigationFolderPanelEntity::InitCreating(float width, float height)
{
	PanelUiEntity::InitCreating();

	SetTexture("DarkGray.png");
	SetSize(width, height);
	const DirectX::XMFLOAT3& panelPos = GetTransform().GetPositionWorld();

	System* system = GetSystem();

	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	//mBackButton = static_cast<ButtonUiEntity*>(map->CreateObject("ButtonUiEntity"));
	mBackButton = ButtonUiEntity::Create(map, mapLayerIndex);

	//mFowardButton = static_cast<ButtonUiEntity*>(map->CreateObject("ButtonUiEntity"));
	mFowardButton = ButtonUiEntity::Create(map, mapLayerIndex);



	//mBackButton.SetSystem(system);
	//mFowardButton.SetSystem(system);

	//mBackButton->Initialize();
	//mFowardButton->Initialize();


	mBackButton->SetTexture("ArrowRightButton.png");
	DirectX::XMFLOAT4X4 backButtonTexTransform;
	DirectX::XMStoreFloat4x4(&backButtonTexTransform, DirectX::XMMatrixRotationZ(DirectX::XM_PI));
	mBackButton->SetTextureTransform(backButtonTexTransform);

	mFowardButton->SetTexture("ArrowRightButton.png");


	mBackButton->GetTransform().SetIndependentScaleFlag(true);
	mFowardButton->GetTransform().SetIndependentScaleFlag(true);
	mBackButton->SetSize(30, 30);
	mFowardButton->SetSize(30, 30);

	float backButtonPosX = panelPos.x - width / 2 + 15;
	mBackButton->SetPosition(backButtonPosX, panelPos.y, panelPos.z - 1);
	mFowardButton->SetPosition(backButtonPosX + 5 + 30, panelPos.y, panelPos.z - 1);

	//간격

	mBackButton->SetSelectAvailableFlag(false);
	mFowardButton->SetSelectAvailableFlag(false);


	AddChildPanelUiEntity(mBackButton);
	AddChildPanelUiEntity(mFowardButton);



	SetButtonCallback();



}

void Quad::NavigationFolderPanelEntity::Initialize()
{
	PanelUiEntity::Initialize();


	RegisterAcceptEvent("LButtonDown");
	RegisterAcceptEvent("LButtonUp");
	RegisterAcceptEvent("MouseMove");





}

Quad::NavigationFolderPanelEntity* Quad::NavigationFolderPanelEntity::Create(Map* map, int mapLayerIndex, float width, float height)
{

	NavigationFolderPanelEntity* ob = static_cast<NavigationFolderPanelEntity*>(map->CreateObject("NavigationFolderPanelEntity", mapLayerIndex));
	  
	//ob->Initialize();
	ob->InitCreating(width, height);
	//ob->Start();
	return ob;
}

void Quad::NavigationFolderPanelEntity::SetButtonCallback()
{

	//back

	mBackButton->SetLButtonUpCallback([button = mBackButton , navigationPanel = this]() 
		{
			if (button->GetStateComponent()->GetHoverState())
			{

				button->SetColorItensity({ 0.5f,0.0f,0.0f });
				navigationPanel->BackFolder();
			}
			else
				button->SetColorItensity({ 1.0f,1.0f,1.0f });
	
		});

	mBackButton->SetLButtonDownCallback([button = mBackButton]() { button->SetColorItensity({ 2.0f,0.0f,0.0f }); });
	mBackButton->SetHoverCallback([button = mBackButton]() {button->SetColorItensity({ 0.5f,0.0f,0.0f });  });
	mBackButton->SetReleaseHoverCallback([button = mBackButton ]() { button->SetColorItensity({ 1.0f,1.0f,1.0f }); });






	//foward

	mFowardButton->SetLButtonUpCallback([button = mFowardButton, navigationPanel = this]()
		{
			if (button->GetStateComponent()->GetHoverState())
			{
				button->SetColorItensity({ 0.5f,0.0f,0.0f });
				navigationPanel->FowardFolder();

			}
			else
				button->SetColorItensity({ 1.0f,1.0f,1.0f });

		});

	mFowardButton->SetLButtonDownCallback([button = mFowardButton]() { button->SetColorItensity({ 2.0f,0.0f,0.0f }); });
	mFowardButton->SetHoverCallback([button = mFowardButton]() {button->SetColorItensity({ 0.5f,0.0f,0.0f });  });
	mFowardButton->SetReleaseHoverCallback([button = mFowardButton]() { button->SetColorItensity({ 1.0f,1.0f,1.0f }); });














}

void Quad::NavigationFolderPanelEntity::BackFolder()
{

	//항상 최근폴더는 back에 있다(deque의 back)

	//if (mBackFolderEntityDeque.empty())
	//	return;


	FileUiUiSystem* system = (FileUiUiSystem*)GetSystem();

	//LogicalContentItemUiEntityFolder* currentFolder = system->GetCurrentFolder();
	//mFowardFolderEntityDeque.push_back(currentFolder);
	//
	//currentFolder = mBackFolderEntityDeque.back();
	//mBackFolderEntityDeque.pop_back();

	//system->SetCurrentFolder(currentFolder,true);





	if (mBackFolderCount == 0)
		return;


	LogicalContentItemUiEntityFolder* currentFolder = system->GetCurrentFolder();
	mFowardFolderEntityVector[mCurrentFowardVectorIndex] = currentFolder;
	mCurrentFowardVectorIndex = (mCurrentFowardVectorIndex + 1) % mVectorMaxCount;
	mFowardFolderCount++;

	if (mFowardFolderCount > mVectorMaxCount)
		mFowardFolderCount = mVectorMaxCount;



	mCurrentBackVectorIndex -= 1;
	if (mCurrentBackVectorIndex < 0)
		mCurrentBackVectorIndex = (mVectorMaxCount-1);

	currentFolder = mBackFolderEntityVector[mCurrentBackVectorIndex];
	mBackFolderCount--;


	system->SetCurrentFolder(currentFolder, true);






}

void Quad::NavigationFolderPanelEntity::FowardFolder()
{
	//if (mFowardFolderEntityDeque.empty())
		//return;


	FileUiUiSystem* system = (FileUiUiSystem*)GetSystem();
	LogicalContentItemUiEntityFolder * currentFolder =	system->GetCurrentFolder();
	//mBackFolderEntityDeque.push_back(currentFolder);

	
	//currentFolder =	mFowardFolderEntityDeque.back();
	//mFowardFolderEntityDeque.pop_back();

	//system->SetCurrentFolder(currentFolder,true);*/



	if (mFowardFolderCount == 0)
		return;



	mBackFolderEntityVector[mCurrentBackVectorIndex] = currentFolder;
	mCurrentBackVectorIndex = (mCurrentBackVectorIndex + 1) % mVectorMaxCount;

	mBackFolderCount++;
	if (mBackFolderCount > mVectorMaxCount)
		mBackFolderCount = mVectorMaxCount;


	mCurrentFowardVectorIndex--;
	if (mCurrentFowardVectorIndex < 0)
		mCurrentFowardVectorIndex = mVectorMaxCount - 1;

	currentFolder = mFowardFolderEntityVector[mCurrentFowardVectorIndex];
	mFowardFolderCount--;

	system->SetCurrentFolder(currentFolder, true);









}

void Quad::NavigationFolderPanelEntity::Start()
{
	//PanelUiEntity::Start();
	//SetButtonCallback();
}

void Quad::NavigationFolderPanelEntity::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);
}

void Quad::NavigationFolderPanelEntity::OnEvent(Event* event)
{
	PanelUiEntity::OnEvent(event);







}
