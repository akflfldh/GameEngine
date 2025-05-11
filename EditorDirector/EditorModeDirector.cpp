#include "EditorDirector/EditorModeDirector.h"

#include"Parser/JsonParser.h"
#include"Core/ResourceController.h"
#include"Core/ResourceLoader.h"

#include"ResourceManager/TextureManager/TextureManager.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"Utility/UniqueIDTable.h"


#include"EditorDirector/ButtonUiEntitiy.h"
#include"Core/WindowResizeEvent.h"

#include"System/WindowChromeSystem.h"
#include"EditorDirector/EditorDirector.h"


#include"EditorDirector/GamePlayWindowController.h"

#include"Asset/Mesh/Mesh.h"


std::vector<Quad::Asset*> Quad::EditorModeDirector::mAssetVector;

void Quad::EditorModeDirector::SetGamePlayWindowChromeSystem(WindowChromeSystem* gamePlayWindowChromeSystem)
{
	mGamePlayWindowChromeSystem = gamePlayWindowChromeSystem;

}

std::vector<Quad::Asset*> & Quad::EditorModeDirector::LoadAsset(const std::string& assetPath)
{

	
	

	LoadEffect(assetPath);

	LoadTexture(assetPath);


	LoadMaterial(assetPath);


	LoadMesh(assetPath);





	return mAssetVector;



}

//void Quad::EditorModeDirector::SendAssetToFileSystem()
//{
//	ResourceController* resourceController = ResourceController::GetInstance();
//	resourceController->SendLoadFileResponseEvent(mAssetVector);
//}

void Quad::EditorModeDirector::InitGameWindowLayoutSystem()
{
	auto instance = GetInstance();

	EditorDirector* editorDirector = EditorDirector::GetInstance();
	WindowChromeSystem* gamePlayWindowChromeSystem = instance->mGamePlayWindowChromeSystem;

	Map* currMap = gamePlayWindowChromeSystem->GetMap();

	// system->AddEntity(windowCloseButton);

   //   ButtonUiEntitiy* gamePlayButton = new ButtonUiEntitiy("GamePlayButton");
	 // ButtonUiEntity* gamePlayButton = static_cast<ButtonUiEntity*>(currMap->CreateObject("ButtonUiEntity"));
	ButtonUiEntity* gamePlayButton = ButtonUiEntity::Create(currMap, 0);
	std::pair<float, float> buttonSize = { 35,35 };
	// gamePlayButton->SetSystem(system);
   //  gamePlayButton->Initialize();
	gamePlayButton->SetSize(buttonSize.first, buttonSize.second);
	gamePlayButton->SetTexture("gamePlayButtonOff.png");
	gamePlayButton->SetPosition(100, 3000 - 20, 5);
	gamePlayButton->RegisterAcceptEvent("WindowResizeEvent");
	gamePlayButton->RegisterAcceptEvent("LButtonDown");
	gamePlayButton->RegisterAcceptEvent("LButtonUp");
	gamePlayButton->RegisterEventCallback("WindowResizeEvent", [button = gamePlayButton](Event* pEvent) {

		WindowResizeEvent* resizeEvent = (WindowResizeEvent*)pEvent;

		float windowWidth = resizeEvent->GetWindowWidth();
		DirectX::XMFLOAT3 pos = button->GetTransform().GetPositionLocal();
		pos.x = windowWidth / 2;
		button->GetTransform().SetPositionLocal(pos);
		});



	gamePlayButton->SetHoverCallback([button = gamePlayButton]() {

		button->SetColorItensity({ 1.3f,1.3f,1.3f });

		});

	gamePlayButton->SetReleaseHoverCallback([button = gamePlayButton]() {

		button->SetColorItensity({ 1.0f,1.0f,1.0f });
		});
	gamePlayButton->SetLButtonDownCallback([button = gamePlayButton]() {

		// button->SetColorItensity({ 0.0f,0.0f,1.0f });


		});


	gamePlayButton->SetLButtonUpCallback([button = gamePlayButton, editorDirector = editorDirector, gamePlayWindowController =
		GamePlayWindowController::GetInstance()]() {


			//play mode시작.
			bool prePlayMode = editorDirector->GetPlayModeState();


			if (prePlayMode == false)
			{
				editorDirector->SetPlayModeState(true);
				gamePlayWindowController->SetPause(false);
				button->SetTexture("gamePlayButtonOn.png");
			}
			else
			{
				
				gamePlayWindowController->SetPause(true);
				button->SetTexture("gamePlayButtonOff.png");

				/*if (gamePlayWindowController->GetPause())
				{
					gamePlayWindowController->SetPause(false);
					button->SetTexture("gamePlayButtonOn.png");
				}
				else
				{
					gamePlayWindowController->SetPause(true);
					button->SetTexture("gamePlayButtonOff.png");
				}*/
			}

			//esc 는 play mode 종료

		});


	//  ButtonUiEntitiy * gamePlayEndButton = new ButtonUiEntitiy("GamePlayEndButton");
	 // ButtonUiEntity * gamePlayEndButton = static_cast<ButtonUiEntity*>(currMap->CreateObject("ButtonUiEntity"));
	ButtonUiEntity* gamePlayEndButton = ButtonUiEntity::Create(currMap, 0);
	// gamePlayEndButton->SetSystem(system);
   //  gamePlayEndButton->Initialize();
	gamePlayEndButton->GetTransform().SetIndependentRotationFlag(true);
	gamePlayEndButton->GetTransform().SetIndependentScaleFlag(true);
	gamePlayEndButton->SetSize(buttonSize.first, buttonSize.second);
	gamePlayEndButton->SetTexture("gamePlayEndButton.png");

	DirectX::XMFLOAT3 pos = gamePlayButton->GetTransform().GetPositionWorld();
	pos.x += gamePlayButton->GetWidth() / 2 + 5 + gamePlayEndButton->GetWidth() / 2;
	//  system->AddEntity(gamePlayEndButton);
	gamePlayEndButton->GetTransform().SetPositionWorld(pos);

	gamePlayEndButton->RegisterAcceptEvent("WindowResizeEvent");
	gamePlayEndButton->RegisterEventCallback("WindowResizeEvent", [button = gamePlayEndButton](Event* pEvent) {

		WindowResizeEvent* resizeEvent = (WindowResizeEvent*)pEvent;


		float windowWidth = resizeEvent->GetWindowWidth();
		DirectX::XMFLOAT3 pos = button->GetTransform().GetPositionLocal();
		pos.x = windowWidth / 2 + button->GetWidth() + 5;
		button->GetTransform().SetPositionLocal(pos);

		});


	gamePlayEndButton->SetHoverCallback([button = gamePlayEndButton]() {

		button->SetColorItensity({ 1.3f,1.3f,1.3f });
		});
	gamePlayEndButton->SetReleaseHoverCallback([button = gamePlayEndButton] {

		button->SetColorItensity({ 1.0f,1.0f,1.0f });

		});


	gamePlayEndButton->RegisterAcceptEvent("LButtonDown");
	gamePlayEndButton->SetLButtonDownCallback([gamePlayEndButton = gamePlayEndButton, playOnButton = gamePlayButton, editorDirector = editorDirector, gamePlayWindowController =
		GamePlayWindowController::GetInstance()]() {

		//리셋         - playmode 가 false가되고 ,다 초기상태로

			if (editorDirector->GetPlayModeState())
			{
				editorDirector->SetPlayModeState(false);
				gamePlayWindowController->SetPause(false);

				playOnButton->SetTexture("gamePlayButtonOff.png");

			}


		});




















}

void Quad::EditorModeDirector::LoadEffect(const std::string& assetPath)
{
	ResourceController* resourceController = ResourceController::GetInstance();

	
	const std::string effectFolderPath = assetPath + "\\Effect";
	//const std::string preDirectoryPath = Utility::SetNewCurrentDirectory(effectFolderPath);
	
	resourceController->LoadEffect(effectFolderPath);


}

void Quad::EditorModeDirector::LoadMesh(const std::string& assetPath)
{
	ResourceController* resourceController = ResourceController::GetInstance();
	ResourceLoader* resourceLoader = resourceController->mResourceLoader;


	//metaData
	unsigned long long nextAvailableUniqueID = 0;
	const std::string meshMetaDataFilePath = assetPath + "\\MeshMetaData.json";
	JsonParser::ReadFile(meshMetaDataFilePath);
	JsonParser::ReadStart();

	JsonParser::Read("NextAvailableUniqueID", nextAvailableUniqueID);

	MeshManager* meshManager = MeshManager::GetInstance();
	meshManager->mIDTable.SetNextAvailalbeUniqueID(nextAvailableUniqueID);



	//rawData
	const std::string meshFolderPath = assetPath + "\\Mesh";
	

	char currentDirectory[1000];
	GetCurrentDirectoryA(1000, currentDirectory);
	SetCurrentDirectoryA(meshFolderPath.c_str());

	WIN32_FIND_DATAW fileData;

	HANDLE handle = FindFirstFileW(L"*.mesh", &fileData);

	if (handle != INVALID_HANDLE_VALUE)
	{

		do 
		{

			const std::string fileName = Utility::ConvertToString(fileData.cFileName, true);
			Mesh* mesh = resourceLoader->LoadMeshFile(fileName);
			if (mesh != nullptr)
			{
				mesh->SetDestLogicalFolderUnqiueID(2);
				mesh->SetEngineContentItemFlag(true);
				mAssetVector.push_back(mesh);
			}

		} while (FindNextFileW(handle, &fileData));

	}




	SetCurrentDirectoryA(currentDirectory);



}

void Quad::EditorModeDirector::LoadMaterial(const std::string& assetPath)
{


	ResourceController* resourceController = ResourceController::GetInstance();
	ResourceLoader* resourceLoader = resourceController->mResourceLoader;

	//metaData

	unsigned long long nextAvailableUniqueID = 0;
	const std::string materialMetaDataFilePath = assetPath + "\\MaterialMetaData.json";

	JsonParser::ReadFile(materialMetaDataFilePath);
	JsonParser::ReadStart();


	JsonParser::Read("NextAvailableUniqueID", nextAvailableUniqueID);

	MaterialManager* materialManager = MaterialManager::GetInstance();
	materialManager->mIDTable.SetNextAvailalbeUniqueID(nextAvailableUniqueID);


//Raw Data


	const std::string materialFolderPath = assetPath + "\\Material";

	char currentDirectory[1000];
	GetCurrentDirectoryA(1000, currentDirectory);
	SetCurrentDirectoryA(materialFolderPath.c_str());




	WIN32_FIND_DATAW fileData;
	HANDLE handle = FindFirstFileW(L"*.json", &fileData);

	if (handle != INVALID_HANDLE_VALUE)
	{

		do {
			const std::string fileName = Utility::ConvertToString(fileData.cFileName,true);
		

			Material * material = 	resourceLoader->LoadMaterial(fileName);
			if (material != nullptr)
			{
				material->SetDestLogicalFolderUnqiueID(3);
				material->SetEngineContentItemFlag(true);
				mAssetVector.push_back(material);
			}
		} while (FindNextFileW(handle, &fileData));

	}
	SetCurrentDirectoryA(currentDirectory);




}

void Quad::EditorModeDirector::LoadTexture(const std::string& assetPath)
{


	ResourceController* resourceController = ResourceController::GetInstance();
	ResourceLoader * resourceLoader =  resourceController->mResourceLoader;

	wchar_t currentDirectoryBuffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDirectoryBuffer);


	const std::string textureMetaDataFilePath = assetPath + "\\TextureMetaData.json";

	JsonParser::ReadFile(textureMetaDataFilePath);

	JsonParser::ReadStart();


	unsigned long long readObjectIndex = 0;
	unsigned long long nextTextureAvailableUniqueID = 0;
	JsonParser::Read("NextAvailableUniqueID", nextTextureAvailableUniqueID);

	TextureManager* textureManager = TextureManager::GetInstance();

	textureManager->mIDTable.SetNextAvailalbeUniqueID(nextTextureAvailableUniqueID);


	unsigned long long textureNum = 0;
	JsonParser::SetCurrentIndex(++readObjectIndex);
	JsonParser::Read("TextureNum", textureNum);
	

	unsigned long long assetID = 0;


	const std::wstring textureFolderPath = Utility::ConvertToWString((assetPath + "\\Texture").c_str(), true);
	int ret = SetCurrentDirectory(textureFolderPath.c_str());


	for (unsigned long long i = 0; i < textureNum; ++i)
	{
		if (i == 56)
		{
			int a = 2;
		}

		JsonParser::SetCurrentIndex(++readObjectIndex);

		
		JsonParser::Read("AssetUniqueID", assetID);

		if (assetID == 426)
		{
			int a = 2;
		}
		std::string textureFileName;
		std::string assetUniqueName;
		unsigned int logicalFolderID = 0;
		JsonParser::Read("FileName", textureFileName);
		JsonParser::Read("AssetUniqueName", assetUniqueName);
		JsonParser::Read("LogicalFolderUniqueID", logicalFolderID);
		Texture * texture =resourceLoader->LoadTextureFromFile(textureFileName, assetUniqueName, assetID);


		if (texture != nullptr)
		{
			texture->SetDestLogicalFolderUnqiueID(logicalFolderID);
			texture->SetEngineContentItemFlag(true);
			mAssetVector.push_back(texture);
		}

		OutputDebugString(std::to_wstring(i).c_str());
		OutputDebugString(L"\n");
	}




	SetCurrentDirectory(currentDirectoryBuffer);


}
