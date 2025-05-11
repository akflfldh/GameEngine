#include "Project.h"
#include "Project.h"
#include"MapMetaData.h"
#include"Parser/JsonParser.h"
#include"ProjectDirector.h"

#include"System/GamePlaySystem.h"
#include"System/GamePlayUiSystem.h"


#include"EditorSystem.h"
#include"ObjectManager/EditGameObjectManager.h"


#include"GamePlayWindowCamera.h"
#include"ResourceController.h"


#include"EditorMap.h"
#include"System/AttributeUiSystem.h"
#include"System/FileUiUiSystem.h"

#include"GamePlayWindowController.h"

#include"AnimationStateTransitionGraphBasePanel.h"


Quad::Project::Project()
	:mCurrent3DMapIndex(0), mCurrentUiMapIndex(0)
{

}

Quad::Project::~Project()
{

}



void Quad::Project::SaveProject()
{
	ProjectMetaData* projectMetaData = ProjectDirector::GetProjectMetaData(GetName());
	const std::string & projectFolderPath = projectMetaData->GetProjectFolderPath();


	//Asset save


	//map metaData save
	const std::string  mapMetaDataFileName = "mapMetaDataFile.json";
	const std::string mapMetaDataFilePath = projectFolderPath + "\\" + mapMetaDataFileName;

	JsonParser::StartWrite();

	size_t project_SystemMapMetaDataNum = mSystemMapMetaDataVector.size();
	JsonParser::StartWriteObject();
	JsonParser::Write("Project_SystemMapMetaDataNum", project_SystemMapMetaDataNum);

	for (auto& mapMetaData : mSystemMapMetaDataVector)
	{
		JsonParser::StartWriteObject();
		mapMetaData->Serialize();
	}


	/*size_t project_UiSystemMapMetaDataNum = mUiSystemMapMetaDataVector.size();
	JsonParser::StartWriteObject();
	JsonParser::Write("Project_UiSystemMapMetaDataNum", project_UiSystemMapMetaDataNum);

	for (auto& mapMetaData : mUiSystemMapMetaDataVector)
	{
		JsonParser::StartWriteObject();
		mapMetaData->Serialize();
	}*/

	JsonParser::Save(mapMetaDataFilePath);









	//map save
	SaveCurrentMap();








	SaveMapCommonData();

	



}

void Quad::Project::LoadProject()
{


	ProjectMetaData* projectMetaData = ProjectDirector::GetProjectMetaData(GetName());
	const std::string& projectFolderPath = projectMetaData->GetProjectFolderPath();



	//userCode dll load
	const std::string userCodeDllPath = projectFolderPath + "\\x64\\Debug\\UserCode.dll";
	HANDLE userCodeDllHandle = LoadLibraryA(userCodeDllPath.c_str());

	//이제는 attribute uiSystem에게 엔티티클래스 리스트를 등록
	//근덴여기서하면 Project클래스가 또 에디터의 것과 연관된다는단점 일단 해보고 성공하는것을 보고 수정 
	auto * attributeUiSystem = 	AttributeUiSystem::GetInstance();

	for (const auto& objectClassName : ObjectTable::GetObjectTable())
	{


		
		attributeUiSystem->AddClassListItem(objectClassName);
	}


	//Asset load




	//map metaData load
	const std::string  mapMetaDataFileName = "mapMetaDataFile.json";
	const std::string mapMetaDataFilePath = projectFolderPath + "\\" + mapMetaDataFileName;

	JsonParser::ReadFile(mapMetaDataFilePath);


	JsonParser::ReadStart();


	size_t currentReadObjectIndex = 0;

	size_t project_3DSystemMapMetaDataNum = 0;
	JsonParser::Read("Project_SystemMapMetaDataNum", project_3DSystemMapMetaDataNum);

	mSystemMapMetaDataVector.resize(project_3DSystemMapMetaDataNum,nullptr);

	currentReadObjectIndex++;
	for (int i = 0; i < project_3DSystemMapMetaDataNum; ++i)
	{
		JsonParser::SetCurrentIndex(currentReadObjectIndex);

		mSystemMapMetaDataVector[i] = new MapMetaData;
		mSystemMapMetaDataVector[i]->DeSerialize();

		currentReadObjectIndex++;
	}

	JsonParser::SetCurrentIndex(currentReadObjectIndex);

	//size_t project_UiSystemMapMetaDataNum = 0;

	//JsonParser::Read("Project_UiSystemMapMetaDataNum", project_UiSystemMapMetaDataNum);
	//
	//mUiSystemMapMetaDataVector.resize(project_UiSystemMapMetaDataNum, nullptr);

	//currentReadObjectIndex++;
	//for (int i = 0; i < project_UiSystemMapMetaDataNum; ++i)
	//{
	//	JsonParser::SetCurrentIndex(currentReadObjectIndex);
	//	mUiSystemMapMetaDataVector[i] = new MapMetaData;
	//	mUiSystemMapMetaDataVector[i]->DeSerialize();
	//	currentReadObjectIndex++;
	//}





	//map load

	//기본 default map을 로드하자 지금은일단 항상 0번 map

	//gamePlaySystem 에 map파일의 경로를 알려줘서 로드하고 역직렬화하라고하는거다.
	const std::string defaultMapName = mSystemMapMetaDataVector[0]->GetMapName();

	const std::string gamePlayDefaultMapPath = projectFolderPath + "\\Map\\" + defaultMapName + ".json";
	GamePlaySystem * gamePlaySystem = GamePlaySystem::GetInstance();
	EditorMap* gamePlayEditorModeMap = static_cast<EditorMap*>(EditorSystem::CreateMap(gamePlaySystem, defaultMapName,false,true));
	

	float clientWidth = gamePlaySystem->GetClientWidth();
	float clientHeight = gamePlaySystem->GetClientHeight();


//	const std::string gamePlayUiDefaultMapPath = projectFolderPath + "\\Map\\" + mUiSystemMapMetaDataVector[0]->GetMapName() + ".json";
//	GamePlayUiSystem* gamePlayUiSystem = GamePlayUiSystem::GetInstance();
	//Map* gamePlayUiEditorModeMap = gamePlayUiSystem->GetMap();
//	EditorMap* gamePlayUiEditorModeMap = EditorSystem::CreateMap(gamePlayUiSystem, mUiSystemMapMetaDataVector[0]->GetMapName(), false, true);



	gamePlaySystem->SetMap(gamePlayEditorModeMap);
	//gamePlayUiSystem->SetMap(gamePlayUiEditorModeMap);


	LoadMapCommonData();


	JsonParser::ReadFile(gamePlayDefaultMapPath);
	gamePlayEditorModeMap->DeSerialize();

	/*JsonParser::ReadFile(gamePlayUiDefaultMapPath);
	gamePlayUiEditorModeMap->DeSerialize();*/

	ProjectDirector::InitProjectEditorMode();
	//editor용 카메라가  DeSerialize후에 만들어짐으로 여기서 수행한다.
	gamePlayEditorModeMap->SetMainEngineCamera();
	//gamePlayUiEditorModeMap->SetMainEngineCamera();

	LoadEditorCurrentMapData(projectFolderPath, defaultMapName);



}

void Quad::Project::Serialize()
{







}

void Quad::Project::DeSerialize()
{





}

void Quad::Project::SetName(const std::string& name)
{
	mName = name;
}

const std::string& Quad::Project::GetName() const
{
	return mName;
	// TODO: 여기에 return 문을 삽입합니다.
}

bool Quad::Project::SaveCurrentMap()
{
	auto gamePlaySystem = 	GamePlaySystem::GetInstance();
	Map * currMap=gamePlaySystem->GetMap();

	Project* currProject = ProjectDirector::GetCurrentProject();
	ProjectMetaData* currProjectMetaData = ProjectDirector::GetProjectMetaData(currProject->GetName());
	const std::string& currProjectFolderPath = currProjectMetaData->GetProjectFolderPath();


	const std::string& currMapName = currMap->GetName();

	std::string currMapPath = currProjectFolderPath + "\\Map\\" + currMapName + ".json";

	JsonParser::StartWrite();
	currMap->Serialize();
	JsonParser::Save(currMapPath);


	SaveMapCommonData();
	SaveEditorCurrentMapData(currProjectFolderPath, currMapName);



	return true;
}

void Quad::Project::SaveMapCommonData()
{

	//맵들이 공통적으로 사용하는 데이터를 저장한다.


	ProjectMetaData* projectMetaData = ProjectDirector::GetProjectMetaData(GetName());
	const std::string& projectPath = projectMetaData->GetProjectFolderPath();


	//Asset
	const std::string& assetPath = projectPath + "\\Asset";
	ResourceController *  resourceController =	ResourceController::GetInstance();
	resourceController->SaveUserAsset(assetPath);

		
	//ObjectAvailableNextUniqueID

	
	const std::string projectDataFilePath = projectPath + "\\ProjectData.json";

	EditGameObjectManager* objectManager = EditGameObjectManager::GetInstance();

	unsigned long long nextAvailableObjectUniqueID = objectManager->PeekObjectUniqueID();

	JsonParser::StartWrite();
	JsonParser::StartWriteObject();
	JsonParser::Write("NextAvailableObjectUniqueID", nextAvailableObjectUniqueID);

	JsonParser::Save(projectDataFilePath);






}

void Quad::Project::SaveEditorCurrentMapData(const std::string & projectFolderPath ,const std::string & currMapName)
{

	JsonParser::StartWrite();

	JsonParser::StartWriteObject();
	//animation Graph save
	AnimationStateTransitionGraphBasePanel* animationStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();

	animationStateTransitionGraphBasePanel->Serialize();


	std::string currEditMapDataPath = projectFolderPath + "\\Map\\" + currMapName + ".edit";
	JsonParser::Save(currEditMapDataPath);




}

void Quad::Project::LoadEditorCurrentMapData(const std::string& projectFolderPath, const std::string& currMapName)
{

	const std::string currEditMapDataPath  = projectFolderPath + "\\Map\\" + currMapName + ".edit";

	JsonParser::ReadFile(currEditMapDataPath);
	JsonParser::ReadStart();


	AnimationStateTransitionGraphBasePanel* animationStateTransitionGraphBasePanel = AnimationStateTransitionGraphBasePanel::GetInstance();

	animationStateTransitionGraphBasePanel->DeSerialize();






}

void Quad::Project::LoadMapCommonData()
{
	ProjectMetaData* projectMetaData = ProjectDirector::GetProjectMetaData(GetName());
	const std::string& projectPath = projectMetaData->GetProjectFolderPath();


	//Asset
	const std::string& assetPath = projectPath + "\\Asset";
	ResourceController* resourceController = ResourceController::GetInstance();
	//리턴된 asset들을 fileUiUI창에게 전달하여 시각적으로 보여지도록해야한다.

	resourceController->LoadEffect(assetPath + "\\Effect", GamePlayWindowController::GetInstance());
	std::vector<Asset*> assetVector = resourceController->LoadUserAsset(assetPath);
	
	FileUiUiSystem * fileUiUISystem = FileUiUiSystem::GetInstance();
	fileUiUISystem->AddAsset(assetVector);
	




	//ObjectAvailableNextUniqueID
	const std::string projectDataFilePath = projectPath + "\\ProjectData.json";

	JsonParser::ReadFile(projectDataFilePath);

	JsonParser::ReadStart();
	unsigned long long nextAvailableUniqueID = 0;
	JsonParser::Read("NextAvailableObjectUniqueID", nextAvailableUniqueID);
	EditGameObjectManager* objectManager = EditGameObjectManager::GetInstance();
	objectManager->SetNextAvailableObjectUniqueID(nextAvailableUniqueID);





}

Quad::MapMetaData* Quad::Project::GetMapMetaData(const std::string& mapName) const
{

	

		for (auto& mapMetaData : mSystemMapMetaDataVector)
		{
			if (mapMetaData->GetMapName() == mapName)
				return mapMetaData;
		}
	
	
	return nullptr;
}

void Quad::Project::CreateMapMetaData(const std::string& mapPath, const std::string& mapName)
{

	MapMetaData* mapMetaData = new MapMetaData;
	mapMetaData->SetMapName(mapName);
	//mapMetaData->SetMapPath(mapPath);



	
	mSystemMapMetaDataVector.push_back(mapMetaData);
	


}

Quad::Project* Quad::Project::CreateDefaultProject()
{
	

	Project* project = new Project;
	

	return project;
}

