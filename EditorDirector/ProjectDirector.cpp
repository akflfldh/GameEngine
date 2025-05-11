#include "ProjectDirector.h"
#include"Project.h"
#include"Parser/JsonParser.h"
#include"System/GamePlaySystem.h"
#include"System/GamePlayUiSystem.h"

#include"ResourceManager/TextureManager/TextureManager.h"
#include"System/FrameWindowUiSystem.h"

#include"ContentItemUiPanelEntity.h"
#include"Controller.h"
#include"DockingWindowController.h"
#include"ContentIProjecttemUiEntity.h"
#include"ProjectMetaData.h"

#include"ButtonUiEntitiy.h"

#include"TextBox.h"
#include"TextBoxPanel.h"


#include"Utility.h"
#include"FrameWindow.h"
#include"System/WindowChromeSystem.h"
#include"System/WindowDockingSystem.h"
#include"PopupWindowController.h"
#include"System/PopupWindowUiSystem.h"
#include"FrameWindowController.h"
#include"System/FrameWindowDockingSystem.h"
#include"System.h"
#include"System/GamePlaySystem.h"
#include"EditorSystem.h"



#include"Object/Camera/FrustumCamera.h"
#include"Object/Camera/OrthogoanlCamera.h"

#include"VectorSpace.h"
#include"Collision/CollisionWorld.h"
#include"Object/Light/Light.h"

#include"GamePlayWindowCamera.h"

#include"Object/Line/Grid.h"


#include"FrameWindowMenuDirector.h"

#include"EditorMap.h"

#include"UserBuildFileGenerator.h"


#include"Parser/JsonParser.h"

#include"EditorAssetImportVersionModule.h"
#include"ImportModule.h"



#include"FileUiWindowContoller.h"

#include"System/FileUiUiSystem.h"
#include<sstream>


#include"ResourceManager/MaterialManager/MaterialManager.h"

#include"ResourceManager/MeshManager.h"

#include"ResourceController.h"
#include"GamePlayWindowController.h"


#include"EditorDirector/EditorDirector.h"

#include"Core/DefaultMoveFrustumCamera.h"

#include"Core/CollisionWorldFactory.h"
#include"Core/SpacePartitioningStructureFactory.h"


Quad::ProjectDirector::ProjectDirector()
	:mCurrProject(nullptr)
{
}

Quad::ProjectDirector::~ProjectDirector()
{
	for (auto& projectMetaData : mProjectMetaDataVector)
	{
		delete projectMetaData;
	}
}

void Quad::ProjectDirector::Initialize(FrameWindow* frameWindow, std::vector<DockingWindowController*> normalWindowControllerVector, PopupWindowController* popupWindowController, FrameWindowController* frameWindowController, FrameWindowUiSystem* frameWindowUiSystem)
{


	mNormalWindowControllerVector = std::move(normalWindowControllerVector);
	mPopupWindowController = popupWindowController;
	mFrameWindow = frameWindow;
	mFrameWindowController = frameWindowController;





	LoadProjectMetaData();

	mFrameWindowMainSceneDirector.Initialize(frameWindowUiSystem);


}

bool Quad::ProjectDirector::CreateProject(const std::string& projectName, const std::string& projectParentPath)
{

	const std::string projectPath = projectParentPath + "\\Project";

	//폴더게층
	CreateProjectDirectoryHierarchy(projectParentPath);

	//솔루션,프로젝트 빌드파일 생성
	UserBuildFileGenerator::CreateBuildFile("C:\\Users\\dongd\\gitproject\\GameEngine\\EditorTemplate", projectPath);





	//프로젝트 및 맵 메타데이터,메인데이터생성





	ProjectMetaData* projectMetaData = new ProjectMetaData(projectName);
	projectMetaData->SetProjectFolderPath(projectPath);
	

	auto instance = ProjectDirector::GetInstance();

	instance->mProjectMetaDataVector.push_back(projectMetaData);


	//Project* project = Project::CreateDefaultProject();
	Project* project = new Project;
	project->SetName(projectName);

	const std::string mapName= "Map0";
	project->CreateMapMetaData("", "Map0");



	//현재 시스템에있는 map은 버리고 새로운 맵을 생성하고 DeSerialize()해서 설정한다.
	//Map * gamePlay3DSystemMap =  gamePlaySystem->GetMap();		//당연히 currMap은  edit모드 map 
	//gamePlay3DSystemMap->SetName(mapName3D);


	Map* gamePlaySystemMap = CreateDefaultMap(mapName);



	


	instance->mCurrProject = project;


	GamePlaySystem* gamePlaySystem = GamePlaySystem::GetInstance();
	gamePlaySystem->SetMap(gamePlaySystemMap);




	InitProjectEditorMode();
	GenerateProjectDefaultUserObjects();


	//맨처음 초기 맵파일을 생성한다 (저장기능)
//	gamePlay3DSystemMap->SetMainUserCamera();
	gamePlaySystemMap->Serialize();
	//마지막으로 파일저장하는것은 외부에서 처리한다. (에디터에서 게임플레이 에디터모드와 런타임모드로 전환할떄 직렬화를사용하는데 이떄는 굳이 파일을 저장하는것은 불필요한 작업임으로 분리한다)
	const std::string& mapPath = projectPath + "\\Map\\" + mapName + ".json";
	JsonParser::Save(mapPath);





	//새로운프로젝트 생성후 그내용을 일단 바로 저장한다.
	instance->SaveProjectMetaData();



	//엔진(에디터)의 config파일을 읽어서 user에게 복사해야할 asset들을 파악하고
//동일한속성값,다른 고유의이름을 가지는 asset들을 생성한다.
//고유의 이름도 이미 정해져있다.
//이것은 딱 프로젝트생성할때 한번만 수행된다.

	//CopyDefaultAssetToUser();




	project->SaveProject();


	//화면전환
	EditorDirector::SetSwitchWindowSceneModeFlag(2);

	//SetOhterWindowSystemOn();

	return true;



}

Quad::ProjectMetaData* Quad::ProjectDirector::GetProjectMetaData(const std::string& projectName)
{
	auto instance = ProjectDirector::GetInstance();



	for (auto& projectMetaData : instance->mProjectMetaDataVector)
	{	
		
		//const std::string & projectName = projectMetaData->GetName();
		if (projectMetaData->GetName() == projectName)
		{
			return projectMetaData;
		}
	}

	return nullptr;
}

Quad::ProjectMetaData* Quad::ProjectDirector::GetCurrentProjectMetaData()
{
	auto instance = ProjectDirector::GetInstance();
	if (instance->mCurrProject == nullptr)
		return nullptr;
	
	return GetProjectMetaData(instance->mCurrProject->GetName());
}

const std::vector<Quad::ProjectMetaData*>& Quad::ProjectDirector::GetProjectMetaDataVector() 
{

	auto instance = ProjectDirector::GetInstance();

	return instance->mProjectMetaDataVector;

	// TODO: 여기에 return 문을 삽입합니다.
}

Quad::Project* Quad::ProjectDirector::GetCurrentProject()
{
	auto instance = GetInstance();

	return instance->mCurrProject;
}

void Quad::ProjectDirector::SelectProject(ProjectMetaData* projectMetaData)
{
	auto instance = GetInstance();

	const std::string projectPath = projectMetaData->GetProjectFolderPath();

	if (instance->mCurrProject == nullptr)
	{
		Project * project = new Project;
		instance->mCurrProject = project;
		project->SetName(projectMetaData->GetName());
		project->LoadProject();


		//화면을 에디터main화면으로 전환

		SetOhterWindowSystemOn();
		SetOtherWindowVisibleOnOff(true);
	}
	else
	{




	}



}



void Quad::ProjectDirector::SaveProjectMetaData()
{
	JsonParser::StartWrite();

	JsonParser::StartWriteObject();
	JsonParser::Write("ProjectDirector_MetaDataNum", mProjectMetaDataVector.size());

	for (auto& projectMetaData : mProjectMetaDataVector)
	{
		JsonParser::StartWriteObject();
		projectMetaData->Serialize();
	}












	JsonParser::Save("./ProjectMetaDataFile.json");
	
}

void Quad::ProjectDirector::LoadProjectMetaData()
{
	//if (!JsonParser::ReadFile("./ProjectMetaDataFile.json"))
	//	return;

	
	auto editorDirector =EditorDirector::GetInstance();
	const std::string projectListFile = editorDirector->GetEditorPathA() + "\\ProjectMetaDataFile.json";
	if (!JsonParser::ReadFile(projectListFile))
		return;


	JsonParser::ReadStart();
	
	size_t projectMetaDataNum = 0;
	JsonParser::Read("ProjectDirector_MetaDataNum", projectMetaDataNum);
	mProjectMetaDataVector.resize(projectMetaDataNum,nullptr);
	

	for (size_t i = 0; i < projectMetaDataNum; ++i)
	{
		mProjectMetaDataVector[i] = new ProjectMetaData;

		JsonParser::SetCurrentIndex(i + 1);
		mProjectMetaDataVector[i]->DeSerialize();
	}



}

void Quad::ProjectDirector::InitProjectEditorMode()
{

	//GamePlayUiSystem* gamePlayUiSystem = GamePlayUiSystem::GetInstance();
	GamePlaySystem* gamePlaySystem = GamePlaySystem::GetInstance();


	EditorMap* gamePlayMap = static_cast<EditorMap*>(gamePlaySystem->GetMap());
	//EditorMap* gamePlayUiMap =static_cast<EditorMap*>(gamePlayUiSystem->GetMap());


	float clientWidth = gamePlaySystem->GetClientWidth();
	float clientHeight = gamePlaySystem->GetClientHeight();
	//카메라, 광원,기즈모,격자 생성
	//gamePlay3DMap

	
	GamePlayWindowCamera* frustumCamera = GamePlayWindowCamera::Create(gamePlayMap, 1, DirectX::XM_PIDIV2, clientWidth / clientHeight);
	//gamePlay3DMap->SetMainCamera(frustumCamera);
	//frustumCamera->SetEngineObjectFlag(true);
	
	//gamePlayMap->mMapLayerVector[1].mCamera = frustumCamera;
	gamePlayMap->AddEditorMainCamera(frustumCamera, 1);
	frustumCamera->SetEngineObjectFlag(true);
	//gamePlay3DMap->mEngineMainCamera[0] = frustumCamera;



	Gizmo::Create(gamePlayMap, 1);

	//밑에깔리는 Grid
	Grid* grid = Grid::Create(gamePlayMap, 1, 500, 500, 100, 100);
	grid->SetEngineObjectFlag(true);


	//Ui

	OrthogoanlCamera* orthogonalCamera = OrthogoanlCamera::Create(gamePlayMap, 0, clientWidth, clientHeight);


	//gamePlayUiMap->SetMainCamera(orthogonalCamera);
//	gamePlayUiMap->mMapLayerVector[0].mCamera = orthogonalCamera;
//	gamePlayUiMap->mEngineMainCamera[0] = orthogonalCamera;

	gamePlayMap->AddEditorMainCamera(orthogonalCamera, 0);

	orthogonalCamera->SetEngineObjectFlag(true);

//	Gizmo::Create(gamePlayMap, 0);




}

void Quad::ProjectDirector::GenerateProjectDefaultUserObjects()
{


	GamePlaySystem* gamePlaySystem = GamePlaySystem::GetInstance();

	Map* gamePlayMap = gamePlaySystem->GetMap();
//	Map* gamePlayUiMap = gamePlayUiSystem->GetMap();

	float clientWidth = gamePlaySystem->GetClientWidth();
	float clientHeight = gamePlaySystem->GetClientHeight();

	Light* directionalLight = static_cast<Light*>(gamePlayMap->CreateObject("Light"));
	directionalLight->SetEngineObjectFlag(false);

	//런타임에 사용될 기본 유저용 카메라 

	//FrustumCamera* userFrustumCamera = FrustumCamera::Create(gamePlay3DMap, 0, DirectX::XM_PIDIV2, clientWidth / clientHeight);
	DefaultMoveFrustumCamera* userFrustumCamera = DefaultMoveFrustumCamera::Create(gamePlayMap, 1, DirectX::XM_PIDIV2, clientWidth / clientHeight);

	/*GamePlayWindowCamera* userFrustumCamera = GamePlayWindowCamera::Create(gamePlay3DMap, 0, DirectX::XM_PIDIV2, clientWidth / clientHeight);*/


	//userFrustumCamera->SetEngineObjectFlag(false);

	OrthogoanlCamera* userOrthogonalCamera = OrthogoanlCamera::Create(gamePlayMap, 0, clientWidth, clientHeight);

	gamePlayMap->SetMainCamera(userFrustumCamera,1);

	gamePlayMap->SetMainCamera(userOrthogonalCamera,0);


}

bool Quad::ProjectDirector::SaveCurrentMap()
{
	/*auto instance = GetInstance();
	Project* currProject = instance->mCurrProject;

	ProjectMetaData *currProjectMetaData = GetProjectMetaData(currProject->GetName());
	const std::string & currProjectFolderPath =	currProjectMetaData->GetProjectFolderPath();


	GamePlaySystem * gamePlaySystem = 	GamePlaySystem::GetInstance();
	Map * currMap =	gamePlaySystem->GetMap();


	const std::string & currMapName = currMap->GetName();

	std::string currMapPath = currProjectFolderPath + "\\Map\\" + currMapName+".json";

	JsonParser::StartWrite();
	currMap->Seralize();	
	JsonParser::Save(currMapPath);*/

	return true;
}

bool Quad::ProjectDirector::CreateProjectDirectoryHierarchy(const std::string& projectParentPath)
{

	std::wstring projectParentPathW = Utility::ConvertToWString(projectParentPath, true);

	const std::wstring& projectPathW = projectParentPathW + L"\\Project";


	if (!CreateDirectoryW(projectPathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}


	const std::wstring& assetPathW = projectPathW + L"\\Asset";
	if (!CreateDirectoryW(assetPathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}

	const std::wstring& meshPathW = assetPathW + L"\\Mesh";
	if (!CreateDirectoryW(meshPathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}

	const std::wstring effectPathW = assetPathW + L"\\Effect";
	if (!CreateDirectoryW(effectPathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}




	const std::wstring& materialPathW = assetPathW + L"\\Material";
	if (!CreateDirectoryW(materialPathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}


	const std::wstring& texturePathW = assetPathW + L"\\Texture";
	if (!CreateDirectoryW(texturePathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}


	const std::wstring & skeletonPathW = assetPathW + L"\\Skeleton";
	if (!CreateDirectoryW(skeletonPathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}

	const std::wstring& AnimationClipPathW = assetPathW + L"\\AnimationClip";
	if (!CreateDirectoryW(AnimationClipPathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}





	const std::wstring& mapPathW = projectPathW + L"\\Map";
	if (!CreateDirectoryW(mapPathW.c_str(), nullptr))
	{
		DWORD error = GetLastError();
		return false;
	}


	return true;


}

void Quad::ProjectDirector::SetOhterWindowSystemOn()
{
	auto instance = GetInstance();
	instance->mFrameWindow->SetProjectSelectSceneFlag(false);
	instance->mFrameWindowMainSceneDirector.SetNullScene();

	for (auto& windowController : instance->mNormalWindowControllerVector)
	{
		//windowController->mUiSystem->SetActiveState(true);
		windowController->mWorldSystem->SetActiveState(true);
		windowController->mWindowLayoutSystem->SetActiveState(true);
		windowController->mWindowDockingSystem->SetActiveState(true);
		//windowController->SetWindowActiveFlag(true);

	}
	
	instance->mPopupWindowController->mUiSystem->SetActiveState(true);
	//instance->mFrameWindowController->mUiSystem->SetActiveState(false);
	//instance->mFrameWindowController->GetRenderSystem()
	instance->mFrameWindowController->mWorldSystem->SetActiveState(false);
	instance->mFrameWindowController->mFrameWindowDockingSystem->SetActiveState(true);



	FrameWindowMenuDirector::SetMainScene();



}

void Quad::ProjectDirector::SetOhterWindowSystemOff()
{

	auto instance = GetInstance();
	//instance->mFrameWindow->SetProjectSelectSceneFlag(false);
	//instance->mFrameWindowMainSceneDirector.SetNullScene();

	for (auto& windowController : instance->mNormalWindowControllerVector)
	{
	//	windowController->mUiSystem->SetActiveState(false);
		windowController->mWorldSystem->SetActiveState(false);
		windowController->mWindowLayoutSystem->SetActiveState(false);
		windowController->mWindowDockingSystem->SetActiveState(false);
		//windowController->SetWindowActiveFlag(false);

	}

	//instance->mPopupWindowController->mUiSystem->SetActiveState(false);
	//instance->mFrameWindowController->mUiSystem->SetActiveState(true);
	instance->mFrameWindowController->mWorldSystem->SetActiveState(true);
	instance->mFrameWindowController->mFrameWindowDockingSystem->SetActiveState(false);



	//FrameWindowMenuDirector::SetMainScene();








}

void Quad::ProjectDirector::SetOtherWindowVisibleOnOff(bool flag)
{
	auto instance = GetInstance();
	//instance->mFrameWindow->SetProjectSelectSceneFlag(false);
	//instance->mFrameWindowMainSceneDirector.SetNullScene();

	for (auto& windowController : instance->mNormalWindowControllerVector)
	{
		windowController->SetWindowActiveFlag(flag);
	}
}


Quad::EditorMap* Quad::ProjectDirector::CreateDefaultMap(const std::string& mapName)
{
	GamePlaySystem* gamePlaySystem = GamePlaySystem::GetInstance();

	EditorMap* map =static_cast<EditorMap*>(EditorSystem::CreateMap(gamePlaySystem, mapName, false, true));
	DockingWindowController* windowController = static_cast<DockingWindowController*>(gamePlaySystem->GetController());

	float titleBarHeight = windowController->GetTitleBarHeight();

	float clientWidth = gamePlaySystem->GetClientWidth();
	float clientHeight = gamePlaySystem->GetClientHeight();





	//ui map Layer
	SpacePartitioningStructure<UiCollider>* spaitalSpaceUi = SpacePartitioningStructureFactory<UiCollider>::CreateSpacePartitioningStructure("VectorSpace");

	UiCollisionWorld* collisionWorldUi = CollisionWorldFactory::CreateUiCollisionWorld(spaitalSpaceUi);
	map->CreateMapLayer(0, 0, nullptr, { 0,titleBarHeight,clientWidth,clientHeight,0,1.0f });

	map->SetDefaultUiCollisionWorld(collisionWorldUi);



	//3d map Layer


	SpacePartitioningStructure<Collider>* spaitalSpace3D = SpacePartitioningStructureFactory<Collider>::CreateSpacePartitioningStructure("VectorSpace");

	CollisionWorld* collisionWorld3D = CollisionWorldFactory::CreateCollisionWorld(spaitalSpace3D);
	map->CreateMapLayer(1, 0, nullptr, { 0,titleBarHeight,clientWidth,clientHeight,0,1.0f });
	
	map->SetDefault3DCollisionWorld(collisionWorld3D);













	return map;
}

//Quad::EditorMap* Quad::ProjectDirector::CretaeDefaultUiMap(const std::string& mapName)
//{
//	GamePlayUiSystem* gamePlayUiSystem = GamePlayUiSystem::GetInstance();
//
//	DockingWindowController* windowController = static_cast<DockingWindowController*>(gamePlayUiSystem->GetController());
//
//	float titleBarHeight= windowController->GetTitleBarHeight();
//
//	EditorMap* map = EditorSystem::CreateMap(gamePlayUiSystem, mapName, false, true);
//
//	float clientWidth = gamePlayUiSystem->GetClientWidth();
//	float clientHeight = gamePlayUiSystem->GetClientHeight();
//
//	//map->SetViewPort( 0,0,clientWidth,clientHeight,0,1.0f);
//
//	//원근투영 카메라
//
//
////	map->CreateDefaultMapLayer();
//
//
//	//map->SetMainCamera()
//	SpacePartitioningStructure<UiCollider>* spaitalSpace = SpacePartitioningStructureFactory<UiCollider>::CreateSpacePartitioningStructure("VectorSpace");
//
//	//CollisionWorld* collisionWorld = new CollisionWorld(spaitalSpace);
//	UiCollisionWorld* collisionWorld = CollisionWorldFactory::CreateUiCollisionWorld(spaitalSpace);
//	map->CreateMapLayer(0, 0, nullptr, { 0,titleBarHeight,clientWidth,clientHeight,0,1.0f });
//
//	map->SetDefaultUiCollisionWorld(collisionWorld);
//	return map;
//}

void Quad::ProjectDirector::CopyDefaultAssetToUser()
{
	//config file은 에디터 실행파일위치에같이 존재할것이니 config파일이름이면 충분


	const std::string editorAssetFolderPath = "C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset";
	//const std::string editorAssetConfigFilePath = "C:\\Users\\dongd\\gitproject\\GameEngine\\EditorConfig\\EditorAssetConfig.json";
	const std::string editorAssetConfigFilePath = "C:\\Users\\dongd\\gitproject\\GameEngine\\EditorConfig\\Test.json";

	JsonParser* jsonParser = JsonParser::GetInstance();
	jsonParser->ReadFile(editorAssetConfigFilePath);
	jsonParser->ReadStart();
	//texture
	CopyDefaultTextureAssetToUser(jsonParser, editorAssetFolderPath);


	//effect 
	CopyDefaultEffectToUser(jsonParser,editorAssetFolderPath);
	//복사한 userEffect를 로드한다.
	//LoadDefaultUserEffect()
	LoadUserDefaultEffect();

	//material
	CopyDefaultMaterialAssetToUser(jsonParser, editorAssetFolderPath);

	//mesh
	CopyDefaultMeshAssetToUser(jsonParser, editorAssetFolderPath);

	 


	


	









}

void Quad::ProjectDirector::CopyDefaultTextureAssetToUser(JsonParser * parser, const std::string& editorAssetPath)
{
	std::string editorTexturePath = editorAssetPath + "\\Texture";
	std::string beforeDirectory = Utility::SetNewCurrentDirectory(editorTexturePath);


	SetCurrentDirectoryW(Utility::ConvertToWString(editorTexturePath, true).c_str());

	//실제 텍스처들을 유저의 Asset/Texture경로에  복사해야하고
	
	//그 복사본파일의 이름은 그냥 원본이름으로? 

	
	//그후 동일하게 임포트수행


	//아니면 그냥 에디터asset/texture폴더에있는 텍스처들을 임포트수행,고유의 이름은 asset/texture/name 으로 해서 생성

	//그럼유저의 텍스처임포트는 유저의 asset/texture폴더경로에 텍스처를 직접복사하니 문제없다.


	//에디터의 texture asset들의 고유의 이름은 blue.png이렇게되었으니 이것들을 찾아서 실제 파일의 이름 id+_+uniqueName 텍스처파일을 임포트 
	//즉 이것은 외부에있는 파일을 유저가 임포트한거랑 마찬가지 

	auto* fileUiWindowController = FileUiWindowContoller::GetInstance();
	auto* fileUiUISystem = FileUiUiSystem::GetInstance();
	LogicalContentItemUiEntityFolder* folder = fileUiUISystem->GetLogicalContentItemUiEntityFolder(reinterpret_cast<const char*>(u8"텍스처"));
	fileUiUISystem->SetCurrentFolder(folder);

	ImportModule * importModule =ImportModule::GetInstance();



	const rapidjson::Value::ConstArray& textureArray = parser->ReadArray("Texture");
	
	for (auto& element : textureArray)
	{
		//Value::ConstMemberIterator citer =textureObject.FindMember("Name");

		std::string textureUniqueName = element.GetString();




		//textureManager에서 해당 texture 가져오고, id가져오고 file 이름을 만들어서 그 파일을 임포트
		//그리고 이때 uniquename은 Asset/Texture/name 으로 해서 임포트하도록 , 임포트할떄 자동으로 user의 Asset/Texture에 텍스처가 복사될것이다또한


		Texture * texture =	TextureManager::GetTexture(textureUniqueName);
		unsigned long long uniqueID =	texture->GetUniqueID();


		std::stringstream textureFileNameStringStream;
		textureFileNameStringStream << std::hex;
		textureFileNameStringStream << uniqueID << "_" << textureUniqueName;
	//	std::string textureFileName = ss;	//엔진 Texture Asset name 형식 = id + _ + uniqueName

		
		//이텍스처로드

		//근데 단순히 로드에서 끝나는게아니라 fileUiWIndow에도 인식되어야한다.
		//즉 드래그앤드랍한것처럼 인식하면된다.
	//	importModule->ImportFile(textureFileName, "Asset/Texture/", userProjectTextureFolderPath);
	
		fileUiWindowController->DragAndDropFile({ textureFileNameStringStream.str()});

	}


	Utility::SetNewCurrentDirectory(beforeDirectory);







}


void Quad::ProjectDirector::CopyDefaultMaterialAssetToUser(JsonParser* parser, const std::string& editorAssetPath)
{

	std::vector<Asset*> materialVector;
	//default 머터리얼 복사본 생성 ,

	Material * defaultEditorMaterial  = 	MaterialManager::GetMaterial("Default");
	
	const std::string defaultMaterialUniqueName = reinterpret_cast<const char*>(u8"Asset\\머터리얼\\Default");

	Material * defaultMaterial = MaterialManager::CreateMaterial(defaultMaterialUniqueName);
	
	defaultMaterial->Copy(defaultEditorMaterial);

	defaultMaterial->SetDiffuseMap(TextureManager::GetTexture(492));
	defaultMaterial->SetDestLogicalFolderUnqiueID(3);
	defaultMaterial->SetEngineContentItemFlag(false);
	defaultMaterial->SetEffectName("User_Default.effect");
	materialVector.push_back(defaultMaterial);


	//그외 유저에게 제공될 다른 기본머터리얼들을 생성
	








	//파일ui창을통해 시각적으로 보여진다
	auto fileUiUISystem = FileUiUiSystem::GetInstance();
	fileUiUISystem->AddAsset(materialVector);


}

void Quad::ProjectDirector::CopyDefaultMeshAssetToUser(JsonParser* parser,const std::string& editorAssetPath)
{

	std::vector<Asset*> meshVector;


	//Rectangle

	Mesh* rectangleEditorMesh = MeshManager::GetMesh("Rectangle");

	Mesh * rectangleMesh = MeshManager::CreateMesh(reinterpret_cast<const char*>(u8"Asset\\메시\\Rectangle"), Quad::EMeshType::eStaticMesh);

	rectangleMesh->Copy(rectangleEditorMesh);


	std::vector<SubMesh> rectangleSubMeshVector = rectangleMesh->GetSubMeshVector();

	const char* defaultMaterialName = reinterpret_cast<const char*>(u8"Asset\\머터리얼\\Default");
	rectangleSubMeshVector[0].mMaterialPointer = MaterialManager::GetMaterial(defaultMaterialName);
	rectangleSubMeshVector[0].mMesh = rectangleMesh;

	rectangleMesh->SetSubMeshVector(std::move(rectangleSubMeshVector));

	rectangleMesh->SetEngineContentItemFlag(false);
	rectangleMesh->SetDestLogicalFolderUnqiueID(2);

	meshVector.push_back(rectangleMesh);

	//Cube
	Mesh* cubeEditorMesh = MeshManager::GetMesh("Cube");


	Mesh* cubeMesh = MeshManager::CreateMesh(reinterpret_cast<const char*>(u8"Asset\\메시\\Cube"), Quad::EMeshType::eStaticMesh);

	cubeMesh->Copy(cubeEditorMesh);

	std::vector<SubMesh> cubeSubMeshVector = cubeMesh->GetSubMeshVector();

	cubeSubMeshVector[0].mMaterialPointer = MaterialManager::GetMaterial(defaultMaterialName);
	cubeSubMeshVector[0].mMesh = cubeEditorMesh;

	cubeMesh->SetSubMeshVector(std::move(cubeSubMeshVector));

	cubeMesh->SetEngineContentItemFlag(false);
	cubeMesh->SetDestLogicalFolderUnqiueID(2);
	

	meshVector.push_back(cubeMesh);



	//파일ui창을통해 시각적으로 보여진다
	auto fileUiUISystem = FileUiUiSystem::GetInstance();
	fileUiUISystem->AddAsset(meshVector);

}

void Quad::ProjectDirector::CopyDefaultEffectToUser(JsonParser * parser ,const std::string& editorAssetPath)
{

	ProjectMetaData * currProjectMetaData =	GetCurrentProjectMetaData();
	const std::string projectPath =	currProjectMetaData->GetProjectFolderPath();


	const std::string editorEffectFolderPath = editorAssetPath + "\\Effect";
	const std::string userEffectFolderPath = projectPath + "\\Asset\\Effect";
	const std::wstring userEffectFolderPathW = Utility::ConvertToWString(userEffectFolderPath, true);



	std::string beforeDirectoryPath = Utility::SetNewCurrentDirectory(editorEffectFolderPath);

	const rapidjson::Value::ConstArray &  effectArray =	parser->ReadArray("Effect");

	for (auto& element : effectArray)
	{
		 const char * effectName = element.GetString();
		 std::wstring effectNameW = Utility::ConvertToWString(effectName, true);
		 std::wstring effectUserFilePathW = userEffectFolderPathW + L"\\User_" + effectNameW;
		 CopyFile(effectNameW.c_str(), effectUserFilePathW.c_str(), true);
	}

	const rapidjson::Value::ConstArray& hlslArray = parser->ReadArray("HLSL");
	for (auto& element : hlslArray)
	{
		const char* hlslName = element.GetString();
		std::wstring hlslNameW = Utility::ConvertToWString(hlslName, true);
		std::wstring hlslUserFilePathW = userEffectFolderPathW +L"\\"+hlslNameW;
		CopyFile(hlslNameW.c_str(), hlslUserFilePathW.c_str(), true);
	}





	Utility::SetNewCurrentDirectory(beforeDirectoryPath);





	








}

void Quad::ProjectDirector::LoadUserDefaultEffect()
{

	auto resourceController =	ResourceController::GetInstance();



	auto gamePlayWindowController = GamePlayWindowController::GetInstance();
//	RenderSystem* renderSystem = gamePlayWindowController->GetRenderSystem();

	ProjectMetaData* currProjectMetaData = GetCurrentProjectMetaData();
	const std::string projectFolderPath = currProjectMetaData->GetProjectFolderPath();

	const std::string effectFolderPath = projectFolderPath + "\\Asset\\Effect";

	//const std::string beforeDirectoryPath =	Utility::SetNewCurrentDirectory(effectFolderPath);

	resourceController->LoadEffect(effectFolderPath, gamePlayWindowController);

	//auto * resourceLoader =	ResourceLoader::GetInstance();

	//WIN32_FIND_DATA fileData;
	//HANDLE handle = FindFirstFile(L"*.effect" ,&fileData);
	//

	//
	//if (handle != INVALID_HANDLE_VALUE)
	//{

	//	do
	//	{
	//		const std::string fileName = Utility::ConvertToString(fileData.cFileName, true);
	//		resourceLoader->LoadEffect(fileName);
	//		renderSystem->AddEffect(*EffectManager::GetEffect(fileName), Quad::ESystemType::eMainSystem);

	//	} while (FindNextFile(handle, &fileData));
	//}



	////





	//Utility::SetNewCurrentDirectory(beforeDirectoryPath);









}
