#include "ResourceLoader.h"
#include"Utility.h"

#include"Parser/JsonParser.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"ResourceManager/SkeletonManager.h"
#include"ResourceManager/AnimationClipManager.h"


#include"ObjectManager/ObjectManager.h"
#include"Object/Object.h"
#include"Asset/Skeleton/Skeleton.h"

#include"Asset/Animation/AnimationClip.h"
#include"Asset/Animation/BoneAnimationClip.h"



#include"Core/BoneFactory.h"
#include"GraphicCommand.h"

#include"ResourceFileType.h"


#include"DirectTexX/DirectXTex.h"


namespace Quad
{
	void ResourceLoader::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, GraphicCommand* graphicsCommandObject,
		DescriptorHeapManagerMaster * descriptorHeapManagerMaster)
	{
		mDevice = device;
		mGraphicsCommandObject = graphicsCommandObject;
		mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;
		mTextureLoader.Initialize(device, graphicsCommandObject);
		mEffectLoader.Initialize(device);
	}

	Mesh * ResourceLoader::LoadMeshFile(const std::string& filePath)
	{
		/*if(mGraphicsCommandObject->GetCloseState())
			mGraphicsCommandObject->ResetCommandList(nullptr);*/

		//20250313version1

		std::ifstream fin(filePath, std::ios_base::binary);
		if (!fin.is_open())
		{
			const std::string errorMessage = filePath + "(메시)파일열기 실패\n";
			OutputDebugStringA(errorMessage.c_str());
			assert(0);
		}
		
		unsigned long long assetID = 0;
		int meshTypeInt = 0;
		unsigned int meshNameLength = 0;

		unsigned long long vertexNum = 0;
		unsigned long long indexNum = 0;
		
		std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> minMaxPoint;
		unsigned long long subMeshNum = 0;



		fin.read(reinterpret_cast<char*>(&assetID), sizeof(assetID));
		fin.read(reinterpret_cast<char*>(&meshTypeInt), sizeof(meshTypeInt));
		fin.read(reinterpret_cast<char*>(&meshNameLength), sizeof(meshNameLength));

		std::unique_ptr<char> meshNameArrayUptr (new char[meshNameLength+1]);

		char * meshNameArray = meshNameArrayUptr.get();
		meshNameArray[meshNameLength] = '\0';

		fin.read(meshNameArray, sizeof(char) * meshNameLength);


		EMeshType meshType = static_cast<EMeshType>(meshTypeInt);


		Mesh* mesh = MeshManager::CreateMeshFromFile(meshNameArray, meshType, assetID);
		if (mesh == nullptr)
			return nullptr;

		ReadMeshCore(fin,mesh);


		//fin.read(reinterpret_cast<char*>(&vertexNum), sizeof(vertexNum));
		//fin.read(reinterpret_cast<char*>(&indexNum), sizeof(indexNum));


		////min max point

		//Read(fin, minMaxPoint.first);
		//Read(fin, minMaxPoint.second);


		////submesh
		//fin.read(reinterpret_cast<char*>(&subMeshNum), sizeof(subMeshNum));
		//
		//std::vector<SubMesh> subMeshVector;
		//subMeshVector.resize(subMeshNum);

		//unsigned long long materialUniqueID = 0;
		//unsigned long long subMeshNameLength = 0;
		//for (auto& subMesh : subMeshVector)
		//{
		//	fin.read(reinterpret_cast<char*>(&subMesh.mID), sizeof(subMesh.mID));
		//	fin.read(reinterpret_cast<char*>(&subMesh.mIndexRange.first), sizeof(subMesh.mIndexRange.first));
		//	fin.read(reinterpret_cast<char*>(&subMesh.mIndexRange.second), sizeof(subMesh.mIndexRange.second));

		//	fin.read(reinterpret_cast<char*>(&subMesh.mVertexNum), sizeof(subMesh.mVertexNum));
		//	fin.read(reinterpret_cast<char*>(&subMesh.mVertexOffset), sizeof(subMesh.mVertexOffset));

		//	
		//	fin.read(reinterpret_cast<char*>(&materialUniqueID), sizeof(materialUniqueID));
		//	subMesh.mMaterialPointer = materialUniqueID;
		//	fin.read(reinterpret_cast<char*>(&subMeshNameLength), sizeof(subMeshNameLength));


		//	std::string subMeshName(subMeshNameLength,0);
		//	fin.read(subMeshName.data(), sizeof(char) * subMeshNameLength);
		//
		//	subMesh.mName = subMeshName;
		//	subMesh.mMesh = mesh;

		//}


		//Microsoft::WRL::ComPtr<ID3D12Resource> vertexDefaultBuffer;
		//Microsoft::WRL::ComPtr<ID3D12Resource> indexDefaultBuffer;
		//Microsoft::WRL::ComPtr<ID3D12Resource> vertexUploadBuffer;
		//Microsoft::WRL::ComPtr<ID3D12Resource> indexUploadBuffer;

		//std::vector<SkinningVertex> skinningVertexVector;
		//std::vector<StaticVertex> staticVertexVector;
		//if (meshType == EMeshType::eSknningMesh)
		//{
		//	skinningVertexVector.resize(vertexNum);
		//	for(auto & vertex : skinningVertexVector)
		//	{ 
		//		Read(fin, vertex.mPos);
		//		Read(fin, vertex.mNormal);
		//		Read(fin, vertex.mTex);
		//		Read(fin, vertex.mBoneWeight[0]);
		//		Read(fin, vertex.mBoneWeight[1]);
		//		Read(fin, vertex.mBoneIndex[0]);
		//		Read(fin, vertex.mBoneIndex[1]);
		//	}

		//	vertexDefaultBuffer = Utility::CreateDefaultBuffer(mDevice, mGraphicsCommandObject->GetGraphicsCommandList(), skinningVertexVector.data(), sizeof(SkinningVertex), vertexNum, vertexUploadBuffer);
		//}
		//else if (meshType == EMeshType::eStaticMesh)
		//{
		//	staticVertexVector.resize(vertexNum);
		//	for (auto& vertex : staticVertexVector)
		//	{
		//		Read(fin, vertex.mPos);
		//		Read(fin, vertex.mNormal);
		//		Read(fin, vertex.mTex);
		//	}

		//	vertexDefaultBuffer = Utility::CreateDefaultBuffer(mDevice, mGraphicsCommandObject->GetGraphicsCommandList(), staticVertexVector.data(), sizeof(StaticVertex), vertexNum, vertexUploadBuffer);
		//}


		//std::vector<MeshIndexType> indexVector(indexNum);
		//for (auto& index : indexVector)
		//{
		//	fin.read(reinterpret_cast<char*>(&index), sizeof(index));
		//}

		//fin.close();


		//indexDefaultBuffer = Utility::CreateDefaultBuffer(mDevice, mGraphicsCommandObject->GetGraphicsCommandList(), indexVector.data(), sizeof(MeshIndexType), indexNum, indexUploadBuffer);


		//mGraphicsCommandObject->ExecuteCommandList();
		//mGraphicsCommandObject->FlushCommandQueue();
	




		//if (meshType == EMeshType::eSknningMesh)
		//{
		//	SkinningMesh* skinningMesh = static_cast<SkinningMesh*>(mesh);
		//	skinningMesh->SetVertexVector(std::move(skinningVertexVector));
		//}
		//else if (meshType == EMeshType::eStaticMesh)
		//{
		//	StaticMesh* staticMesh = static_cast<StaticMesh*>(mesh);
		//	staticMesh->SetVertexVector(std::move(staticVertexVector));
		//}

		//mesh->SetVertexNum(vertexNum);
		//mesh->SetIndexNum(indexNum);
		//mesh->SetMinMaxPoint(minMaxPoint.first, minMaxPoint.second);
		//mesh->SetSubMeshVector(std::move(subMeshVector));
	

		//mesh->SetVertexBuffer(vertexDefaultBuffer);
		//mesh->SetIndexBuffer(indexDefaultBuffer);
		//mesh->SetIndexVector(std::move(indexVector));



	

		return mesh;
	

	}

	/*bool ResourceLoader::LoadMaterialFile(const std::string& filePath, RawMaterialData& oRawMaterialData)
	{
		const std::string& extension = Utility::GetExtension(filePath);
		if (extension !=".mat")
			return false;

		return true;

	}*/

	Texture *  ResourceLoader::LoadTextureFile(const std::string& filePath)
	{
		
		std::string fileName = Utility::GetFileNameFromPath(filePath);

		Texture* texture = TextureManager::GetTexture(fileName);
		if (texture != nullptr)
		{
			return nullptr;
		}


		texture = TextureManager::CreateTexture(fileName);
		bool ret = mTextureLoader.LoadTextureFile(filePath, *texture);
		if (ret == false)
		{
			TextureManager::KillTexture(texture);	
			return nullptr;
		}



		TextureFactory::CreateSrvTex2D(texture);

		return texture;
	}

	Texture* ResourceLoader::LoadTextureFromFile(const std::string& filePath, const std::string & unqiueName, unsigned long long uniqueID)
	{

		std::string fileName = Utility::GetFileNameFromPath(filePath);

		Texture* texture = TextureManager::GetTexture(fileName);
		if (texture != nullptr)
		{
			return nullptr;
		}


		texture = TextureManager::CreateTextureFromFile(unqiueName,uniqueID);
		bool ret = mTextureLoader.LoadTextureFile(filePath, *texture);
		if (ret == false)
		{
			TextureManager::KillTexture(texture);
			return nullptr;
		}



		TextureFactory::CreateSrvTex2D(texture);

		return texture;
	}

	//bool ResourceLoader::LoadTexture(const std::wstring& filePath, Texture& oTextureData)
	//{
	//	return mTextureLoader.LoadTexture(filePath, oTextureData);
	//}

	/*bool ResourceLoader::LoadFbxFile(const std::string& filePath, std::vector<RawMeshData*>& oRawMeshDataVector, 
		std::vector<RawMaterialData>&  oRawMaterialDataVector, std::vector<RawTextureData> & oRawTextureDataVector,
		Skeleton*& oSkeleton,
		AnimationClip*& oAnimationClip, LoadResultCommonData& loadResultCommonData)
	{
		mFbxParser.LoadFbxFile(filePath,oRawMeshDataVector,oRawMaterialDataVector,oRawTextureDataVector,oSkeleton,oAnimationClip,
			loadResultCommonData);









		return true;
	}*/

	bool ResourceLoader::LoadSchema(const std::wstring& filePath, RawSchemaData& oSchemaData)
	{
		//return mSchemaLoader.Load(filePath, oSchemaData);
		return true;
	}


	bool ResourceLoader::LoadEffect(const std::string& filePath)
	{
		 mEffectLoader.Load(filePath);
		 return true; 


	}

	void ResourceLoader::LoadProjectData()
	{

		//순서 중요
		//effect
		LoadProjectEffect();

		//texture
		//mat
		//mesh
		LoadProjectAsset();


		//object(entity)
		LoadProjectObject();

	}

	Material * ResourceLoader::LoadMaterial(const std::string& filePath)
	{

		JsonParser::ReadFile(filePath);
		JsonParser::ReadStart();
		
		unsigned long long uniqueID = 0;
		std::string name;
		JsonParser::Read("ContentItem_Name", name);
		JsonParser::Read("Asset_ID", uniqueID);

		Material * material =  MaterialManager::CreateMaterialFromFile(name, uniqueID);

		if (material != nullptr)	//nullptr은 이미로드했다는의미(같은게존재한다는의미,게임엔진에서 기본제공하는 assset을은 실패할것이다. 이미 엔진실행초기에 로드해버리니깐)
			material->DeSerialize();
		else
			return material;

		return material;
	}

	Skeleton* ResourceLoader::LoadSkeleton(const std::string& filePath)
	{

		std::ifstream fin(filePath,std::ios_base::binary);
		
		if (!fin.is_open())
		{
			const std::string errorMessage = filePath + "(스켈레톤)파일열기 실패\n";
			OutputDebugStringA(errorMessage.c_str());
			assert(0);
		}

		//name

		size_t  nameLength = 0;
		Read(fin, nameLength);


		std::unique_ptr<char> nameBuffer = std::unique_ptr<char>(new char[nameLength+1]);
		fin.read(nameBuffer.get(), sizeof(char) * nameLength);
		nameBuffer.get()[nameLength] = '\0';

		//id 

		unsigned long long uniqueID = 0;
		Read(fin, uniqueID);

	//	SkeletonManager

		Skeleton * skeleton =  SkeletonManager::CreateSkeletonFromFile(nameBuffer.get(), uniqueID);


		//bone 
		ReadSkeletonCore(fin, skeleton);




		//return skeleton;
		return skeleton;
	}

	AnimationClip* ResourceLoader::LoadAnimationClip(const std::string& filePath)
	{
			

		std::ifstream fin(filePath,std::ios_base::binary);

		if (!fin.is_open())
		{
			const std::wstring errorMessage =Utility::ConvertToWString(filePath,true) + L"(에니메이션클립)파일열기 실패\n";
			OutputDebugStringW(errorMessage.c_str());
			assert(0);

		}
		

		//name
		size_t nameLength = 0;
		Read(fin, nameLength);


		std::unique_ptr<char> nameBuffer(new char[nameLength+1]);
		fin.read(nameBuffer.get(), sizeof(char) * nameLength);
		nameBuffer.get()[nameLength] = '\0';
	
		//id

		unsigned long long uniqueID = 0;
		Read(fin, uniqueID);




		AnimationClip* animClip = AnimationClipManager::CreateAnimationClipFromFile(nameBuffer.get(), uniqueID);

		ReadAnimationClipCore(fin, animClip);
	/*	int endFrameCount = 0;
		Read(fin, endFrameCount);
		animClip->SetFrameEndCount(endFrameCount);


		float endSecondTime = 0.0f;
		Read(fin, endSecondTime);
		animClip->SetEndTimeSecond(endSecondTime);
		


		int timeMode = 0;
		Read(fin, timeMode);
		animClip->SetTimeMode(Quad::EAnimationTimeMode(timeMode));



		unsigned long long defaultMeshID = 0;
		Read(fin, defaultMeshID);
		animClip->SetDefaultMesh(MeshManager::GetMesh(defaultMeshID));
	

		unsigned long long defaultSkeletonID = 0;
		Read(fin, defaultSkeletonID);
		animClip->SetDefaultSkeleton(SkeletonManager::GetSkeleton(defaultSkeletonID));


		size_t  boneAnimClipNum = 0;
		Read(fin,boneAnimClipNum);

		std::vector<BoneAnimationClip> boneAnimationClipVector(boneAnimClipNum);

		for (auto& boneAnimClip : boneAnimationClipVector)
		{
			int boneID = 0; 
			Read(fin, boneID);

			boneAnimClip.SetBoneID(boneID);

			size_t keyFrameNum = 0;
			Read(fin, keyFrameNum);

			std::vector<AnimationKeyFrame> animKeyFrameVector(keyFrameNum);

			for (auto& keyFrame : animKeyFrameVector)
			{
				Read(fin, keyFrame.mScale);
				Read(fin, keyFrame.mQuaternion);
				Read(fin, keyFrame.mPosition);
				Read(fin, keyFrame.mTime);
			}

			boneAnimClip.SetAnimationKeyFrameVector(std::move(animKeyFrameVector));

		}

		animClip->SetBoneAnimationClipVector(std::move(boneAnimationClipVector));
*/




		return animClip;
	}

	void ResourceLoader::LoadTexturePackage(const std::string& filePath)
	{

		std::ifstream fin(filePath, std::ios_base::binary);


		if (!fin.is_open())
		{
			OutputDebugStringW(L"텍스처 패키지 파일없음\n");
			assert(0);
		}


		//header

			//file num
		unsigned long long fileNum = 0;
		Read(fin, fileNum);

			//table offset
		unsigned long long tableOffset = 0;
		Read(fin, tableOffset);

			//file data offset
		unsigned long long fileDataOffset = 0;
		Read(fin, fileDataOffset);
		

		//table

		std::vector<ResourcePackageEntryMeta> packageEntyMetaVector(fileNum);
		Read(fin, packageEntyMetaVector);
		

		//멀티스레드로 텍스처파일들을 동시에 읽어드리자

		int threadNum = 1;

		//파일개수에 따라 thread  num 설정

		//일단 단일스레드로 
		std::vector<DirectX::Blob> blobVector(fileNum);

		for (size_t i = 0; i < fileNum; ++i)
		{
			fin.seekg(packageEntyMetaVector[i].mDataOffset);
			blobVector[i].Initialize(packageEntyMetaVector[i].mDataSize);
			fin.read(reinterpret_cast<char*>(blobVector[i].GetBufferPointer()), packageEntyMetaVector[i].mDataSize);
		}

 		

		TextureManager* textureManager = TextureManager::GetInstance();

		for (size_t i = 0; i < fileNum; ++i)
		{
			//이름이 에디터에서사용하던 경로 구성되어야 한다 여전히
			Texture* texture = textureManager->CreateTextureFromFile(packageEntyMetaVector[i].mName, packageEntyMetaVector[i].mID);
			if(texture == nullptr)
			{
				//에러출력
				continue;
			}

			mTextureLoader.LoadDDSFileFromMemory(blobVector[i].GetBufferPointer(), blobVector[i].GetBufferSize(), *texture);
			TextureFactory::CreateSrvTex2D(texture);
		}









	}

	void ResourceLoader::LoadMaterialPackage(const std::string& filePath)
	{

		std::ifstream fin(filePath, std::ios::binary);
		if (!fin.is_open())
		{
			OutputDebugStringW(L"텍스처 패키지 파일없음\n");
			assert(0);
		}

		//header 
		unsigned long long materialNum = 0;
		Read(fin, materialNum);


		unsigned long long tableOffset = 0;
		Read(fin, tableOffset);

		unsigned long long rawDataOffset = 0;

		Read(fin, rawDataOffset);


		//table
	
		std::vector<ResourcePackageEntryMeta> resourcePackageEntryMetaVector(materialNum);
		Read(fin, resourcePackageEntryMetaVector);



		MaterialManager * materialManager =	MaterialManager::GetInstance();

		//raw data
		for (size_t i = 0; i < materialNum; ++i)
		{
			Material * material = materialManager->CreateMaterialFromFile(resourcePackageEntryMetaVector[i].mName, resourcePackageEntryMetaVector[i].mID);	


			if (material == nullptr)
			{

				OutputDebugStringW(L"머터리얼 패키지 - 머터리얼생성실패\n");
				assert(0);


			}

			Read(fin, material);

		}


	}

	void ResourceLoader::LoadMeshPackage(const std::string& filePath)
	{

		if (mGraphicsCommandObject->GetCloseState())
			mGraphicsCommandObject->ResetCommandList(nullptr);

		std::ifstream fin(filePath, std::ios::binary);


		if (!fin.is_open())
		{
			OutputDebugStringW(L"메시 패키지 파일없음\n");
			assert(0);
		}

		//header 
		unsigned long long meshlNum = 0;
		Read(fin, meshlNum);


		unsigned long long tableOffset = 0;
		Read(fin, tableOffset);

		unsigned long long rawDataOffset = 0;

		Read(fin, rawDataOffset);


		//table

		std::vector<ResourceMeshPackageEntryMeta> resourceMeshPackageEntryMetaVector(meshlNum);
		Read(fin, resourceMeshPackageEntryMetaVector);


		MeshManager* meshManager = MeshManager::GetInstance();

		//raw data
		for (size_t i = 0; i < meshlNum; ++i)
		{

			Mesh* mesh = meshManager->CreateMeshFromFile(resourceMeshPackageEntryMetaVector[i].mName,
				(EMeshType)resourceMeshPackageEntryMetaVector[i].mMeshType, resourceMeshPackageEntryMetaVector[i].mID);


			if (mesh == nullptr)
			{

				OutputDebugStringW(L"메시 패키지 -메시 생성실패\n");
				assert(0);


			}

			ReadMeshCore(fin, mesh);

		}








	}

	void ResourceLoader::LoadSkeletonPackage(const std::string& filePath)
	{



		std::ifstream fin(filePath, std::ios::binary);
		if (!fin.is_open())
		{
			OutputDebugStringW(L"스켈레톤 패키지 파일없음\n");
			assert(0);
		}

		//header 
		unsigned long long skeletonlNum = 0;
		Read(fin, skeletonlNum);


		unsigned long long tableOffset = 0;
		Read(fin, tableOffset);

		unsigned long long rawDataOffset = 0;

		Read(fin, rawDataOffset);


		//table

		std::vector<ResourcePackageEntryMeta> resourcePackageEntryMetaVector(skeletonlNum);
		Read(fin, resourcePackageEntryMetaVector);


		SkeletonManager* skeletonManager = SkeletonManager::GetInstance();

		//raw data
		for (size_t i = 0; i < skeletonlNum; ++i)
		{

			Skeleton* skeleton = skeletonManager->CreateSkeletonFromFile(resourcePackageEntryMetaVector[i].mName,
				resourcePackageEntryMetaVector[i].mID);


			if (skeleton == nullptr)
			{

				OutputDebugStringW(L"스켈레톤 패키지 -스켈레톤 생성실패\n");
				assert(0);


			}

			ReadSkeletonCore(fin, skeleton);
		}








	}

	void ResourceLoader::LoadAnimClipPackage(const std::string& filePath)
	{

		std::ifstream fin(filePath, std::ios::binary);
		if (!fin.is_open())
		{
			OutputDebugStringW(L"에니메이션클립 패키지 파일없음\n");
			assert(0);
		}

		//header 
		unsigned long long animClipNum = 0;
		Read(fin, animClipNum);


		unsigned long long tableOffset = 0;
		Read(fin, tableOffset);

		unsigned long long rawDataOffset = 0;

		Read(fin, rawDataOffset);


		//table

		std::vector<ResourcePackageEntryMeta> resourcePackageEntryMetaVector(animClipNum);
		Read(fin, resourcePackageEntryMetaVector);


		AnimationClipManager* animClipManager = AnimationClipManager::GetInstance();

		//raw data
		for (size_t i = 0; i < animClipNum; ++i)
		{

			AnimationClip* animClip = animClipManager->CreateAnimationClipFromFile(resourcePackageEntryMetaVector[i].mName,
				resourcePackageEntryMetaVector[i].mID);


			if (animClip == nullptr)
			{

				OutputDebugStringW(L"에니메이션클립 패키지 -에니메이션클립 생성실패\n");
				assert(0);


			}

			ReadAnimationClipCore(fin, animClip);
		}
























	}

	inline void ResourceLoader::Read(std::ifstream& fin, DirectX::XMFLOAT2& float2)
	{
		fin.read((char*)&float2.x, sizeof(float2.x));
		fin.read((char*)&float2.y, sizeof(float2.y));
		
	}

	inline void ResourceLoader::Read(std::ifstream& fin, DirectX::XMFLOAT3& float3)
	{
		fin.read((char*)&float3.x, sizeof(float3.x));
		fin.read((char*)&float3.y, sizeof(float3.y));
		fin.read((char*)&float3.z, sizeof(float3.z));


	}

	inline void ResourceLoader::Read(std::ifstream& fin, DirectX::XMFLOAT4& float4)
	{
		fin.read((char*)&float4.x, sizeof(float4.x));
		fin.read((char*)&float4.y, sizeof(float4.y));
		fin.read((char*)&float4.z, sizeof(float4.z));
		fin.read((char*)&float4.w, sizeof(float4.w));
	}

	inline void ResourceLoader::Read(std::ifstream& fin, DirectX::XMINT4& int4)
	{
		fin.read((char*)&int4.x, sizeof(int4.x));
		fin.read((char*)&int4.y, sizeof(int4.y));
		fin.read((char*)&int4.z, sizeof(int4.z));
		fin.read((char*)&int4.w, sizeof(int4.w));
	}

	inline void ResourceLoader::Read(std::ifstream& fin, DirectX::XMFLOAT4X4& float4x4)
	{

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				Read(fin, float4x4.m[i][j]);
			}
		}

	}

	inline void ResourceLoader::Read(std::ifstream& fin, float& f)
	{
		fin.read(reinterpret_cast<char*>(&f), sizeof(float));
	}

	inline void ResourceLoader::Read(std::ifstream& fin, int& i)
	{
		fin.read(reinterpret_cast<char*>(&i), sizeof(int));
	}

	inline void ResourceLoader::Read(std::ifstream& fin, unsigned long long& ull)
	{
		fin.read(reinterpret_cast<char*>(&ull), sizeof(unsigned long long));
	}

	inline void ResourceLoader::Read(std::ifstream& fin, unsigned int& ui)
	{
		fin.read(reinterpret_cast<char*>(&ui), sizeof(unsigned int));
	}

	inline void ResourceLoader::Read(std::ifstream& fin, char* cp, size_t size)
	{
		fin.read(cp, sizeof(char) * size);

	}

	inline void ResourceLoader::Read(std::ifstream& fin, std::vector<ResourcePackageEntryMeta>& oResourcePackageEntryMetaVector)
	{




		unsigned long long nameLength = 0;
		char nameBuff[MAX_PATH + 1];

		unsigned long long id = 0;
		unsigned long long dataSize = 0;
		unsigned long long dataOffset = 0;

		

		for (size_t i = 0; i < oResourcePackageEntryMetaVector.size(); ++i)
		{
			//name length
			Read(fin, nameLength);
			if (nameLength > MAX_PATH)
			{
				OutputDebugStringW(L"비정상적인 텍스처 파일이름길이\n");
				assert(0);
			}

			//name
			Read(fin, nameBuff, nameLength);
			nameBuff[nameLength] = '\0';


			//id
			Read(fin, id);

			//data size
			Read(fin, dataSize);

			//data offset
			Read(fin, dataOffset);


			oResourcePackageEntryMetaVector[i].mNameLength = nameLength;
			oResourcePackageEntryMetaVector[i].mName = nameBuff;
			oResourcePackageEntryMetaVector[i].mID = id;
			oResourcePackageEntryMetaVector[i].mDataSize = dataSize;
			oResourcePackageEntryMetaVector[i].mDataOffset = dataOffset;

		}



	}


	inline void ResourceLoader::Read(std::ifstream& fin, std::vector< ResourceMeshPackageEntryMeta>& oResourceMeshPackageEntryMetaVector)
	{
		unsigned long long nameLength = 0;
		char nameBuff[MAX_PATH + 1];

		int meshType = 0;
		unsigned long long id = 0;
		unsigned long long dataSize = 0;
		unsigned long long dataOffset = 0;



		for (size_t i = 0; i < oResourceMeshPackageEntryMetaVector.size(); ++i)
		{
			//name length
			Read(fin, nameLength);
			if (nameLength > MAX_PATH)
			{
				OutputDebugStringW(L"비정상적인 텍스처 파일이름길이\n");
				assert(0);
			}

			//name
			Read(fin, nameBuff, nameLength);
			nameBuff[nameLength] = '\0';

			//meshType
			Read(fin, meshType);

			//id
			Read(fin, id);

			//data size
			Read(fin, dataSize);

			//data offset
			Read(fin, dataOffset);


			oResourceMeshPackageEntryMetaVector[i].mNameLength = nameLength;
			oResourceMeshPackageEntryMetaVector[i].mName = nameBuff;
			oResourceMeshPackageEntryMetaVector[i].mMeshType = meshType;
			oResourceMeshPackageEntryMetaVector[i].mID = id;
			oResourceMeshPackageEntryMetaVector[i].mDataSize = dataSize;
			oResourceMeshPackageEntryMetaVector[i].mDataOffset = dataOffset;

		}

	}


	inline void ResourceLoader::Read(std::ifstream& fin, Material* mat)
	{
		MaterialPackageRawData materialPackageRawData;
		fin.read(reinterpret_cast<char*>(&materialPackageRawData),sizeof(MaterialPackageRawData));

		mat->SetAmbient(materialPackageRawData.mAmbient);
		mat->SetDiffuse(materialPackageRawData.mDiffuse);
		mat->SetSpecular(materialPackageRawData.mSpecular);
		mat->SetType(materialPackageRawData.mType);
		mat->SetColor(materialPackageRawData.mColor);
		mat->SetColorIntensity(materialPackageRawData.mColorIntensity);
		mat->SetFresnelRO(materialPackageRawData.mFresnelR0);
		mat->SetShiness(materialPackageRawData.mShiness);
		
		unsigned long long diffuseMapID = materialPackageRawData.mDiffuseMapID;
		unsigned long long normalMapID = materialPackageRawData.mNormalMapID;

		mat->SetDiffuseMap(diffuseMapID);
		mat->SetNormalMap(normalMapID);
		
		mat->SetTextureTransformMatrix(0, materialPackageRawData.mTextureTransformMatrix[0]);
		mat->SetTextureTransformMatrix(1, materialPackageRawData.mTextureTransformMatrix[1]);
		mat->SetTextureTransformMatrix(2, materialPackageRawData.mTextureTransformMatrix[2]);




	}



	

	void ResourceLoader::LoadProjectEffect()
	{

		const std::string effectDirectoryPath = "./Resource/Effect/*.effect";

		WIN32_FIND_DATAA fileData;
		HANDLE handle = FindFirstFileA(effectDirectoryPath.c_str(),&fileData);
		

		if (handle != INVALID_HANDLE_VALUE)
		{
			mEffectLoader.Load(fileData.cFileName);
			while (FindNextFileA(handle, &fileData))
			{
				mEffectLoader.Load(fileData.cFileName);
			}
		}

		//Effect폴더안에있는 *.effect 이름을 얻어온다.

		//loop begin
		//mEffectLoader.Load()
		//loop end



	}

	void ResourceLoader::LoadProjectAsset()
	{

		//로드 순서중요
	
		LoadProjectTexture();

		LoadProjectMaterial();
		
		LoadProjectMesh();

	}

	void ResourceLoader::LoadProjectTexture()
	{

		const std::string textureDirectoryPath = "./TextureMeta.json";

		JsonParser::ReadFile(textureDirectoryPath);

		unsigned int textureNum = JsonParser::GetObjectNum();


		for (unsigned int i = 0; i < textureNum; ++textureNum)
		{
			JsonParser::SetCurrentIndex(i);

			std::string textureFilePath; 
			JsonParser::Read("Texture_FilePath", textureFilePath);
			std::string name;
			JsonParser::Read("ContentItem_Name", name);
			unsigned long long id = 0;
			JsonParser::Read("Asset_ID", id);
			Texture * newTexture = TextureManager::CreateTextureFromFile(name, id);
			newTexture->DeSerialize();

			mTextureLoader.LoadTextureFile(textureFilePath, *newTexture);
			TextureFactory::CreateSrvTex2D(newTexture);
		}










	}

	void ResourceLoader::LoadProjectMaterial()
	{

		const std::string materialMetaFilePath = "./MaterialMeta.json";
		JsonParser::ReadFile(materialMetaFilePath);

		unsigned long long materialNum = JsonParser::GetObjectNum();

		for (unsigned int i = 0; i < materialNum; ++i)
		{

			JsonParser::SetCurrentIndex(i);
		
			std::string name;
			JsonParser::Read("ContentItem_Name",name);

			unsigned long long id = 0;
			JsonParser::Read("Asset_ID", id);

			Material* newMaterial = MaterialManager::CreateMaterialFromFile(name,id);

			newMaterial->DeSerialize();

		}
	}

	void ResourceLoader::LoadProjectMesh()
	{

		const std::string& meshMetaFilePath = "./MeshMeta.json";
		LoadMeshFile(meshMetaFilePath);
	}

	void ResourceLoader::LoadProjectObject()
	{


		BaseObjectManager* objectManager= EditObjectManager::GetInstance();

		const std::string objectMetaFilePath = "./ObjectMeta.json";


		JsonParser::ReadFile(objectMetaFilePath);

		unsigned int objectNum = JsonParser::GetObjectNum();

		for (unsigned int i = 0; i < objectNum; ++i)
		{
			JsonParser::SetCurrentIndex(i);
			std::string className;
			JsonParser::Read("ClassName", className);
			unsigned long long id = 0;
			JsonParser::Read("Object_ID", id);
			std::string name;
			JsonParser::Read("Object_Name", name);
			Object * newObject = objectManager->CreateObjectFromFile(className, name, id);

			newObject->DeSerialize();

		}


	}

	void ResourceLoader::LoadBone(std::ifstream& fin, std::vector<Bone*>& oBoneVector)
	{
		size_t nameLength = 0;
		char name[MAX_PATH+1] ="";
		unsigned int boneType = 0;
		for (int i = 0; i < oBoneVector.size(); ++i)
		{
			Bone* bone = BoneFactory::CreateBone();
			
			
			Read(fin, nameLength);
			fin.read(reinterpret_cast<char*>(name), sizeof(char) * nameLength);
			Read(fin, boneType);

			name[nameLength + 1] = '\0';
			bone->SetName(name);
			bone->SetBoneType((Quad::EBoneType)boneType);
			oBoneVector[i] = bone;

		}



	}

	void ResourceLoader::ReadMeshCore(std::ifstream& fin, Mesh* mesh)
	{

		if (mGraphicsCommandObject->GetCloseState())
			mGraphicsCommandObject->ResetCommandList(nullptr);

		EMeshType meshType = mesh->GetMeshType();

		unsigned long long vertexNum = 0;
		unsigned long long indexNum = 0;

		std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> minMaxPoint;
		unsigned long long subMeshNum = 0;


		fin.read(reinterpret_cast<char*>(&vertexNum), sizeof(vertexNum));
		fin.read(reinterpret_cast<char*>(&indexNum), sizeof(indexNum));


		//min max point

		Read(fin, minMaxPoint.first);
		Read(fin, minMaxPoint.second);

		//submesh
		fin.read(reinterpret_cast<char*>(&subMeshNum), sizeof(subMeshNum));

		std::vector<SubMesh> subMeshVector;
		subMeshVector.resize(subMeshNum);

		unsigned long long materialUniqueID = 0;
		unsigned long long subMeshNameLength = 0;
		for (auto& subMesh : subMeshVector)
		{
			fin.read(reinterpret_cast<char*>(&subMesh.mID), sizeof(subMesh.mID));
			fin.read(reinterpret_cast<char*>(&subMesh.mIndexRange.first), sizeof(subMesh.mIndexRange.first));
			fin.read(reinterpret_cast<char*>(&subMesh.mIndexRange.second), sizeof(subMesh.mIndexRange.second));

			fin.read(reinterpret_cast<char*>(&subMesh.mVertexNum), sizeof(subMesh.mVertexNum));
			fin.read(reinterpret_cast<char*>(&subMesh.mVertexOffset), sizeof(subMesh.mVertexOffset));


			fin.read(reinterpret_cast<char*>(&materialUniqueID), sizeof(materialUniqueID));
			subMesh.mMaterialPointer = materialUniqueID;
			fin.read(reinterpret_cast<char*>(&subMeshNameLength), sizeof(subMeshNameLength));


			std::string subMeshName(subMeshNameLength, 0);
			fin.read(subMeshName.data(), sizeof(char) * subMeshNameLength);

			subMesh.mName = subMeshName;
			subMesh.mMesh = mesh;

		}


		Microsoft::WRL::ComPtr<ID3D12Resource> vertexDefaultBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexDefaultBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexUploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexUploadBuffer;

		std::vector<SkinningVertex> skinningVertexVector;
		std::vector<StaticVertex> staticVertexVector;
		if (meshType == EMeshType::eSknningMesh)
		{
			skinningVertexVector.resize(vertexNum);
			for (auto& vertex : skinningVertexVector)
			{
				Read(fin, vertex.mPos);
				Read(fin, vertex.mNormal);
				Read(fin, vertex.mTex);
				Read(fin, vertex.mBoneWeight[0]);
				Read(fin, vertex.mBoneWeight[1]);
				Read(fin, vertex.mBoneIndex[0]);
				Read(fin, vertex.mBoneIndex[1]);
			}

			vertexDefaultBuffer = Utility::CreateDefaultBuffer(mDevice, mGraphicsCommandObject->GetGraphicsCommandList(), skinningVertexVector.data(), sizeof(SkinningVertex), vertexNum, vertexUploadBuffer);
		}
		else if (meshType == EMeshType::eStaticMesh)
		{
			staticVertexVector.resize(vertexNum);
			for (auto& vertex : staticVertexVector)
			{
				Read(fin, vertex.mPos);
				Read(fin, vertex.mNormal);
				Read(fin, vertex.mTex);
			}

			vertexDefaultBuffer = Utility::CreateDefaultBuffer(mDevice, mGraphicsCommandObject->GetGraphicsCommandList(), staticVertexVector.data(), sizeof(StaticVertex), vertexNum, vertexUploadBuffer);
		}


		std::vector<MeshIndexType> indexVector(indexNum);
		for (auto& index : indexVector)
		{
			fin.read(reinterpret_cast<char*>(&index), sizeof(index));
		}

		

		indexDefaultBuffer = Utility::CreateDefaultBuffer(mDevice, mGraphicsCommandObject->GetGraphicsCommandList(), indexVector.data(), sizeof(MeshIndexType), indexNum, indexUploadBuffer);


		mGraphicsCommandObject->ExecuteCommandList();
		mGraphicsCommandObject->FlushCommandQueue();





		if (meshType == EMeshType::eSknningMesh)
		{
			SkinningMesh* skinningMesh = static_cast<SkinningMesh*>(mesh);
			skinningMesh->SetVertexVector(std::move(skinningVertexVector));
		}
		else if (meshType == EMeshType::eStaticMesh)
		{
			StaticMesh* staticMesh = static_cast<StaticMesh*>(mesh);
			staticMesh->SetVertexVector(std::move(staticVertexVector));
		}

		mesh->SetVertexNum(vertexNum);
		mesh->SetIndexNum(indexNum);
		mesh->SetMinMaxPoint(minMaxPoint.first, minMaxPoint.second);
		mesh->SetSubMeshVector(std::move(subMeshVector));


		mesh->SetVertexBuffer(vertexDefaultBuffer);
		mesh->SetIndexBuffer(indexDefaultBuffer);
		mesh->SetIndexVector(std::move(indexVector));







	}

	void ResourceLoader::ReadSkeletonCore(std::ifstream& fin, Skeleton* skeleton)
	{
		size_t boneNum = 0;
		Read(fin, boneNum);

		std::vector<Bone* > boneVector(boneNum, nullptr);
		std::vector<int> boneHierarchyVector(boneNum);
		std::vector<DirectX::XMFLOAT4X4> mOffsetMatrixVector(boneNum);

		LoadBone(fin, boneVector);

		fin.read(reinterpret_cast<char*>(boneHierarchyVector.data()), sizeof(int) * boneNum);

		for (auto& offsetMatrix : mOffsetMatrixVector)
		{
			Read(fin, offsetMatrix);
		}

		skeleton->mBoneVector = std::move(boneVector);
		skeleton->mBoneHiererachyVector = std::move(boneHierarchyVector);
		skeleton->mOffsetMatrixVector = std::move(mOffsetMatrixVector);



	}

	void ResourceLoader::ReadAnimationClipCore(std::ifstream& fin, AnimationClip* animClip)
	{

		int endFrameCount = 0;
		Read(fin, endFrameCount);
		animClip->SetFrameEndCount(endFrameCount);


		float endSecondTime = 0.0f;
		Read(fin, endSecondTime);
		animClip->SetEndTimeSecond(endSecondTime);



		int timeMode = 0;
		Read(fin, timeMode);
		animClip->SetTimeMode(Quad::EAnimationTimeMode(timeMode));



		unsigned long long defaultMeshID = 0;
		Read(fin, defaultMeshID);
		animClip->SetDefaultMesh(MeshManager::GetMesh(defaultMeshID));


		unsigned long long defaultSkeletonID = 0;
		Read(fin, defaultSkeletonID);
		animClip->SetDefaultSkeleton(SkeletonManager::GetSkeleton(defaultSkeletonID));


		size_t  boneAnimClipNum = 0;
		Read(fin, boneAnimClipNum);

		std::vector<BoneAnimationClip> boneAnimationClipVector(boneAnimClipNum);

		for (auto& boneAnimClip : boneAnimationClipVector)
		{
			int boneID = 0;
			Read(fin, boneID);

			boneAnimClip.SetBoneID(boneID);

			size_t keyFrameNum = 0;
			Read(fin, keyFrameNum);

			std::vector<AnimationKeyFrame> animKeyFrameVector(keyFrameNum);

			for (auto& keyFrame : animKeyFrameVector)
			{
				Read(fin, keyFrame.mScale);
				Read(fin, keyFrame.mQuaternion);
				Read(fin, keyFrame.mPosition);
				Read(fin, keyFrame.mTime);
			}

			boneAnimClip.SetAnimationKeyFrameVector(std::move(animKeyFrameVector));

		}

		animClip->SetBoneAnimationClipVector(std::move(boneAnimationClipVector));



	}




}