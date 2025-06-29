#include "RenderPassSystem.h"
#include"Effect/RenderPassTwo.h"
#include"GraphicCommand.h"
#include"RenderSystem.h"
#include"Asset/Mesh/Mesh.h"
#include"Shader/ShaderResourceTexture.h"
#include"Shader/ShaderResourceSampler.h"
#include"Object/Object.h"
#include"Object/Line/LineBase.h"

#include"HeapManager/DescriptorHeapManagerMaster.h"


#include"TextCharacterEntity.h"
#include"TextCharacterPanelEntity.h"
#include"PassData.h"
#include"Asset/Mesh/MeshType.h"
#include"Shader/ShaderResourceConstantBuffer.h"

#include"Object/Camera/Camera.h"
#include"Object/Light/Light.h"

#include"Asset/Texture/RenderTargetTexture.h"

#include"Object/ObjectType.h"
#include"Object/Entity.h"

#include"Core/MapLayerRenderData.h"
#include<Component/IMeshComponent.h>

void Quad::RenderPassSystem::Initialize(GraphicCommand* graphicsCommandObject,
	DescriptorHeapManagerMaster* descriptorHeapManagerMaster)
{
	mGraphicsCommandObject = graphicsCommandObject;
	mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;

}
void Quad::RenderPassSystem::AddRenderItem(RenderPassTwo* renderPass, RenderItem* renderItem,int mapLayerIndex)
{

	ERenderPassID renderPassID = renderPass->GetRenderPassID();


	std::vector<RenderPassItem>& currentRenderItemVector = mRenderPassItemContainer[renderPassID];

	std::vector<RenderPassItem>::iterator it = std::find_if(currentRenderItemVector.begin(), currentRenderItemVector.end(),
		[&renderPass](const RenderPassItem& passItem) { return passItem.mRenderPass == renderPass ? true : false; });

	ERenderPassDrawType renderPassDrawType = renderPass->GetRenderPassDrawType();

	if (it == currentRenderItemVector.end())
	{

		RenderPassItem newRenderPassItem;
		newRenderPassItem.mRenderItemVector.resize(10);
		newRenderPassItem.mRenderPass = renderPass;
		newRenderPassItem.mRenderItemVector[mapLayerIndex].push_back(renderItem);

		currentRenderItemVector.push_back(std::move(newRenderPassItem));
		return;
	}


	if (renderPassDrawType == ERenderPassDrawType::eObjectDraw)
	{
		it->mRenderItemVector[mapLayerIndex].push_back(renderItem);

	}
	else if (renderPassDrawType == ERenderPassDrawType::eEntirePostDraw)
	{
		//무시
		//이미 처음 렌더패스 아이템이 생성될때 전체 RECT에대한 렌더아이템이 들어갔다.
	}




	return;

}

const std::vector < Quad::RenderPassItem > & Quad::RenderPassSystem::GetRenderPassItemVector(ERenderPassID id) const
{
	return mRenderPassItemContainer[id];
}

void Quad::RenderPassSystem::UploadData(const PassData& passData)
{

	PassData passDataL = passData;

	for (int mapLayerIndex = 0; mapLayerIndex < mMapLayerSettingItemVector.size(); ++mapLayerIndex)
	{
		passDataL.mCamera = mMapLayerSettingItemVector[mapLayerIndex].mCamera;
		passDataL.mViewPort = mMapLayerSettingItemVector[mapLayerIndex].mViewPort;

		for (int renderItemID = 0; renderItemID < 10; ++renderItemID)
		{
			std::vector<RenderPassItem>& renderPassItemVector = mRenderPassItemContainer[renderItemID];

			for (int renderPassIndex = 0; renderPassIndex < renderPassItemVector.size(); ++renderPassIndex)
			{
				RenderPassTwo* renderPass = renderPassItemVector[renderPassIndex].mRenderPass;



				std::vector<RenderItem*> renderItemVector = renderPassItemVector[renderPassIndex].mRenderItemVector[mapLayerIndex];
				UploadDataToRenderPassShaderResource(renderPass, renderItemVector, passDataL);

			}

		}


	}






}

//void Quad::RenderPassSystem::SetRenderSettingItem(RenderSettingItem* renderSettingItem)
//{
//	mRenderSettingItem = renderSettingItem;
//
//
//
//}

void Quad::RenderPassSystem::SetMapLayerVector(const std::vector<MapLayer>& mapLayerVector, const std::vector<MapLayerRenderData> & mapLayerRenderDataVector)
{

	mMapLayerSettingItemVector.resize(mapLayerVector.size());

	for (int i = 0; i < mMapLayerSettingItemVector.size(); ++i)
	{
		mMapLayerSettingItemVector[i].mCamera = mapLayerVector[i].mCamera;
		mMapLayerSettingItemVector[i].mViewPort.Width = mapLayerVector[i].mViewPortGlobal.Width;
		mMapLayerSettingItemVector[i].mViewPort.Height = mapLayerVector[i].mViewPortGlobal.Height;
		mMapLayerSettingItemVector[i].mViewPort.MinDepth = mapLayerVector[i].mViewPortGlobal.MinDepth;
		mMapLayerSettingItemVector[i].mViewPort.MaxDepth = mapLayerVector[i].mViewPortGlobal.MaxDepth;
		mMapLayerSettingItemVector[i].mViewPort.TopLeftX = mapLayerVector[i].mViewPortGlobal.TopLeftX;
		mMapLayerSettingItemVector[i].mViewPort.TopLeftY = mapLayerVector[i].mViewPortGlobal.TopLeftY;


		mMapLayerSettingItemVector[i].mViewPortLocal = mMapLayerSettingItemVector[i].mViewPort;
		mMapLayerSettingItemVector[i].mViewPortLocal.TopLeftX = 0;
		mMapLayerSettingItemVector[i].mViewPortLocal.TopLeftY = 0;

		mMapLayerSettingItemVector[i].mScissorRectLocal.left = 0;
		mMapLayerSettingItemVector[i].mScissorRectLocal.right = mMapLayerSettingItemVector[i].mViewPortLocal.Width;
		mMapLayerSettingItemVector[i].mScissorRectLocal.top= 0;
		mMapLayerSettingItemVector[i].mScissorRectLocal.bottom = mMapLayerSettingItemVector[i].mViewPortLocal.Height;

		if (mapLayerVector[i].mRenderTarget == nullptr)
		{
		//	mMapLayerSettingItemVector[i].mRenderTarget = mDefaultRenderTargetTexture;
			mMapLayerSettingItemVector[i].mRenderTarget = mapLayerRenderDataVector[i].GetDefaultRenderTarget();
	
		}
		else
		{
			mMapLayerSettingItemVector[i].mRenderTarget = mapLayerVector[i].mRenderTarget;
		}

		if (mapLayerVector[i].mDepthStencilBuffer == nullptr)
		{
			//mMapLayerSettingItemVector[i].mDepthStencilBuffer = mDefaultDepthStencilBuffer;
			mMapLayerSettingItemVector[i].mDepthStencilBuffer = mapLayerRenderDataVector[i].GetDefaultDepthStencilBuffer();
		}
		else
		{

			mMapLayerSettingItemVector[i].mDepthStencilBuffer = mapLayerVector[i].mDepthStencilBuffer;
		}
	}

}

const std::vector<Quad::RenderSettingItem>& Quad::RenderPassSystem::GetMapLayerSettingVector() const
{

	return mMapLayerSettingItemVector;

	// TODO: 여기에 return 문을 삽입합니다.
}

void Quad::RenderPassSystem::SetDefaultRenderTargetAndDepthStencil(RenderTargetTexture* renderTargetTexture, Texture* depthStencilBuffer)
{
	mDefaultRenderTargetTexture = renderTargetTexture;
	mDefaultDepthStencilBuffer = depthStencilBuffer;

}

void Quad::RenderPassSystem::Draw(bool stencilDrawFlag)
{



	for (int mapLayerIndex = 0; mapLayerIndex < mMapLayerSettingItemVector.size(); ++mapLayerIndex)
	{

	
		BeforeDraw(mapLayerIndex,stencilDrawFlag);			//mapLayer의 preDrawPass 수행 &&  초기셋팅 


		for (int renderPassIndex = 0; renderPassIndex < mRenderPassItemContainer.size(); ++renderPassIndex)
		{
			
			std::vector<RenderPassItem>& renderPassItemVector = mRenderPassItemContainer[renderPassIndex];



			for (int renderPassItemIndex = 0; renderPassItemIndex < renderPassItemVector.size(); ++renderPassItemIndex)
			{
				

				//렌더타켓 설정등의 전처리
				//다른곳으로 복사 전처리 
				//preRenderPass

				//PreRenderPass(renderPassItemVector[renderPassItemIndex],mapLayerIndex);
				//draw render pass 내부로 옮김 - texture bind command 때문에 root signature, pipeline 이후에 수행되어야하기에


				if (stencilDrawFlag && renderPassItemVector[renderPassItemIndex].mRenderPass->GetStencilWriteFlag())
				{

					DrawRenderPass(renderPassItemVector[renderPassItemIndex] ,mapLayerIndex);
				}
				else if (stencilDrawFlag == false)
				{
					DrawRenderPass(renderPassItemVector[renderPassItemIndex],mapLayerIndex);
				}



				//다른곳으로 복사 후처리 RenderItmeDraw의 후처리(명령수행등등)
				PostRenderPass(renderPassItemVector[renderPassItemIndex], mapLayerIndex, stencilDrawFlag);

			}

		}






		AfterDraw(mapLayerIndex, stencilDrawFlag);		//mapLayer에대해서 postDrawPass등을 수행

	}

}

void Quad::RenderPassSystem::Reset()
{
	
	for (int i = 0; i < mRenderPassItemContainer.size(); ++i)
	{
		mRenderPassItemContainer[i].clear();
	}

}

void Quad::RenderPassSystem::SetCurrentRenderTargetDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv)
{

	mCurrentDefaultRtv = rtv;
	mCurrentDefaultDsv = dsv;
	//mViewport = viewport;

}

//Quad::RenderSettingItem& Quad::RenderPassSystem::GetRenderSettingItem()
//{
//	return *mRenderSettingItem;
//	// TODO: 占쏙옙占썩에 return 占쏙옙占쏙옙 占쏙옙占쏙옙占쌌니댐옙.
//}


void Quad::RenderPassSystem::BeforeDraw(int mapLayerIndex, bool stencilDrawFlag)
{
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();










	//graphicsCommandList->RSSetViewports(1, &mRenderSettingItem->mViewPort);
//D3D12_VIEWPORT viewport = mMapLayerSettingItemVector[mapLayerIndex].mViewPortLocal;
	//viewport.Height = 1000;
	//graphicsCommandList->RSSetViewports(1, &viewport);
	graphicsCommandList->RSSetViewports(1, &mMapLayerSettingItemVector[mapLayerIndex].mViewPortLocal);

	
	if (mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget == nullptr)	
		return;
	else
	{
		RenderTargetTexture* renderTarget = mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget;
		Texture* depthStencilBuffer = mMapLayerSettingItemVector[mapLayerIndex].mDepthStencilBuffer;

		auto resourceBarrierTransition = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->GetResource().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
		graphicsCommandList->ResourceBarrier(1, &resourceBarrierTransition);


		D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle = mDescriptorHeapManagerMaster->GetRtvCpuDescriptorHandle(renderTarget->GetRendedrTargetViewIndex());


		D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuDescirptorHandle = mDescriptorHeapManagerMaster->GetDsvCpuDescriptorHandle(depthStencilBuffer->GetViewIndex());

		//float color[4] = { 0.0f,0.0f,0.0f,1.0f };
		//graphicsCommandList->ClearRenderTargetView(rtvCpuDescriptorHandle,color, 0, nullptr);
		//graphicsCommandList->ClearDepthStencilView(dsvCpuDescirptorHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);




		if (stencilDrawFlag)
		{
			graphicsCommandList->OMSetRenderTargets(0, nullptr,false, &dsvCpuDescirptorHandle);
		}
		else
		{
			graphicsCommandList->OMSetRenderTargets(1, &rtvCpuDescriptorHandle, false, &dsvCpuDescirptorHandle);
		}

	}




}

void Quad::RenderPassSystem::PreRenderPass(const RenderPassItem & renderPassItem,int mapLayerindex)
{


	RenderPassTwo* renderPass = renderPassItem.mRenderPass;


	const std::vector<RenderPassCommand*> & renderPassCommandVector = renderPass->GetPreRenderPassCommandVector();

	ExecuteRenderPassCommand(renderPass->GetDestEffect(),renderPassCommandVector, mapLayerindex);


}

void Quad::RenderPassSystem::PostRenderPass(const RenderPassItem& renderPassItem, int mapLayerIndex,bool stencilDrawFlag)
{
	

	RenderPassTwo* renderPass = renderPassItem.mRenderPass;

	const std::vector<RenderPassCommand*>& renderPassCommandVector = renderPass->GetPostRenderPassCommandVector();

	ExecuteRenderPassCommand(renderPass->GetDestEffect(), renderPassCommandVector, mapLayerIndex);



}

void Quad::RenderPassSystem::ExecuteRenderPassCommand(Effect * effect ,const std::vector<RenderPassCommand*>& renderPassCommandVector , int mapLayerIndex)
{

	//렌더패스 명령 별로 적절한 작업을 수행한다.


	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();

	const std::vector<Texture*>& effectCustomTextureVector = effect->GetCustomTextureVector();

	for (auto& renderPassCommandElement : renderPassCommandVector)
	{
		ERenderPassCommandType commandType = renderPassCommandElement->GetRenderPassCommandType();

		switch (commandType)
		{
		case ERenderPassCommandType::eTextureCopy:
		{
			RenderPassTextureCopyCommnad* textureCopyCommand = static_cast<RenderPassTextureCopyCommnad*>(renderPassCommandElement);

		
			int sourceTextureLocalID = textureCopyCommand->GetSourceTextureLocalID();
			int destTextureLocalID = textureCopyCommand->GetDestTextureLocalID();
				

			Texture* sourceTexture = nullptr;
			Texture* destTexture = nullptr;
			if (sourceTextureLocalID == -1)
			{
				//기본 렌더 타겟 
				sourceTexture = mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget;
			}
			else
			{
				sourceTexture = effectCustomTextureVector[sourceTextureLocalID];
			}

			if (destTextureLocalID == -1)
			{
				//기본 렌더 타겟
				destTexture = mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget;;
			}
			else
			{
				destTexture = effectCustomTextureVector[destTextureLocalID];
			}


			//실제 texture 를 얻어와야지

		//	Texture* sourceTexture = textureCopyCommand->GetSourceTexture();
		//	Texture* destTexture = textureCopyCommand->GetDestTexture();


			//복사하기위해서 상태변경이 필요 -> 이것도 이전에 명령을 넘겨서 처리해야하는것이다.



			//복사
			graphicsCommandList->CopyResource(sourceTexture->GetResource().Get(), destTexture->GetResource().Get());
			

		}
		break;
		case ERenderPassCommandType::eTextureBind:
		{

			RenderPassTextureBind* textureBindCommand = static_cast<RenderPassTextureBind*>(renderPassCommandElement);

			int textureLocalID = textureBindCommand->GetTextureLocalID();

			Texture* texture = effectCustomTextureVector[textureLocalID];

			
			int rootParameter = textureBindCommand->GetRootParameter();

			//문제는 여기서 뭐가있어야돼 
			//이미 pipeline , root signature가 바인딩된상태어야한다는거지 후후 그러면  preRenderPass를 밖이아니라 내부로 옮겨야할수도 .
			D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle =mDescriptorHeapManagerMaster->GetCbvSrvUavGpuDescriptorHandle(texture->GetViewIndex());

			graphicsCommandList->SetGraphicsRootDescriptorTable(rootParameter, gpuDescriptorHandle);



		}
		break;

		case ERenderPassCommandType::eRenderTargetSetting:
		{
			RenderPassSetRenderTargetCommand* renderTargetSettingCommand = static_cast<RenderPassSetRenderTargetCommand*>(renderPassCommandElement);

			

			D3D12_CPU_DESCRIPTOR_HANDLE renderTargetCpuDescriptorHandle;
			D3D12_CPU_DESCRIPTOR_HANDLE depthStencilBufferCpuDescriptorHandle;


			if (renderTargetSettingCommand->GetRenderTargetUsage())
			{

				int renderTargetTextureLocalID =renderTargetSettingCommand->GetRenderTargetTextureLocalID();
						


				RenderTargetTexture* renderTargetTexture = nullptr;

				if (renderTargetTextureLocalID == -1)
				{
					//기본렌더타겟
					RenderTargetTexture* defaultRenderTargetTexture = mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget;


					renderTargetCpuDescriptorHandle = mDescriptorHeapManagerMaster->GetRtvCpuDescriptorHandle(defaultRenderTargetTexture->GetRendedrTargetViewIndex());
				}
				else
				{
					renderTargetTexture = static_cast<RenderTargetTexture*>(effectCustomTextureVector[renderTargetTextureLocalID]);

					renderTargetCpuDescriptorHandle = mDescriptorHeapManagerMaster->GetRtvCpuDescriptorHandle(renderTargetTexture->GetRendedrTargetViewIndex());
				}
			}


			if (renderTargetSettingCommand->GetDepthStencilBufferUsage())
			{

				int depthStencilBufferLocalID = renderTargetSettingCommand->GetDepthStencilBufferLocalID();





				


				if (depthStencilBufferLocalID == -1)
				{

					Texture* defaultdepthStencilBuffer = mMapLayerSettingItemVector[mapLayerIndex].mDepthStencilBuffer;

					//기본 depthStencil 버퍼
					depthStencilBufferCpuDescriptorHandle = mDescriptorHeapManagerMaster->GetDsvCpuDescriptorHandle(defaultdepthStencilBuffer->GetViewIndex());
				}
				else
				{

					
					Texture* depthStencilBuffer =effectCustomTextureVector[depthStencilBufferLocalID];

					depthStencilBufferCpuDescriptorHandle = mDescriptorHeapManagerMaster->GetDsvCpuDescriptorHandle(depthStencilBuffer->GetViewIndex());
				}
			}

			graphicsCommandList->OMSetRenderTargets(1, &renderTargetCpuDescriptorHandle, false, &depthStencilBufferCpuDescriptorHandle);


		}
			break;


		case ERenderPassCommandType::eResourceStateTransition:
		{

			

			RenderPassResourceStateTransitionCommand* resourceStateTransitionCommand = static_cast<RenderPassResourceStateTransitionCommand*>(renderPassCommandElement);

			int textureLocalID = resourceStateTransitionCommand->GetTextureLocalID();

		

			ID3D12Resource* resource = nullptr;
			if (textureLocalID == -1)
			{
				resource = mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget->GetResource().Get();
			}
			else
			{
				Texture* customResource = effectCustomTextureVector[textureLocalID];

				resource = customResource->GetResource().Get();

			}

			D3D12_RESOURCE_STATES stateBefore  = resourceStateTransitionCommand->GetStateBefore();
			D3D12_RESOURCE_STATES stateAfter  = resourceStateTransitionCommand->GetStateAfter();
			CD3DX12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, stateBefore,
				stateAfter);

			graphicsCommandList->ResourceBarrier(1, &transitionBarrier);


		}
		break;
		}

	}




}

void Quad::RenderPassSystem::AfterDraw(int mapLayerIndex, bool stencilDrawFlag)
{


	//if (mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget == nullptr)
	//	return;


	//Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();



	//RenderTargetTexture* renderTarget = mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget;
	//Texture* depthStencilBuffer = mMapLayerSettingItemVector[mapLayerIndex].mDepthStencilBuffer;

	//auto resourceBarrierTransition = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->GetResource().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	//graphicsCommandList->ResourceBarrier(1, &resourceBarrierTransition);
	//if (stencilDrawFlag)
	//{
	//	graphicsCommandList->OMSetRenderTargets(0, nullptr, false, &mCurrentDefaultDsv);
	//

	//}else
	//{
	//	graphicsCommandList->OMSetRenderTargets(1, &mCurrentDefaultRtv, false, &mCurrentDefaultDsv);
	//}



}

void Quad::RenderPassSystem::UploadDataToRenderPassShaderResource(RenderPassTwo* renderPass, std::vector<RenderItem*>& renderItemVector,const PassData & passData)
{
	std::vector<ShaderResource*> shaderResourceVector = renderPass->GetShaderResourceVector();

	for (int shaderResourceIndex = 0; shaderResourceIndex < shaderResourceVector.size(); ++shaderResourceIndex)
	{
		ShaderResource* currShaderResource = shaderResourceVector[shaderResourceIndex];
		EShaderResourceType currShaderResourceType = currShaderResource->GetShaderResourceType();

		switch (currShaderResourceType)
		{

		case EShaderResourceType::eConstantBuffer:
			{
				ShaderResourceConstantBuffer* shaderResourceConstantBuffer = (ShaderResourceConstantBuffer*)currShaderResource;
				bool objectOrPassPassFlag = shaderResourceConstantBuffer->GetObjectOrPassFlag();
				if (objectOrPassPassFlag)
				{
					for (int renderItemIndex = 0; renderItemIndex < renderItemVector.size(); ++renderItemIndex)
					{

						if (renderItemIndex == 80)
						{
							int a = 2;
						}
						RenderItem* currRenderItem = renderItemVector[renderItemIndex];
					//	shaderResourceConstantBuffer->UploadData(currRenderItem,passData);
						UploadDataToConstantBuffer(shaderResourceConstantBuffer, currRenderItem, passData);
						
					}
				}
				else
				{
					//shaderResourceConstantBuffer->UploadData(nullptr, passData);
					UploadDataToConstantBuffer(shaderResourceConstantBuffer, nullptr, passData);
				}

			}
			break;

		}

	}




}

void Quad::RenderPassSystem::DrawRenderPass(const RenderPassItem& renderPassItem ,int mapLayerIndex)
{




	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();




	RenderPassTwo* renderPass = renderPassItem.mRenderPass;
	const std::vector<RenderItem*>& renderItemVector = renderPassItem.mRenderItemVector[mapLayerIndex];
	graphicsCommandList->SetPipelineState(renderPass->GetPipelineState().Get());

	graphicsCommandList->SetGraphicsRootSignature(renderPass->GetRootSignature().Get());


	PreRenderPass(renderPassItem, mapLayerIndex);



	//graphicsCommandList->RSSetViewports(1, &renderSettingItem.mViewPort);
	//resource bind 
	const std::vector<ShaderResource*>& shaderResourceVector = renderPass->GetShaderResourceVector();

	//pass resource per pass 
	BindShaderResource(shaderResourceVector, nullptr, mapLayerIndex, false);


	for (int i = 0; i < renderItemVector.size(); ++i)
	{

		RenderItem* currRenderItem = renderItemVector[i];
		//bind resource per object pass
		BindShaderResource(shaderResourceVector, currRenderItem, i, true);


		//stencil 
		graphicsCommandList->OMSetStencilRef(currRenderItem->mStencilRefValue);


		//draw 
		D3D_PRIMITIVE_TOPOLOGY topologyType = renderPass->GetPrimitiveTopologyType();
		graphicsCommandList->IASetPrimitiveTopology(topologyType);


		//scissorRect
		SetScissorRect(*currRenderItem, mapLayerIndex);


		//drawRenderItem
		DrawRenderItem(currRenderItem);


		//EObjectType objectType = currRenderItem->mObjectType;
		//switch (objectType)
		//{
		//case EObjectType::eEntity:
		//case EObjectType::eCollider:
		//	DrawRenderItem(currRenderItem);
		//	break;
		//case EObjectType::eLine:
		//	DrawLineBaseRenderItem(currRenderItem);
		//	break;
		//}

		
		
	}


	


}

void Quad::RenderPassSystem::BindShaderResource(const std::vector<ShaderResource*>& shaderResourceVector, RenderItem* renderItem, int elementIndex, bool objectOrPassFlag)
{





	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();
	//D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
	//

	//shaderResource
	for (int shaderResourceIndex = 0; shaderResourceIndex < shaderResourceVector.size(); ++shaderResourceIndex)
	{
		ShaderResource* currentShaderResource = shaderResourceVector[shaderResourceIndex];
		if (currentShaderResource->GetObjectOrPassFlag() != objectOrPassFlag)
			continue;


		switch (currentShaderResource->GetShaderResourceType())
		{
		case EShaderResourceType::eConstantBuffer:
		{
			ShaderResourceConstantBuffer* shaderResourceConstantBuffer = static_cast<ShaderResourceConstantBuffer*>(currentShaderResource);

			ViewIndex viewIndex = shaderResourceConstantBuffer->GetViewIndex(elementIndex);

			D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle = mDescriptorHeapManagerMaster->GetCbvSrvUavGpuDescriptorHandle(viewIndex);

			graphicsCommandList->SetGraphicsRootDescriptorTable(shaderResourceIndex, gpuDescriptorHandle);

		}
		break;
		case EShaderResourceType::eTexture:
		
		{
			ShaderResourceTexture* shaderResourceTexture = static_cast<ShaderResourceTexture*>(currentShaderResource);

			const std::wstring& shaderResourceTextureName = shaderResourceTexture->GetName();

			if (shaderResourceTextureName == L"gDiffuseMap")
			{
				Texture* texture = nullptr;

				const ModelSubMesh& subMesh = renderItem->mMeshComponent->GetSubMesh()[renderItem->mSubMeshIndex];
				texture = subMesh.mModelMaterial.GetDiffuseMap();
				

				ViewIndex viewIndex = texture->GetViewIndex();
				D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle = mDescriptorHeapManagerMaster->GetCbvSrvUavGpuDescriptorHandle(viewIndex);

				graphicsCommandList->SetGraphicsRootDescriptorTable(shaderResourceIndex, gpuDescriptorHandle);

			}
			else if (shaderResourceTextureName == L"gNormalMap")
			{




			}

		}
		break;
		case EShaderResourceType::eSampler:
		{

			ShaderResourceSampler* shaderResourceSampler = static_cast<ShaderResourceSampler*>(currentShaderResource);

			ViewIndex viewIndex = shaderResourceSampler->GetViewIndex();

			D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle = mDescriptorHeapManagerMaster->GetSpvGpuDescriptorHandle(viewIndex);

			graphicsCommandList->SetGraphicsRootDescriptorTable(shaderResourceIndex, gpuDescriptorHandle);
		}
		break;
		}





	}


	return;








}

void Quad::RenderPassSystem::DrawRenderItem(RenderItem* renderItem)
{

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();

	IMeshComponent* meshComponent = renderItem->mMeshComponent;
	Mesh* mesh = meshComponent->GetMesh();
	const ModelSubMesh& subMesh = meshComponent->GetSubMesh()[renderItem->mSubMeshIndex];

	D3D12_VERTEX_BUFFER_VIEW vbv = mesh->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW ibv = mesh->GetIndexBufferView();

	graphicsCommandList->IASetVertexBuffers(0, 1, &vbv);
	graphicsCommandList->IASetIndexBuffer(&ibv);

	UINT instanceCount = renderItem->mInstanceCount;
	UINT indexStart = subMesh.mIndexRange.first;
	UINT indexEnd = subMesh.mIndexRange.second;
	UINT vertexStart = subMesh.mVertexOffset;
	UINT indexCount = indexEnd - indexStart;
	graphicsCommandList->DrawIndexedInstanced(indexCount, instanceCount, indexStart, vertexStart, 0);

}

//void Quad::RenderPassSystem::DrawLineBaseRenderItem(RenderItem* renderItem)
//{
//	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();
//
//	LineBase* lineBase = (LineBase*)renderItem->mSceneElement;
//	D3D12_VERTEX_BUFFER_VIEW vbv = lineBase->GetVertexBufferView();
//	graphicsCommandList->IASetVertexBuffers(0, 1, &vbv);
//
//	size_t vertexNum = renderItem->mSubMesh->mVertexNum;
//	size_t vertexOffset = renderItem->mSubMesh->mVertexOffset;//
//
//	graphicsCommandList->DrawInstanced(vertexNum, 1, vertexOffset, 0);
//
//
//}

void Quad::RenderPassSystem::UploadDataToConstantBuffer(ShaderResourceConstantBuffer* shaderConstantBuffer,  RenderItem* renderItem,   const PassData& passData)
{

	const std::vector< ShaderResourceVariable>& shaderResourceVariableVector = shaderConstantBuffer->GetShaderResourceVariableVector();


	IMeshComponent* meshComponent = renderItem->mMeshComponent;
	const ModelSubMesh& subMesh = meshComponent->GetSubMesh()[renderItem->mSubMeshIndex];


	

	for (int variableIndex = 0; variableIndex < shaderResourceVariableVector.size(); ++variableIndex)
	{

		const std::wstring& name = shaderResourceVariableVector[variableIndex].mName;
		UINT mDataOffsetInBuffer = shaderResourceVariableVector[variableIndex].mOffsetInShaderResource;
		UINT mDataSize = shaderResourceVariableVector[variableIndex].mSize;
		D3D_SHADER_VARIABLE_CLASS variableClass = shaderResourceVariableVector[variableIndex].mClass;
		int elementNum = shaderResourceVariableVector[variableIndex].mElementNum;


		//table에서 string -> id 로 변경 히여 switch문을 이용하자

		void* src = nullptr;
		if (name == L"gWorld")
		{

			//object가 아니라 MeshComponent 단위로 들어와야한다.
		//	DirectX::XMFLOAT4X4 gWorldMatrix = object->GetTransform().GetWorldMatrix();
			DirectX::XMFLOAT4X4 gWorldMatrix = meshComponent->GetTransformWorldMatrixF();

			DirectX::XMFLOAT4X4 gWorldMatrixTransPose;
			DirectX::XMStoreFloat4x4(&gWorldMatrixTransPose, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&gWorldMatrix)));
			src = &gWorldMatrixTransPose;
		//	memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gWorldMatrixTransPose, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, src,sizeof(gWorldMatrixTransPose));
		}
		else if (name == L"gWorldInvTrans")
		{

			DirectX::XMFLOAT4X4 gWorldMatrix = meshComponent->GetTransformWorldMatrixF();

			DirectX::XMFLOAT4X4 gWorldMatrixInvTransPose;
			DirectX::XMStoreFloat4x4(&gWorldMatrixInvTransPose, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&gWorldMatrix)));
			src = &gWorldMatrixInvTransPose;
			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], src, mDataSize);

			shaderConstantBuffer->UploadData(variableIndex, src,sizeof(gWorldMatrixInvTransPose));
		}
		else if (name == L"gEye")
		{
			DirectX::XMFLOAT3 gEye = passData.mCamera->GetObjectPositionWorld();
			src = &gEye;
			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], src, mDataSize);
		}
		else if (name == L"gViewProj")
		{
			DirectX::XMFLOAT4X4 gViewProj = passData.mCamera->GetViewProjMatrix();
			DirectX::XMFLOAT4X4 gViewProjTranspose;
			DirectX::XMStoreFloat4x4(&gViewProjTranspose, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&gViewProj)));
			src = &gViewProjTranspose;

		//	memcpy(&mDataTempBuffer[mDataOffsetInBuffer], src, mDataSize);

			shaderConstantBuffer->UploadData(variableIndex, src,sizeof(gViewProj));
		}
		else if (name == L"gDirectionalLight")
		{
			//빛은 Light라는 구조체 타입이여하고 무조건

			const std::wstring& structTypeName = shaderResourceVariableVector[variableIndex].mTypeName;
			const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc = shaderConstantBuffer->GetShaderResourceStructDesc(structTypeName);


			UploadLightStructData(shaderConstantBuffer, variableIndex, pShaderResourceStructTypeDesc, elementNum,
				mDataOffsetInBuffer, passData, ELightType::eDirect);


		}
		else if (name == L"gSpotLight")
		{
			const std::wstring& structTypeName = shaderResourceVariableVector[variableIndex].mTypeName;
			const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc = shaderConstantBuffer->GetShaderResourceStructDesc(structTypeName);


			UploadLightStructData(shaderConstantBuffer, variableIndex, pShaderResourceStructTypeDesc, elementNum,
				mDataOffsetInBuffer, passData, ELightType::eSpot);

		}
		else if (name == L"gPointLight")
		{
			const std::wstring& structTypeName = shaderResourceVariableVector[variableIndex].mTypeName;
			const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc = shaderConstantBuffer->GetShaderResourceStructDesc(structTypeName);


			UploadLightStructData(shaderConstantBuffer, variableIndex,pShaderResourceStructTypeDesc, elementNum,
				mDataOffsetInBuffer, passData, ELightType::ePoint);

		}
		else if (name == L"gFresnelR0")
		{


			DirectX::XMFLOAT3 gFresnelR0 = subMesh.mModelMaterial.GetFresnelR0();

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gFresnelR0, mDataSize);
			src = &gFresnelR0;
			shaderConstantBuffer->UploadData(variableIndex, src);

		}
		else if (name == L"gShiness")
		{
			float gShiness = subMesh.mModelMaterial.GetShiness();

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gShiness, mDataSize);
			
			shaderConstantBuffer->UploadData(variableIndex, &gShiness);
		}
		else if (name == L"gSpecular")
		{
			DirectX::XMFLOAT3 gSpecular = subMesh.mModelMaterial.GetSpecular();

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gSpecular, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &gSpecular);
		}
		else if (name == L"gAmbient")
		{
			DirectX::XMFLOAT3 gAmbient = subMesh.mModelMaterial.GetAmbient();

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gAmbient, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &gAmbient);
		}
		else if (name == L"gColor")
		{


			DirectX::XMFLOAT3 gColor;
			gColor = subMesh.mModelMaterial.GetColor();


		/*		case EObjectType::eLine:
				{
					const LineModelSubMesh* modelSubMesh = static_cast<const LineModelSubMesh*>(renderItem->mSubMesh);
					LineBase* lineBase = (LineBase*)renderItem->mSceneElement;
					gColor = modelSubMesh->mModelMaterial.GetColor();
				}
				break;*/
			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gColor, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &gColor);


		}
		else if (name == L"gColorIntensity")
		{
			DirectX::XMFLOAT3 gColorIntensity;
			gColorIntensity = subMesh.mModelMaterial.GetColorIntensity();


			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gColorIntensity, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &gColorIntensity);


		}
		else if (name == L"gEmissive")
		{
			DirectX::XMFLOAT3 gEmissive;
			gEmissive = subMesh.mModelMaterial.GetEmissive();

			shaderConstantBuffer->UploadData(variableIndex, &gEmissive);

		}
		else if (name == L"gClientWidth")
		{
			float clientWidth = passData.mViewPort.Width;
			
			shaderConstantBuffer->UploadData(variableIndex, &clientWidth);



		}
		else if (name == L"gClientHeight")
		{

			float clientHeight = passData.mViewPort.Height;

			shaderConstantBuffer->UploadData(variableIndex, &clientHeight);



		}
		else if (name == L"gTextureTrans0")
		{
			DirectX::XMFLOAT4X4  textureTransformMatrix = subMesh.mModelMaterial.GetTextureTransformMatrix(0);
			DirectX::XMStoreFloat4x4(&textureTransformMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&textureTransformMatrix)));

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &textureTransformMatrix, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &textureTransformMatrix);

		}
		else if (name == L"gDiffuseMapGammaFlag")
		{
	
			EColorFormat textureFormat = subMesh.mModelMaterial.GetDiffuseMap()->GetTextureColorFormat();
			float flag;

			if (textureFormat == EColorFormat::eRgb)
			{
				flag = 0.0f;
			}
			else if (textureFormat == EColorFormat::eSRgb)
			{
				flag = 1.0f;
			}

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &flag, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &flag);
		}
		else if (name == L"gBoneAnimationMatrix")
		{
			//엔티티만일단 가능하다고 보자
			//그냥 정적메시도 단일 본이라고 생각할수도있겠지만 다음에 생각하자


			//Entity* entity = dynamic_cast <Entity*> (object);
			//if (entity == nullptr)
			//{
			//	OutputDebugString(L"Entity가아닌 오브젝트에 에니메이션effect를 사용할려고한다.\n");
			//	assert(0);
			//}

			//const AnimationComponent& animComponent = *entity->GetComponent<AnimationComponent>();
			//const std::vector<DirectX::XMFLOAT4X4>& finalTransformVector = animComponent.GetFinalTransformMatrixVector();
			////transpose가 된 상태(여기서 복사해서 전치하면 비용발생하니) 
			//if (finalTransformVector.size() > elementNum)
			//{
			//	OutputDebugString(L"뼈대팔렛트행렬의수가 HLSL에 정의된 수보다 많다.\n");
			//	assert(0);
			//}

			////memcpy(&mDataTempBuffer[mDataOffsetInBuffer], finalTransformVector.data(), sizeof(DirectX::XMFLOAT4X4) * finalTransformVector.size());
			//src = const_cast<DirectX::XMFLOAT4X4*>(finalTransformVector.data());
			//shaderConstantBuffer->UploadData(variableIndex, src,sizeof(DirectX::XMFLOAT4X4)* finalTransformVector.size());
		}
		else if (name == L"gBoneAnimationInverseTransposeMatrix")
		{

		//	Entity* entity = dynamic_cast <Entity*> (object);
			//if (entity == nullptr)
			//{
			//	OutputDebugString(L"Entity가아닌 오브젝트에 에니메이션effect를 사용할려고한다.\n");
			//	assert(0);
			//}

			//const AnimationComponent& animComponent = *entity->GetComponent<AnimationComponent>();
			//const std::vector<DirectX::XMFLOAT4X4>& finalInverseTransposeTransformVector = animComponent.GetFinalInverseTransposeTransformMatrixVector();
			////transpose가 된 상태(여기서 복사해서 전치하면 비용발생하니) 
			//if (finalInverseTransposeTransformVector.size() > elementNum)
			//{
			//	OutputDebugString(L"뼈대팔렛트행렬의수가 HLSL에 정의된 수보다 많다.\n");
			//	assert(0);
			//}

			////memcpy(&mDataTempBuffer[mDataOffsetInBuffer], finalInverseTransposeTransformVector.data(), sizeof(DirectX::XMFLOAT4X4) * finalInverseTransposeTransformVector.size());

			//src = const_cast<DirectX::XMFLOAT4X4*>(finalInverseTransposeTransformVector.data());
			//shaderConstantBuffer->UploadData(variableIndex, src, sizeof(DirectX::XMFLOAT4X4) * finalInverseTransposeTransformVector.size());

		}
		//	float4x4 gBoneAnimationMatrix[120];
	//	float4x4 gBoneAnimationInverseTransposeMatrix[120];







	}




	shaderConstantBuffer->EndUploadData();







}

void Quad::RenderPassSystem::UploadLightStructData(	ShaderResourceConstantBuffer * shaderResourceConstantBuffer ,int variableIndex, const ShaderResourceStructTypeDesc* pShaderResourceStructTypeDesc, int elementNum, int structOffset, const PassData& passData, ELightType lightType)
{


	const std::vector<LightItem*>* pLightItemVector = nullptr;
	switch (lightType)
	{
	case ELightType::eDirect:
		pLightItemVector = &passData.mDirectionalLightItemVector;
		break;
	case ELightType::ePoint:
		pLightItemVector = &passData.mPointSpotLightItemVector;
		break;
	case ELightType::eSpot:
		pLightItemVector = &passData.mSpotLightItemVector;
		break;
	}



	const std::vector<ShaderResourceVariable> structVariableVector = pShaderResourceStructTypeDesc->mVariableVector;
	unsigned int structSize = pShaderResourceStructTypeDesc->mSize;
	unsigned int structAlignedSize = pShaderResourceStructTypeDesc->mAlignedSize;


	if (elementNum == 0)
	{//배열이 아닌 그냥 단일변수 /원소가 하나뿐인 배열로 취급하자.
		elementNum = 1;
	}


	for (int elementIndex = 0; elementIndex < elementNum; ++elementIndex)
	{
		if (elementIndex >= (*pLightItemVector).size())
			return;

		for (int structVariableIndex = 0; structVariableIndex < structVariableVector.size(); ++structVariableIndex)
		{
			const std::wstring& variableName = structVariableVector[structVariableIndex].mName;
			int offset = structVariableVector[structVariableIndex].mOffsetInShaderResource;
			if (variableName == L"gLight")
			{
				DirectX::XMFLOAT3 light = (*pLightItemVector)[elementIndex]->mLight->GetLight();
				//memcpy(&dataTempBuffer[structOffset + offset], &light, structVariableVector[structVariableIndex].mSize);

				shaderResourceConstantBuffer->UploadStructData(variableIndex, elementIndex, structAlignedSize,offset, &light, structVariableVector[structVariableIndex].mSize);
			}
			else if (variableName == L"gDirection")
			{

				DirectX::XMFLOAT3 direction = (*pLightItemVector)[elementIndex]->mLight->GetLightDirect();

				//memcpy(&dataTempBuffer[structOffset + offset], &direction, structVariableVector[structVariableIndex].mSize);
				shaderResourceConstantBuffer->UploadStructData(variableIndex, elementIndex, structAlignedSize,offset, &direction, structVariableVector[structVariableIndex].mSize);

			}
			else if (variableName == L"gPosW")
			{

				DirectX::XMFLOAT3 positionW = (*pLightItemVector)[elementIndex]->mLight->GetObjectPositionWorld();
				//memcpy(&dataTempBuffer[structOffset + offset], &positionW, structVariableVector[structVariableIndex].mSize);
				shaderResourceConstantBuffer->UploadStructData(variableIndex, elementIndex, structAlignedSize, offset, &positionW, structVariableVector[structVariableIndex].mSize);
			}

		}
	}




}

void Quad::RenderPassSystem::SetScissorRect(const RenderItem& renderItem, int mapLayerIndex)
{

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList =	mGraphicsCommandObject->GetGraphicsCommandList();

	if(renderItem.mScissorRectUsageFlag)	
		graphicsCommandList->RSSetScissorRects(1, &renderItem.mScissorRect);
	else
		graphicsCommandList->RSSetScissorRects(1, &mMapLayerSettingItemVector[mapLayerIndex].mScissorRectLocal);

}
