#include "EditorAssetImportVersionModule.h"
#include"Parser/JsonParser.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"Utility/Utility.h"
#include"ImportModule.h"
#include"ResourceController.h"
#include<sstream>


void Quad::EditorAssetImportVersionModule::ImportAsset(const std::string& assetFolderPath)
{

	const std::string textureFolderPath = assetFolderPath + "\\Texture";
	const std::string meshFolderPath = assetFolderPath + "\\Mesh";

	ImportTexture(textureFolderPath);
	ImportMesh(meshFolderPath);

}

void Quad::EditorAssetImportVersionModule::SaveAsset(const std::string& folderPath)
{
	//resourceManager

	

	ResourceController * resourceController= ResourceController::GetInstance();


	const std::string materialFilePath = folderPath + "\\Material";
	const std::string textureFilePath = folderPath + "\\Texture";


	

	SaveTexture(folderPath);

	SaveMaterial(folderPath);

	SaveMesh(folderPath);



}

void Quad::EditorAssetImportVersionModule::ImportMesh(const std::string& meshFolderPath)
{
	wchar_t currentDirectoryBuffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDirectoryBuffer);

	//std::wstring textureFolderPathW = Utility::ConvertToWString(textureFolderPath, true);
	SetCurrentDirectoryW(Utility::ConvertToWString(meshFolderPath, true).c_str());

	TCHAR debug[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, debug);
	OutputDebugString(debug);

	WIN32_FIND_DATAW fileData;
	
	ImportModule* importMoudle = ImportModule::GetInstance();



	

	std::wstring fileName = L"*.fbx";
	HANDLE handle = FindFirstFileW(fileName.c_str(), &fileData);

	if (handle != INVALID_HANDLE_VALUE)
	{

		while (1)
		{

			importMoudle->ImportFile(Utility::ConvertToString(fileData.cFileName, true), "", "C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset\\Texture");
			if (FindNextFileW(handle, &fileData) == 0)
				break;
		}

		FindClose(handle);

	}
	SetCurrentDirectoryW(currentDirectoryBuffer);





}

void Quad::EditorAssetImportVersionModule::ImportTexture(const std::string& textureFolderPath)
{
	wchar_t currentDirectoryBuffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, currentDirectoryBuffer);
	
	//std::wstring textureFolderPathW = Utility::ConvertToWString(textureFolderPath, true);
	SetCurrentDirectoryW(Utility::ConvertToWString(textureFolderPath, true).c_str());

	TCHAR debug[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, debug);
	OutputDebugString(debug);

	WIN32_FIND_DATAW fileData;
	const std::vector<std::wstring> fileExtensionVector = { L"bmp",L"png",L"jpg" };

	ImportModule* importMoudle = ImportModule::GetInstance();

	for (int extensionIndex = 0; extensionIndex < fileExtensionVector.size(); ++extensionIndex)
	{

		std::wstring fileName = L"*." + fileExtensionVector[extensionIndex];
		HANDLE handle =	 FindFirstFileW(fileName.c_str(), &fileData);

		if (handle == INVALID_HANDLE_VALUE)
			continue;



			while (1)
			{

				importMoudle->ImportFile(Utility::ConvertToString(fileData.cFileName, true), "",
					"C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset");
				if (FindNextFileW(handle, &fileData)==0)
					break;

			}

		FindClose(handle);

	}
	SetCurrentDirectoryW(currentDirectoryBuffer);




}

void Quad::EditorAssetImportVersionModule::SaveMesh(const std::string& assetFolderPath)
{

	const std::string meshFolderPath = assetFolderPath + "\\Mesh";

	ResourceController* resourceController = ResourceController::GetInstance();
	ResourceStorer* resourceStorer = ResourceStorer::GetInstance();

	MeshManager* meshManager = MeshManager::GetInstance();
	const std::unordered_map<unsigned long long, Mesh*>& meshIDTable = meshManager->mIDTable.GetTable();


	//metaData
	unsigned long long nextMeshAvailableUniqueID = meshManager->mIDTable.PeekUniqueID();
	const std::string meshMetaDataFilePath = assetFolderPath + "\\MeshMetaData.json";
	JsonParser::StartWrite();
	JsonParser::StartWriteObject();
	JsonParser::Write("NextAvailableUniqueID", nextMeshAvailableUniqueID);
	JsonParser::Save(meshMetaDataFilePath);

	//RawData
	for (auto& element : meshIDTable)
	{
		Mesh* mesh = element.second;
		resourceStorer->SaveMesh(meshFolderPath, mesh);
	}


}

void Quad::EditorAssetImportVersionModule::SaveMaterial(const std::string& assetFolderPath)
{

	const std::string materialPath = assetFolderPath + "\\Material";

	//MaterialManager
	ResourceController* resourceController = ResourceController::GetInstance();
	ResourceStorer* resourceStorer = ResourceStorer::GetInstance();

	MaterialManager* materialManager = MaterialManager::GetInstance();
	const std::unordered_map<unsigned long long, Material*>& materialIDTable = materialManager->mIDTable.GetTable();

	//metaData
	unsigned long long nextMaterialAvailableUniqueID = materialManager->mIDTable.PeekUniqueID();
	const std::string materialMetaDataFilePath = assetFolderPath + "\\MaterialMetaData.json";
	JsonParser::StartWrite();
	JsonParser::StartWriteObject();
	JsonParser::Write("NextAvailableUniqueID", nextMaterialAvailableUniqueID);
	JsonParser::Save(materialMetaDataFilePath);


	//RawData

	for (auto& element : materialIDTable)
	{
		Material* material = element.second;
		resourceStorer->SaveMaterial(materialPath, material);
	}





}

void Quad::EditorAssetImportVersionModule::SaveTexture(const std::string& assetFolderPath)
{

	const std::string& textureMetaDataFilePath = assetFolderPath + "\\TextureMetaData.json";

	

	JsonParser::StartWrite();

	unsigned long long textureNum = 0;

	TextureManager* textureManager = TextureManager::GetInstance();
	const std::unordered_map<unsigned long long, Texture*>& textureIDTable = textureManager->mIDTable.GetTable();


	//nextAavailableUniqueID
	unsigned long long nextAvailableUniqueID = textureManager->mIDTable.PeekUniqueID();

	JsonParser::StartWriteObject();
	JsonParser::Write("NextAvailableUniqueID", nextAvailableUniqueID);

	Texture* texture = nullptr;
	for (auto& element : textureIDTable)
	{
		texture = element.second;
		if (!texture->GetEngineContentItemFlag())
			textureNum++;
	}

	JsonParser::StartWriteObject();
	JsonParser::Write("TextureNum", textureNum);

	for (auto& element : textureIDTable)
	{

		texture = element.second;

		if (texture->GetEngineContentItemFlag())
			continue;


		JsonParser::StartWriteObject();


		std::string rawName = Utility::GetFileNameFromPath(texture->GetName());
		std::stringstream ss;
		ss << std::hex;
		ss << texture->GetUniqueID() << "_" << rawName;


		//const std::string& filePath = assetFolderPath + "\\Texture\\" + texture->GetName();
		JsonParser::Write("FileName", ss.str());
		JsonParser::Write("AssetUniqueName", texture->GetName());
		JsonParser::Write("AssetUniqueID", texture->GetUniqueID());
		JsonParser::Write("LogicalFolderUniqueID", texture->GetDestLogicalFolderUnqiueID());

	}

	JsonParser::Save(textureMetaDataFilePath);




}
