#include "EngineModeDirector.h"
#include<string>

#include"MeshManager.h"
#include"MaterialManager.h"
#include"TextureManager.h"

#include"UniqueIDTable.h"
#include"ResourceController.h"

void Quad::EngineModeDirector::SaveAsset()
{

	const std::string AssetPath = "C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset";




	SaveMesh(AssetPath);
	SaveMaterial(AssetPath);
	SaveTexture(AssetPath);







}

void Quad::EngineModeDirector::SaveMesh(const std::string& assetPath)
{

	const std::string meshPath = assetPath + "\\Mesh";
	ResourceController * resourceController	=	ResourceController::GetInstance();
	ResourceStorer* resourceStorer = resourceController->mResourceStorer;
	
	MeshManager * meshManager= MeshManager::GetInstance();
	const std::unordered_map<unsigned long long, Mesh*> & meshIDTable= meshManager->mIDTable.GetTable();


	//metaData
	unsigned long long nextMeshAvailableUniqueID =  meshManager->mIDTable.PeekUniqueID();
	const std::string meshMetaDataFilePath = assetPath + "\\MeshMetaData.json";
	JsonParser::StartWrite();
	JsonParser::StartWriteObject();
	JsonParser::Write("NextAvailableUniqueID", nextMeshAvailableUniqueID);
	JsonParser::Save(meshMetaDataFilePath);





	//RawData
	for (auto& element : meshIDTable)
	{
		Mesh* mesh = element.second;
		resourceStorer->SaveMesh(meshPath, mesh);

	}


	








}

void Quad::EngineModeDirector::SaveMaterial(const std::string& assetPath)
{
	const std::string materialPath = assetPath + "\\Material";

	//MaterialManager
	ResourceController* resourceController = ResourceController::GetInstance();
	ResourceStorer* resourceStorer = resourceController->mResourceStorer;

	MaterialManager* materialManager = MaterialManager::GetInstance();
	const std::unordered_map<unsigned long long, Material*>& materialIDTable = materialManager->mIDTable.GetTable();

	//metaData
	unsigned long long nextMaterialAvailableUniqueID = materialManager->mIDTable.PeekUniqueID();
	const std::string materialMetaDataFilePath = assetPath + "\\MaterialMetaData.json";
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

void Quad::EngineModeDirector::SaveTexture(const std::string& assetPath)
{
	const std::string textureMetaDataFilePath = assetPath + "\\TextureMetaData.json";

//	TextureManager


	//engine mode에서 import할때 텍스처는 복사해두었고
	//여기서는 텍스처의 메타데이터 생성
	ResourceController* resourceController = ResourceController::GetInstance();

	TextureManager* textureManager = TextureManager::GetInstance();
	const std::unordered_map<unsigned long long, Texture*>& textureIDTable = textureManager->mIDTable.GetTable();
	

	JsonParser::StartWrite();
	
	JsonParser::StartWriteObject();
	unsigned long long textureNum = 0;
	for (auto& element : textureIDTable)
	{
		Texture* texture = element.second;
		if (!texture->GetEngineContentItemFlag())
			textureNum++;
	}


	JsonParser::Write("TextureNum", textureNum);


	Texture* texture = nullptr;
	unsigned long long nextTextureAvailableUnqiueID = 0;
	unsigned long long assetID = 0;
	for (auto& element : textureIDTable)
	{
		texture = element.second;
		if (texture->GetEngineContentItemFlag())
			continue;

		JsonParser::StartWriteObject();

	
		assetID = texture->GetUniqueID();
		JsonParser::Write("AssetID", assetID);
		const std::string filePath = assetPath + "\\Texture\\" + texture->GetName();
		JsonParser::Write("FilePath", filePath);


		if (nextTextureAvailableUnqiueID < assetID)
			nextTextureAvailableUnqiueID = assetID;
	}


	JsonParser::Write("NextTextureAvailableUniqueID", ++nextTextureAvailableUnqiueID);

	JsonParser::Save(textureMetaDataFilePath);




}
