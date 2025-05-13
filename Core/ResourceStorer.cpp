#include "ResourceStorer.h"
#include"ResourceManager/MeshManager.h"
#include"Parser/JsonParser.h"
#include"fstream"
#include"ResourceManager/TextureManager/TextureManager.h"
//#include"EnginTexture.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"ResourceManager/EffectManager/EffectManager.h"

#include"Asset/Mesh/StaticMesh.h"
#include"Asset/Mesh/SkinningMesh.h"
#include"Asset/Skeleton/Skeleton.h"
#include"Asset/Animation//Bone.h"

#include"Asset/Animation/AnimationClip.h"
#include"DirectXTex.h"
#include<sstream>

#include"ResourceHelperClass.h"
#include"ResourceFileType.h"

namespace Quad
{
	void ResourceStorer::Initialize()
	{
	}

	void ResourceStorer::SaveMesh(const std::string & FolderPath, Mesh * mesh)
	{


		//20250313version
		const std::string meshName = mesh->GetName();

		const std::string rawMeshName = Utility::GetFileNameFromPath(meshName);

		std::stringstream ss;
		ss << std::hex;
		ss << mesh->GetUniqueID()<<"_"<<rawMeshName << ".mesh";

		const std::string meshFilePath = FolderPath + "\\" + ss.str();
		std::ofstream fout(meshFilePath,std::ios_base::binary);

		if (!fout.is_open())
		{

			const std::string errorMessage = mesh->GetName() + "(메시)저장하기 실패\n";
			OutputDebugStringA(errorMessage.c_str());
			assert(0);
		}


		//MetaData
		unsigned long long assetID = mesh->GetUniqueID();

		EMeshType meshType = mesh->GetMeshType();
		int meshTypeInt = (int)meshType;
		unsigned int meshNameLength = static_cast<unsigned int>(meshName.size());

		unsigned long long vertexNum = mesh->GetVertexNum();
		unsigned long long indexNum = mesh->GetIndexNum();
		const std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>& minMaxPoint = mesh->GetMinMaxPoint();
		const std::vector<SubMesh>& subMeshVector = mesh->GetSubMeshVector();



		fout.write(reinterpret_cast<const char*>(&assetID), sizeof(assetID));
		fout.write(reinterpret_cast<const char*>(&meshTypeInt), sizeof(meshTypeInt));
		fout.write(reinterpret_cast<const char*>(&meshNameLength), sizeof(meshNameLength));
		fout.write(meshName.c_str(), sizeof(char) * meshNameLength);


		WriteMeshCore(fout, mesh);

		//fout.write(reinterpret_cast<const char*>(&vertexNum), sizeof(vertexNum));
		//fout.write(reinterpret_cast<const char*>(&indexNum), sizeof(indexNum));
	
		////min max


		//Write(fout, minMaxPoint.first);
		//Write(fout, minMaxPoint.second);

		////submesh
		//

		//unsigned long long subMeshNum = subMeshVector.size();
		//fout.write(reinterpret_cast<const char*>(&subMeshNum), sizeof(subMeshNum));
		//unsigned long long materialUniqueID = 0;
		//unsigned long long subMeshNameLength = 0;
		//for (auto& subMesh : subMeshVector)
		//{
		//	fout.write(reinterpret_cast<const char*>(&subMesh.mID), sizeof(subMesh.mID));
		//	fout.write(reinterpret_cast<const char*>(&subMesh.mIndexRange.first), sizeof(subMesh.mIndexRange.first));
		//	fout.write(reinterpret_cast<const char*>(&subMesh.mIndexRange.second), sizeof(subMesh.mIndexRange.second));
		//
		//	fout.write(reinterpret_cast<const char*>(&subMesh.mVertexNum), sizeof(subMesh.mVertexNum));
		//	fout.write(reinterpret_cast<const char*>(&subMesh.mVertexOffset), sizeof(subMesh.mVertexOffset));

		//	materialUniqueID = subMesh.mMaterialPointer->GetUniqueID();
		//	fout.write(reinterpret_cast<const char*>(&materialUniqueID), sizeof(materialUniqueID));

		//	const std::string & subMeshName = subMesh.mName;
		//	subMeshNameLength = subMeshName.size();
		//	fout.write(reinterpret_cast<const char*>(&subMeshNameLength), sizeof(subMeshNameLength));

		//
		//	fout.write(reinterpret_cast<const char*>(subMeshName.c_str()), sizeof(char) * subMeshNameLength);

		//}


		//








		//if (meshType == EMeshType::eSknningMesh)
		//{
		//	SkinningMesh* skinningMesh = static_cast<SkinningMesh*>(mesh);

		//	const std::vector<SkinningVertex>& vertexVector = skinningMesh->GetVertexVector();
		//	for (auto& vertex : vertexVector)
		//	{
		//		
		//		Write(fout, vertex.mPos);
		//		Write(fout, vertex.mNormal);
		//		Write(fout, vertex.mTex);
		//		Write(fout, vertex.mBoneWeight[0]);
		//		Write(fout, vertex.mBoneWeight[1]);
		//		Write(fout, vertex.mBoneIndex[0]);
		//		Write(fout, vertex.mBoneIndex[1]);
		//	}
		//
		//}
		//else if (meshType == EMeshType::eStaticMesh)
		//{
		//	StaticMesh* staticMesh = static_cast<StaticMesh*>(mesh);

		//	const std::vector<StaticVertex>& vertexVector = staticMesh->GetVertexVector();
		//	for (auto& vertex : vertexVector)
		//	{
		//		Write(fout, vertex.mPos);
		//		Write(fout, vertex.mNormal);
		//		Write(fout, vertex.mTex);
		//	}
		//}


		//const std::vector<MeshIndexType>& indexVector = mesh->GetIndexVector();

		//for (auto& index : indexVector)
		//{
		//	fout.write(reinterpret_cast<const char*>(&index), sizeof(index));
		//}


		fout.close();




	
















		////meta file, data file 

		////meta file
		//MeshManager * meshManager = MeshManager::GetInstance();
		//const std::unordered_map<unsigned long long ,Mesh *>& meshTable =	meshManager->mMeshTable.GetTable();

		//JsonParser::StartWrite();

		//for(const auto & element  : meshTable)
		//{
		//	Mesh* mesh = element.second;
		//	JsonParser::StartWriteObject();
		//	
		//	mesh->Serialize();
		//	const std::string& meshName = mesh->GetName();
		//	const std::string filePath = "./Resource/Mesh/" + meshName + ".mesh";
		//	JsonParser::Write("FilePath", filePath);

		//	//.mesh 저장
		//	//data file 

		//	std::ofstream fout(meshName + ".mesh",std::ios_base::binary);
		//	

		//	//vertex, index
		//	switch (mesh->GetMeshType())
		//	{
		//	case  EMeshType::eStaticMesh:
		//	{
		//		StaticMesh* staticMesh = static_cast<StaticMesh*>(mesh);
		//		const std::vector<StaticVertex>& vertexVector = staticMesh->GetVertexVector();

		//		for (const auto& vertex : vertexVector)
		//		{
		//			Write(fout, vertex.mPos);
		//			Write(fout, vertex.mNormal);
		//			Write(fout, vertex.mTex);
		//		}


		//	

		//	}

		//		break;
		//	case EMeshType::eSknningMesh:
		//	{
		//		SkinningMesh* skinningMesh = static_cast<SkinningMesh*>(mesh);
		//		const std::vector<SkinningVertex>& vertexVector = skinningMesh->GetVertexVector();

		//		for (const auto& vertex : vertexVector)
		//		{
		//			Write(fout, vertex.mPos);
		//			Write(fout, vertex.mNormal);
		//			Write(fout, vertex.mTex);
		//			Write(fout, vertex.mBoneWeight[0]);
		//			Write(fout, vertex.mBoneWeight[1]);
		//			Write(fout, vertex.mBoneIndex[0]);
		//			Write(fout, vertex.mBoneIndex[1]);

		//		}

		//	}

		//		break;
		//	}


		//	const std::vector<MeshIndexType>& indexVector = mesh->GetIndexVector();
		//	fout.write((const char*)indexVector.data(), sizeof(MeshIndexType) * indexVector.size());


		//	fout.close();

		//} 

		//JsonParser::Save("MeshMeta.json");





	}

	void ResourceStorer::SaveTexture()
	{
		//json meta data저장이다.


	/*	const std::string textureMetaFileName = "TextureMeta.json";


		TextureManager* textureManager =  TextureManager::GetInstance();
		const std::unordered_map<unsigned long long, Texture*>& table = textureManager->mIDTable.GetTable();

		JsonParser::StartWrite();
		for (auto& element : table)
		{
			JsonParser::StartWriteObject();
			element.second->Serialize();
			Texture* texture = element.second;
			const std::string& textureFilePath = static_cast<EnginTexture*>(texture)->GetFilePath();
			JsonParser::Write("Texture_FilePath", textureFilePath);
			texture->Serialize();
		}

		JsonParser::Save(textureMetaFileName);*/





	}

	void ResourceStorer::SaveMaterial(const std::string & materialFileFolderPath ,Material * material)
	{


		std::string rawName =Utility::GetFileNameFromPath(material->GetName());
		std::stringstream ss;
		ss << std::hex;


		ss << rawName << material->GetUniqueID() << ".json";
		const std::string materialMetaFilePath = materialFileFolderPath +"\\"+ss.str();


		JsonParser::StartWrite();
		JsonParser::StartWriteObject();
		material->Serialize();	
		JsonParser::Save(materialMetaFilePath);

		

	}

	void ResourceStorer::SaveEffect()
	{
		
		const std::string effectMetaFileName = "EffectMeta.json";

		EffectManager* effectManager = EffectManager::GetInstance();

		const std::unordered_map<unsigned long long, Effect*>& table = effectManager->mIDTable.GetTable();
		JsonParser::StartWrite();
		for (auto& element : table)
		{

			JsonParser::StartWriteObject();
			Effect* effect = element.second;
			effect->Serialize();
		
		}
		JsonParser::Save(effectMetaFileName);

	}

	void ResourceStorer::SaveSkeleton(const std::string& skeletonFolderPath, Skeleton* skeleton)
	{


		//20250313version
		const std::string skeletonName = skeleton->GetName();

		const std::string rawSkeletonName = Utility::GetFileNameFromPath(skeletonName);

		std::stringstream ss;
		ss << std::hex;
		ss << skeleton->GetUniqueID() << "_" << rawSkeletonName << ".skeleton";
		


		std::string skeletonFilePath = skeletonFolderPath + "\\" + ss.str();
		std::ofstream fout(skeletonFilePath, std::ios_base::binary);
		
		if (!fout.is_open())
		{
			const std::string errorMessage = skeletonName + reinterpret_cast<const char*>(u8"(스켈레톤)저장하기 실패\n");
			OutputDebugStringA(errorMessage.c_str());
			assert(0);
		}


		// name 
		size_t  nameLength = skeletonName.size();
		fout.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
		fout.write(skeletonName.c_str(), sizeof(char) * nameLength);




		//asset id 
		unsigned long long uniqueID = skeleton->GetUniqueID();
		fout.write(reinterpret_cast<const char*>(&uniqueID), sizeof(uniqueID));
	


		WriteSkeletonCore(fout, skeleton);


		////bone 
		//size_t boneNum = skeleton->GetBoneVector().size();
	
		//fout.write(reinterpret_cast<const char*>(&boneNum), sizeof(boneNum));

		//const std::vector<Bone * >& boneVector = skeleton->GetBoneVector();
		//const std::vector<int>& boneHiererachyVector = skeleton->GetBoneHiererachyVector();
		//const std::vector<DirectX::XMFLOAT4X4> & mOffsetMatrixVector = skeleton->GetOffsetMatrixVector();



		//SaveBone(fout, boneVector);
		//fout.write(reinterpret_cast<const char*>(boneHiererachyVector.data()), sizeof(int) * boneHiererachyVector.size());
		//for (auto& offsetMatrix : mOffsetMatrixVector)
		//{
		//	Write(fout, offsetMatrix);
		//}


		


	}

	void ResourceStorer::SaveAnimationClip(const std::string& animationClipFolderPath, AnimationClip* animClip)
	{


		const std::string animClipName = animClip->GetName();

		const std::string rawAnimClipName = Utility::GetFileNameFromPath(animClipName);

		std::stringstream ss;
		ss << std::hex;
		ss << animClip->GetUniqueID() << "_" << rawAnimClipName << ".anim";


		std::string animFilePath = animationClipFolderPath + "\\" + ss.str();
		std::ofstream fout(animFilePath, std::ios_base::binary);


		// name 
		size_t  nameLength = animClipName.size();
		Write(fout, nameLength);

		fout.write(animClipName.c_str(), sizeof(char) * nameLength);




		//asset id 
		unsigned long long uniqueID = animClip->GetUniqueID();
		Write(fout, uniqueID);


		WriteAnimationClipCore(fout, animClip);

		/*int endFrameCount =	 animClip->GetEndFrameCount();
		Write(fout, endFrameCount);

		float endSecondTime =  animClip->GetEndTimeSecond();
		Write(fout, endSecondTime);

		int timeMode = static_cast<int>(animClip->GetTimeMode());
		Write(fout, timeMode);


		Mesh * defaultMesh = animClip->GetDefaultMesh();
		unsigned long long defaultMeshID =defaultMesh->GetUniqueID();

		Write(fout, defaultMeshID);

		Skeleton * defaultSkeleton =animClip->GetDefaultSkeleton();
		unsigned long long defaultSkeletonID =	 defaultSkeleton->GetUniqueID();
		Write(fout, defaultSkeletonID);
		 
		
		
		const std::vector<BoneAnimationClip> & boneAnimationClipVector = animClip->GetBoneAnimationClipVector();

		size_t  boneAnimClipNum = boneAnimationClipVector.size();
		Write(fout, boneAnimClipNum);


		for (const auto& boneAnimClip : boneAnimationClipVector)
		{
			int boneID = boneAnimClip.GetBoneID();
			Write(fout, boneID);

			const std::vector<AnimationKeyFrame> & animKeyFrameVector =	boneAnimClip.GetKeyFrameVector();

			size_t keyFrameNum = animKeyFrameVector.size();
			Write(fout, keyFrameNum);

			for (const auto& keyFrame : animKeyFrameVector)
			{
				Write(fout, keyFrame.mScale);
				Write(fout, keyFrame.mQuaternion);
				Write(fout, keyFrame.mPosition);
				Write(fout, keyFrame.mTime);
			}

		}
*/




	}

	void ResourceStorer::SaveTexturePackage(const std::string & filePath , const std::vector<ResourcePackageEntryMeta >& texturePackageEntryMetaVector, const std::vector<DirectX::Blob>& textureBlobVector)
	{

		std::ofstream fout(filePath.c_str(), std::ios_base::binary);

		if (!fout.is_open())
		{
			OutputDebugStringW(L"텍스처패키지 파일저장실패\n");
			assert(0);
		}


		//header 

			//texture file num 
		unsigned long long textureFileNum = texturePackageEntryMetaVector.size();
		Write(fout, textureFileNum);

			//entry table offset 
		unsigned long long tableOffset = sizeof(unsigned long long) * 3;
		Write(fout, tableOffset);

			//texture file data offset
		unsigned long long rawFileDataOffset = tableOffset;
			

		for (const auto& textureElement : texturePackageEntryMetaVector)
		{
			// nameLength_size ,  name_size,id_size,dataSize_size, dataOffset_size

			//const std::string textureName = Utility::GetFileNameFromPath(texturePackageEntryMetaVector->GetName());

			rawFileDataOffset += sizeof(unsigned long long) * 4 + textureElement.mName.size();
		}
		Write(fout, rawFileDataOffset);
		
	


		//entry table 

		unsigned long long id = 0;
		unsigned long long nameLength = 0;
		unsigned long long rawDataOffset = rawFileDataOffset;
		unsigned long long rawDataSize = 0;

			
		for(size_t i =0; i< textureFileNum; ++i)
		{
			//texture name length	 -ull 
			nameLength = texturePackageEntryMetaVector[i].mNameLength;
			Write(fout, nameLength);


			//texture name	-가변적
			const std::string& name = texturePackageEntryMetaVector[i].mName;
			Write(fout, name);


			//id		-ull
			id = texturePackageEntryMetaVector[i].mID;
			Write(fout, id);


			//raw data size -ull
			rawDataSize = textureBlobVector[i].GetBufferSize();
			Write(fout, rawDataSize);


			//raw data offset	-ull
			Write(fout, rawDataOffset);


			rawDataOffset += rawDataSize;
		
		}



		//raw data 
		for (auto& blobElement : textureBlobVector)
		{
			Write(fout, blobElement.GetBufferPointer(), blobElement.GetBufferSize());
		}
		

	}

	void ResourceStorer::SaveMaterialPackage(const std::string& filePath, const std::vector<Material*>& materialVector)
	{

		std::ofstream fout(filePath.c_str(), std::ios_base::binary);
		if (!fout.is_open())
		{
			OutputDebugStringW(L"텍스처패키지 파일저장실패\n");
			assert(0);
		}


		std::vector<ResourcePackageEntryMeta> packageEntryMetaVector(materialVector.size());

		

		//header 
			//material num
		unsigned long long materialNum = materialVector.size();
		Write(fout, materialNum);


			//table offset
		unsigned long long tableOffset = sizeof(unsigned long long) * 3;
		Write(fout, tableOffset);

			//raw data offset
		unsigned long long rawFileDataOffset = tableOffset;


		unsigned long long dataOffset = rawFileDataOffset;
		for (size_t i = 0; i < materialNum; ++i)
		{

			packageEntryMetaVector[i].mNameLength = materialVector[i]->GetName().size();
			packageEntryMetaVector[i].mName = materialVector[i]->GetName();
			packageEntryMetaVector[i].mID = materialVector[i]->GetUniqueID();
			packageEntryMetaVector[i].mDataSize = sizeof(MaterialPackageRawData); //material의 고정된값 - 그러면 하나만 저장해도 될듯 지금은 일단 모든 asset들이 최대한 비슷한 구조를 가지게하자
			
	
			
			rawFileDataOffset += sizeof(unsigned long long) * 4 + materialVector[i]->GetName().size();
		}

		Write(fout, rawFileDataOffset);
		dataOffset = rawFileDataOffset;

		for (auto& entryElement : packageEntryMetaVector)
		{
			entryElement.mDataOffset = dataOffset;
			Write(fout,entryElement);
			dataOffset += entryElement.mDataSize;
		}
	
		//raw data

		for (auto material : materialVector)
		{
			Write(fout, material);
		}











	}

	void ResourceStorer::SaveMeshPackage(const std::string& filePath, const std::vector<Mesh*>& meshVector)
	{
		std::ofstream fout(filePath.c_str(), std::ios_base::binary);
		if (!fout.is_open())
		{
			OutputDebugStringW(L"메시패키지 파일저장실패\n");
			assert(0);
		}


		//header 
		//material num
		unsigned long long  meshNum = meshVector.size();
		Write(fout, meshNum);


		//table offset
		unsigned long long tableOffset = sizeof(unsigned long long) * 3;
		Write(fout, tableOffset);

		//raw data offset (맨처음 rawData 시작 offset)
		unsigned long long rawFileDataOffset = tableOffset;


		//(각 rawData시작 offset)
		unsigned long long dataOffset = rawFileDataOffset;


		//table
		std::vector<ResourceMeshPackageEntryMeta> packageMeshEntryMetaVector(meshNum);

		for (size_t i = 0; i < meshNum; ++i)
		{
			Mesh* mesh = meshVector[i];


			packageMeshEntryMetaVector[i].mNameLength = mesh->GetName().size();
			packageMeshEntryMetaVector[i].mName = mesh->GetName();
			packageMeshEntryMetaVector[i].mMeshType = (int)mesh->GetMeshType();
			packageMeshEntryMetaVector[i].mID = mesh->GetUniqueID();
			packageMeshEntryMetaVector[i].mDataSize = GetMeshDataSize(mesh);
			//packageMeshEntryMetaVector[i].mDataOffset = dataOffset;
			dataOffset += packageMeshEntryMetaVector[i].mDataSize;
	
			rawFileDataOffset+= sizeof(unsigned long long) * 4 + 
				sizeof(packageMeshEntryMetaVector[i].mMeshType)+ mesh->GetName().size();

		}
		Write(fout, rawFileDataOffset);

		dataOffset = rawFileDataOffset;
		for (auto& entryElement : packageMeshEntryMetaVector)
		{
			entryElement.mDataOffset = dataOffset;
			Write(fout, entryElement);
			dataOffset += entryElement.mDataSize;
		}

		//raw data

		for (auto meshElement : meshVector)
		{
			WriteMeshCore(fout, meshElement);
		}
	}

	void ResourceStorer::SaveSkeletonPackage(const std::string& filePath, const std::vector<Skeleton*> & skeletonVector)
	{


		std::ofstream fout(filePath.c_str(), std::ios_base::binary);
		if (!fout.is_open())
		{
			OutputDebugStringW(L"스켈레톤패키지 파일저장실패\n");
			assert(0);
		}


		//header 
		//material num
		unsigned long long  skeletonNum = skeletonVector.size();
		Write(fout, skeletonNum);


		//table offset
		unsigned long long tableOffset = sizeof(unsigned long long) * 3;
		Write(fout, tableOffset);

		//raw data offset (맨처음 rawData 시작 offset)
		unsigned long long rawFileDataOffset = tableOffset;


		//(각 rawData시작 offset)
		unsigned long long dataOffset = rawFileDataOffset;


		//table
		std::vector<ResourcePackageEntryMeta> packageEntryMetaVector(skeletonNum);
		for (size_t i = 0; i < skeletonNum; ++i)
		{
			Skeleton* skeleton = skeletonVector[i];


			packageEntryMetaVector[i].mNameLength = skeleton->GetName().size();
			packageEntryMetaVector[i].mName = skeleton->GetName();
			packageEntryMetaVector[i].mID = skeleton->GetUniqueID();
			packageEntryMetaVector[i].mDataSize = GetSkeletonDataSize(skeleton);
			//packageEntryMetaVector[i].mDataOffset = dataOffset;
			dataOffset += packageEntryMetaVector[i].mDataSize;

			rawFileDataOffset += sizeof(unsigned long long) * 4 +
			 +skeleton->GetName().size();

		}
		Write(fout, rawFileDataOffset);

		dataOffset = rawFileDataOffset;
		for (auto& entryElement : packageEntryMetaVector)
		{
			entryElement.mDataOffset = dataOffset;
			Write(fout, entryElement);
			dataOffset += entryElement.mDataSize;
		}

		for (auto skeletonElement : skeletonVector)
		{

			WriteSkeletonCore(fout, skeletonElement);
		}









	}

	void ResourceStorer::SaveAnimationClipPackage(const std::string& filePath, const std::vector<AnimationClip*>& animClipVector)
	{


		std::ofstream fout(filePath.c_str(), std::ios_base::binary);
		if (!fout.is_open())
		{
			OutputDebugStringW(L"에니메이션클립 패키지 파일저장실패\n");
			assert(0);
		}


		//header 
		//material num
		unsigned long long  animClipnNum = animClipVector.size();
		Write(fout, animClipnNum);


		//table offset
		unsigned long long tableOffset = sizeof(unsigned long long) * 3;
		Write(fout, tableOffset);

		//raw data offset (맨처음 rawData 시작 offset)
		unsigned long long rawFileDataOffset = tableOffset;


		//(각 rawData시작 offset)
		unsigned long long dataOffset = rawFileDataOffset;


		//table
		std::vector<ResourcePackageEntryMeta> packageEntryMetaVector(animClipnNum);
		for (size_t i = 0; i < animClipnNum; ++i)
		{
			AnimationClip* animClip = animClipVector[i];


			packageEntryMetaVector[i].mNameLength = animClip->GetName().size();
			packageEntryMetaVector[i].mName = animClip->GetName();
			packageEntryMetaVector[i].mID = animClip->GetUniqueID();
			packageEntryMetaVector[i].mDataSize = GetAnimationDataSize(animClip);
			//packageEntryMetaVector[i].mDataOffset = dataOffset;
			dataOffset += packageEntryMetaVector[i].mDataSize;

			rawFileDataOffset += sizeof(unsigned long long) * 4 +
				+animClip->GetName().size();

		}
		Write(fout, rawFileDataOffset);

		dataOffset = rawFileDataOffset;
		for (auto& entryElement : packageEntryMetaVector)
		{
			entryElement.mDataOffset = dataOffset;
			Write(fout, entryElement);
			dataOffset += entryElement.mDataSize;
		}

		for (auto animClipElement : animClipVector)
		{

			WriteAnimationClipCore(fout, animClipElement);
		}

















	}

	void ResourceStorer::Write(std::ofstream& fout,const DirectX::XMFLOAT2& float2)
	{
		fout.write((const char*)&float2.x, sizeof(float2.x));
		fout.write((const char*)&float2.y, sizeof(float2.y));
	}


	void ResourceStorer::Write(std::ofstream& fout,const DirectX::XMFLOAT3& float3)
	{
		fout.write((const char*)&float3.x, sizeof(float3.x));
		fout.write((const char*)&float3.y, sizeof(float3.y));
		fout.write((const char*)&float3.z, sizeof(float3.z));
	}


	void ResourceStorer::Write(std::ofstream& fout, const DirectX::XMFLOAT4& float4)
	{
		fout.write((const char*)&float4.x, sizeof(float4.x));
		fout.write((const char*)&float4.y, sizeof(float4.y));
		fout.write((const char*)&float4.z, sizeof(float4.z));
		fout.write((const char*)&float4.w, sizeof(float4.w));
	}

	void ResourceStorer::Write(std::ofstream& fout, const DirectX::XMINT4& int4)
	{

		fout.write((const char*)&int4.x, sizeof(int4.x));
		fout.write((const char*)&int4.y, sizeof(int4.y));
		fout.write((const char*)&int4.z, sizeof(int4.z));
		fout.write((const char*)&int4.w, sizeof(int4.w));

	}

	void ResourceStorer::Write(std::ofstream& fout, const DirectX::XMFLOAT4X4& float4x4)
	{
		const size_t size = sizeof(float4x4.m[0][0]);
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				fout.write((const char*)&float4x4.m[i][j], size);
			}
		}

	}

	inline void ResourceStorer::Write(std::ofstream& fout, const int i)
	{
		fout.write(reinterpret_cast<const char*>(&i), sizeof(int));
	}

	inline void ResourceStorer::Write(std::ofstream& fout, const float f)
	{
		fout.write(reinterpret_cast<const char*>(&f), sizeof(float));
	}

	inline void ResourceStorer::Write(std::ofstream& fout, const unsigned long long ull)
	{
		fout.write(reinterpret_cast<const char*>(&ull), sizeof(unsigned long long));
	}

	inline void ResourceStorer::Write(std::ofstream& fout, const std::string& str)
	{
		fout.write(reinterpret_cast<const char*>(str.c_str()), sizeof(char)* str.size());
	}

	inline void ResourceStorer::Write(std::ofstream& fout, const std::uint8_t* ptr, size_t size)
	{
		fout.write(reinterpret_cast<const char*>(ptr), sizeof(uint8_t) * size );

	}

	inline void ResourceStorer::Write(std::ofstream& fout, const Material* material)
	{

		MaterialPackageRawData materialPackageRawData;
		materialPackageRawData.mType = material->GetType();
		materialPackageRawData.mDiffuse = material->GetDiffuse();
		materialPackageRawData.mAmbient = material->GetAmbient();
		materialPackageRawData.mColor = material->GetColor();
		materialPackageRawData.mColorIntensity = material->GetColorIntensity();
		materialPackageRawData.mFresnelR0 = material->GetFresnelR0();
		materialPackageRawData.mShiness = material->GetShiness();
		
		Texture* diffuseMap = material->GetDiffuseMap();
		unsigned long long diffuseMapID = 0;
		if (diffuseMap != nullptr)
			diffuseMapID = diffuseMap->GetUniqueID();

		materialPackageRawData.mDiffuseMapID = diffuseMapID;


		Texture* nomralMap = material->GetNormalMap();
		unsigned long long normalMapID = 0;
		if (nomralMap != nullptr)
			normalMapID = nomralMap->GetUniqueID();
		materialPackageRawData.mNormalMapID = normalMapID;
		
		materialPackageRawData.mEffectID = EffectManager::GetEffect(material->GetEffectName())->GetUniqueID();

		materialPackageRawData.mTextureTransformMatrix[0] = material->GetTextureTransformMatrix(0);
		materialPackageRawData.mTextureTransformMatrix[1] = material->GetTextureTransformMatrix(1);
		materialPackageRawData.mTextureTransformMatrix[2] = material->GetTextureTransformMatrix(2);

		fout.write(reinterpret_cast<const char*>(&materialPackageRawData), sizeof(MaterialPackageRawData));

	}

	

	inline void ResourceStorer::WriteMeshCore(std::ofstream& fout, const Mesh* mesh)
	{

		EMeshType meshType = mesh->GetMeshType();

		unsigned long long vertexNum = mesh->GetVertexNum();
		unsigned long long indexNum = mesh->GetIndexNum();
		const std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3>& minMaxPoint = mesh->GetMinMaxPoint();
		const std::vector<SubMesh>& subMeshVector = mesh->GetSubMeshVector();



		fout.write(reinterpret_cast<const char*>(&vertexNum), sizeof(vertexNum));
		fout.write(reinterpret_cast<const char*>(&indexNum), sizeof(indexNum));

		//min max


		Write(fout, minMaxPoint.first);
		Write(fout, minMaxPoint.second);

		//submesh


		unsigned long long subMeshNum = subMeshVector.size();
		fout.write(reinterpret_cast<const char*>(&subMeshNum), sizeof(subMeshNum));
		unsigned long long materialUniqueID = 0;
		unsigned long long subMeshNameLength = 0;
		for (auto& subMesh : subMeshVector)
		{
			fout.write(reinterpret_cast<const char*>(&subMesh.mID), sizeof(subMesh.mID));
			fout.write(reinterpret_cast<const char*>(&subMesh.mIndexRange.first), sizeof(subMesh.mIndexRange.first));
			fout.write(reinterpret_cast<const char*>(&subMesh.mIndexRange.second), sizeof(subMesh.mIndexRange.second));

			fout.write(reinterpret_cast<const char*>(&subMesh.mVertexNum), sizeof(subMesh.mVertexNum));
			fout.write(reinterpret_cast<const char*>(&subMesh.mVertexOffset), sizeof(subMesh.mVertexOffset));

			materialUniqueID = subMesh.mMaterialPointer->GetUniqueID();
			fout.write(reinterpret_cast<const char*>(&materialUniqueID), sizeof(materialUniqueID));

			const std::string& subMeshName = subMesh.mName;
			subMeshNameLength = subMeshName.size();
			fout.write(reinterpret_cast<const char*>(&subMeshNameLength), sizeof(subMeshNameLength));


			fout.write(reinterpret_cast<const char*>(subMeshName.c_str()), sizeof(char) * subMeshNameLength);

		}





		if (meshType == EMeshType::eSknningMesh)
		{
			const SkinningMesh* skinningMesh = static_cast<const SkinningMesh*>(mesh);

			const std::vector<SkinningVertex>& vertexVector = skinningMesh->GetVertexVector();
			for (auto& vertex : vertexVector)
			{

				Write(fout, vertex.mPos);
				Write(fout, vertex.mNormal);
				Write(fout, vertex.mTex);
				Write(fout, vertex.mBoneWeight[0]);
				Write(fout, vertex.mBoneWeight[1]);
				Write(fout, vertex.mBoneIndex[0]);
				Write(fout, vertex.mBoneIndex[1]);
			}

		}
		else if (meshType == EMeshType::eStaticMesh)
		{
			const StaticMesh* staticMesh = static_cast<const StaticMesh*>(mesh);

			const std::vector<StaticVertex>& vertexVector = staticMesh->GetVertexVector();
			for (auto& vertex : vertexVector)
			{
				Write(fout, vertex.mPos);
				Write(fout, vertex.mNormal);
				Write(fout, vertex.mTex);
			}
		}


		const std::vector<MeshIndexType>& indexVector = mesh->GetIndexVector();

		for (auto& index : indexVector)
		{
			fout.write(reinterpret_cast<const char*>(&index), sizeof(index));
		}



	}

	inline void ResourceStorer::WriteHeader(std::ofstream& fout, unsigned long long entryNum)
	{
		//num
		Write(fout, entryNum);

		//table offset
		unsigned long long tableOffset = sizeof(unsigned long long) * 3;
		Write(fout, tableOffset);



	}

	

	inline void ResourceStorer::Write(std::ofstream& fout, const ResourcePackageEntryMeta& resourcePackageEntryMeta)
	{
		Write(fout, resourcePackageEntryMeta.mNameLength);

		const std::string& name = resourcePackageEntryMeta.mName;

		Write(fout, name);

		Write(fout, resourcePackageEntryMeta.mID);

		Write(fout, resourcePackageEntryMeta.mDataSize);

		Write(fout, resourcePackageEntryMeta.mDataOffset);

	}

	inline void ResourceStorer::Write(std::ofstream& fout, const ResourceMeshPackageEntryMeta& resourcePackageMeshEntryMeta)
	{
		Write(fout, resourcePackageMeshEntryMeta.mNameLength);

		const std::string& name = resourcePackageMeshEntryMeta.mName;

		Write(fout, name);

		Write(fout, resourcePackageMeshEntryMeta.mMeshType);

		Write(fout, resourcePackageMeshEntryMeta.mID);

		Write(fout, resourcePackageMeshEntryMeta.mDataSize);

		Write(fout, resourcePackageMeshEntryMeta.mDataOffset);


	}

	inline void ResourceStorer::WriteSkeletonCore(std::ofstream& fout, Skeleton* skeleton)
	{

		//bone 
		size_t boneNum = skeleton->GetBoneVector().size();

		fout.write(reinterpret_cast<const char*>(&boneNum), sizeof(boneNum));

		const std::vector<Bone* >& boneVector = skeleton->GetBoneVector();
		const std::vector<int>& boneHiererachyVector = skeleton->GetBoneHiererachyVector();
		const std::vector<DirectX::XMFLOAT4X4>& mOffsetMatrixVector = skeleton->GetOffsetMatrixVector();



		SaveBone(fout, boneVector);
		fout.write(reinterpret_cast<const char*>(boneHiererachyVector.data()), sizeof(int) * boneHiererachyVector.size());
		for (auto& offsetMatrix : mOffsetMatrixVector)
		{
			Write(fout, offsetMatrix);
		}


	}

	inline void ResourceStorer::WriteAnimationClipCore(std::ofstream& fout, AnimationClip* animClip)
	{

		int endFrameCount = animClip->GetEndFrameCount();
		Write(fout, endFrameCount);

		float endSecondTime = animClip->GetEndTimeSecond();
		Write(fout, endSecondTime);

		int timeMode = static_cast<int>(animClip->GetTimeMode());
		Write(fout, timeMode);


		Mesh* defaultMesh = animClip->GetDefaultMesh();
		unsigned long long defaultMeshID = defaultMesh->GetUniqueID();

		Write(fout, defaultMeshID);

		Skeleton* defaultSkeleton = animClip->GetDefaultSkeleton();
		unsigned long long defaultSkeletonID = defaultSkeleton->GetUniqueID();
		Write(fout, defaultSkeletonID);



		const std::vector<BoneAnimationClip>& boneAnimationClipVector = animClip->GetBoneAnimationClipVector();

		size_t  boneAnimClipNum = boneAnimationClipVector.size();
		Write(fout, boneAnimClipNum);


		for (const auto& boneAnimClip : boneAnimationClipVector)
		{
			int boneID = boneAnimClip.GetBoneID();
			Write(fout, boneID);

			const std::vector<AnimationKeyFrame>& animKeyFrameVector = boneAnimClip.GetKeyFrameVector();

			size_t keyFrameNum = animKeyFrameVector.size();
			Write(fout, keyFrameNum);

			for (const auto& keyFrame : animKeyFrameVector)
			{
				Write(fout, keyFrame.mScale);
				Write(fout, keyFrame.mQuaternion);
				Write(fout, keyFrame.mPosition);
				Write(fout, keyFrame.mTime);
			}

		}




	}

	void ResourceStorer::SaveBone(std::ofstream& fout, const std::vector<Bone*>& boneVector)
	{


		for (auto& bone : boneVector)
		{
			const std::string boneName = bone->GetName();
			size_t boneNameLength = boneName.size();

			fout.write(reinterpret_cast<const char*>(&boneNameLength), sizeof(boneNameLength));
			fout.write(boneName.c_str(), sizeof(char) * boneNameLength);

			unsigned int boneType = (unsigned int )bone->GetBoneType();
			fout.write(reinterpret_cast<const char*>(&boneType), sizeof(boneType));
		}




	}

	unsigned long long ResourceStorer::GetMeshDataSize(Mesh* mesh)
	{

		unsigned long long dataSize = 0;


		EMeshType meshType = mesh->GetMeshType();
		//mesh type
		dataSize += sizeof(EMeshType);

		//min,max
		dataSize += sizeof(DirectX::XMFLOAT3) * 2;

		//sub mesh num
		dataSize += sizeof(unsigned long long);

		//sub mesh data
		const std::vector<SubMesh>& subMeshVector = mesh->GetSubMeshVector();

		for (size_t i = 0; i < subMeshVector.size(); ++i)
		{
			//name length
			dataSize += sizeof(unsigned long long);
			//name
			dataSize += subMeshVector[i].mName.size();

			dataSize += sizeof(subMeshVector[i].mIndexRange.first);
			dataSize += sizeof(subMeshVector[i].mIndexRange.second);

			dataSize += sizeof(subMeshVector[i].mVertexNum);
			dataSize += sizeof(subMeshVector[i].mVertexOffset);

			//material id
			dataSize += sizeof(unsigned long long);


			//submesh id
			dataSize += sizeof(subMeshVector[i].mID);

		}


		//vertex num
		dataSize += sizeof(mesh->GetVertexNum());


		//vertex data
		if (meshType == EMeshType::eSknningMesh)
		{
			dataSize += mesh->GetVertexNum()* (sizeof(SkinningVertex::mBoneIndex)+ sizeof(SkinningVertex::mBoneWeight)
				+ sizeof(SkinningVertex::mNormal)+ sizeof(SkinningVertex::mPos)+ sizeof(SkinningVertex::mTex));

		}
		else if (meshType == EMeshType::eStaticMesh)
		{
			dataSize += (sizeof(StaticVertex::mNormal)+sizeof(StaticVertex::mPos)+sizeof(StaticVertex::mTex)
				)* mesh->GetVertexNum();
		}


		//index num
		dataSize += sizeof(mesh->GetIndexNum());

		//index data
		dataSize += sizeof(MeshIndexType) * mesh->GetIndexNum();











		return dataSize;
	}

	unsigned long long ResourceStorer::GetSkeletonDataSize(Skeleton* skeleton)
	{
		unsigned long long dataSize = 0;


		
		const std::vector<Bone*>& boneVector = skeleton->GetBoneVector();
		const std::vector<int>& boneHiererachyVector= skeleton->GetBoneHiererachyVector();
		const std::vector<DirectX::XMFLOAT4X4> & offsetMatrixVector = skeleton->GetOffsetMatrixVector();


		//bone Num 
		dataSize += sizeof(size_t);



		for (size_t i = 0; i < boneVector.size(); ++i)
		{
			const std::string& name = boneVector[i]->GetName();

			//name length 
			dataSize += sizeof(size_t);

			//name 
			dataSize += name.size();

			//bone type
			dataSize += sizeof(EBoneType);

			//Transform		//임시적
			//DirectX::XMFLOAT3 mPosition;
		//DirectX::XMFLOAT3 mScale;
		//DirectX::XMFLOAT3 mRotation;
			dataSize += sizeof(DirectX::XMFLOAT3) * 3;
		}


		//아마 각 벡터의 size가 bone num이랑 같은거같다.
		dataSize += sizeof(int) * boneHiererachyVector.size();


	
		dataSize += sizeof(DirectX::XMFLOAT4X4) * offsetMatrixVector.size();








		return dataSize;
	}

	unsigned long long ResourceStorer::GetAnimationDataSize(AnimationClip* animClip)
	{

		unsigned long long dataSize = 0;


		





		int endFrameCount = animClip->GetEndFrameCount();
		dataSize += sizeof(endFrameCount);

		float endSecondTime = animClip->GetEndTimeSecond();
		dataSize += sizeof(endSecondTime);

		int timeMode = static_cast<int>(animClip->GetTimeMode());
		dataSize += sizeof(timeMode);


		Mesh* defaultMesh = animClip->GetDefaultMesh();
		unsigned long long defaultMeshID = defaultMesh->GetUniqueID();

		dataSize += sizeof(defaultMeshID);

		Skeleton* defaultSkeleton = animClip->GetDefaultSkeleton();
		unsigned long long defaultSkeletonID = defaultSkeleton->GetUniqueID();
		dataSize += sizeof(defaultSkeletonID);



		const std::vector<BoneAnimationClip>& boneAnimationClipVector = animClip->GetBoneAnimationClipVector();

		size_t  boneAnimClipNum = boneAnimationClipVector.size();
		dataSize += sizeof(boneAnimClipNum);


		for (const auto& boneAnimClip : boneAnimationClipVector)
		{
			int boneID = boneAnimClip.GetBoneID();
			dataSize += sizeof(boneID);

			const std::vector<AnimationKeyFrame>& animKeyFrameVector = boneAnimClip.GetKeyFrameVector();

			size_t keyFrameNum = animKeyFrameVector.size();
			dataSize += sizeof(keyFrameNum);

			for (const auto& keyFrame : animKeyFrameVector)
			{
				dataSize += sizeof(keyFrame.mScale);
				dataSize += sizeof(keyFrame.mQuaternion);
				dataSize += sizeof(keyFrame.mPosition);
				dataSize += sizeof(keyFrame.mTime);
			}

		}











		return dataSize;
	}



}