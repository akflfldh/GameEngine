#include "GameWindowController.h"
#include"GameWindow.h"
#include"Game3DSystem.h"
#include"GameUiSystem.h"
#include"RenderSystem.h"
#include"Application.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"
#include"GameMapInstanceGenerator.h"
#include<vector>
#include"Core/KeyInputHandlerComponent.h"
#include"Core/MouseHandlerComponent.h"
#include"DefaultBaseMsgHandlerComponet.h"

Quad::GameWindowController::GameWindowController(GameWindow* gameWindow)
	:BaseWindowController(new RenderSystem),mWindow(gameWindow)
{
	mGame3DSystem = new Game3DSystem;
	mGameUiSystem = new GameUiSystem;



	UINT clientWidth = mWindow->GetClientWidth();
	UINT clientHeight = mWindow->GetClientHeight();


	//의미없는 Map이다. 곧바로 PlayMode로 전환되고 , 유저프로젝트의 맵이 설정될것이기에
	//다만 정해진틀을 위해서 생성 (즉 dummy 같은 존재이다.)
	//Map* dummy3DMap = GameMapInstanceGenerator::CreateMap(mGame3DSystem,"dummy3D");
	//Map* dummyUiMap = GameMapInstanceGenerator::CreateMap(mGameUiSystem, "dummyUi");
	
	//dummy3DMap->CreateDefaultMapLayer();
	//dummyUiMap->CreateDefaultMapLayer();


	//dummy3DMap->CreateMapLayer(0, 0, nullptr, nullptr, { 0,0,0,0,0,0 });
	//->CreateMapLayer(0, 0, nullptr, nullptr, { 0,0,0,0,0,0 });




	mGame3DSystem->Initialize(clientWidth, clientHeight, nullptr);
	mGameUiSystem->Initialize(clientWidth, clientHeight, nullptr);



	auto app = Application::GetInstance();

	auto renderSystem =GetRenderSystem();
	renderSystem->Initialize(app->GetD3D12Device(), app->GetD3DFactory(), &app->GetGraphicCommand(), mWindow->GetWindowHandle(),
		clientWidth, clientHeight, app->GetDescriptorHeapManagerMaster(), GAMEWINDOW);


	AddMsgHanlderComponent(new KeyInputHandlerComponent(this));
	AddMsgHanlderComponent(new MouseHandlerComponent(this));
	AddMsgHanlderComponent(new DefaultBaseMsgHandlerComponet);


}

Quad::BaseWindow* Quad::GameWindowController::GetWindow() const
{
	return mWindow;
}

void Quad::GameWindowController::Update(float deltaTime)
{


	mGameUiSystem->Update(deltaTime);
	mGame3DSystem->Update(deltaTime);

}

void Quad::GameWindowController::UploadObjectToRenderSystem()
{
	auto renderSystem = GetRenderSystem();

	//RenderSystem* renderSystem = GetRenderSystem();


	renderSystem->PreUpdate();

	const std::vector<MapLayer>& mapLayerVectorUi =	mGameUiSystem->GetEntity();
	renderSystem->SetMapLayerVector(mapLayerVectorUi, Quad::ESystemType::eUiSystem);


	const std::vector<MapLayer>& mapLayerVector3D = mGame3DSystem->GetEntity();
	renderSystem->SetMapLayerVector(mapLayerVector3D, Quad::ESystemType::eMainSystem);

	renderSystem->UploadEntityData();
}


void Quad::GameWindowController::EndUpdate()
{
	mGameUiSystem->EndUpdate();
	mGame3DSystem->EndUpdate();

}

void Quad::GameWindowController::Draw()
{
	auto renderSystem = GetRenderSystem();
	renderSystem->Draw();
}

void Quad::GameWindowController::OnResize(int clientWidth, int clientHeight, int direction)
{
	mGameUiSystem->OnResize(clientWidth, clientHeight);
	mGame3DSystem->OnResize(clientWidth, clientHeight);

}
