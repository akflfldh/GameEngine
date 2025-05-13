#include "BuildModule.h"
#include<Windows.h>
#include"Utility/Utility.h"
#include"ResourceController.h"
#include"Parser/JsonParser.h"

#include"ResourceManager/TextureManager/TextureManager.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"ProjectDirector.h"

#include"EditorDirector/EditorDirector.h"


bool CreateBuildFolderHierarchy(const std::string& outputBuildPath,const std::string & programName)
{
	const std::wstring outputBuildPathW = Quad::Utility::ConvertToWString(outputBuildPath, true);
	const std::wstring programPathW = outputBuildPathW + L"\\" + Quad::Utility::ConvertToWString(programName, true);
	const std::wstring assetPathW = programPathW + L"\\Asset";
	const std::wstring mapPathW = programPathW + L"\\Map";

//	const std::wstring meshPathW = assetPathW + L"\\Mesh";
//	const std::wstring materialPathW = assetPathW + L"\\Material";
//	const std::wstring texturePathW = assetPathW + L"\\Texture";
	const std::wstring effectPathW = assetPathW + L"\\Effect";


	if (!CreateDirectory(programPathW.c_str(), nullptr))
	{
		//실패 
		return false;
	}


	if (!CreateDirectory(assetPathW.c_str(), nullptr))
	{
		//실패 
		return false;
	}

	
	if (!CreateDirectory(mapPathW.c_str(), nullptr))
	{
		//실패
		return false;
	}
	
	//if (!CreateDirectory(meshPathW.c_str(), nullptr))
	//{
	//	//실패
	//	return false;
	//}

	//if (!CreateDirectory(materialPathW.c_str(), nullptr))
	//{
	//	//실패
	//	return false;
	//}

	//if (!CreateDirectory(texturePathW.c_str(), nullptr))
	//{
	//	//실패
	//	return false;
	//}

	if (!CreateDirectory(effectPathW.c_str(), nullptr))
	{
		//실패
		return false;
	}



}

Quad::BuildModule::BuildModule()
{
}

Quad::BuildModule::~BuildModule()
{
}

bool Quad::BuildModule::Build(const std::string& outputBuildPath,const std::string & programName)
{

	//최종 asset 파일 생성 
	if (!CreateBuildFolderHierarchy(outputBuildPath, programName))
	{
		//
		return false;
	}

	const std::string programFolderPath = outputBuildPath + "\\" + programName;

	ProjectMetaData* currProjectMetaData = ProjectDirector::GetCurrentProjectMetaData();
	const std::string currProjectFolderPathW = currProjectMetaData->GetProjectFolderPath();



	GenerateFinalAssetFile(programFolderPath, currProjectFolderPathW);


	//유저의 프로젝트폴더에있는 mapMetaData와 map들도 복사 
	//ProjectData.json도 복사
	//UserCode.dll도 복사
	//Core.dll도 복사
	//Utility.dll도 복사	
	CopyProjectFiles(programFolderPath, programName);




	return true;
}

bool Quad::BuildModule::GenerateFinalAssetFile(const std::string & outputPath,const std::string & currProjectPath)
{



	//일단 현재 모든 asset들을 저장한다.(즉 사용하지않은 asset들도 포함된다)
	//config파일을 읽어서 editor의 기본asset들중 user에게 드러난 asset들의 목록을 읽어
	//그 asset들의 engineFlag를 false로 설정한다.(이러면 saveUserAsset시 userAsset으로 인식되어 저장된다)
	EditorDirector* editorDirector = EditorDirector::GetInstance();

	const std::string editorFolderPath = editorDirector->GetEditorPathA();
	const std::string editorAssetFolderPath = editorFolderPath + "\\Asset";
	const std::string userAssetFolderPath = currProjectPath + "\\Asset";


	
	JsonParser::ReadFile(editorFolderPath+"\\EditorConfig\\EditorAssetVisibleConfig.json");
	JsonParser::ReadStart();
	const rapidjson::Value::ConstArray & textureArray =	JsonParser::ReadArray("Texture");
	std::vector<Asset*> editorDefaultAssetVector;
	for (auto& element : textureArray)
	{
		const std::string textureName =	 element.GetString();
		
		Asset * asset =	TextureManager::GetTexture(textureName);
		if (asset != nullptr)
		{
			asset->SetEngineContentItemFlag(false);
			editorDefaultAssetVector.push_back(asset);
		}

	}
	const rapidjson::Value::ConstArray& matArray = JsonParser::ReadArray("Material");

	for (auto& element : matArray)
	{
		const std::string matName = element.GetString();

		Asset* asset = MaterialManager::GetMaterial(matName);
		if (asset != nullptr)
		{
			asset->SetEngineContentItemFlag(false);
			editorDefaultAssetVector.push_back(asset);
		}
	}

	const rapidjson::Value::ConstArray& meshArray = JsonParser::ReadArray("Mesh");

	for (auto& element : meshArray)
	{
		const std::string meshName = element.GetString();

		Asset* asset = MeshManager::GetMesh(meshName);
		if (asset != nullptr)
		{
			asset->SetEngineContentItemFlag(false);
			editorDefaultAssetVector.push_back(asset);
		}
	}










	////user effect와 editor defualt effect들은 복사 

	//editor effect path 
	std::string beforeDirectoryPath = Utility::SetNewCurrentDirectory(editorAssetFolderPath+"\\Effect");

	const rapidjson::Value::ConstArray& effectArray = JsonParser::ReadArray("Effect");
	const rapidjson::Value::ConstArray& hlslArray = JsonParser::ReadArray("HLSL");

	const std::string outputEffectPath = outputPath+ + "\\Asset\\Effect";
	const std::wstring outputEffectPathW = Utility::ConvertToWString(outputEffectPath, true);

	for (auto& element : effectArray)
	{
		//std::string effectFileName =element.GetString();
		std::wstring effectFileNameW = Utility::ConvertToWString(element.GetString(), true);
		CopyFile(effectFileNameW.c_str(), (outputEffectPathW + +L"\\" + effectFileNameW).c_str(), false);
	}

	for (auto& element : hlslArray)
	{
		//std::string effectFileName =element.GetString();
		std::wstring hlslFileNameW = Utility::ConvertToWString(element.GetString(), true);
		CopyFile(hlslFileNameW.c_str(), (outputEffectPathW + +L"\\" + hlslFileNameW).c_str(), false);
	}

	//user effect Path

	Utility::SetNewCurrentDirectory(beforeDirectoryPath);




	auto resourceController = ResourceController::GetInstance();
	
	//resourceController->SaveUserAsset(outputAssetPath);

	resourceController->SaveAssetPackage(editorAssetFolderPath, userAssetFolderPath, outputPath);


	//editor default asset들의 engine flag를 복구
	for (auto& element : editorDefaultAssetVector)
	{
		element->SetEngineContentItemFlag(true);
	}







	ProjectMetaData* currProjectMetaData = ProjectDirector::GetCurrentProjectMetaData();
	const std::wstring currProjectFolderPathW = Utility::ConvertToWString(currProjectMetaData->GetProjectFolderPath().c_str(), true);

	beforeDirectoryPath = Utility::SetNewCurrentDirectory(currProjectMetaData->GetProjectFolderPath() + "\\Asset\\Effect");


	std::vector<std::wstring> extensionVector = { L"*.effect",L"*.hlsl" };



	for (int i = 0; i < extensionVector.size(); ++i)
	{
		WIN32_FIND_DATA fileData;
		HANDLE handle = FindFirstFile(extensionVector[i].c_str(), &fileData);

		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{				
				CopyFile(fileData.cFileName, (outputEffectPathW + L"\\" + fileData.cFileName).c_str(), false);

			} while (FindNextFile(handle, &fileData));
		}

	}

	Utility::SetNewCurrentDirectory(beforeDirectoryPath);

	return true;
}

bool Quad::BuildModule::CopyProjectFiles(const std::string & programFolderPath,const std::string & programName)
{
	
	const std::wstring programFolderPathW = Utility::ConvertToWString(programFolderPath, true);

	ProjectMetaData * currProjectMetaData=	ProjectDirector::GetCurrentProjectMetaData();
	const std::wstring currProjectFolderPathW =	Utility::ConvertToWString(currProjectMetaData->GetProjectFolderPath().c_str(), true);

	

	//ProjectData.json
	CopyFile((currProjectFolderPathW + L"\\ProjectData.json").c_str(), (programFolderPathW + L"\\ProjectData.json").c_str(), false);


	//mapMetaDataFile.json
	CopyFile((currProjectFolderPathW + L"\\mapMetaDataFile.json").c_str(), (programFolderPathW + L"\\mapMetaDataFile.json").c_str(), false);









	//DLL list
	//UserCode.dll
	CopyFile((currProjectFolderPathW + L"\\x64\\Debug\\UserCode.dll").c_str(), (programFolderPathW + L"\\UserCode.dll").c_str(), false);
	//Core.dll

	auto editorDirector = EditorDirector::GetInstance();
	const std::string & editorFolderPath = editorDirector->GetEditorPathA();
	const std::string gameDllListFilePath = editorFolderPath+"\\EditorConfig\\GameDllList.json";
	const std::wstring dllFolderPath = Utility::ConvertToWString(editorFolderPath + "\\Dll\\x64\\Debug\\",true);
	JsonParser::ReadFile(gameDllListFilePath);

	JsonParser::ReadStart();

	const rapidjson::Value::ConstArray & dllArray =	JsonParser::ReadArray("DllList");
	for (auto& element : dllArray)
	{
		const std::wstring dllName =	Utility::ConvertToWString(element.GetString(),true);
		const std::wstring orginalDllPath = dllFolderPath + dllName;
		const std::wstring outputFilePath = programFolderPathW + L"\\" + dllName;
		CopyFile(orginalDllPath.c_str(), outputFilePath.c_str(), false);
	}


	//실행파일 copy
	const std::wstring originalGameFilePath = editorDirector->GetEditorPathW() + L"\\GameDirector.exe";
	const std::wstring UserGameFilePath = programFolderPathW + L"\\" + Utility::ConvertToWString(programName, true) + L".exe";
	CopyFile(originalGameFilePath.c_str(), UserGameFilePath.c_str(), false);





//	CopyFile((currProjectFolderPathW + L"\\UserCode.dll").c_str(), (programFolderPathW + L"\\UserCode.dll").c_str(), false);











	//Map/*.json (map) 
	std::string beforeFolderPath =Utility::SetNewCurrentDirectory(currProjectMetaData->GetProjectFolderPath() + "\\Map");
	
	WIN32_FIND_DATA fileData;
	HANDLE handle = FindFirstFile(L"*.*", &fileData);

	if (handle != INVALID_HANDLE_VALUE)
	{

		do
		{
			std::wstring destFilePath = programFolderPathW + L"\\Map\\" + fileData.cFileName;
			CopyFile(fileData.cFileName, destFilePath.c_str(), false);

		} while (FindNextFile(handle, &fileData));
	}

	Utility::SetNewCurrentDirectory(beforeFolderPath);



	//CopyFile((currProjectFolderPathW + L"\\UserCode.dll").c_str(), (programFolderPathW + L"\\UserCode.dll").c_str(), false);


	//그외 core.dll ,utility.dll

	return true;
}
