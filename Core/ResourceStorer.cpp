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
#include<sstream>


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

			const std::string & subMeshName = subMesh.mName;
			subMeshNameLength = subMeshName.size();
			fout.write(reinterpret_cast<const char*>(&subMeshNameLength), sizeof(subMeshNameLength));

		
			fout.write(reinterpret_cast<const char*>(subMeshName.c_str()), sizeof(char) * subMeshNameLength);

		}


		








		if (meshType == EMeshType::eSknningMesh)
		{
			SkinningMesh* skinningMesh = static_cast<SkinningMesh*>(mesh);

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
			StaticMesh* staticMesh = static_cast<StaticMesh*>(mesh);

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
	


		

		//bone 
		size_t boneNum = skeleton->GetBoneVector().size();
	
		fout.write(reinterpret_cast<const char*>(&boneNum), sizeof(boneNum));

		const std::vector<Bone * >& boneVector = skeleton->GetBoneVector();
		const std::vector<int>& boneHiererachyVector = skeleton->GetBoneHiererachyVector();
		const std::vector<DirectX::XMFLOAT4X4> & mOffsetMatrixVector = skeleton->GetOffsetMatrixVector();



		SaveBone(fout, boneVector);
		fout.write(reinterpret_cast<const char*>(boneHiererachyVector.data()), sizeof(int) * boneHiererachyVector.size());
		for (auto& offsetMatrix : mOffsetMatrixVector)
		{
			Write(fout, offsetMatrix);
		}


		


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


		int endFrameCount =	 animClip->GetEndFrameCount();
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



}