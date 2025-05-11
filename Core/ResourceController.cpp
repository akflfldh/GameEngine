




#include "ResourceController.h"
//#include"FileUiEntity.h"
//#include"DirectoryUiEntity.h"
//#include"ButtonUiEntitiy.h"
//#include"WorldEditUiEntity.h"
#include"DeleteTextureEvent.h"
#include"Buffer/StructuredBuffer.h"
#include"Effect/Effect.h"
#include"Buffer/ConstantBuffer.h"
#include"EffectTable.h"
#include"InputLayoutTable.h"
#include"unicode/unistr.h"
#include"unicode/uversion.h"
#include"Buffer/ConstantBufferManager.h"
#include"Shader/ShaderResource.h"
#include"StructuredBufferFactory.h"
#include"Buffer/StructuredBuffer.h"
#include"Utility.h"
#include"GraphicCommand.h"
#include"ResourceManager/MaterialManager/MaterialManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"



//#include"LoadRequestFileEvent.h"
//#include"LoadFileResponseEvent.h"

#include"Core/ShaderManager.h"
#include"Core/Shader.h"
#include"Effect/RenderPassTwo.h"
#include"Shader/ShaderResourceConstantBuffer.h"
#include"Shader/ShaderResourceTexture.h"
#include"Shader/ShaderResourceSampler.h"



#include"ResourceManager/SkeletonManager.h"
#include"ResourceManager/AnimationClipManager.h"
#include"Asset/Skeleton/Skeleton.h"
#include"Asset/Animation/AnimationClip.h"
#include<tchar.h>

//#include"FileUiUiSystem.h"
//#include"LogicalContentItemUiEntityFolder.h"
//#include"ContentItemUiPanelEntity.h"

//#include"ProjectDirector.h"
//#include"Project.h"

//#include"EditorModeDirector.h"

#include"Asset/Mesh/Mesh.h"
#include"Asset/Mesh/StaticMesh.h"
#include"Asset/Mesh/SkinningMesh.h"

#include<sstream>

#include"BaseWindowController.h"
namespace Quad
{

	void ResourceController::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
		GraphicCommand* graphicsCommandObject,
		MeshManager* meshManager, MaterialManager* materialManager, TextureManager* textureManager,
		MapManager* mapManager, RenderSystem* renderSystem, ResourceLoader* resourceLoader,
		ResourceStorer* resourceStorer, FileUISystem* fileUiSystem, GamePlayUiSystem* gamePlayUiSystem,
		DescriptorHeapManagerMaster* descriptorHeapManagerMaster)
	{
		
		Controller::Initialize();
		mDevice = device;
		mGraphicsCommandObject = graphicsCommandObject;
		mMeshManager = meshManager;
		mMaterialManager = materialManager;
		mTextureManager = textureManager;
		mMapManager = mapManager;
		mRenderSystem = renderSystem;
		mResourceLoader = resourceLoader;
		mResourceStorer = resourceStorer;
		mFileUiSystem = fileUiSystem;
		mUiSystem = gamePlayUiSystem;
		mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;
		mConstantBufferManager = new ConstantBufferManager;
		mConstantBufferManager->Initialize(mDevice);
		mStructuredBufferFactory = new StructuredBufferFactory;
		mStructuredBufferFactory->Initialize(device,mDescriptorHeapManagerMaster);

		CoInitialize(NULL);

		////mShaderResourceFactory = new ShaderResourceFactory;
		//mShaderResourceFactory->Initialize(mDevice,mDescriptorHeapManagerMaster, mConstantBufferManager,mStructuredBufferFactory);

	
		//mEventQueue.Initialize(100);
		CreateSampler();
		InitBlendEnumUnMap();
		InitStencilEnumUnMap();
		InitComparisonFuncEnumUnMap();
		InitLayout();

		//LoadCommonTexture();


		//std::vector<Asset*> assetVector;
		////LoadFbxFile(L"C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Resource\\black_bison.fbx", assetVector);

		////
		//RawMaterialData rawMaterialData;
		//MaterialGenerator::CreateDefaultMaterial(rawMaterialData);
		//CreateMaterial(rawMaterialData);

		//RawStaticMeshData* rawMeshData = new RawStaticMeshData;
		//GeometryGenerator::CreateRawRectMeshData(rawMeshData);

		//CreateMesh(rawMeshData);
	
	
		//LoadDefaultModel();



#ifdef EditorMode

	//	EditorModeDirector::LoadAsset("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset");

#endif



		CreateDefaultShaderResource();
		//LoadCommonEffect();
		
	
		auto effectManager = EffectManager::GetInstance();

	}

	void ResourceController::Update()
	{
		ResourceController* resourceControllerInstance = GetInstance();
		resourceControllerInstance->mConstantBufferManager->Update();


		while (1)
		{
			Event* pEvent = resourceControllerInstance->GetEvent();//or unique_ptr 로 리턴 그럼마지막에 delete를 명시하는것을 체크하지않아도된다.

			if (pEvent == nullptr)
				break;
			resourceControllerInstance->HandleEvent(pEvent);
			delete pEvent;
		}

	}

	void ResourceController::OnResize()
	{

		//mFileUiSystem->Resize()

	}

	/*void ResourceController::AddEvent(Event* event)
	{
		WriterStart();

		if (!mEventQueue.isFull())
			mEventQueue.PushBack(event);


		WriterEnd();
	}*/

	StructuredBuffer* ResourceController::CreateStructuredBuffer(UINT elementSize, UINT elementMaxNum)
	{
		
		StructuredBuffer * structuredBuffer = mStructuredBufferFactory->CreateStructuredBuffer(elementSize, elementMaxNum);


		//srv
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = elementMaxNum;
		srvDesc.Buffer.StructureByteStride = elementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		ViewIndex viewIndex =	mDescriptorHeapManagerMaster->CreateSrv(structuredBuffer->mUploadBuffer->mUploadBuffer, &srvDesc);
		
		structuredBuffer->SetViewIndex(viewIndex);
		return structuredBuffer;


	}

	void ResourceController::RemoveStructuredBuffer(StructuredBuffer* buffer)
	{
		//ResourceController* instance = GetInstance();
		//if (buffer == nullptr)
		//	return;

		//ViewIndex viewIndex = buffer->GetViewIndex();
		//buffer->mUploadBuffer.mUploadBuffer.Reset();	//이게 comptr의 참조카운팅을 줄이니깐 확실히제거되지가 않는데.
		////핸들이 필요할거같다.
		//instance->mDescriptorHeapManagerMaster->ReleaseCbvSrvUav(viewIndex);



	}



	
//	std::vector<Asset*> ResourceController::ImportFile(const std::string& filePath)
//	{
//		 
//		//확장자를 보는거지
//		const std::string &  extension= Utility::GetExtension(filePath);
//		Asset* asset = nullptr;
//		std::vector<Asset*> assetVector;
//		if (extension == "fbx")
//		{
//			ImportFbxFile(filePath, assetVector);
//		}
//		/*else if (extension == "mesh")
//		{
//			asset=	LoadMeshFile(filePath);
//			assetVector.push_back(asset);
//		}*/
//		else if (extension == "png" || extension =="jpg" || extension=="bmp")
//		{
//			asset= LoadTextureFile(filePath);
//
//			//복사까지 
//			if (asset != nullptr)
//			{
//
//#ifdef EditorMode
//				/*Project* currProject = ProjectDirector::GetCurrentProject();
//				ProjectMetaData * currProjectMetaData =	ProjectDirector::GetProjectMetaData(currProject->GetName());
//				const std::string & currProjectFolderPath = currProjectMetaData->GetProjectFolderPath();
//				const std::string copiedTextureFilePath = currProjectFolderPath + "\\Asset\\Texture\\" + asset->GetName();
//
//				CopyFile(Utility::ConvertToWString(filePath, true).c_str(), Utility::ConvertToWString(copiedTextureFilePath, true).c_str(), TRUE);
//*/
//
//
//			
//			/*	const std::string copiedTextureFilePath = "C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset\\Texture\\" + asset->GetName();
//				
//				CopyFile(Utility::ConvertToWString(filePath,true).c_str(), Utility::ConvertToWString(copiedTextureFilePath,true).c_str(), TRUE);*/
//
//#endif
//			    assetVector.push_back(asset);
//			}
//		}
//
//		//SendLoadFileResponseEvent(assetVector);
//		
//		return assetVector;
//
//
//
//	}

//	void ResourceController::ImportFbxFile(const std::string& filePath,std::vector<Asset*> & oAssetVector)
//	{
//		std::string fileName = PathFindFileNameA(filePath.c_str());
//		fileName = Utility::RemoveExtension(fileName);
//		std::vector<RawMeshData*> rawMeshDataVector;
//		std::vector<RawMaterialData> rawMaterialDataVector;
//		std::vector<RawTextureData> rawTextureDataVector;
//		Skeleton* skeleton = nullptr;
//		AnimationClip* animationClip = nullptr;
//		LoadResultCommonData loadResultCommonData;
//		if (!mResourceLoader->LoadFbxFile(filePath, rawMeshDataVector, rawMaterialDataVector, rawTextureDataVector, skeleton,
//			animationClip, loadResultCommonData))
//		{ //디버깅용 메세지를 출력하면좋을듯
//			return;
//		}
//
//
//		//이름이 겹치는지 아닌지 확인해서 조정한다.
//
//
//		std::vector<std::string> meshNameVector;
//	
//		for (int i = 0; i < rawMeshDataVector.size(); ++i)
//		{
//			meshNameVector.push_back(rawMeshDataVector[i]->mName);
//		}
//
//		std::vector<std::string> materialNameVector;
//		for (int i = 0; i < rawMaterialDataVector.size(); ++i)
//		{
//			materialNameVector.push_back(rawMaterialDataVector[i].mName);
//		}
//		
//		//mesh랑 머터리얼이랑 이름이같은게있는지체크
//
//		for (int meshIndex = 0; meshIndex < rawMeshDataVector.size(); ++meshIndex)
//		{
//			for (int matNameIndex = 0; matNameIndex < materialNameVector.size(); ++matNameIndex)
//			{
//				if (rawMeshDataVector[meshIndex]->mName == materialNameVector[matNameIndex])
//				{
//					//mesh의 이름을 바꾸자. 
//					//그게더 간단하다.
//					//머터리얼의 이름을바꾸면 다 일일이 그 머터리얼을 참조하는 메시의 머터리얼필드의 이름값을 바꿔야하니.
//
//					int i = 1;
//					rawMeshDataVector[meshIndex]->mName += std::to_string(i);
//				}
// 			}
//
//		}
//
//
//
//		for (int i = 0; i < rawTextureDataVector.size(); ++i)
//		{
//			//CreateTexture(rawTextureDataVector[i]);
//			Asset * asset =  LoadTextureFile(rawTextureDataVector[i].mFilePath);
//
//			if (asset != nullptr)
//			{
//				//import이기때문에 텍스처 복사본을 Texture 폴더에 생성해준다.
//#ifdef EditorMode
//
//			/*	Project* currProject = ProjectDirector::GetCurrentProject();
//				ProjectMetaData* currProjectMetaData = ProjectDirector::GetProjectMetaData(currProject->GetName());
//				const std::string& currProjectFolderPath = currProjectMetaData->GetProjectFolderPath();
//				const std::string copiedTextureFilePath = currProjectFolderPath + "\\Asset\\Texture\\" + asset->GetName();
//
//				CopyFile(Utility::ConvertToWString(rawTextureDataVector[i].mFilePath, true).c_str(), Utility::ConvertToWString(copiedTextureFilePath, true).c_str(), TRUE);*/
//
//
//
//			/*	const std::wstring copiedTextureFilePath = L"C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Asset\\Texture\\" + Utility::ConvertToWString( asset->GetName(),true);
//
//				std::wstring originalTextureFilePath = Utility::ConvertToWString(rawTextureDataVector[i].mFilePath, true);
//				BOOL ret =CopyFileW(originalTextureFilePath.c_str(), copiedTextureFilePath.c_str(), FALSE);
//
//				if (ret == 0)
//				{
//					OutputDebugString(L"텍스처복사 실패\n");
//				}*/
//
//#endif
//				oAssetVector.push_back(asset);
//			}
//			//생성한 텍스처를 리턴해주자.
//		}
//
//		for (int i = 0; i < rawMaterialDataVector.size(); ++i)
//		{
//			if (rawMaterialDataVector[i].mDefaultFlag)
//				continue;
//
//			Asset* asset = CreateMaterial(rawMaterialDataVector[i]);
//			if (asset != nullptr)
//				oAssetVector.push_back(asset);
//		}
//
//		int animMeshIndex = oAssetVector.size();
//		for (int i = 0; i < rawMeshDataVector.size(); ++i)
//		{
//			if (rawMeshDataVector[i]->mName == "")
//			{
//				//여러 메시가 합쳐진 skinning메시일경우 이름이없을것이니
//				rawMeshDataVector[i]->mName = fileName + "_mesh";
//			}
//			Asset* asset = CreateMesh(rawMeshDataVector[i]);
//			if (asset != nullptr)
//				oAssetVector.push_back(asset);
//		}
//
//
//		//에니메이션모델을 임포트하는경우 다른 정적 메시들은 임포트하지않는다. 
//		//따라서 딱하나의 메시만 임포트되었을것이다 , 그것을 animaion clip이 default로서 참조하도록한다.
//		//그래야 속성창에서 에니메이션클립asset을 드래그앤드랍하여 에니메이션 편집기능을 구현할수있겠다.
//	
//		if (skeleton != nullptr)
//		{
//			std::string skeletonName = fileName + "_Skeleton";
//			skeleton->SetName(std::move(skeletonName));
//			SkeletonManager::AddSkeleton(skeleton);
//			oAssetVector.push_back(skeleton);
//		}
//
//		if (animationClip != nullptr)
//		{
//			std::string animationClipName = fileName + "_Anim";
//			animationClip->SetName(std::move(animationClipName));
//			AnimationClipManager::AddAnimationClip(animationClip);
//			oAssetVector.push_back(animationClip);
//
//			animationClip->SetDefaultMesh((Mesh*)oAssetVector[animMeshIndex]);
//			animationClip->SetDefaultSkeleton(skeleton);
//
//		}
//
//
//
//		//하나의 event만만들어서 fileui에게전달하면 좋을거같은데
//		//그러면 파일마다 event를 동적할당으로 생성해서 전달할 필요없이 
//		// 하나의 event만동적할당하게됨으로
//		//그럼 send CreatefileEvent메서드를 만들고 원할떄 보낼수있게하자.
//
//		
//	}

	//Asset* ResourceController::LoadMeshFile(const std::wstring& filePath)
	//{






	//	//SendLoadFileResponseEvent()
	//	return nullptr;

	//}


	//Asset* ResourceController::LoadMaterialFile(const std::wstring& filePath)
	//{
	//	return nullptr;
	//}

	Texture * ResourceController::LoadTextureFile(const std::string& filePath)
	{

		Texture* texture = mResourceLoader->LoadTextureFile(filePath);
		return texture;
			

		





		//std::wstring fileName =	Utility::GetFileNameFromPath(filePath);

		//if(mTextureManager->CheckDuplicate(fileName))
		//{
		//	int num = 0;
		//	std::wstring newFileName = fileName+std::to_wstring(num);

		//	while (mTextureManager->CheckDuplicate(newFileName))
		//	{
		//		num++;
		//		newFileName = fileName + std::to_wstring(num);
		//	}
		//	fileName = newFileName;
		//}


		//Texture  * texture = mTextureManager->CreateTexture(fileName);
		//if (texture != nullptr)
		//{
		//	if (!mResourceLoader->LoadTextureFile(filePath, *texture))
		//	{
		//		mTextureManager->DeleteTexture(texture);
		//		return nullptr;
		//	}






			////VIEW 생성  //일단은 2차원texture나 buffer만 생각한다.
			//Microsoft::WRL::ComPtr<ID3D12Resource> resource = texture->GetResource();

			//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
			//srvDesc.Format = resource->GetDesc().Format;
			//srvDesc.ViewDimension = ConvertSRVDimensionEnum(resource->GetDesc().Dimension);
			//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			//if (srvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2D)
			//	FillSRVTEX2DDesc(srvDesc, resource);

			//texture->SetTextureFormat(ConvertTextureFormatEnum(srvDesc.Format));



			//ViewIndex viewIndex = mDescriptorHeapManagerMaster->CreateSrv(resource, &srvDesc);
			//texture->SetViewIndex(viewIndex);
		//}


	//	return texture;



		//그 이벤트안에는 그 리소스에대한 포인터, 어떤리소스인지에대한 식별자등등을 포함하자.
		//Controller::AddEvent("FileUiController", nullptr);





	}


	Mesh* ResourceController::CreateMesh(RawMeshData* rawMeshData)
	{
		Mesh* mesh = mMeshManager->CreateMesh(rawMeshData->mName, rawMeshData->mMeshType);

		
		if (mesh == nullptr)
		{
			OutputDebugString(L"mesh 로드 이름겹침\n");
			return nullptr;
			//renamning
			
		}

		//vertexBuffer,indexBuffer생성
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexUploadBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexUploadBuffer;



		if (mGraphicsCommandObject->GetCloseState())
			mGraphicsCommandObject->ResetCommandList(nullptr);
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();


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

		Microsoft::WRL::ComPtr<ID3D12Resource> vertexDefaultBuffer = Utility::CreateDefaultBuffer(mDevice, graphicsCommandList, pVertexVectorData,
			vertexElementSize, vertexVectorSize, vertexUploadBuffer);

		Microsoft::WRL::ComPtr<ID3D12Resource> indexDefaultBuffer = Utility::CreateDefaultBuffer(mDevice, graphicsCommandList, rawMeshData->mIndexVector.data(),
			sizeof(MeshIndexType), rawMeshData->mIndexVector.size(), indexUploadBuffer);
		mGraphicsCommandObject->ExecuteCommandList();
		mGraphicsCommandObject->FlushCommandQueue();

		int subMeshNum = rawMeshData->mRawSubMeshVector.size();
		const std::vector<RawSubMeshData>& rawSubMeshVector = rawMeshData->mRawSubMeshVector;
		std::vector<SubMesh> subMeshVector(subMeshNum);

		for (int subMeshIndex = 0; subMeshIndex < subMeshNum; ++subMeshIndex)
		{
			subMeshVector[subMeshIndex].mVertexOffset = rawSubMeshVector[subMeshIndex].mVertexOffset;
			subMeshVector[subMeshIndex].mIndexRange.first = rawSubMeshVector[subMeshIndex].mIndexFirst;
			subMeshVector[subMeshIndex].mIndexRange.second = rawSubMeshVector[subMeshIndex].mIndexLast;
			subMeshVector[subMeshIndex].mMesh = mesh;
			const std::string& materialName = rawSubMeshVector[subMeshIndex].mMaterialName;
			subMeshVector[subMeshIndex].mMaterialPointer = mMaterialManager->GetMaterial(materialName);
			
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

	Material * ResourceController::CreateMaterial(RawMaterialData& rawMaterialData)
	{

		Material * material =  mMaterialManager->CreateMaterial(rawMaterialData.mName );
		material->SetEngineContentItemFlag(true);

		if (material != nullptr)
		{
			material->SetEffectName(rawMaterialData.mEffectName);
			material->SetSpecular(rawMaterialData.mSpecular);
			material->SetShiness(rawMaterialData.mShiness);

			Texture*  mTexture = TextureManager::GetTexture(rawMaterialData.mDiffuseMapName);
			if (mTexture == nullptr)
			{
				mTexture = TextureManager::GetTexture("Default.bmp");
			}

			material->SetDiffuseMap(mTexture);
			
			//material->SetNormalMap(TextureManager::GetTexture(rawMaterialData.mNormalMapName));
		}
		return material;
	}

	bool ResourceController::CreateTexture(RawTextureData& rawTextureData)
	{

	





		return false;
	}




	//bool ResourceController::RegisterNewClassFromSchema(const std::string& filePath)
	//{
	//	RawSchemaData rawSchemaData;
	//	bool ret = mResourceLoader->LoadSchema(filePath, rawSchemaData);
	//	
	//	if (ret)
	//	{
	//		WorldEditEntityFactory::RegisterNewEntityClass(rawSchemaData);
	//	}

	//	return ret;
	//}

	void ResourceController::CreateDefaultInputLayout()
	{

		D3D12_INPUT_LAYOUT_DESC * inputLayoutDesc= new D3D12_INPUT_LAYOUT_DESC;
		D3D12_INPUT_ELEMENT_DESC* elementDescArray = new D3D12_INPUT_ELEMENT_DESC[2];
		inputLayoutDesc->NumElements = 2;
		inputLayoutDesc->pInputElementDescs = elementDescArray;


		elementDescArray[0].SemanticName = "POSITION";
		elementDescArray[0].SemanticIndex = 0;
		elementDescArray[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		elementDescArray[0].InputSlot = 0;
		elementDescArray[0].AlignedByteOffset = 0;
		elementDescArray[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		elementDescArray[0].InstanceDataStepRate = 0;

		elementDescArray[1].SemanticName = "TEX";
		elementDescArray[1].SemanticIndex = 0;
		elementDescArray[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		elementDescArray[1].InputSlot = 0;
		elementDescArray[1].AlignedByteOffset = 12;
		elementDescArray[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		elementDescArray[1].InstanceDataStepRate = 0;

		InputLayoutTable::AddInputLayout(L"default", inputLayoutDesc);

	}


	
	bool ResourceController::CreateConstantBuffer(const std::string & name, UINT bufferElementSize, UINT bufferElementNum)
	{

		ConstantBuffer*  constantBuffer =mConstantBufferManager->CreateConstantBuffer(name, bufferElementSize, bufferElementNum);
		if (constantBuffer == nullptr)
			return false;

		constantBuffer->mViewIndexVector.resize(bufferElementNum);
		for (int i = 0; i < bufferElementNum; ++i)
		{
					
			
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = constantBuffer->mUploadBuffer->GetResource()->GetGPUVirtualAddress() + i * constantBuffer->GetElementSize();
			cbvDesc.SizeInBytes = constantBuffer->GetElementSize();

			ViewIndex viewIndex = mDescriptorHeapManagerMaster->CreateCbv(&cbvDesc);
			if (viewIndex == InvalidViewIndex)
			{
				assert(-1);
			}
			constantBuffer->mViewIndexVector[i] = viewIndex;
		}
		return true;

	}



	//void ResourceController::SetPipelineStateOption(D3D12_GRAPHICS_PIPELINE_STATE_DESC& oGraphicsPipelineStateDesc, const std::vector<std::pair<std::wstring, std::wstring>>& pipelineStateOptionVector)
	//{
	//	oGraphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	//	oGraphicsPipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//	for (int i = 0; i < pipelineStateOptionVector.size(); ++i)
	//	{
	//		const std::wstring& optionName = pipelineStateOptionVector[i].first;
	//		const std::wstring& optionValue = pipelineStateOptionVector[i].second;

	//		if (optionName == L"BLENDENABLE")
	//		{
	//			//true, false;
	//			std::wstring valueUpper = optionValue;
	//			std::transform(valueUpper.begin(), valueUpper.end(), valueUpper.begin(), [](const wchar_t ch) { return std::toupper(ch); });
	//			bool blendEnableFlag = false;


	//			if (valueUpper == L"TRUE")
	//			{
	//				blendEnableFlag = true;
	//			}

	//			oGraphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = blendEnableFlag;
	//			
	//		}
	//		else if (optionName == L"BLENDSRCFACTOR")
	//		{

	//			oGraphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = ConvertBlendFactorEnum(optionValue);	
	//		}
	//		else if (optionName == L"BLENDDESTFACTOR")
	//		{
	//			oGraphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = ConvertBlendFactorEnum(optionValue);
	//			
	//		}
	//		else if (optionName == L"BLENDOP")
	//		{

	//			oGraphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp= 	ConvertBlendOpEnum(optionValue);
	//		}
	//		else if (optionName == L"STENCILENABLE")
	//		{
	//			std::wstring valueUpper = optionValue;
	//			std::transform(valueUpper.begin(), valueUpper.end(), valueUpper.begin(), [](const wchar_t ch) { return std::toupper(ch); });
	//			bool stencilEnableFlag = false;
	//			if (valueUpper == L"TRUE")
	//			{
	//				stencilEnableFlag = true;
	//			}
	//			oGraphicsPipelineStateDesc.DepthStencilState.StencilEnable = stencilEnableFlag;
	//		}
	//		else if (optionName == L"STENCILFAILOP")
	//		{

	//			//oGraphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = ConvertStencilOpEnum(optionValue);
	//			oGraphicsPipelineStateDesc.DepthStencilState.BackFace.StencilFailOp = ConvertStencilOpEnum(optionValue);


	//		}
	//		else if (optionName == L"STENCILDEPTHFAILOP")
	//		{
	//			//oGraphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = ConvertStencilOpEnum(optionValue);
	//			oGraphicsPipelineStateDesc.DepthStencilState.BackFace.StencilDepthFailOp = ConvertStencilOpEnum(optionValue);

	//		}
	//		else if (optionName == L"STENCILPASSOP")
	//		{
	//			//oGraphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = ConvertStencilOpEnum(optionValue);
	//			oGraphicsPipelineStateDesc.DepthStencilState.BackFace.StencilPassOp = ConvertStencilOpEnum(optionValue);

	//		}
	//		else if (optionName == L"STENCILFUNC")
	//		{
	//			//oGraphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = ConvertComparisonFuncEnum(optionValue);
	//			oGraphicsPipelineStateDesc.DepthStencilState.BackFace.StencilFunc = ConvertComparisonFuncEnum(optionValue);

	//		}


	//	}




	//}

	//void ResourceController::LoadEffect(const std::string& name)
	//{

	//	
	//	//RawEffectDataTwo rawEffectDataTwo;
	//	//if(!mResourceLoader->LoadEffect(name, rawEffectDataTwo));
	//	//{
	//	//	//디버깅창에 출력
	//	//}

	//	//CreateEffect(rawEffectDataTwo);







	//}

	//void ResourceController::CreateEffect(RawEffectDataTwo& rawEffectDataTwo)
	//{
	//	

	//	//effect클래스가 존재하면된다.
	//	std::vector<RenderPassTwo* >renderPassVector;
	//	for (int i = 0; i < rawEffectDataTwo.mRenderPassDataVector.size(); ++i)
	//	{
	//		RenderPassTwo * renderPass =CreateRenderPass(rawEffectDataTwo.mRenderPassDataVector[i],false);
	//		renderPassVector.push_back(renderPass);
	//	}



	//	std::vector<RenderPassTwo*> renderPassVectorS;//skinning버전
	//	for (int i = 0; i < rawEffectDataTwo.mRenderPassDataVector.size(); ++i)
	//	{
	//		RenderPassTwo* renderPass = CreateRenderPass(rawEffectDataTwo.mRenderPassDataVector[i],true);
	//		renderPassVectorS.push_back(renderPass);
	//	}




	//	///Effect* effect = new Effect(rawEffectDataTwo.mEffectName);
	//	Effect* effect = mEffectManager.GetEffect(rawEffectDataTwo.mEffectName);
	//	if (effect != nullptr)
	//	{
	//		effect->SetStaticMeshRenderPassVector(std::move(renderPassVector));
	//		effect->SetSkinningMeshRenderPassVector(std::move(renderPassVectorS));
	//	}
	//	//EffectTable::AddEffect(effect->GetName(), effect);







	//}

	//RenderPassTwo*  ResourceController::CreateRenderPass(RawRenderPassDataTwo& rawRenderPassTwoData, bool isSkinningShader)
	//{

	//	RenderPassTwo* renderPass = new RenderPassTwo;

	//	ID3DInclude* pInclude = D3D_COMPILE_STANDARD_FILE_INCLUDE;
	//	UINT compileFlags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	//	UINT compileFlags2 = 0;

	//	std::vector<Shader*> shaderVector;
	//	Shader* vertexShader = nullptr;

	//	std::vector<D3D_SHADER_MACRO> macroVector;
	//	D3D_SHADER_MACRO* pMarco = nullptr;
	//	std::wstring shaderMeshType= L"Static";
	//	if (isSkinningShader)
	//	{
	//		shaderMeshType = L"Skinning";
	//		D3D_SHADER_MACRO skinningMarco;
	//		skinningMarco.Name = "SKINNING";
	//		skinningMarco.Definition = 0;

	//		macroVector.push_back(skinningMarco);
	//		macroVector.push_back({NULL,NULL});

	//		pMarco = macroVector.data();
	//	}





	//	if (rawRenderPassTwoData.mVertexShaderData.mFile.size() != 0)
	//	{
	//		const std::string& file = rawRenderPassTwoData.mVertexShaderData.mFile;
	//		const std::string& entryPoint = rawRenderPassTwoData.mVertexShaderData.mEntryPoint;
	//		const std::string& version = rawRenderPassTwoData.mVertexShaderData.mVersion;

	//		//컴파일하기전에  이미 컴파일한 세이더인지 점검
	//		Shader* shader = ShaderManager::GetShader(file, entryPoint, version, shaderMeshType);

	//		if (shader == nullptr)
	//		{
	//			

	//			Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode = CompileShader(file, pMarco, pInclude, entryPoint, version, compileFlags1, compileFlags2);
	//			shader = new Shader;
	//			shader->Initialize(file, entryPoint, version, shaderMeshType, shaderByteCode,EShaderType::eVS);
	//			ShaderManager::AddShader(shader);
	//		}

	//		// shader resource 생성
	//		ShaderResourceReflection(renderPass, shader,EShaderResourceVisibility::eVertex);
	//		shaderVector.push_back(shader);
	//		vertexShader = shader;
	//	}


	//	if (rawRenderPassTwoData.mPixelShaderData.mFile.size() != 0)
	//	{

	//		const std::wstring& file = rawRenderPassTwoData.mPixelShaderData.mFile;
	//		const std::wstring& entryPoint = rawRenderPassTwoData.mPixelShaderData.mEntryPoint;
	//		const std::wstring& version = rawRenderPassTwoData.mPixelShaderData.mVersion;

	//		//컴파일하기전에  이미 컴파일한 세이더인지 점검
	//		Shader* shader = ShaderManager::GetShader(file, entryPoint, version, shaderMeshType);

	//		if (shader == nullptr)
	//		{
	//			Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode = CompileShader(file, pMarco, pInclude, entryPoint, version, compileFlags1, compileFlags2);
	//			shader = new Shader;
	//			shader->Initialize(file, entryPoint, version, shaderMeshType, shaderByteCode, EShaderType::ePS);
	//			ShaderManager::AddShader(shader);
	//		}

	//		// shader resource 생성
	//		ShaderResourceReflection(renderPass, shader, EShaderResourceVisibility::ePixel);
	//		shaderVector.push_back(shader);
	//	}


	//	//create root signature 
	//	// 
	//	
	//	CreateRootSignature(renderPass);

	//	CreateGraphicsPipeline(rawRenderPassTwoData,renderPass,shaderVector,vertexShader);
	//	




	//	//effect파일의여러 옵션들을 읽어서 ,
	//	//최종적으로 pipeline생성
	//	//create pipeline 









	//	














	//	return renderPass;


	//}




	//void ResourceController::ShaderResourceReflection(RenderPassTwo* renderPass, Shader* shader, EShaderResourceVisibility shaderResourceVisibility)
	//{

	//	Microsoft::WRL::ComPtr< ID3D12ShaderReflection> shaderReflectionInterface;
	//	D3DReflect(shader->GetShaderByteCode().Get()->GetBufferPointer(), shader->GetShaderByteCode().Get()->GetBufferSize(), IID_PPV_ARGS(shaderReflectionInterface.GetAddressOf()));

	//	D3D12_SHADER_DESC shaderDesc;
	//	shaderReflectionInterface->GetDesc(&shaderDesc);
	//	UINT shaderResourceNums = shaderDesc.BoundResources;
	//	


	//	for (UINT i = 0; i < shaderResourceNums; ++i)
	//	{

	//		D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
	//		shaderReflectionInterface->GetResourceBindingDesc(i, &shaderInputBindDesc);
	//		
	//		const std::wstring shaderResourceName = Utility::ConvertToWString(shaderInputBindDesc.Name,true);
	//		if (renderPass->GetShaderResource(shaderResourceName) != nullptr)
	//		{

	//			//이미 리소수가 존재함으로 새로 만들필요없이
	//			//두개이상의 셰이더에서 이 리소스에 접근함으로 셰이더의 가시성설정을 ALL로 변경한다.
	//			ShaderResource* shaderResource = renderPass->GetShaderResource(shaderResourceName);
	//			shaderResource->SetShaderResourceVisibility(EShaderResourceVisibility::eAll);
	//			continue;
	//		}
	//		int registerIndex = shaderInputBindDesc.BindPoint;
	//		int registerSpace = shaderInputBindDesc.Space;


	//		switch (shaderInputBindDesc.Type)
	//		{

	//		case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER://상수버퍼
	//		{
	//			ID3D12ShaderReflectionConstantBuffer* pShaderReflectionConstantBuffer = shaderReflectionInterface->GetConstantBufferByName(shaderInputBindDesc.Name);




	//			D3D12_SHADER_BUFFER_DESC shaderBufferDesc;
	//			pShaderReflectionConstantBuffer->GetDesc(&shaderBufferDesc);

	//			UINT bufferSize = shaderBufferDesc.Size;
	//			UINT variableNum = shaderBufferDesc.Variables;


	//			ShaderResourceConstantBuffer* shaderResourceCosntantBuffer =
	//				new ShaderResourceConstantBuffer(shaderResourceName, registerIndex, registerSpace);
	//			SetPassOrPassFlag(shaderResourceCosntantBuffer);
	//			std::vector<ShaderResourceVariable> variableVector(variableNum);


	//			//리소스내의 변수
	//			for (UINT variableIndex = 0; variableIndex < variableNum; ++variableIndex)
	//			{
	//				ID3D12ShaderReflectionVariable* pShaderReflectionVariable = pShaderReflectionConstantBuffer->GetVariableByIndex(variableIndex);

	//				D3D12_SHADER_VARIABLE_DESC shaderVariableDesc;
	//				pShaderReflectionVariable->GetDesc(&shaderVariableDesc); //그 변수의 이름,사이즈정보가있고

	//				//그변수의 타입정보 
	//				ID3D12ShaderReflectionType* pVariableType = pShaderReflectionVariable->GetType();

	//				//ID3D12ShaderReflectionType* pBaseVariableType = pVariableType->GetBaseClass();
	//				D3D12_SHADER_TYPE_DESC shaderTypeDesc;
	//				pVariableType->GetDesc(&shaderTypeDesc);
	//			
	//				variableVector[variableIndex].mName = Utility::ConvertToWString(shaderVariableDesc.Name, true);
	//				variableVector[variableIndex].mOffsetInShaderResource = shaderVariableDesc.StartOffset;
	//				variableVector[variableIndex].mSize = shaderVariableDesc.Size;
	//				variableVector[variableIndex].mElementNum = shaderTypeDesc.Elements;
	//				variableVector[variableIndex].mClass = shaderTypeDesc.Class;
	//				variableVector[variableIndex].mTypeName = Utility::ConvertToWString(shaderTypeDesc.Name,true);

	//				
	//				//변수가 구조체인경우

	//				//구조체를서술하는 객체를생성한다.
	//					
	//				//구조체의 크기, 정렬크기를 계산해준다(변수들의타입을 보고)
	//				//이미있는구조체라면 무시
	//					if (variableVector[variableIndex].mClass == D3D_SVC_STRUCT && shaderResourceCosntantBuffer->GetShaderResourceStructDesc(variableVector[variableIndex].mTypeName) == nullptr)
	//					{

	//						ShaderResourceStructTypeDesc* pShadarResourceStructTypeDesc = new ShaderResourceStructTypeDesc;
	//						//구조체기본설정
	//						pShadarResourceStructTypeDesc->mName = variableVector[variableIndex].mTypeName;
	//						pShadarResourceStructTypeDesc->mVariableVector.resize(shaderTypeDesc.Members);

	//						int structSize = 0;
	//						//구조체내부변수들
	//						for (int memberIndex = 0; memberIndex < shaderTypeDesc.Members; ++memberIndex)
	//						{
	//					
	//							ID3D12ShaderReflectionType* pMemberType = pVariableType->GetMemberTypeByIndex(memberIndex);
	//							
	//							pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mName = Utility::ConvertToWString(pVariableType->GetMemberTypeName(memberIndex), true);
	//						
	//							//D3D12_SHADER_VARIABLE_DESC
	//							
	//							D3D12_SHADER_TYPE_DESC memberTypeDesc;
	//							pMemberType->GetDesc(&memberTypeDesc);
	//						

	//							pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mOffsetInShaderResource=
	//							memberTypeDesc.Offset;
	//							pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mClass = memberTypeDesc.Class;
	//							pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mElementNum = memberTypeDesc.Elements;
	//							pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mTypeName =Utility::ConvertToWString(memberTypeDesc.Name,true);
	//				

	//							
	//							pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mSize = 0;
	//							pShadarResourceStructTypeDesc->mVariableVector[memberIndex].mSize = GetDefaultVariableSize(memberTypeDesc);


	//						}

	//						pShadarResourceStructTypeDesc->mSize = pShadarResourceStructTypeDesc->mVariableVector[shaderTypeDesc.Members-1].mOffsetInShaderResource + pShadarResourceStructTypeDesc->mVariableVector[shaderTypeDesc.Members - 1].mSize;

	//						pShadarResourceStructTypeDesc->mAlignedSize = (variableVector[variableIndex].mSize - pShadarResourceStructTypeDesc->mSize) / (variableVector[variableIndex].mElementNum - 1);

	//						//int lastMemeberIndex = shaderTypeDesc.Members - 1;
	//						////마지막멤버의 offset+마지막멤버의 크기 = 구조체의 총크기
	//						//pShadarResourceStructTypeDesc->mSize = pShadarResourceStructTypeDesc->mVariableVector[lastMemeberIndex].mOffsetInShaderResource + pShadarResourceStructTypeDesc->mVariableVector[lastMemeberIndex].mSize;


	//						


	//						shaderResourceCosntantBuffer->AddShaderResourceStructDesc(pShadarResourceStructTypeDesc);

	//					}
	//	
	//			}




	//			//변수들의타입,안에들어있는 변수들의 종류,등등이올바른지 점검한다.


	//			shaderResourceCosntantBuffer->Initialize(std::move(variableVector), bufferSize);

	//			shaderResourceCosntantBuffer->SetConstantBuffer(ConstantBufferManager::GetConstantBuffer(bufferSize));
	//			
	//			shaderResourceCosntantBuffer->SetShaderResourceVisibility(shaderResourceVisibility);
	//			renderPass->AddShaderResourece(shaderResourceCosntantBuffer);

	//		}
	//		break;
	//		case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:

	//		{
	//			EShaderResourceTextureType eShaderResourceTextureType;
	//			switch (shaderInputBindDesc.Dimension)
	//			{
	//			case D3D_SRV_DIMENSION_TEXTURE2D:
	//				eShaderResourceTextureType = EShaderResourceTextureType::eT2D;
	//				break;
	//			}
	//			ShaderResourceTexture* shaderResourceTexture = new ShaderResourceTexture(
	//				shaderResourceName, registerIndex, registerSpace,eShaderResourceTextureType);

	//			
	//			shaderResourceTexture->SetShaderResourceVisibility(shaderResourceVisibility);
	//			shaderResourceTexture->SetObjectOrPassFlag(true);
	//			renderPass->AddShaderResourece(shaderResourceTexture);

	//		}

	//			break;
	//		case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:

	//			
	//		{

	//			ViewIndex viewIndex = InvalidViewIndex;

	//			viewIndex=mSamplerManager.GetSamplerViewIndex(shaderResourceName);
	//			if (viewIndex == InvalidViewIndex)
	//			{
	//				MessageBox(nullptr, L"샘플러이름이 잘못됨", L"error", 0);
	//				assert(0);
	//			}

	//			/*if (shaderResourceName == L"gSamMinMagMipLinear")
	//			{
	//				viewIndex= mSamplerManager.GetSamplerViewIndex(shaderResourceName);

	//			}
	//			else
	//			{
	//				MessageBox(nullptr, L"샘플러이름이 잘못됨", L"error", 0);
	//				assert(0);
	//			}*/

	//			ShaderResourceSampler* shaderResourceSampler = new ShaderResourceSampler(shaderResourceName, registerIndex, registerSpace, viewIndex);

	//			shaderResourceSampler->SetShaderResourceVisibility(shaderResourceVisibility);
	//			shaderResourceSampler->SetObjectOrPassFlag(false);
	//			renderPass->AddShaderResourece(shaderResourceSampler);

	//		}
	//			break;
	//		}


	//	}

	//}




	//bool ResourceController::CreateRootSignature(RenderPassTwo* renderPass)
	//{
	//	const std::vector<ShaderResource*>& shaderResourceVector = renderPass->GetShaderResourceVector();
	//	std::vector<D3D12_ROOT_PARAMETER> mRootParameterVector(shaderResourceVector.size());
	//	std::vector< D3D12_ROOT_DESCRIPTOR_TABLE> mRootDescriptorTableVector(shaderResourceVector.size());
	//	std::vector< D3D12_DESCRIPTOR_RANGE > mRootDescriptorRangeVector(shaderResourceVector.size());

	//	for (int shaderResourceIndex = 0; shaderResourceIndex < shaderResourceVector.size(); ++shaderResourceIndex)
	//	{
	//		ShaderResource* currentShaderResource = shaderResourceVector[shaderResourceIndex];
	//		D3D12_ROOT_PARAMETER& currentRootParameter = mRootParameterVector[shaderResourceIndex];
	//		D3D12_DESCRIPTOR_RANGE& currentDescriptorRange = mRootDescriptorRangeVector[shaderResourceIndex];


	//		currentRootParameter.ShaderVisibility = ConvertD3DShaderVisibility(currentShaderResource->GetShaderResourceVisibility());
	//		currentRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//일단 고정


	//		currentDescriptorRange.BaseShaderRegister = currentShaderResource->GetRegisterIndex();
	//		currentDescriptorRange.NumDescriptors = 1;
	//		currentDescriptorRange.OffsetInDescriptorsFromTableStart = 0;
	//		currentDescriptorRange.RangeType = ConvertD3DShaderRangeType(currentShaderResource);
	//		currentDescriptorRange.RegisterSpace = currentShaderResource->GetRegisterSpace();

	//		currentRootParameter.DescriptorTable.NumDescriptorRanges = 1;
	//		currentRootParameter.DescriptorTable.pDescriptorRanges = &currentDescriptorRange;
	//	
	//	}


	//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//	rootSignatureDesc.NumParameters = mRootParameterVector.size();
	//	rootSignatureDesc.pParameters = mRootParameterVector.data();
	//	rootSignatureDesc.NumStaticSamplers = 0;
	//	rootSignatureDesc.pStaticSamplers = nullptr;
	//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//	//루트시그니처 직렬화
	//	ID3DBlob* serializedRootSignatureBlob;
	//	ID3DBlob* errorBlob;
	//	HRESULT ret =D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &serializedRootSignatureBlob, &errorBlob);

	//	if (errorBlob != nullptr)
	//	{
	//		MessageBox(nullptr, L"루트시그니처 직렬화 에러", L"error", 0);
	//		assert(0);
	//	}
	//	ThrowIfFailed(ret);

	//	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	//	ThrowIfFailed(mDevice->CreateRootSignature(0, serializedRootSignatureBlob->GetBufferPointer(), serializedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf())));

	//	renderPass->SetRootSignature(rootSignature);
	//	return true;


	//}

	//bool ResourceController::CreateGraphicsPipeline(const RawRenderPassDataTwo& rawRenderPassTwoData,
	//	RenderPassTwo* renderPass,const std::vector<Shader*> & shaderVector,
	//	Shader *vertexShader )
	//{

	//	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc;

	//	
	//	ZeroMemory(&graphicsPipelineStateDesc, sizeof(graphicsPipelineStateDesc));

	//	graphicsPipelineStateDesc.NodeMask = 0;
	//	graphicsPipelineStateDesc.pRootSignature = renderPass->GetRootSignature().Get();
	//	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	//effect파일의 옵션
	//	renderPass->SetPrimitiveToplogyType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	graphicsPipelineStateDesc.BlendState = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());
	//	graphicsPipelineStateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
	//	graphicsPipelineStateDesc.SampleMask = 0xFFFFFFFF;		//이거 0하면 렌더링안됨
	//	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	graphicsPipelineStateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT());
	//	//graphicsPipelineStateDesc.RasterizerState.FillMode =
	//	//graphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//	graphicsPipelineStateDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	//	graphicsPipelineStateDesc.NumRenderTargets = 1;
	//	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	graphicsPipelineStateDesc.SampleDesc.Count=1;
	//	graphicsPipelineStateDesc.SampleDesc.Quality=0;
	//	
	//	SetGraphicsPipelineCfg(renderPass, graphicsPipelineStateDesc, rawRenderPassTwoData.mGraphicsPipelineCfgVector);
	//	
	//	//input layout 
	//	Microsoft::WRL::ComPtr< ID3D12ShaderReflection> shaderReflectionInterface;
	//	D3DReflect(vertexShader->GetShaderByteCode().Get()->GetBufferPointer(), vertexShader->GetShaderByteCode().Get()->GetBufferSize(), IID_PPV_ARGS(shaderReflectionInterface.GetAddressOf()));

	//	D3D12_SHADER_DESC shaderDesc;
	//	shaderReflectionInterface->GetDesc(&shaderDesc);
	//	UINT inputParameterNums = shaderDesc.InputParameters;

	//	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescVector;
	//	inputElementDescVector.resize(inputParameterNums);
	//	for (UINT inputParameterIndex = 0; inputParameterIndex < inputParameterNums; ++inputParameterIndex)
	//	{
	//		D3D12_SIGNATURE_PARAMETER_DESC pSignatureParameterDesc;
	//		//디버그 출력할것
	//		ThrowIfFailed(shaderReflectionInterface->GetInputParameterDesc(inputParameterIndex, &pSignatureParameterDesc));

	//		inputElementDescVector[inputParameterIndex].SemanticName = pSignatureParameterDesc.SemanticName;
	//		inputElementDescVector[inputParameterIndex].SemanticIndex = pSignatureParameterDesc.SemanticIndex;
	//		inputElementDescVector[inputParameterIndex].InputSlot = 0;
	//		inputElementDescVector[inputParameterIndex].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	//		inputElementDescVector[inputParameterIndex].InstanceDataStepRate = 0;
	//		inputElementDescVector[inputParameterIndex].Format = GetInputElementFormat(pSignatureParameterDesc.SemanticName,
	//			pSignatureParameterDesc.ComponentType, pSignatureParameterDesc.Mask);

	//		inputElementDescVector[inputParameterIndex].AlignedByteOffset = GetInputElementOffset(pSignatureParameterDesc.SemanticName);
	//	}

	//	graphicsPipelineStateDesc.InputLayout.NumElements = inputElementDescVector.size();//이것도 reflection;
	//	graphicsPipelineStateDesc.InputLayout.pInputElementDescs = inputElementDescVector.data();//이것도 reflection;






	//	//shader

	//	for (int i = 0; i < shaderVector.size(); ++i)
	//	{

	//		Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode = shaderVector[i]->GetShaderByteCode();

	//		switch (shaderVector[i]->GetShaderType())
	//		{
	//		case EShaderType::eVS:
	//			graphicsPipelineStateDesc.VS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

	//			graphicsPipelineStateDesc.VS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
	//			break;

	//		case EShaderType::ePS:
	//			graphicsPipelineStateDesc.PS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

	//			graphicsPipelineStateDesc.PS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
	//			
	//			break;

	//		case EShaderType::eGS:
	//			graphicsPipelineStateDesc.GS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

	//			graphicsPipelineStateDesc.GS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
	//			break;
	//		case EShaderType::eDS:

	//			graphicsPipelineStateDesc.DS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

	//			graphicsPipelineStateDesc.DS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
	//			break;
	//		case EShaderType::eHS:
	//			graphicsPipelineStateDesc.HS.pShaderBytecode = shaderByteCode.Get()->GetBufferPointer();

	//			graphicsPipelineStateDesc.HS.BytecodeLength = shaderByteCode.Get()->GetBufferSize();
	//			break;
	//		}
	//	}
	//	
	//	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	//	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	//	{
	//		debugController->EnableDebugLayer();
	//	}

	//	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
	//	ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(graphicsPipelineState.GetAddressOf())));

	//	
	//	////Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	//	//if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	//	//{
	//	//	debugController->EnableDebugLayer();
	//	//}

	//	renderPass->SetPipelineState(graphicsPipelineState);
	//	
	//	return true;
	//}

	//void ResourceController::SetGraphicsPipelineCfg(RenderPassTwo* renderPass,
	//	D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipelineStateDesc, const std::vector<std::pair<std::wstring, std::wstring>>& graphicsPipelineCfgVector)
	//{

	//	for (int i = 0; i < graphicsPipelineCfgVector.size(); ++i)
	//	{
	//		const std::wstring& key = graphicsPipelineCfgVector[i].first;
	//		const std::wstring& value = graphicsPipelineCfgVector[i].second;
	//		
	//		if (key == L"STENCILENABLE")
	//		{
	//			graphicsPipelineStateDesc.DepthStencilState.StencilEnable = Utility::ConvertBoolValue(value);
	//		//	graphicsPipelineStateDesc.DepthStencilState.StencilEnable = Utility::ConvertBoolValue(value);
	//		}
	//		else if (key == L"STENCILFAILOP")
	//		{
	//			graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp = ConvertStencilOP(value, renderPass);
	//			graphicsPipelineStateDesc.DepthStencilState.BackFace.StencilFailOp = graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFailOp;
	//	
	//		}
	//		else if (key == L"STENCILDEPTHFAILOP")
	//		{
	//			graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp = ConvertStencilOP(value, renderPass);
	//			graphicsPipelineStateDesc.DepthStencilState.BackFace.StencilDepthFailOp = graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilDepthFailOp;
	//		}
	//		else if (key == L"STENCILPASSOP")
	//		{
	//			graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp = ConvertStencilOP(value, renderPass);
	//			graphicsPipelineStateDesc.DepthStencilState.BackFace.StencilPassOp = graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilPassOp;
	//		}
	//		else if (key == L"STENCILFUNC")
	//		{
	//			graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFunc = ConvertComparisionFunEnum(value);
	//			graphicsPipelineStateDesc.DepthStencilState.BackFace.StencilFunc = graphicsPipelineStateDesc.DepthStencilState.FrontFace.StencilFunc;
	//		}
	//		else if (key == L"PRIMITIVETOPOLOGYTYPE")
	//		{

	//			graphicsPipelineStateDesc.PrimitiveTopologyType = ConvertPrimitiveTopologyType(value);
	//			renderPass->SetPrimitiveToplogyType(ConvertCommandListPrimitiveTopologyType(value));
	//		
	//		}
	//		else if (key == L"FILLMODE")
	//		{
	//			if (value == L"WIREFRAME")
	//			{
	//				graphicsPipelineStateDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//			}
	//		}
	//		else if (key == L"CULLMODE")
	//		{

	//			graphicsPipelineStateDesc.RasterizerState.CullMode = ConvertCullMode(value);

	//		}
	//		else if (key == L"DEPTHFUNC")
	//		{
	//			graphicsPipelineStateDesc.DepthStencilState.DepthFunc = ConvertComparisonFuncEnum(value);
	//		}
	//		else if (key == L"DEPTHENABLE")
	//		{
	//			graphicsPipelineStateDesc.DepthStencilState.DepthEnable = Utility::ConvertBoolValue(value);
	//		}
	//		else if (key == L"RENDERPASSID")
	//		{
	//			renderPass->SetRenderPassID(ConvertRenderPassID(value));
	//		}
	//		else if (key == L"BLENDOP")
	//		{
	//			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = ConvertD3DBlendOpEnum(value);
	//		}
	//		else if (key == L"BLENDENABLE")
	//		{
	//			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable= Utility::ConvertBoolValue(value);
	//		}
	//		else if (key == L"SRCBLEND")
	//		{
	//			graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = ConvertBlendFactorEnum(value);
	//		}
	//		else if (key == L"DESTBLEND")
	//		{
	//			graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = ConvertBlendFactorEnum(value);
	//		}



	//	}









	//}

	//D3D12_SHADER_VISIBILITY ResourceController::ConvertD3DShaderVisibility(EShaderResourceVisibility visibility)
	//{
	//	switch (visibility)
	//	{

	//	case EShaderResourceVisibility::eAll:
	//		return D3D12_SHADER_VISIBILITY_ALL;

	//	case EShaderResourceVisibility::ePixel:

	//		return D3D12_SHADER_VISIBILITY_PIXEL;

	//	case EShaderResourceVisibility::eVertex:

	//		return D3D12_SHADER_VISIBILITY_VERTEX;

	//	case EShaderResourceVisibility::eDomain:
	//		return D3D12_SHADER_VISIBILITY_DOMAIN;

	//	case EShaderResourceVisibility::eGeometry:
	//		return D3D12_SHADER_VISIBILITY_GEOMETRY;

	//	case EShaderResourceVisibility::eHull:

	//		return D3D12_SHADER_VISIBILITY_HULL;

	//		
	//	case EShaderResourceVisibility::eMesh:

	//		return D3D12_SHADER_VISIBILITY_MESH;
	//		
	//	case EShaderResourceVisibility::eAmplification:

	//		return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
	//	}


	//}

	//D3D12_DESCRIPTOR_RANGE_TYPE ResourceController::ConvertD3DShaderRangeType(ShaderResource* shaderResource)
	//{
	//	switch (shaderResource->GetShaderResourceType())
	//	{
	//	case EShaderResourceType::eConstantBuffer:

	//		return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;

	//	case EShaderResourceType::eTexture:

	//		return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	//	case EShaderResourceType::eSampler:

	//		return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	//	}

	//	
	//}

	//UINT ResourceController::GetInputElementOffset(const std::string& sementicName)
	//{

	//	if (sementicName == "POSITION")
	//	{
	//		return 0;
	//	}
	//	else if (sementicName == "NORMAL")
	//	{
	//		return 12;
	//	}
	//	else if (sementicName == "TEX")
	//	{
	//		return 24;
	//	}
	//	else if (sementicName == "BONEWEIGHTONE")
	//	{
	//		return 24 + 8;
	//	}
	//	else if (sementicName == "BONEWEIGHTTWO")
	//	{
	//		return 48;
	//	}
	//	else if (sementicName == "BONEINDEXONE")
	//	{
	//		return 64;
	//	}
	//	else if (sementicName == "BONEINDEXTWO")
	//	{
	//		return 80;
	//	}


	//	return 0;
	//}

	//DXGI_FORMAT ResourceController::GetInputElementFormat(const std::string& sementicName , D3D_REGISTER_COMPONENT_TYPE componentType , BYTE mask)
	//{

	//	if (sementicName == "POSITION")
	//	{
	//		if (componentType != D3D_REGISTER_COMPONENT_FLOAT32 || (mask != 0b0111))
	//		{
	//			//디버그출력 

	//			OutputDebugString(L"input layout의 POSITION 시맨틱에 해당하는 속성의 타입이틀리다.\n");
	//			assert(0);
	//		}

	//		return DXGI_FORMAT_R32G32B32_FLOAT;
	//	}
	//	else if (sementicName == "NORMAL")
	//	{
	//		if (componentType != D3D_REGISTER_COMPONENT_FLOAT32 || (mask != 0b0111))
	//		{
	//			//디버그출력 

	//			OutputDebugString(L"input layout의 NORMAL 시맨틱에 해당하는 속성의 타입이틀리다.\n");
	//			assert(0);
	//		}



	//		return DXGI_FORMAT_R32G32B32_FLOAT;
	//	}
	//	else if (sementicName == "TEX")
	//	{
	//		if (componentType != D3D_REGISTER_COMPONENT_FLOAT32 || (mask != 0b0011))
	//		{
	//			//디버그출력 

	//			OutputDebugString(L"input layout의 TEX 시맨틱에 해당하는 속성의 타입이틀리다.\n");
	//			assert(0);
	//		}



	//		return DXGI_FORMAT_R32G32_FLOAT;
	//	}
	//	else if (sementicName == "BONEWEIGHTONE" || sementicName =="BONEWEIGHTTWO")
	//	{
	//		if (componentType != D3D_REGISTER_COMPONENT_FLOAT32 || (mask != 0b1111))
	//		{
	//			//디버그출력 

	//			OutputDebugString(L"input layout의 BONEWEIGHTONE 또는 BONEWEIGHTTWO 시맨틱에 해당하는 속성의 타입이틀리다.\n");
	//			assert(0);
	//		}
	//		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	//	}
	//	else if (sementicName == "BONEINDEXONE" || sementicName == "BONEINDEXTWO")
	//	{
	//		if (componentType != D3D_REGISTER_COMPONENT_SINT32 || (mask != 0b1111))
	//		{
	//			//디버그출력 
	//		
	//			OutputDebugString(L"input layout의 BONEINDEXONE 또는 BONEINDEXTWO 시맨틱에 해당하는 속성의 타입이틀리다.\n");
	//			assert(0);
	//		}
	//		return DXGI_FORMAT_R32G32B32A32_SINT;


	//	}

	//}

	//void ResourceController::SetPassOrPassFlag(ShaderResourceConstantBuffer* shaderResourceConstantBuffer)
	//{
	//	const std::wstring& name = shaderResourceConstantBuffer->GetName();
	//	if (name == L"ObjectConstant" || name == L"MaterialConstant")
	//	{
	//		shaderResourceConstantBuffer->SetObjectOrPassFlag(true);
	//	}
	//	else if (name == L"PassConstant"|| name == L"LightConstant")
	//	{
	//		shaderResourceConstantBuffer->SetObjectOrPassFlag(false);
	//	}

	//}

	//D3D12_STENCIL_OP ResourceController::ConvertStencilOP(const std::wstring& str, RenderPassTwo * renderPass) const
	//{
	//	if (str == L"KEEP")
	//	{
	//		return D3D12_STENCIL_OP_KEEP;
	//	}
	//	else
	//	{
	//		renderPass->SetStencilWriteFlag(true);

	//		if (str == L"ZERO")
	//		{
	//		return D3D12_STENCIL_OP_ZERO;
	//		}
	//		else if (str == L"REPLACE")
	//		{
	//		return D3D12_STENCIL_OP_REPLACE;
	//		}
	//		else if (str == L"INCR_SAT")
	//		{
	//		return D3D12_STENCIL_OP_INCR_SAT;
	//		}
	//		else if (str == L"DESCR_SAT")
	//		{
	//		return D3D12_STENCIL_OP_DECR_SAT;
	//		}
	//		else if (str == L"INVERT")
	//		{
	//		return D3D12_STENCIL_OP_INVERT;

	//		}
	//		else if (str == L"INCR")
	//		{
	//		return D3D12_STENCIL_OP_INCR;
	//		}
	//		else if (str == L"DECR")
	//		{
	//		return D3D12_STENCIL_OP_DECR;
	//		}
	//		else
	//		{
	//		//디버그 출력
	//		return D3D12_STENCIL_OP_KEEP;
	//		}
	//	}
	//}

	//D3D12_COMPARISON_FUNC ResourceController::ConvertComparisionFunEnum(const std::wstring& str) const
	//{
	//	if (str == L"NEVER")
	//	{
	//		return D3D12_COMPARISON_FUNC_NEVER;
	//	}
	//	else if (str == L"LESS")
	//	{
	//		return D3D12_COMPARISON_FUNC_LESS;
	//	}
	//	else if (str == L"EQUAL")
	//	{
	//		return D3D12_COMPARISON_FUNC_EQUAL;
	//	}
	//	else if (str == L"LESS_EQUAL")
	//	{
	//		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//	}
	//	else if (str == L"GREATER")
	//	{
	//		return D3D12_COMPARISON_FUNC_GREATER;
	//	}
	//	else if (str == L"NOT_EQUAL")
	//	{
	//		return D3D12_COMPARISON_FUNC_GREATER;
	//	}
	//	else if (str == L"GREATER_EQUAL")
	//	{
	//		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	//	}
	//	else if (str == L"ALWAYS")
	//	{
	//		return D3D12_COMPARISON_FUNC_ALWAYS;
	//	}
	//	else
	//	{
	//		//디버그 출력
	//		return D3D12_COMPARISON_FUNC_LESS;
	//	}
	//
	//}

	//D3D12_PRIMITIVE_TOPOLOGY_TYPE ResourceController::ConvertPrimitiveTopologyType(const std::wstring& str) const
	//{
	//	if (str == L"POINTLIST")
	//	{
	//		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	//	}
	//	else if (str == L"LINELIST" || str==L"LINESTRIP")
	//	{
	//		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	//	}
	//	else if (str == L"TRIANGLELIST"||str==L"TRIANGLESTRIP")
	//	{
	//		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//	}
	//	else if (str == L"PATCH")//패치는 다시고려할것
	//	{
	//		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	//	}
	//	else
	//	{
	//		//디버그출력
	//		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//	}

	//}

	//D3D_PRIMITIVE_TOPOLOGY ResourceController::ConvertCommandListPrimitiveTopologyType(const std::wstring& str) const
	//{

	//	if (str == L"POINTLIST")
	//	{
	//		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	//	}
	//	else if (str == L"LINELIST")
	//	{
	//		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	//	}
	//	else if (str == L"LINESTRIP")
	//	{
	//		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	//	}
	//	else if (str == L"TRIANGLESTRIP")
	//	{
	//		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	//	}
	//	else if (str == L"TRIANGLELIST")
	//	{
	//		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	}
	//	else if (str == L"PATCH")//패치는 다시고려할것
	//	{
	//		//return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	//	}
	//	else
	//	{
	//		//디버그출력
	//		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	}
	//}

	//UINT ResourceController::GetDefaultVariableSize(const D3D12_SHADER_TYPE_DESC& variableTypeDesc)
	//{

	//	D3D_SHADER_VARIABLE_CLASS variableClass = variableTypeDesc.Class;
	//	int variableElementSize = GetDefaultVariableElementSize(variableTypeDesc.Type);


	//	int row = variableTypeDesc.Rows;
	//	int column = variableTypeDesc.Columns;

	//	int elementNum = variableTypeDesc.Elements;

	//	switch (variableClass)
	//	{

	//	case D3D_SVC_SCALAR :
	//		if (elementNum == 0)
	//		{
	//			return variableElementSize;
	//		}
	//		else
	//		{
	//			return 16 * elementNum;
	//		}
	//
	//	case D3D_SVC_VECTOR:
	//		if (elementNum == 0)
	//		{
	//			return variableElementSize * column;
	//		}
	//		else
	//		{
	//			return 16 * column * elementNum;
	//		}
	//	case D3D_SVC_MATRIX_ROWS:
	//		if (elementNum == 0)
	//		{
	//			return variableElementSize * column * row;
	//		}
	//		else
	//		{
	//			return 16 * 4 * row * elementNum;
	//		}
	//	case D3D_SVC_MATRIX_COLUMNS:
	//		if (elementNum == 0)
	//		{
	//			return variableElementSize * column * row;
	//		}
	//		else
	//		{
	//			return 16 * 4 * column * elementNum;
	//		}
	//	}











	//	return 0;
	//}

	//UINT ResourceController::GetDefaultVariableElementSize(D3D_SHADER_VARIABLE_TYPE type)
	//{

	//	switch (type)
	//	{
	//	case   D3D_SVT_BOOL:

	//	case D3D_SVT_INT:
	//	case D3D_SVT_FLOAT:
	//	case D3D_SVT_UINT:
	//		return 4;
	//	case D3D_SVT_UINT8:
	//		return 1;
	//	case D3D_SVT_DOUBLE:
	//		return 8;
	//	}

	//	return 0;
	//}





	bool ResourceController::CreateNewMeshFromGenerator()
	{

		return true;
	}

	bool ResourceController::CreateNewDefaultMaterialFromGenerator()
	{
		
		return true;
	}

	void ResourceController::CreateSampler()
	{

		D3D12_SAMPLER_DESC sprDesc;
		sprDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sprDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sprDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sprDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sprDesc.MipLODBias = 0;
		sprDesc.MaxAnisotropy = 16;
		sprDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sprDesc.BorderColor[0] = 1.0f;
		sprDesc.BorderColor[1] = 1.0f;
		sprDesc.BorderColor[2] = 1.0f;
		sprDesc.BorderColor[3] = 1.0f;
		sprDesc.MinLOD = 0;
		sprDesc.MaxLOD = 100;


		ViewIndex viewIndex = mDescriptorHeapManagerMaster->CreateSPV(&sprDesc);

		mSamplerManager.RegisterSampler(L"gSamMinMagMipLinear", viewIndex);



		//gSamMinMagMipLinearClamp
		sprDesc;
		sprDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sprDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sprDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sprDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		sprDesc.MipLODBias = 0;
		sprDesc.MaxAnisotropy = 16;
		sprDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sprDesc.BorderColor[0] = 1.0f;
		sprDesc.BorderColor[1] = 1.0f;
		sprDesc.BorderColor[2] = 1.0f;
		sprDesc.BorderColor[3] = 1.0f;
		sprDesc.MinLOD = 0;
		sprDesc.MaxLOD = 100;


		viewIndex = mDescriptorHeapManagerMaster->CreateSPV(&sprDesc);

		mSamplerManager.RegisterSampler(L"gSamMinMagMipLinearClamp", viewIndex);








		//gSamMinMagMipLinearBorder
		//sprDesc;
		sprDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sprDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sprDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sprDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sprDesc.MipLODBias = 0;
		sprDesc.MaxAnisotropy = 16;
		sprDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		sprDesc.BorderColor[0] = 0.0f;
		sprDesc.BorderColor[1] = 0.0f;
		sprDesc.BorderColor[2] = 0.0f;
		sprDesc.BorderColor[3] = 1.0f;
		sprDesc.MinLOD = 0;
		sprDesc.MaxLOD = 100;


		viewIndex = mDescriptorHeapManagerMaster->CreateSPV(&sprDesc);

		mSamplerManager.RegisterSampler(L"gSamMinMagMipLinearBorder", viewIndex);














	}

	bool ResourceController::DeleteTexture(const std::wstring& name)
	{
		return false;
	}

	bool ResourceController::DeleteTexture(Texture* texture)
	{

		mTextureManager->KillTexture(texture);

		//ViewIndex viewIndex = texture->GetViewIndex();
		//mDescriptorHeapManagerMaster->ReleaseCbvSrvUav(viewIndex);
		//mTextureManager->DeleteTexture(texture);
		
		//문제는뭐냐 댕글링포인터가 된다는거지 ,
		
		return true;
	}

	//Event* ResourceController::GetEvent()
	//{
	//	WriterStart();

	//	Event* pEvent = nullptr;
	//	if (!mEventQueue.isEmpty())
	//	{
	//		pEvent = mEventQueue.Front();
	//		mEventQueue.Pop();

	//		WriterEnd();
	//		return pEvent;
	//	}

	//	WriterEnd();
	//	return pEvent;
	//}

	void ResourceController::DragAndDropFile(Event* event)
	{
		DragAndDropEvent* pEvent = (DragAndDropEvent*)event;

		std::wstring filePath = pEvent->GetFilePath();

		std::wstring extension = PathFindExtensionW(filePath.c_str());

		if (extension == L"mesh")
		{

		}
		else if (extension == L"mat")
		{

		}
		else
		{
			mResourceLoader->LoadTextureFile(Utility::ConvertToString(filePath, true));




			//std::wstring fileNameW = PathFindFileName(filePath.c_str());
			//Texture* texture = mTextureManager->CreateTexture(fileNameW);
			//if (!mResourceLoader->LoadTextureFile(filePath, *texture));
			////mTextureManager->Delete;

			//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
			//Microsoft::WRL::ComPtr<ID3D12Resource> resource = texture->GetResource();
			//srvDesc.Format = resource->GetDesc().Format;
			//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			//if (resource->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
			//{
			//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			//}
			//srvDesc.Texture2D.MipLevels = resource->GetDesc().MipLevels;
			//srvDesc.Texture2D.MostDetailedMip = 0;
			//srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			//srvDesc.Texture2D.PlaneSlice = 0;


			//ViewIndex viewIndex = mDescriptorHeapManagerMaster->CreateSrv(texture->GetResource(), &srvDesc);
			//if (viewIndex != -1)
			//	texture->SetViewIndex(viewIndex);

		/*	FileUiEntity* entity = new FileUiEntity(L"first");
			entity->Initialize();


			Model* model = entity->GetModel();
			model->SetTextureToAllSubMesh(texture);*/


			//mFileUiSystem->AddFileUiEntity(entity);

		}





	}

	//bool ResourceController::LoadAndCreateTextureResource(const std::wstring  & filePath, const std::wstring  & textureName)
	//{
	//	Texture* texture = mTextureManager->CreateTexture(textureName);

	//	bool result = mResourceLoader->LoadTextureFile(filePath, *texture);
	//	if (result != true)
	//		return result;

	//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = texture->GetResource();
	//	srvDesc.Format = textureResource->GetDesc().Format;
	//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//	if (textureResource->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)
	//	{
	//		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//	}
	//	srvDesc.Texture2D.MipLevels = textureResource->GetDesc().MipLevels;
	//	srvDesc.Texture2D.MostDetailedMip = 0;
	//	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	//	srvDesc.Texture2D.PlaneSlice = 0;


	//	int viewIndex = mDescriptorHeapManagerMaster->CreateSrv(textureResource, &srvDesc);
	//	if (viewIndex != -1)
	//	{
	//		texture->SetViewIndex(viewIndex);
	//		return true;
	//	}
	//	else return false;
	//}

	void ResourceController::HandleDeleteTextureEvent(Event* pEvent)
	{
		DeleteTexture(((DeleteTextureEvent*)pEvent)->GetTexture());
	}

	//void ResourceController::SendLoadFileResponseEvent(const std::vector<Asset*> & assetVector)
	//{
	//	if (assetVector.size() == 0)
	//		return;

	//	LoadFileResponseEvent* loadFileResponseEvent = new LoadFileResponseEvent;

	//	loadFileResponseEvent->SetAssetVector(assetVector);

	//	Controller::AddEvent("FileUiWindowController", loadFileResponseEvent);


	//}

	//void ResourceController::SendLoadFileResponseEvent(std::vector<Asset*>&& assetVector)
	//{
	//	LoadFileResponseEvent* loadFileResponseEvent = new LoadFileResponseEvent;

	//	loadFileResponseEvent->SetAssetVector(std::move(assetVector));

	//	Controller::AddEvent("FileUiWindowController", loadFileResponseEvent);


	//}

	void ResourceController::SendDefaultAssetLoadResponseEvent()
	{

		//보내기전 엔진에서 기본적으로 제공하는 contentItem들은(asset)
		//gameEngineFlag를 설정한다.


		for(auto & asset : mEditorDefaultAssetVector)
		{ 
			//asset->SetEngineContentItemFlag(true);

			switch(asset->GetAssetType())
			{ 
			case EAssetType::eMesh:
				asset->SetDestLogicalFolderUnqiueID(2);
				break;

			case EAssetType::eMaterial:
				asset->SetDestLogicalFolderUnqiueID(3);

				break;

			case EAssetType::eTexture:
				asset->SetDestLogicalFolderUnqiueID(4);
				break;

			case EAssetType::eAnimationClip:
				asset->SetDestLogicalFolderUnqiueID(2);
			case EAssetType::eSkeleton:
				asset->SetDestLogicalFolderUnqiueID(2);

			}
		}


	//	SendLoadFileResponseEvent(mEditorDefaultAssetVector);
	}

	void ResourceController::LoadEffect(const std::string& effectFolderPath, BaseWindowController* controller)
	{


		std::string beforeDirectoryPath =Utility::SetNewCurrentDirectory(effectFolderPath);

		

		WIN32_FIND_DATA fileData;

		HANDLE handle =	FindFirstFile(L"*.effect", &fileData);

		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				std::string effectName = Utility::ConvertToString(fileData.cFileName, true);
				if (mResourceLoader->LoadEffect(effectName))
				{
					controller->GetRenderSystem()->AddEffect(*EffectManager::GetEffect(effectName), Quad::ESystemType::eMainSystem);
				}
				

			} while (FindNextFile(handle, &fileData));

		}
			

		Utility::SetNewCurrentDirectory(beforeDirectoryPath);


	}

	void ResourceController::SaveUserAssetPackage(const std::string& assetFolderPath)
	{

		














	}

	std::vector<Asset*> ResourceController::LoadUserAssetPackage(const std::string& assetFolderPath)
	{
		return std::vector<Asset*>();
	}



	void ResourceController::SaveUserAsset(const std::string& assetFolderPath)
	{



		//Texture

	//	SaveUserTexture(assetFolderPath);
		SaveTextureMetaDataFile(assetFolderPath);




		//Material
		SaveUserMaterial(assetFolderPath);
		//materialMetaDataFile저장
		const std::string materialMetaFilePath = assetFolderPath + "\\MaterialMetaData.json";
		SaveMaterialMetaDataFile(materialMetaFilePath);




		//Mesh
		SaveUserMesh(assetFolderPath);


		//Skeleton
		SaveUserSkeleton(assetFolderPath);
		const std::string skeletonMetaFilePath = assetFolderPath + "\\SkeletonMetaData.json";
		SaveSkeletonMetaDataFile(skeletonMetaFilePath);

		//AnimClip
		SaveUserAnimClip(assetFolderPath);
		const std::string animClipMetaFilePath = assetFolderPath + "\\AnimClipMetaData.json";
		SaveAnimationClipMetaDataFile(animClipMetaFilePath);


		const std::string meshMetaFilePath = assetFolderPath + "\\MeshMetaData.json";
		SaveMeshMetaDataFile(meshMetaFilePath);

		
	}

	std::vector<Asset*> ResourceController::LoadUserAsset(const std::string& assetFolderPath)
	{

		const std::string textureFolderPath = assetFolderPath + "\\Texture";
		const std::string materialFolderPath = assetFolderPath + "\\Material";
		const std::string meshFolderPath = assetFolderPath + "\\Mesh";




		//Texture

	//	LoadUserTexture(assetFolderPath);
		std::vector<Asset*> assetVector;
	
		std::vector<Asset*> assetTempVector;
		assetTempVector = LoadTextureMetaDataFile(assetFolderPath);
		assetVector.insert(assetVector.end(), assetTempVector.begin(), assetTempVector.end());


		//Material
		 LoadUserMaterial(assetFolderPath);



		const std::string materialMetaFilePath = assetFolderPath + "\\MaterialMetaData.json";
		assetTempVector = 	LoadMaterialMetaDataFile(materialMetaFilePath);
		assetVector.insert(assetVector.end(), assetTempVector.begin(), assetTempVector.end());


		//Mesh
		LoadUserMesh(assetFolderPath);
		const std::string meshMetaDataFilePath = assetFolderPath + "\\MeshMetaData.json";
		assetTempVector = LoadMeshMetaDataFile(meshMetaDataFilePath);
		assetVector.insert(assetVector.end(), assetTempVector.begin(), assetTempVector.end());





		//Skeleton
		LoadUserSkeleton(assetFolderPath);
		const std::string skeletonMetaDataFilePath = assetFolderPath + "\\SkeletonMetaData.json";
		assetTempVector = LoadSkeletonMetaDataFile(skeletonMetaDataFilePath);
		assetVector.insert(assetVector.end(), assetTempVector.begin(), assetTempVector.end());



		//AnimClip
		LoadUserAnimClip(assetFolderPath);
		const std::string animClipMetaDataFilePath = assetFolderPath + "\\AnimClipMetaData.json";
		assetTempVector = LoadAnimationClipMetaDatFile(animClipMetaDataFilePath);
		assetVector.insert(assetVector.end(), assetTempVector.begin(), assetTempVector.end());



		return assetVector;









	}

	void ResourceController::SaveUserMaterial(const std::string& assetFolderPath)
	{
		//다저정할필요없이
		//새로추가되거나 변경된 머터리얼만 저장하면된다.

		const std::string materialFolderPath = assetFolderPath + "\\Material";

		MaterialManager* materialManager = MaterialManager::GetInstance();

		const std::unordered_map<unsigned long long, Material*>& materialIDTable = materialManager->mIDTable.GetTable();

	
		//materialFile저장
		//일단 다저장해보자.


		std::string materialFilePath;
		for (auto element : materialIDTable)
		{
			Material* material = element.second;
			if(!material->GetEngineContentItemFlag())
				mResourceStorer->SaveMaterial(materialFolderPath,material);
		}











	}

	void ResourceController::LoadUserMaterial(const std::string& assetFolderPath)
	{

		const std::string materialFolderPath = assetFolderPath + "\\Material";

		char currentDirectoryBuffer[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, currentDirectoryBuffer);

		SetCurrentDirectoryA(materialFolderPath.c_str());
		
		WIN32_FIND_DATAA fileData;
		HANDLE handle=  FindFirstFileA("*.json", &fileData);


		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				std::string fileName = fileData.cFileName;
				const std::string filePath = fileName;
				mResourceLoader->LoadMaterial(filePath);

			} while (FindNextFileA(handle, &fileData));
		}

		SetCurrentDirectoryA(currentDirectoryBuffer);
	}

	void ResourceController::SaveUserTexture(const std::string& assetFolderPath)
	{

	




	}

	void ResourceController::LoadUserTexture(const std::string& assetFolderPath)
	{

		const std::string textureFolderPath = assetFolderPath + "\\Texture";

		char currentDirectoryBuffer[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, currentDirectoryBuffer);

		SetCurrentDirectoryA(textureFolderPath.c_str());

		WIN32_FIND_DATAW fileData;
		HANDLE handle = FindFirstFileW(L"*.*", &fileData);

		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				const std::string fileName = Utility::ConvertToString(fileData.cFileName, true);
				const std::string filePath = textureFolderPath + "\\" + fileName;
				mResourceLoader->LoadTextureFile(filePath);

			} while (FindNextFileW(handle, &fileData));

		}


	}

	void ResourceController::SaveUserMesh(const std::string& assetFolderPath)
	{

		const std::string meshFolderPath = assetFolderPath + "\\Mesh";
		const std::unordered_map<unsigned long long, Mesh*>& meshIDTable = mMeshManager->mIDTable.GetTable();

		Mesh* mesh = nullptr;
		for (auto& element : meshIDTable)
		{
			mesh = element.second;
			
			if (mesh->GetEngineContentItemFlag())
				continue;
		

			//const std::string meshFilePath = meshFolderPath + "\\" + mesh->GetName()+".json";

			mResourceStorer->SaveMesh(meshFolderPath, mesh);

	
		}


	}


	void ResourceController::LoadUserMesh(const std::string& assetFolderPath)
	{

		const std::string meshFolderPath = assetFolderPath + "\\Mesh";
	

		char currentDirectoryBuffer[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, currentDirectoryBuffer);

		SetCurrentDirectoryA(meshFolderPath.c_str());

		WIN32_FIND_DATAW fileData;
		HANDLE handle = FindFirstFileW(L"*.mesh", &fileData);

		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				const std::string fileName = Utility::ConvertToString(fileData.cFileName, true);
			//	const std::string filePath = meshFolderPath + "\\" + fileName;
				const std::string filePath =fileName;
				mResourceLoader->LoadMeshFile(filePath);

			} while (FindNextFileW(handle, &fileData));

		}

		SetCurrentDirectoryA(currentDirectoryBuffer);

	}

	void ResourceController::SaveUserSkeleton(const std::string& assetFolderPath)
	{


		const std::string skeletonFolderPath = assetFolderPath + "\\Skeleton";

		auto skeletonManager =	SkeletonManager::GetInstance();

		const auto & idTable = skeletonManager->mIDTable.GetTable();

		for (auto& element : idTable)
		{

			Skeleton * skeleton =  element.second;
			if (skeleton->GetEngineContentItemFlag())
				continue;

			mResourceStorer->SaveSkeleton(skeletonFolderPath, skeleton);
		}



	}

	void ResourceController::LoadUserSkeleton(const std::string& assetFolderPath)
	{

		const std::string skeletonFolderPath = assetFolderPath + "\\Skeleton";


		char currentDirectoryBuffer[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, currentDirectoryBuffer);

		SetCurrentDirectoryA(skeletonFolderPath.c_str());

		WIN32_FIND_DATAW fileData;
		HANDLE handle = FindFirstFileW(L"*.skeleton", &fileData);

		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				const std::string fileName = Utility::ConvertToString(fileData.cFileName, true);
				//	const std::string filePath = meshFolderPath + "\\" + fileName;
				const std::string filePath = fileName;
				mResourceLoader->LoadSkeleton(filePath);

			} while (FindNextFileW(handle, &fileData));

		}

		SetCurrentDirectoryA(currentDirectoryBuffer);


	}

	void ResourceController::SaveUserAnimClip(const std::string& assetFolderPath)
	{


		const std::string animClipFolderPath = assetFolderPath + "\\AnimationClip";

		const auto& idTable = mAnimationClipManager.mIDTable.GetTable();

		for (auto& element : idTable)
		{

			AnimationClip* animClip = element.second;
			if (animClip->GetEngineContentItemFlag())
				continue;

			mResourceStorer->SaveAnimationClip(animClipFolderPath, animClip);
		}

	}


	void ResourceController::LoadUserAnimClip(const std::string& assetFolderPath)
	{

		const std::string animClipFolderPath = assetFolderPath + "\\AnimationClip";

		std::string beforeDirectory = Utility::SetNewCurrentDirectory(animClipFolderPath);


		WIN32_FIND_DATAW fileData;
		HANDLE handle = FindFirstFileW(L"*.anim", &fileData);


		if (handle != INVALID_HANDLE_VALUE)
		{

			do
			{
				const std::string fileName = Utility::ConvertToString(fileData.cFileName, true);
				const std::string filePath = fileName;

				mResourceLoader->LoadAnimationClip(fileName);

			} while (FindNextFileW(handle, &fileData));
		}

		Utility::SetNewCurrentDirectory(beforeDirectory);



	}

	void ResourceController::HandleEvent(Event* pEvent)
	{
		//이벤트를 만약전달한다면 다른컨트롤러에게새로만들어서 전달해야할것
		//처음이벤트를받은 컨트롤러가 delete하기 떄문에

		const std::string eventName = pEvent->GetName();

		if (eventName == "DeleteTexture")
		{
			HandleDeleteTextureEvent(pEvent);

		}
		else if (eventName == "LoadRequestFile")
		{

			//HandleLoadRequestFile(pEvent);
		

		}



	}

	//void ResourceController::HandleLoadRequestFile(Event* pEvent)
	//{
	//	LoadRequestFileEvent* loadRequestFileEvent = (LoadRequestFileEvent*)pEvent;

	//	const std::vector<std::string> & fileVector = loadRequestFileEvent->GetFileVector();


	//	//load해라
	//	
	//	//load한 파일들에 대해서 적절한 리소스,객체들을 생성한다.
	//	for (int i = 0; i < fileVector.size(); ++i)
	//	{
	//		//로드메서드 내에서 파일들을 로드하고 적절히 리소스를생성한다
	//		//따라서 그것을 파일 Ui에게 알리는것까지 수행할것이다.
	//	

	//		//여기서 텍스처들에대해서 Texture폴더로 복사해야한다.


	//		SendLoadFileResponseEvent(ImportFile(fileVector[i]));


	//	}
	//
	//}

	void ResourceController::InitFileUiSystem()
	{



	}

	void ResourceController::InitGamePlayUiSystem()
	{





	}

	void ResourceController::LoadEffect(const std::string& folderPath)
	{
		wchar_t currentDirectoryBuffer[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDirectoryBuffer);

		SetCurrentDirectory(Utility::ConvertToWString(folderPath, true).c_str());

		//#ifdef RUN
		//		SetCurrentDirectory(_T("..\\..\\SceneGraphQuadTree\\effect"));
		//#else 
		//		SetCurrentDirectory(_T("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\effect"));
		//#endif


		WIN32_FIND_DATAA fileData;

		HANDLE handle = FindFirstFileA("*.effect", &fileData);
		if (handle != INVALID_HANDLE_VALUE)
		{
			while (1)
			{
				mResourceLoader->LoadEffect(fileData.cFileName);
				if (FindNextFileA(handle, &fileData) == 0)
					break;
			}
		}

		SetCurrentDirectory(currentDirectoryBuffer);
		return;
	}



	void ResourceController::LoadCommonTexture()
	{
		//자동으로 texture파일에있는 파일을 다읽어오게 하자.
		wchar_t currentDirectoryBuffer[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDirectoryBuffer);
	//	SetCurrentDirectory(L"C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Texture");

#ifdef RUN
		SetCurrentDirectory(_T("..\\..\\SceneGraphQuadTree\\Texture"));
#else
		SetCurrentDirectory(_T("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Texture"));
#endif

		TCHAR debug[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, debug);
		OutputDebugString(debug);
		WIN32_FIND_DATAW fileData;

		const std::vector<std::wstring> fileExtensionVector = { L"bmp",L"png",L"jpg" };

		for (int i = 0; i < fileExtensionVector.size(); ++i)
		{
			HANDLE handle = FindFirstFileW((L"*." + fileExtensionVector[i]).c_str(), &fileData);

			if (handle == INVALID_HANDLE_VALUE)
				continue;

			while (1)
			{
				/*if (fileData.cFileName == "버튼21.png")
				{
					int a = 2;
				}*/
				Asset* texture = LoadTextureFile(Utility::ConvertToString(fileData.cFileName,true));
				//mEditorDefaultAssetVector.insert(mEditorDefaultAssetVector.end(), assetVector.begin(), assetVector.end());
				mEditorDefaultAssetVector.push_back(texture);
				if (FindNextFileW(handle, &fileData) == 0)
					break;
			}

			FindClose(handle);
		}

		SetCurrentDirectory(currentDirectoryBuffer);

	}

	void ResourceController::InitLayout()
	{
		D3D12_INPUT_LAYOUT_DESC *inputLayout = new D3D12_INPUT_LAYOUT_DESC;
		inputLayout->NumElements = 3;
		D3D12_INPUT_ELEMENT_DESC* inputElementLayoutArray = new D3D12_INPUT_ELEMENT_DESC[inputLayout->NumElements];
		inputElementLayoutArray[0]= MakeInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT);
		inputElementLayoutArray[1]= MakeInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT);
		inputElementLayoutArray[2]= MakeInputElement("TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT);
		inputLayout->pInputElementDescs = inputElementLayoutArray;



		InputLayoutTable::AddInputLayout(L"Default", inputLayout);


	}

	D3D12_INPUT_ELEMENT_DESC ResourceController::MakeInputElement(LPCSTR semanticName, UINT sematicIndex, DXGI_FORMAT format, UINT inputSlot, 
		UINT AlignedByteOffset, D3D12_INPUT_CLASSIFICATION inputSlotClass, UINT instanceDataStepRate)
	{
	
		D3D12_INPUT_ELEMENT_DESC inputElementDesc;
		inputElementDesc.SemanticName = semanticName;
		inputElementDesc.SemanticIndex = sematicIndex;
		inputElementDesc.AlignedByteOffset = AlignedByteOffset;
		inputElementDesc.Format = format;
		inputElementDesc.InputSlot = inputSlot;
		inputElementDesc.InputSlotClass = inputSlotClass;
		inputElementDesc.InstanceDataStepRate = instanceDataStepRate;
		return inputElementDesc;

	}

	void ResourceController::CreateDefaultShaderResource()
	{

		CreateConstantBuffer("default256", 256, 10000);
		CreateConstantBuffer("default512", 512, 10000);
		CreateConstantBuffer("default768", 768, 10000);
		CreateConstantBuffer("default1024", 1024, 10000);
		CreateConstantBuffer("default1280", 1280, 10000);
		CreateConstantBuffer("default1536", 1536, 10000);
		CreateConstantBuffer("default16640", 16640, 10000);

	}

	bool ResourceController::CreateBoundaryMeshFromGenerator()
	{
		//if (mGraphicsCommandObject->GetCloseState())
		//	mGraphicsCommandObject->ResetCommandList(nullptr);


		//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList =	mGraphicsCommandObject->GetGraphicsCommandList();
		//


		//RawStaticMeshData * rawMeshData= new RawStaticMeshData;
		//if (!GeometryGenerator::CreateRawBoundaryRectMeshData(rawMeshData))
		//{
		//	return false;
		//}

		//StaticMesh* pMesh = (StaticMesh*)mMeshManager->CreateMesh("BoundaryRect",EMeshType::eStaticMesh);


		//Microsoft::WRL::ComPtr<ID3D12Resource> uploadVertexTempBuffer;
		//Microsoft::WRL::ComPtr<ID3D12Resource> uploadIndexTempBuffer;

		//Microsoft::WRL::ComPtr<ID3D12Resource> defaultVertexBuffer = Utility::CreateDefaultBuffer(mDevice, graphicsCommandList,
		//	(void*)rawMeshData->mVertexVector.data(), sizeof(rawMeshData->mVertexVector[0]),
		//	rawMeshData->mVertexVector.size(), uploadVertexTempBuffer);


		//Microsoft::WRL::ComPtr<ID3D12Resource> defaultIndexBuffer = Utility::CreateDefaultBuffer(mDevice, graphicsCommandList,
		//	(void*)rawMeshData->mIndexVector.data(), sizeof(rawMeshData->mIndexVector[0]),
		//	rawMeshData->mIndexVector.size(), uploadIndexTempBuffer);


		//mUploadVertexTempBuffer.push_back(uploadVertexTempBuffer);
		//mUploadIndexTempBuffer.push_back(uploadIndexTempBuffer);


		//pMesh->SetVertexBuffer(defaultVertexBuffer);
		//pMesh->SetIndexBuffer(defaultIndexBuffer);


		//pMesh->SetVertexVector(std::move(rawMeshData->mVertexVector));
		//pMesh->SetIndexVector(std::move(rawMeshData->mIndexVector));


		////buffer

		//std::vector<SubMesh> subMeshVector(rawMeshData->mRawSubMeshVector.size());

		//Material* defaultMaterial = mMaterialManager->GetMaterial("Default");
		//for (int i = 0; i < subMeshVector.size(); ++i)
		//{
		//	subMeshVector[i].mVertexOffset = rawMeshData->mRawSubMeshVector[i].mVertexOffset;
		//	subMeshVector[i].mIndexRange.first = rawMeshData->mRawSubMeshVector[i].mIndexFirst;
		//	subMeshVector[i].mIndexRange.second = rawMeshData->mRawSubMeshVector[i].mIndexLast;
		//	subMeshVector[i].mName = rawMeshData->mRawSubMeshVector[i].mName;
		//	subMeshVector[i].mMaterialPointer = defaultMaterial;
		//	subMeshVector[i].mMesh = pMesh;
		//}
		//pMesh->SetMinMaxPoint(rawMeshData->mBoundingBox.first ,rawMeshData->mBoundingBox.second);

		//pMesh->SetSubMeshVector(std::move(subMeshVector));



		return true;


	}

	void ResourceController::InitBlendEnumUnMap()
	{
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_ZERO",D3D12_BLEND_ZERO });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_ONE",D3D12_BLEND_ONE });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_SRC_COLOR",D3D12_BLEND_SRC_COLOR });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_SRC_COLOR",D3D12_BLEND_INV_SRC_COLOR });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_SRC_ALPHA",D3D12_BLEND_SRC_ALPHA });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_SRC_ALPHA",D3D12_BLEND_INV_SRC_ALPHA });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_DEST_ALPHA",D3D12_BLEND_DEST_ALPHA });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_DEST_ALPHA",D3D12_BLEND_INV_DEST_ALPHA });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_DEST_COLOR",D3D12_BLEND_DEST_COLOR });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_DEST_COLOR",D3D12_BLEND_INV_DEST_COLOR });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_SRC_ALPHA_SAT",D3D12_BLEND_SRC_ALPHA_SAT });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_BLEND_FACTOR",D3D12_BLEND_BLEND_FACTOR });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_BLEND_FACTOR",D3D12_BLEND_INV_BLEND_FACTOR });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_SRC1_COLOR",D3D12_BLEND_SRC1_COLOR });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_SRC1_COLOR",D3D12_BLEND_INV_SRC1_COLOR });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_SRC1_ALPHA",D3D12_BLEND_SRC1_ALPHA });
		mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_SRC1_ALPHA",D3D12_BLEND_INV_SRC1_ALPHA });
	//	mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_ALPHA_FACTOR",D3D12_BLEND_ALPHA_FACTOR });
	//	mBlendFactorEnumUnMap.insert({ L"D3D12_BLEND_INV_ALPHA_FACTOR",D3D12_BLEND_INV_ALPHA_FACTOR });


		mBlendOpEnumUnMap.insert({ L"D3D12_BLEND_OP_ADD",D3D12_BLEND_OP_ADD });
		mBlendOpEnumUnMap.insert({ L"D3D12_BLEND_OP_SUBTRACT",D3D12_BLEND_OP_SUBTRACT });
		mBlendOpEnumUnMap.insert({ L"D3D12_BLEND_OP_REV_SUBTRACT",D3D12_BLEND_OP_REV_SUBTRACT });
		mBlendOpEnumUnMap.insert({ L"D3D12_BLEND_OP_MIN",D3D12_BLEND_OP_MIN });
		mBlendOpEnumUnMap.insert({ L"D3D12_BLEND_OP_MAX",D3D12_BLEND_OP_MAX });

	}

	void ResourceController::InitStencilEnumUnMap()
	{
		mStencilOpEnumUnMap.insert({ L"KEEP",D3D12_STENCIL_OP_KEEP });
		mStencilOpEnumUnMap.insert({ L"ZERO",D3D12_STENCIL_OP_ZERO });
		mStencilOpEnumUnMap.insert({ L"REPLACE",D3D12_STENCIL_OP_REPLACE });
		mStencilOpEnumUnMap.insert({ L"INCR_SAT",D3D12_STENCIL_OP_INCR_SAT });
		mStencilOpEnumUnMap.insert({ L"DECR_SAT",D3D12_STENCIL_OP_DECR_SAT });
		mStencilOpEnumUnMap.insert({ L"INVERT",D3D12_STENCIL_OP_INVERT });
		mStencilOpEnumUnMap.insert({ L"INCR",D3D12_STENCIL_OP_INCR });
		mStencilOpEnumUnMap.insert({ L"DECR",D3D12_STENCIL_OP_DECR });





	}

	void ResourceController::InitComparisonFuncEnumUnMap()
	{
		mComparisonFuncEnumUnMap.insert({ L"NEVER",D3D12_COMPARISON_FUNC_NEVER });
		mComparisonFuncEnumUnMap.insert({ L"LESS",D3D12_COMPARISON_FUNC_LESS });
		mComparisonFuncEnumUnMap.insert({ L"EQUAL",D3D12_COMPARISON_FUNC_EQUAL });
		mComparisonFuncEnumUnMap.insert({ L"LESS_EQUAL",D3D12_COMPARISON_FUNC_LESS_EQUAL });
		mComparisonFuncEnumUnMap.insert({ L"GREATER",D3D12_COMPARISON_FUNC_GREATER });
		mComparisonFuncEnumUnMap.insert({ L"NOT_EQUAL",D3D12_COMPARISON_FUNC_NOT_EQUAL });
		mComparisonFuncEnumUnMap.insert({ L"GREATER_EQUAL",D3D12_COMPARISON_FUNC_GREATER_EQUAL });
		mComparisonFuncEnumUnMap.insert({ L"ALWAYS",D3D12_COMPARISON_FUNC_ALWAYS });

	}

	void ResourceController::LoadDefaultModel()
	{

		//TCHAR currentDirectoryBuffer[MAX_PATH];
		//GetCurrentDirectory(MAX_PATH, currentDirectoryBuffer);

#ifdef RUN
		//SetCurrentDirectory(_T("..\\..\\SceneGraphQuadTree\\Resource"));
#else
		//SetCurrentDirectory(_T("C:\\Users\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Resource"));
#endif



	//	TCHAR debug[MAX_PATH];
	////	GetCurrentDirectory(MAX_PATH, debug);
	//	OutputDebugString(debug);
	/*	LoadFile(L"C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Resource\\Cube.fbx");
		LoadFile(L"C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Resource\\Cone.fbx");
		LoadFile(L"C:\\Users\\dongd\\source\\repos\\SecenGraphQuadTree\\SecenGraphQuadTree\\Resource\\GizmoRotationCircle.fbx");*/

		std::vector<std::vector<Asset*>> assetVectorVector(4);

	

	/*	assetVectorVector[0] = ImportFile("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Resource\\black_bison.fbx");

		assetVectorVector[1] = ImportFile("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Resource\\Cone.fbx");
		assetVectorVector[2]= ImportFile("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Resource\\GizmoRotationCircle.fbx");
		assetVectorVector[3] = ImportFile("C:\\Users\\dongd\\gitproject\\GameEngine\\SecenGraphQuadTree\\Resource\\Cube.fbx");*/

		//SetCurrentDirectory(currentDirectoryBuffer);


		for (auto& assetVector : assetVectorVector)
		{
			
			mEditorDefaultAssetVector.insert(mEditorDefaultAssetVector.begin(), assetVector.begin(), assetVector.end());
		}

		



	}

	void ResourceController::SaveMaterialMetaDataFile(const std::string &filePath)
	{
	/*	const std::vector<LogicalContentItemUiEntityFolder*> & logicalContentItemUiEntityVector= FileUiUiSystem::GetLogicalContentItemUiEntityFolderVector();*/


		JsonParser::StartWrite();


		//version 1
		/*
		unsigned int materialNum = 0;
		unsigned long long logicalFolderUniqueID = 0;

		for (auto logicalFolder : logicalContentItemUiEntityVector)
		{
			logicalFolderUniqueID = logicalFolder->GetUniqueID();


			ContentItemUiPanelEntity* contentItemUiPanelEntity = logicalFolder->GetContentItemPanelEntity();
			const std::vector<ContentItemBaseUiEntity*>& contentItemBaseUEntityVector = contentItemUiPanelEntity->GetContentItemBaseUiEntityVector();

			for (auto contentItemUiEntity : contentItemBaseUEntityVector)
			{
				ContentItem* contentItem = contentItemUiEntity->GetAsset();
				if (contentItem->GetEContentItemType() == EContentItemType::eAsset)
				{
					Asset* asset = static_cast<Asset*>(contentItem);
					if (asset->GetAssetType() == EAssetType::eMaterial)
					{
						JsonParser::StartWriteObject();
						JsonParser::Write("AssetUniqueID", asset->GetUniqueID());
						JsonParser::Write("LogicalFolderUnqiueID", logicalFolderUniqueID);	
						materialNum++;
					}
				}
			}
		}
		JsonParser::StartWriteObject();
	*/


		//version 2





		MaterialManager * materialManager= MaterialManager::GetInstance();
		const std::unordered_map<unsigned long long,Material*> &  materialIDTable =materialManager->mIDTable.GetTable();


		//nextAavailableUniqueID
		unsigned long long nextAvailableUniqueID = materialManager->mIDTable.PeekUniqueID();
		JsonParser::StartWriteObject();
		JsonParser::Write("NextAvailableUniqueID", nextAvailableUniqueID);



	
		JsonParser::StartWriteObject();
		unsigned long long materialNum = 0;
		for (auto& element : materialIDTable)
		{
			Material* material = element.second;
			if (!material->GetEngineContentItemFlag())
				materialNum++;
		}
		JsonParser::Write("MaterialNum", materialNum);



		for (auto& element : materialIDTable)
		{
			Material* material = element.second;
			if (material->GetEngineContentItemFlag())
				continue;


			unsigned long long logicalFolderUniqueID = material->GetDestLogicalFolderUnqiueID();
			JsonParser::StartWriteObject();
			JsonParser::Write("AssetUniqueID", material->GetUniqueID());
			JsonParser::Write("LogicalFolderUniqueID", logicalFolderUniqueID);
		}

		//nextAavailableUniqueID


		JsonParser::Save(filePath);
	}

	void ResourceController::SaveTextureMetaDataFile(const std::string& assetPath)
	{
		const std::string& textureMetaDataFilePath = assetPath + "\\TextureMetaData.json";
		
		JsonParser::StartWrite();

		unsigned long long textureNum = 0;

		const std::unordered_map<unsigned long long, Texture*>& textureIDTable = mTextureManager->mIDTable.GetTable();
		

		//nextAavailableUniqueID
		unsigned long long nextAvailableUniqueID = mTextureManager->mIDTable.PeekUniqueID();

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

	/*		const std::string& filePath = assetPath + "\\Texture\\" + texture->GetName();
			JsonParser::Write("FilePath", filePath);
			JsonParser::Write("AssetUniqueID", texture->GetUniqueID());
			JsonParser::Write("LogicalFolderUniqueID", texture->GetDestLogicalFolderUnqiueID());*/

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

	void ResourceController::SaveMeshMetaDataFile(const std::string& filePath)
	{
		
		JsonParser::StartWrite();

		unsigned long long meshNum = 0;
		const std::unordered_map<unsigned long long, Mesh*>& meshIDTable = mMeshManager->mIDTable.GetTable();

		Mesh* mesh = nullptr;
		
		for (auto& element : meshIDTable)
		{
			mesh = element.second;
			if (!mesh->GetEngineContentItemFlag())
				meshNum++;
		}


		//nextAavailableUniqueID
		unsigned long long nextAvailableUniqueID = mMeshManager->mIDTable.PeekUniqueID();

		JsonParser::StartWriteObject();
		JsonParser::Write("NextAvailableUniqueID", nextAvailableUniqueID);



		std::count_if(meshIDTable.begin(), meshIDTable.end(), [](const std::pair<unsigned long long, Mesh*>& element) {
			Mesh* mesh = element.second;
			if (!mesh->GetEngineContentItemFlag())
				return true;
			return false;
			});



		JsonParser::StartWriteObject();
		JsonParser::Write("MeshNum", meshNum);
		
		for (auto& element : meshIDTable)
		{
			mesh = element.second;
			if (mesh->GetEngineContentItemFlag())
				continue;

			JsonParser::StartWriteObject();

			JsonParser::Write("AssetUniqueID", mesh->GetUniqueID());
			JsonParser::Write("LogicalFolderUniqueID", mesh->GetDestLogicalFolderUnqiueID());


		}


	


		JsonParser::Save(filePath);


	}

	void ResourceController::SaveSkeletonMetaDataFile(const std::string& filePath)
	{
		JsonParser::StartWrite();

		unsigned long long skeletonNum = 0;
		const std::unordered_map<unsigned long long, Skeleton*>& skeletonIDTable = mSkeletonManager.mIDTable.GetTable();

		Skeleton* skeleton = nullptr;

		for (auto& element : skeletonIDTable)
		{
			skeleton = element.second;
			if (!skeleton->GetEngineContentItemFlag())
				skeletonNum++;
		}


		//nextAavailableUniqueID
		unsigned long long nextAvailableUniqueID = mSkeletonManager.mIDTable.PeekUniqueID();

		JsonParser::StartWriteObject();
		JsonParser::Write("NextAvailableUniqueID", nextAvailableUniqueID);



		//std::count_if(skeletonIDTable.begin(), skeletonIDTable.end(), [](const std::pair<unsigned long long, Mesh*>& element) {
		//	Mesh* mesh = element.second;
		//	if (!mesh->GetEngineContentItemFlag())
		//		return true;
		//	return false;
		//	});



		JsonParser::StartWriteObject();
		JsonParser::Write("SkeletonNum", skeletonNum);

		for (auto& element : skeletonIDTable)
		{
			skeleton = element.second;
			if (skeleton->GetEngineContentItemFlag())
				continue;

			JsonParser::StartWriteObject();

			JsonParser::Write("AssetUniqueID", skeleton->GetUniqueID());
			JsonParser::Write("LogicalFolderUniqueID", skeleton->GetDestLogicalFolderUnqiueID());


		}





		JsonParser::Save(filePath);


	}

	void ResourceController::SaveAnimationClipMetaDataFile(const std::string& filePath)
	{

		JsonParser::StartWrite();

		unsigned long long animClipNum = 0;
		const std::unordered_map<unsigned long long, AnimationClip*>& animClipIDTable = mAnimationClipManager.mIDTable.GetTable();

		AnimationClip* animClip = nullptr;

		for (auto& element : animClipIDTable)
		{
			animClip = element.second;
			if (!animClip->GetEngineContentItemFlag())
				animClipNum++;
		}


		//nextAavailableUniqueID
		unsigned long long nextAvailableUniqueID = mAnimationClipManager.mIDTable.PeekUniqueID();

		JsonParser::StartWriteObject();
		JsonParser::Write("NextAvailableUniqueID", nextAvailableUniqueID);



		JsonParser::StartWriteObject();
		JsonParser::Write("AnimationClipNum", animClipNum);


		for (auto& element : animClipIDTable)
		{
			animClip = element.second;
			if (animClip->GetEngineContentItemFlag())
				continue;

			JsonParser::StartWriteObject();

			JsonParser::Write("AssetUniqueID", animClip->GetUniqueID());
			JsonParser::Write("LogicalFolderUniqueID", animClip->GetDestLogicalFolderUnqiueID());


		}





		JsonParser::Save(filePath);







	}

	std::vector<Asset*> ResourceController::LoadMaterialMetaDataFile(const std::string& filePath)
	{
		JsonParser::ReadFile(filePath);
		JsonParser::ReadStart();

		MaterialManager* materialManager = MaterialManager::GetInstance();
		unsigned long long readObjectIndex = 0;
		
		
		//nextAavailableUniqueID
		unsigned long long nextAvailableUniqueID = 0;
		JsonParser::Read("NextAvailableUniqueID", nextAvailableUniqueID);
		materialManager->mIDTable.SetNextAvailalbeUniqueID(nextAvailableUniqueID);

		JsonParser::SetCurrentIndex(++readObjectIndex);

		
		unsigned long long materialNum = 0;
		JsonParser::Read("MaterialNum", materialNum);



		unsigned long long logicalFolderUniqueID = 0;
		unsigned long long materialUniqueID = 0;
		std::vector<Asset*> assetVector(materialNum, nullptr);
		for (unsigned long long i = 0; i < materialNum; ++i)
		{
			readObjectIndex++;
			JsonParser::SetCurrentIndex(readObjectIndex);
			JsonParser::Read("AssetUniqueID", materialUniqueID);
			JsonParser::Read("LogicalFolderUniqueID", logicalFolderUniqueID);

			Material * material = materialManager->GetMaterial(materialUniqueID);
			material->SetDestLogicalFolderUnqiueID(logicalFolderUniqueID);
			
			assetVector[i] = material;
		}


		//nextAavailableUniqueID


		//asset을 반환하는식으로 가야한다.
		//그래야 EditorDirector,UserGamePlayDirector가 받아서 적절하게 처리한다.

		return assetVector;

		//SendLoadFileResponseEvent(assetVector);

	}

	std::vector<Asset*> ResourceController::LoadTextureMetaDataFile(const std::string& filePath)
	{

		const std::string textureMetaDataFilePath = filePath + "\\TextureMetaData.json";
		JsonParser::ReadFile(textureMetaDataFilePath);
		JsonParser::ReadStart();


		std::string beforeDirectoryPath = Utility::SetNewCurrentDirectory(filePath + "\\Texture");

	
		TextureManager* textureManager = TextureManager::GetInstance();

		unsigned long long readObjectIndex = 0;
		//nextAavailableUniqueID
		unsigned long long nextAvailableUniqueID = 0;
		JsonParser::Read("NextAvailableUniqueID", nextAvailableUniqueID);
		textureManager->mIDTable.SetNextAvailalbeUniqueID(nextAvailableUniqueID);
		JsonParser::SetCurrentIndex(++readObjectIndex);
		
		
		unsigned long long textureNum;

		JsonParser::Read("TextureNum", textureNum);

		unsigned long long logicalFolderUniqueID = 0;
		unsigned long long textureUniqueID = 0;

		std::vector<Asset*> assetVector(textureNum, nullptr);

		for (unsigned long long i = 0; i < textureNum; ++i)
		{
			readObjectIndex++;
			JsonParser::SetCurrentIndex(readObjectIndex);
			std::string assetUniqueName;
			std::string textureName;
			JsonParser::Read("FileName", textureName);
			JsonParser::Read("AssetUniqueName", assetUniqueName);
			JsonParser::Read("AssetUniqueID", textureUniqueID);
			JsonParser::Read("LogicalFolderUniqueID", logicalFolderUniqueID);

			Texture *texture = mResourceLoader->LoadTextureFromFile(textureName, assetUniqueName,textureUniqueID);
			texture->SetDestLogicalFolderUnqiueID(logicalFolderUniqueID);

			assetVector[i] = texture;
		}



		Utility::SetNewCurrentDirectory(beforeDirectoryPath);
		//nextAavailableUniqueID

		return assetVector;

	//	SendLoadFileResponseEvent(assetVector);

	}

	std::vector<Asset*> ResourceController::LoadMeshMetaDataFile(const std::string& filePath)
	{

		JsonParser::ReadFile(filePath);

		JsonParser::ReadStart();
		unsigned long long readObjectIndex = 0;
		
		MeshManager* meshManager = MeshManager::GetInstance();


		unsigned long long nextAvailableUniqueID = 0;
		JsonParser::Read("NextAvailableUniqueID", nextAvailableUniqueID);
		meshManager->mIDTable.SetNextAvailalbeUniqueID(nextAvailableUniqueID);
		JsonParser::SetCurrentIndex(++readObjectIndex);


		unsigned long long meshNum=0;



		JsonParser::Read("MeshNum", meshNum);

		unsigned long long logicalFolderUniqueID = 0;
		unsigned long long meshUniqueID = 0;



		std::vector<Asset*> assetVector(meshNum, nullptr);

		for (unsigned long long i = 0; i < meshNum; ++i)
		{
			readObjectIndex++;
			JsonParser::SetCurrentIndex(readObjectIndex);


			JsonParser::Read("AssetUniqueID", meshUniqueID);
			JsonParser::Read("LogicalFolderUniqueID", logicalFolderUniqueID);

			Mesh* mesh = mMeshManager->GetMesh(meshUniqueID);
			mesh->SetDestLogicalFolderUnqiueID(logicalFolderUniqueID);

			assetVector[i] = mesh;
		}


		//nextAavailableUniqueID

		//SendLoadFileResponseEvent(assetVector);


		return assetVector;








	}

	std::vector<Asset*> ResourceController::LoadSkeletonMetaDataFile(const std::string& filePath)
	{



		JsonParser::ReadFile(filePath);

		JsonParser::ReadStart();
		unsigned long long readObjectIndex = 0;



		unsigned long long nextAvailableUniqueID = 0;
		JsonParser::Read("NextAvailableUniqueID", nextAvailableUniqueID);
		mSkeletonManager.mIDTable.SetNextAvailalbeUniqueID(nextAvailableUniqueID);
		JsonParser::SetCurrentIndex(++readObjectIndex);


		unsigned long long skeletonNum = 0;



		JsonParser::Read("SkeletonNum", skeletonNum);

		unsigned long long logicalFolderUniqueID = 0;
		unsigned long long meshUniqueID = 0;



		std::vector<Asset*> assetVector(skeletonNum, nullptr);

		for (unsigned long long i = 0; i < skeletonNum; ++i)
		{
			readObjectIndex++;
			JsonParser::SetCurrentIndex(readObjectIndex);


			JsonParser::Read("AssetUniqueID", meshUniqueID);
			JsonParser::Read("LogicalFolderUniqueID", logicalFolderUniqueID);

			Skeleton* skeleton = mSkeletonManager.GetSkeleton(meshUniqueID);
			skeleton->SetDestLogicalFolderUnqiueID(logicalFolderUniqueID);

			assetVector[i] = skeleton;
		}


		//nextAavailableUniqueID

		//SendLoadFileResponseEvent(assetVector);


		return assetVector;



	}

	std::vector<Asset*> ResourceController::LoadAnimationClipMetaDatFile(const std::string& filePath)
	{

		JsonParser::ReadFile(filePath);

		JsonParser::ReadStart();
		unsigned long long readObjectIndex = 0;



		unsigned long long nextAvailableUniqueID = 0;
		JsonParser::Read("NextAvailableUniqueID", nextAvailableUniqueID);
		mAnimationClipManager.mIDTable.SetNextAvailalbeUniqueID(nextAvailableUniqueID);
		JsonParser::SetCurrentIndex(++readObjectIndex);


		unsigned long long animClipNum = 0;



		JsonParser::Read("AnimationClipNum", animClipNum);

		unsigned long long logicalFolderUniqueID = 0;
		unsigned long long meshUniqueID = 0;



		std::vector<Asset*> assetVector(animClipNum, nullptr);

		for (unsigned long long i = 0; i < animClipNum; ++i)
		{
			readObjectIndex++;
			JsonParser::SetCurrentIndex(readObjectIndex);


			JsonParser::Read("AssetUniqueID", meshUniqueID);
			JsonParser::Read("LogicalFolderUniqueID", logicalFolderUniqueID);

			AnimationClip* animClip = mAnimationClipManager.GetAnimationClip(meshUniqueID);
			animClip->SetDestLogicalFolderUnqiueID(logicalFolderUniqueID);

			assetVector[i] = animClip;
		}


		//nextAavailableUniqueID

		//SendLoadFileResponseEvent(assetVector);


		return assetVector;














		return std::vector<Asset*>();
	}


	D3D12_BLEND ResourceController::ConvertBlendFactorEnum(const std::wstring& str)
	{
		//default
		D3D12_BLEND blendEnum = D3D12_BLEND_ZERO;

		std::unordered_map<std::wstring,D3D12_BLEND>::iterator it =mBlendFactorEnumUnMap.find(str);
		if (it != mBlendFactorEnumUnMap.end())
			blendEnum = it->second;
		else
		{
			MessageBox(nullptr, L"blendEnum is not correct", L"error", 0);
		}

		return blendEnum;
	}

	D3D12_BLEND_OP ResourceController::ConvertBlendOpEnum(const std::wstring& str)
	{
		//default
		D3D12_BLEND_OP blendOp = D3D12_BLEND_OP_ADD;

		std::unordered_map<std::wstring,D3D12_BLEND_OP>::iterator it =	mBlendOpEnumUnMap.find(str);
		if (it != mBlendOpEnumUnMap.end())
		{
			blendOp = it->second;
		}
		else
		{
			MessageBox(nullptr, L"Blend Op is not correct", L"error", 0);
		}
		
		return blendOp;
	}

	D3D12_STENCIL_OP ResourceController::ConvertStencilOpEnum(const std::wstring & str)
	{
		
		std::unordered_map<std::wstring, D3D12_STENCIL_OP>::iterator it = mStencilOpEnumUnMap.find(str);
		if(it == mStencilOpEnumUnMap.end())
		{ 
			MessageBox(nullptr, L"StencilOp is not correct ", L"error", 0);
			return D3D12_STENCIL_OP_KEEP;
		}

		return it->second;

	}

	D3D12_COMPARISON_FUNC ResourceController::ConvertComparisonFuncEnum(const std::wstring& str)
	{
		std::unordered_map<std::wstring ,D3D12_COMPARISON_FUNC>::iterator it =	mComparisonFuncEnumUnMap.find(str);
		
		if (it == mComparisonFuncEnumUnMap.end())
		{
			MessageBox(nullptr, L"ComparisonFunc is not correct ", L"error ", 0);
			return D3D12_COMPARISON_FUNC_LESS;
		}
		return it->second;
	}

	D3D12_SRV_DIMENSION ResourceController::ConvertSRVDimensionEnum(D3D12_RESOURCE_DIMENSION dimension)
	{

		switch (dimension)
		{
		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D:
				return D3D12_SRV_DIMENSION_TEXTURE2D;

		case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER:
			return D3D12_SRV_DIMENSION_BUFFER;

		}
		MessageBox(nullptr, L"srv dimension변환에서 처리미룬것이 있다", L"error",0);
		assert(-1);
		
	}

	void ResourceController::FillSRVTEX2DDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& oSrv, Microsoft::WRL::ComPtr<ID3D12Resource> resource)
	{
		oSrv.Texture2D.MipLevels = resource->GetDesc().MipLevels;
		oSrv.Texture2D.MostDetailedMip = 0;
		oSrv.Texture2D.ResourceMinLODClamp= 0.0;
		oSrv.Texture2D.PlaneSlice = 0;

	}

	Microsoft::WRL::ComPtr<ID3DBlob> ResourceController::CompileShader(const std::wstring& fileName, const D3D_SHADER_MACRO* pMacro, ID3DInclude* pInclude, const std::wstring& entryPointW, const std::wstring& versionW, UINT flags1, UINT flags2)
	{

		std::string entryPoint;
		std::string version;

		icu::UnicodeString ustr(entryPointW.c_str());
		ustr.toUTF8String(entryPoint);

		ustr = versionW.c_str();
		ustr.toUTF8String(version);


		Microsoft::WRL::ComPtr<ID3DBlob> shaderByteCode;
		ID3DBlob* errorByteCode;
		HRESULT ret =D3DCompileFromFile(fileName.c_str(), pMacro, pInclude, entryPoint.c_str(), version.c_str(), flags1, flags2,
			shaderByteCode.GetAddressOf(), &errorByteCode);



		if (errorByteCode != nullptr)
		{
			//디버그창에 출력
		}

		ThrowIfFailed(ret);		//일단다음과같이 처리, 나중에는 계속 엔진이 진행되도록해야한다.
		return shaderByteCode;


	}

	D3D12_CULL_MODE ResourceController::ConvertCullMode(const std::wstring& str)
	{
		if (str == L"NONE")
			return D3D12_CULL_MODE_NONE;
		else if (str == L"FRONT")
			return D3D12_CULL_MODE_FRONT;
		else if (str == L"BACK")
			return D3D12_CULL_MODE_BACK;
		
	}

	ERenderPassID ResourceController::ConvertRenderPassID(const std::wstring& str) const
	{

		if (str == L"GIZMO")
		{
			return ERenderPassID::eGizmo;
		}
		else if (str == L"TRANSPARENT")
		{
			return ERenderPassID::eTransparent;
		}
		else if (str == L"OPAQUE")
		{
			return ERenderPassID::eOpaque;
		}
		else if (str == L"SHADOWMAP")
		{
			return ERenderPassID::eShadowMap;
		}
		else
		{
			return ERenderPassID::eOpaque;
		}

	}

	D3D12_BLEND_OP ResourceController::ConvertD3DBlendOpEnum(const std::wstring& str)
	{
		
		if (str == L"ADD")
		{
			return D3D12_BLEND_OP_ADD;
		}
		else if (str == L"SUBTRACT")
		{
			return D3D12_BLEND_OP_SUBTRACT;
		}
		else if (str == L"REV_SUBTRACT")
		{
			return D3D12_BLEND_OP_REV_SUBTRACT;
		}
		else if (str == L"MIN")
		{
			return D3D12_BLEND_OP_MIN;
		}
		else if (str == L"MAX")
		{
			return D3D12_BLEND_OP_MAX;
		}
		else
		{
			MessageBox(0, L"BlendOp가 정확하지않다.", L"error", 0);
			assert(-1);
		}
		
	

	}

	ETextureFormat ResourceController::ConvertTextureFormatEnum(DXGI_FORMAT format)
	{

		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:

			return ETextureFormat::eRgb;

		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return ETextureFormat::eSRgb;
		}
		
	}


}