#include "EditorDirector/FileUiWindowContoller.h"
#include"EditorDirector/LoadRequestFileEvent.h"
#include"EditorDirector/LoadFileResponseEvent.h"
#include"EditorDirector/ContentItemUiEntity.h"
#include"System/FileUiUiSystem.h"

#include"ResourceController.h"
#include"ImportModule.h"

#include"LogicalContentItemUiEntityFolder.h"
#include"FolderPanelEntity.h"

#include"ProjectDirector.h"
#include"Project.h"

#include"FileUiDefaultMsgHandlerComponent.h"



Quad::FileUiWindowContoller::FileUiWindowContoller(RenderSystem* renderSystem)
	:DockingWindowController(renderSystem)
{
}

void Quad::FileUiWindowContoller::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList, MeshManager* meshManager, MaterialManager* materialManager, TextureManager* textureManager, MapManager* mapManager, TaskWindow* window, System* gameWorldSystem, WindowChromeSystem* windowLayoutSystem, WindowDockingSystem* windowDockingSystem, DescriptorHeapManagerMaster* descriptorHeapManagerMaster)
{


	AddMsgHanlderComponent(new FileUiDefaultMsgHandlerComponent(this));
	DockingWindowController::Initialize(device, graphicsCommandList, meshManager, materialManager,
		textureManager, mapManager, window, gameWorldSystem,
		windowLayoutSystem, windowDockingSystem, descriptorHeapManagerMaster);




}


void Quad::FileUiWindowContoller::DragAndDropFile(const std::vector<std::string>& dropFileVector)
{

	//파일경로가 들어있다.

	//리소스 컨트롤러가 수행 
	//리소스 컨트롤러는 로드 , 파싱, 리소스생성,
	//리소스 컨트롤러는 그후 일단은 file ui컨트롤러에게 알린다. 
	//file ui컨트롤러에서는 그에대해서 시각적으로 알리기위해서 엔티티를만들어서 리소스들을 화면에 보여주어야할것이다.
	//리소스자체는 시각적인없고 file ui시스템에서 그리소스와관련한 엔티티를만들고 시각적으로 보여주는것.


	//이벤트로 전달할것이냐 ,아니면 리소스컨트롤러메서드를 바로호출할것이냐
	//리소스를 삭제할때도 그렇게 시스템간의 리소스동기화가 필요할거같은니 이벤트를전달해서 올바른순서로 
	//처리하도록 해보자
	//컨트롤러가 보니깐 공통된게없고 사실상그냥ㄴ 윈도우마다 고유하게 하나씩 가져갈거같다.
	//그럼 컨트롤러에 이벤트를 바로 넣을수있게 스태틱메서드로 처리해도 될거같다. 그렇게처리하도록

	//LoadRequestFileEvent* loadRequestFileEvent = new LoadRequestFileEvent;
	//loadRequestFileEvent->SetFileVector(dropFileVector);
	//Controller::AddEvent("resourceController", loadRequestFileEvent);


//	auto* resourceController = ResourceController::GetInstance();

	auto * importModule = ImportModule::GetInstance();

	FileUiUiSystem* fileUiUiSystem = static_cast<FileUiUiSystem*>(mWorldSystem);
	LogicalContentItemUiEntityFolder* currentLogcialFolder = fileUiUiSystem->GetCurrentFolder();

	//fileUiUiSystem->GetLogicalFolderPath(LogicalContentItemUiEntityFolder);

	//현재 논리적 폴더 경로
	std::string logicalFolderPath = fileUiUiSystem->GetLogicalFolderPath(currentLogcialFolder);
	logicalFolderPath = logicalFolderPath + "/";



	//현재 유저 프로젝트의 Asset파일 경로

	ProjectMetaData * projectMetaData =	ProjectDirector::GetCurrentProjectMetaData();
	const std::string userAssetFolderPath = projectMetaData->GetProjectFolderPath() + "\\Asset";


	std::vector<Asset*> assetVector;
	for (auto& filePath : dropFileVector)
	{
		const std::vector<Asset*>& subAssetVector = importModule->ImportFile(filePath, logicalFolderPath, userAssetFolderPath);
		assetVector.insert(assetVector.end(), subAssetVector.begin(), subAssetVector.end());
	}



	fileUiUiSystem->AddAsset(assetVector);


	//Map* map = fileUiUiSystem->GetMap();
	//
	//for (int i = 0; i < assetVector.size(); ++i)
	//{
	//	//	ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(uiSystem, assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
	//		//ContentItemUiEntity* assetUiEntity = static_cast<ContentItemUiEntity*>(mUiSystem->GetMap()->CreateObject("ContentItemUiEntity"));
	//	ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(map, 0, assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
	//	//assetUiEntity->Initialize(assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
	//	fileUiUiSystem->AddContentItemUiEntity(assetUiEntity);
	//}






}

//void Quad::FileUiWindowContoller::DragAndDropFile(std::vector<std::string>&& dropFileVector)
//{
//
//	//LoadRequestFileEvent* loadRequestFileEvent = new LoadRequestFileEvent;
//	//loadRequestFileEvent->SetFileVector(std::move(dropFileVector));
//	//Controller::AddEvent("resourceController", loadRequestFileEvent);
//
//	auto * resourceController =  ResourceController::GetInstance();
//
//	std::vector<Asset*> assetVector;
//	for (auto& filePath : dropFileVector)
//	{
//		const std::vector<Asset*> & subAssetVector =resourceController->ImportFile(filePath);
//		assetVector.insert(assetVector.end(), subAssetVector.begin(), subAssetVector.end());
//	}
//
//
//	FileUiUiSystem* uiSystem = (FileUiUiSystem*)mUiSystem;
//
//	Map* map = mUiSystem->GetMap();
//
//	for (int i = 0; i < assetVector.size(); ++i)
//	{
//		//	ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(uiSystem, assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
//			//ContentItemUiEntity* assetUiEntity = static_cast<ContentItemUiEntity*>(mUiSystem->GetMap()->CreateObject("ContentItemUiEntity"));
//		ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(map, 0, assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
//		//assetUiEntity->Initialize(assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
//		uiSystem->AddContentItemUiEntity(assetUiEntity);
//	}
//
//
//}

void Quad::FileUiWindowContoller::HandleEvent(Event* pEvent)
{

	DockingWindowController::HandleEvent(pEvent);
	const std::string& eventName = pEvent->GetName();

	if (eventName == "LButtonDown")
	{
		int a = 2;
	}
	if (eventName == "LoadFileResponse")
	{
		//HandleLoadFileResponseEvent(pEvent);
	}
	






}





//void Quad::FileUiWindowContoller::HandleLoadFileResponseEvent(Event* pEvent)
//{
//
//	LoadFileResponseEvent* loadFileResponseEvent = (LoadFileResponseEvent*)pEvent;
//	const std::vector<Asset*> & assetVector = loadFileResponseEvent->GetAssetVector();
//
//	//이제 asset들이 로드되고 그에대한 리소스가 생성되었구나
//	//그런 asset들을 화면에 표현하기위해서 그 asset들과 연관된 asset ui entity를 생성하고file ui 창 화면에 보여주자.
//	FileUiUiSystem* uiSystem = (FileUiUiSystem*)mUiSystem;
//
//	Map* map = mUiSystem->GetMap();
//
//	for (int i = 0; i < assetVector.size(); ++i)
//	{
//	//	ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(uiSystem, assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
//		//ContentItemUiEntity* assetUiEntity = static_cast<ContentItemUiEntity*>(mUiSystem->GetMap()->CreateObject("ContentItemUiEntity"));
//		ContentItemUiEntity* assetUiEntity = ContentItemUiEntity::Create(map, 0, assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
//		//assetUiEntity->Initialize(assetVector[i], ContentItemUiEntity::EType::eFileUiVersion);
//		uiSystem->AddContentItemUiEntity(assetUiEntity);
//	}
//
//}

