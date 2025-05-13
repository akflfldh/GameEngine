#include "ImportModule.h"
#include"Asset/Asset.h"
#include"Utility/Utility.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"TextureLoader.h"
#include"Core/RawResourceType.h"
#include"Core/ResourceImportCommonHeader.h"

#include"Asset/Mesh/Mesh.h"
#include"Asset/Material/Material.h"
#include"Asset/Skeleton/Skeleton.h"
#include"Asset/Animation/AnimationClip.h"


#include"ResourceManager/SkeletonManager.h"
#include"ResourceManager/AnimationClipManager.h"
#include"ResourceManager/MeshManager.h"
#include"FbxParser.h"

#include"GraphicCommand.h"
#include"Application.h"
#include<sstream>

#include"ResourceHelperClass.h"


std::vector<Quad::Asset*> Quad::ImportModule::ImportFile(const std::string& filePath, const std::string& logicalFolderPath,
	const std::string& copyAssetFolderPath)
{

	//확장자를 보는거지
	const std::string& extension = Utility::GetExtension(filePath);
	Asset* asset = nullptr;
	std::vector<Asset*> assetVector;
	if (extension == "fbx")
	{
		ImportFbxFile(filePath,logicalFolderPath, assetVector, copyAssetFolderPath);
	}
	else if (extension == "png" || extension == "jpg" || extension == "bmp")
	{

		asset =  ImportTextureFile(filePath, logicalFolderPath, copyAssetFolderPath);
		assetVector.push_back(asset);
	}

	//SendLoadFileResponseEvent(assetVector);

	return assetVector;
















}

void Quad::ImportModule::ImportFbxFile(const std::string& filePath, const std::string& logicalFolderPath, std::vector<Asset*>& oAssetVector,
	const std::string& copyFolderPath)
{

	std::string fileName = PathFindFileNameA(filePath.c_str());
	fileName = Utility::RemoveExtension(fileName);
	std::vector<RawMeshData*> rawMeshDataVector;
	std::vector<RawMaterialData> rawMaterialDataVector;
	std::vector<RawTextureData> rawTextureDataVector;
	std::unique_ptr<Skeleton> oSkeleton = nullptr;
	std::unique_ptr <AnimationClip> oAnimationClip = nullptr;
	LoadResultCommonData loadResultCommonData;
	
	mFbxParser.LoadFbxFile(filePath, rawMeshDataVector, rawMaterialDataVector, rawTextureDataVector, oSkeleton, oAnimationClip,
		loadResultCommonData);


	//이름이 겹치는지 아닌지 확인해서 조정한다.


	std::vector<std::string> meshNameVector;

	for (int i = 0; i < rawMeshDataVector.size(); ++i)
	{
		meshNameVector.push_back(rawMeshDataVector[i]->mName);
	}

	std::vector<std::string> materialNameVector;
	for (int i = 0; i < rawMaterialDataVector.size(); ++i)
	{
		materialNameVector.push_back(rawMaterialDataVector[i].mName);
	}

	//mesh랑 머터리얼이랑 이름이같은게있는지체크

	for (int meshIndex = 0; meshIndex < rawMeshDataVector.size(); ++meshIndex)
	{
		for (int matNameIndex = 0; matNameIndex < materialNameVector.size(); ++matNameIndex)
		{
			if (rawMeshDataVector[meshIndex]->mName == materialNameVector[matNameIndex])
			{
				//mesh의 이름을 바꾸자. 
				//그게더 간단하다.
				//머터리얼의 이름을바꾸면 다 일일이 그 머터리얼을 참조하는 메시의 머터리얼필드의 이름값을 바꿔야하니.

				int i = 1;
				rawMeshDataVector[meshIndex]->mName += std::to_string(i);
			}
		}

	}




	//이제 로드한 asset들이 다른 이름을 가지게되었지만, 실제 유일한이름은 논리적폴더경로를 붙여서 만든다.


	for (int i = 0; i < rawTextureDataVector.size(); ++i)
	{

		Texture* texture = ImportTextureFile(rawTextureDataVector[i].mFilePath, logicalFolderPath, copyFolderPath);

		if(texture !=nullptr)
			oAssetVector.push_back(texture);
		
		//생성한 텍스처를 리턴해주자.
	}

	for (int i = 0; i < rawMaterialDataVector.size(); ++i)
	{
		if (rawMaterialDataVector[i].mDefaultFlag)
			continue;

		Asset* asset = CreateMaterial(rawMaterialDataVector[i], logicalFolderPath);
		if (asset != nullptr)
			oAssetVector.push_back(asset);
	}

	int animMeshIndex = oAssetVector.size();
	for (int i = 0; i < rawMeshDataVector.size(); ++i)
	{
		if (rawMeshDataVector[i]->mName == "")
		{
			//여러 메시가 합쳐진 skinning메시일경우 이름이없을것이니
			rawMeshDataVector[i]->mName = fileName + "_mesh";
		}
		Asset* asset = CreateMesh(rawMeshDataVector[i], logicalFolderPath, rawMaterialDataVector);
		if (asset != nullptr)
			oAssetVector.push_back(asset);
	}


	//에니메이션모델을 임포트하는경우 다른 정적 메시들은 임포트하지않는다. 
	//따라서 딱하나의 메시만 임포트되었을것이다 , 그것을 animaion clip이 default로서 참조하도록한다.
	//그래야 속성창에서 에니메이션클립asset을 드래그앤드랍하여 에니메이션 편집기능을 구현할수있겠다.
	Skeleton* skeleton = nullptr;
	if (oSkeleton != nullptr)
	{
		std::string skeletonName = logicalFolderPath+ fileName + "_Skeleton";
		skeleton = SkeletonManager::CreateSkeleton(skeletonName);
		skeleton->Copy(std::move(*oSkeleton));


		oAssetVector.push_back(skeleton);
	}

	if (oAnimationClip != nullptr)
	{
		std::string animationClipName = logicalFolderPath+ fileName + "_Anim";
	
		AnimationClip * animClip = AnimationClipManager::CreateAnimationClip(animationClipName);
		animClip->Copy(std::move(*oAnimationClip));
		oAssetVector.push_back(animClip);


		animClip->SetDefaultMesh((Mesh*)oAssetVector[animMeshIndex]);
		animClip->SetDefaultSkeleton(skeleton);

	}




}


Quad::Texture* Quad::ImportModule::ImportTextureFile(const std::string& filePath, const std::string& logicalFolderPath,
	const std::string& copyAssetFolderPath)
{

	std::string textureFolderPath = copyAssetFolderPath + "\\Texture";



	std::string fileName = Utility::GetFileNameFromPath(filePath);

	std::string uniqueFileName = logicalFolderPath +fileName;

	Texture* texture = TextureManager::GetTexture(uniqueFileName);
	if (texture != nullptr)
	{
		return nullptr;
	}
	TextureLoader* textureLoader = TextureLoader::GetInstance();

	texture = TextureManager::CreateTexture(uniqueFileName);
	bool ret = textureLoader->LoadTextureFile(filePath, *texture);
	if (ret == false)
	{
		TextureManager::KillTexture(texture);
		return nullptr;
	}

	TextureFactory::CreateSrvTex2D(texture);




		//복사본을 저장한다.

		std::wstring filePathW = Utility::ConvertToWString(filePath.c_str(), true);

	/*	std::string assetRawName = Utility::GetFileNameFromPath(texture->GetName());

		std::stringstream ss;
		ss << std::hex;
		ss << texture->GetUniqueID() << "_" << assetRawName;
		std::string aa = ss.str();*/

		std::string  textureFileName = ResourceHelperClass::GetTextureFileName(texture);

		std::wstring newFilePathW = Utility::ConvertToWString(textureFolderPath, true) + L"\\" + Utility::ConvertToWString(textureFileName, true);

		CopyFile(filePathW.c_str(), newFilePathW.c_str(), false);


	return texture;

}



Quad::Material* Quad::ImportModule::CreateMaterial(Quad::RawMaterialData& rawMaterialData, const std::string& logicalFolderPath)
{
	MaterialManager* materialManager = MaterialManager::GetInstance();


	std::string materialUnqiueName = logicalFolderPath  + rawMaterialData.mName;

	Material* material = materialManager->CreateMaterial(materialUnqiueName);
	//material->SetEngineContentItemFlag(true);

	if (material != nullptr)
	{
		material->SetEffectName(rawMaterialData.mEffectName);
		material->SetSpecular(rawMaterialData.mSpecular);
		material->SetShiness(rawMaterialData.mShiness);


		std::string diffuseMapUniqueName = logicalFolderPath + rawMaterialData.mDiffuseMapName;

		Texture* mTexture = TextureManager::GetTexture(diffuseMapUniqueName);
		if (mTexture == nullptr)
		{
			mTexture = TextureManager::GetTexture("Default.bmp");
		}

		material->SetDiffuseMap(mTexture);

		//material->SetNormalMap(TextureManager::GetTexture(rawMaterialData.mNormalMapName));
	}
	return material;


}

Quad::Mesh* Quad::ImportModule::CreateMesh(Quad::RawMeshData* rawMeshData, const std::string& logicalFolderPath , const std::vector<RawMaterialData> &  rawMaterialDataVector)
{
	MeshManager * meshManager =	MeshManager::GetInstance();


	std::string meshUniqueName = logicalFolderPath  + rawMeshData->mName;

	Mesh* mesh = meshManager->CreateMesh(meshUniqueName, rawMeshData->mMeshType);


	if (mesh == nullptr)
	{
		OutputDebugString(L"mesh 로드 이름겹침\n");
		return nullptr;
		//renamning

	}

	//vertexBuffer,indexBuffer생성
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexUploadBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexUploadBuffer;

	auto app =	Application::GetInstance();
	Microsoft::WRL::ComPtr<ID3D12Device> device = app->GetD3D12Device();
	GraphicCommand & graphicsCommandObject = app->GetGraphicCommand();

	if (graphicsCommandObject.GetCloseState())
		graphicsCommandObject.ResetCommandList(nullptr);
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = graphicsCommandObject.GetGraphicsCommandList();


	void* pVertexVectorData = nullptr;
	int vertexElementSize = 0;
	size_t vertexVectorSize = 0;
	switch (rawMeshData->mMeshType)
	{
	case EMeshType::eStaticMesh:
		pVertexVectorData = ((RawStaticMeshData*)rawMeshData)->mVertexVector.data();
		vertexElementSize = sizeof(StaticVertex);
		vertexVectorSize = ((RawStaticMeshData*)rawMeshData)->mVertexVector.size();
		break;
	case EMeshType::eSknningMesh:
		pVertexVectorData = ((RawSkinningMeshData*)rawMeshData)->mVertexVector.data();
		vertexElementSize = sizeof(SkinningVertex);
		vertexVectorSize = ((RawSkinningMeshData*)rawMeshData)->mVertexVector.size();
		break;
		//case EMeshType::eLine:
		//	pVertexVectorData = ((RawLineMeshData*)rawMeshData)->mVertexVector.data();
		//	vertexElementSize = sizeof(LineVertex);
		//	vertexVectorSize = ((RawLineMeshData*)rawMeshData)->mVertexVector.size();
		//	break;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexDefaultBuffer = Utility::CreateDefaultBuffer(device, graphicsCommandList, pVertexVectorData,
		vertexElementSize, vertexVectorSize, vertexUploadBuffer);

	Microsoft::WRL::ComPtr<ID3D12Resource> indexDefaultBuffer = Utility::CreateDefaultBuffer(device, graphicsCommandList, rawMeshData->mIndexVector.data(),
		sizeof(MeshIndexType), rawMeshData->mIndexVector.size(), indexUploadBuffer);
	graphicsCommandObject.ExecuteCommandList();
	graphicsCommandObject.FlushCommandQueue();

	int subMeshNum = rawMeshData->mRawSubMeshVector.size();
	const std::vector<RawSubMeshData>& rawSubMeshVector = rawMeshData->mRawSubMeshVector;
	std::vector<SubMesh> subMeshVector(subMeshNum);

	MaterialManager* materialManager = MaterialManager::GetInstance();

	for (int subMeshIndex = 0; subMeshIndex < subMeshNum; ++subMeshIndex)
	{
		subMeshVector[subMeshIndex].mVertexOffset = rawSubMeshVector[subMeshIndex].mVertexOffset;
		subMeshVector[subMeshIndex].mIndexRange.first = rawSubMeshVector[subMeshIndex].mIndexFirst;
		subMeshVector[subMeshIndex].mIndexRange.second = rawSubMeshVector[subMeshIndex].mIndexLast;
		subMeshVector[subMeshIndex].mMesh = mesh;

		const std::string rawMaterialName = rawSubMeshVector[subMeshIndex].mMaterialName;
		auto rawMatIt= std::find_if(rawMaterialDataVector.cbegin(), rawMaterialDataVector.cend(), [rawMaterialName](const RawMaterialData& matData) {
			if (matData.mName == rawMaterialName  && matData.mDefaultFlag)
				return true;
			return false;
			});

		if (rawMatIt != rawMaterialDataVector.cend())
		{
			//디폴트 mat을 사용했다.

			const std::string& materialName = rawSubMeshVector[subMeshIndex].mMaterialName;
			subMeshVector[subMeshIndex].mMaterialPointer = materialManager->GetMaterial(materialName);
		}
		else
		{
			const std::string& materialName = logicalFolderPath + rawSubMeshVector[subMeshIndex].mMaterialName;
			subMeshVector[subMeshIndex].mMaterialPointer = materialManager->GetMaterial(materialName);
		}
	}


	mesh->SetVertexBuffer(vertexDefaultBuffer);
	mesh->SetIndexBuffer(indexDefaultBuffer);
	mesh->SetSubMeshVector(std::move(subMeshVector));
	switch (rawMeshData->mMeshType)
	{
	case EMeshType::eStaticMesh:
	{
		RawStaticMeshData* rawStaticMeshData = (RawStaticMeshData*)rawMeshData;
		((StaticMesh*)mesh)->SetVertexVector(std::move(rawStaticMeshData->mVertexVector));
	}
	break;
	case EMeshType::eSknningMesh:
	{

		RawSkinningMeshData* rawSkinningMeshData = (RawSkinningMeshData*)rawMeshData;
		((SkinningMesh*)mesh)->SetVertexVector(std::move(rawSkinningMeshData->mVertexVector));
	}

	break;

	/*	case EMeshType::eLine:
		{

			RawLineMeshData* rawLineMeshData = (RawLineMeshData*)rawMeshData;
			((LineMesh*)mesh)->SetVertexVector(std::move(rawLineMeshData->mVertexVector));
		}
			break;*/
	}

	mesh->SetIndexVector(std::move(rawMeshData->mIndexVector));
	mesh->SetMinMaxPoint(rawMeshData->mBoundingBox.first, rawMeshData->mBoundingBox.second);



	return mesh;
}