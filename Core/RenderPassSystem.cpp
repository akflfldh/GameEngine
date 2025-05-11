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
void Quad::RenderPassSystem::Initialize(GraphicCommand* graphicsCommandObject,
	DescriptorHeapManagerMaster* descriptorHeapManagerMaster)
{
	mGraphicsCommandObject = graphicsCommandObject;
	mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;

}
void Quad::RenderPassSystem::AddRenderItem(RenderPassTwo* renderPass, RenderItem* renderItem,int mapLayerIndex)
{
	if (renderItem->mSceneElement == nullptr)
	{
		int a = 2;
	}


	ERenderPassID renderPassID = renderPass->GetRenderPassID();

	if (renderPassID == ERenderPassID::eGizmo)
	{
		int a = 2;
	}

	std::vector<RenderPassItem>& currentRenderItemVector = mRenderPassItemContainer[renderPassID];

	std::vector<RenderPassItem>::iterator it = std::find_if(currentRenderItemVector.begin(), currentRenderItemVector.end(),
		[&renderPass](const RenderPassItem& passItem) { return passItem.mRenderPass == renderPass ? true : false; });

	if (it == currentRenderItemVector.end())
	{
	
		RenderPassItem newRenderPassItem;
		newRenderPassItem.mRenderItemVector.resize(10);
		newRenderPassItem.mRenderPass = renderPass;
		newRenderPassItem.mRenderItemVector[mapLayerIndex].push_back(renderItem);
		currentRenderItemVector.push_back(std::move(newRenderPassItem));
		return;
	}

	it->mRenderItemVector[mapLayerIndex].push_back(renderItem);
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

void Quad::RenderPassSystem::SetMapLayerVector(const std::vector<MapLayer>& mapLayerVector)
{

	mMapLayerSettingItemVector.resize(mapLayerVector.size());

	for (int i = 0; i < mMapLayerSettingItemVector.size(); ++i)
	{
		mMapLayerSettingItemVector[i].mCamera = mapLayerVector[i].mCamera;
		mMapLayerSettingItemVector[i].mViewPort = mapLayerVector[i].mViewPort;
		mMapLayerSettingItemVector[i].mRenderTarget = mapLayerVector[i].mRenderTarget;
		mMapLayerSettingItemVector[i].mDepthStencilBuffer = mapLayerVector[i].mDepthStencilBuffer;
	}

}

void Quad::RenderPassSystem::Draw(bool stencilDrawFlag)
{








	for (int mapLayerIndex = 0; mapLayerIndex < mMapLayerSettingItemVector.size(); ++mapLayerIndex)
	{

	
		BeforeDraw(mapLayerIndex,stencilDrawFlag);





		for (int renderPassIndex = 0; renderPassIndex < mRenderPassItemContainer.size(); ++renderPassIndex)
		{
			
			std::vector<RenderPassItem>& renderPassItemVector = mRenderPassItemContainer[renderPassIndex];





			for (int renderPassItemIndex = 0; renderPassItemIndex < renderPassItemVector.size(); ++renderPassItemIndex)
			{



				if (stencilDrawFlag && renderPassItemVector[renderPassItemIndex].mRenderPass->GetStencilWriteFlag())
				{

				
					DrawRenderPass(renderPassItemVector[renderPassItemIndex] ,mapLayerIndex);
				}
				else if (stencilDrawFlag == false)
				{
					DrawRenderPass(renderPassItemVector[renderPassItemIndex],mapLayerIndex);
				}
			}

		}






		AfterDraw(mapLayerIndex, stencilDrawFlag);

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
	graphicsCommandList->RSSetViewports(1, &mMapLayerSettingItemVector[mapLayerIndex].mViewPort);

	//rtv占쏙옙 占쏙옙占쏙옙
	if (mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget == nullptr)	//nullptr == 占식몌옙占쏙옙占?
		return;
	else
	{
		RenderTargetTexture* renderTarget = mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget;
		Texture* depthStencilBuffer = mMapLayerSettingItemVector[mapLayerIndex].mDepthStencilBuffer;

		auto resourceBarrierTransition = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->GetResource().Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
		graphicsCommandList->ResourceBarrier(1, &resourceBarrierTransition);


		D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuDescriptorHandle = mDescriptorHeapManagerMaster->GetRtvCpuDescriptorHandle(renderTarget->GetRendedrTargetViewIndex());


		D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuDescirptorHandle = mDescriptorHeapManagerMaster->GetDsvCpuDescriptorHandle(depthStencilBuffer->GetViewIndex());

		float color[4] = { 0.0f,0.0f,0.0f,1.0f };
		graphicsCommandList->ClearRenderTargetView(rtvCpuDescriptorHandle,color, 0, nullptr);
		graphicsCommandList->ClearDepthStencilView(dsvCpuDescirptorHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);




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

void Quad::RenderPassSystem::AfterDraw(int mapLayerIndex, bool stencilDrawFlag)
{


	if (mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget == nullptr)
		return;


	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();



	RenderTargetTexture* renderTarget = mMapLayerSettingItemVector[mapLayerIndex].mRenderTarget;
	Texture* depthStencilBuffer = mMapLayerSettingItemVector[mapLayerIndex].mDepthStencilBuffer;

	auto resourceBarrierTransition = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->GetResource().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
	graphicsCommandList->ResourceBarrier(1, &resourceBarrierTransition);
	if (stencilDrawFlag)
	{
		graphicsCommandList->OMSetRenderTargets(0, nullptr, false, &mCurrentDefaultDsv);
	

	}else
	{
		graphicsCommandList->OMSetRenderTargets(1, &mCurrentDefaultRtv, false, &mCurrentDefaultDsv);
	}

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
		if (currRenderItem->mStencilRefValue == 1)
		{
			int a = 2; 
		}

		//draw 
		D3D_PRIMITIVE_TOPOLOGY topologyType = renderPass->GetPrimitiveTopologyType();
		graphicsCommandList->IASetPrimitiveTopology(topologyType);

		EObjectType objectType = currRenderItem->mObjectType;
		switch (objectType)
		{
		case EObjectType::eEntity:
		case EObjectType::eCollider:
			DrawEntityRenderItem(currRenderItem);
			break;
		case EObjectType::eLine:
			DrawLineBaseRenderItem(currRenderItem);
			break;
		}

		
		
	}





}

void Quad::RenderPassSystem::BindShaderResource(const std::vector<ShaderResource*>& shaderResourceVector, RenderItem* renderItem, int elementIndex, bool objectOrPassFlag)
{





	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();
	//D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle
	//占싹쇽옙占싱되몌옙 占쏙옙占쏙옙占싱부븝옙占쏙옙 switch占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙.

	//shaderResource占쏙옙占쏙옙占쏙옙占?rootsignature占쏙옙 占쏙옙占싸듸옙占싫댐옙.
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
			//占쌔쏙옙처占쏙옙 占쏙옙占쏙옙占쏙옙占쏙옙占쌜븝옙占싹쇽옙占쏙옙占쌍곤옙 占쏙옙카占싱박쏙옙占쏙옙占쏙옙究占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙甄占? 확占쏙옙占?
		{
			ShaderResourceTexture* shaderResourceTexture = static_cast<ShaderResourceTexture*>(currentShaderResource);

			const std::wstring& shaderResourceTextureName = shaderResourceTexture->GetName();

			if (shaderResourceTextureName == L"gDiffuseMap")
			{
				Texture* texture = nullptr;
				switch (renderItem->mObjectType)
				{
				case EObjectType::eEntity:
				{
					const ModelSubMesh* entitySubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
					texture = entitySubMesh->mModelMaterial.GetDiffuseMap();
				}
				}

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

void Quad::RenderPassSystem::DrawEntityRenderItem(RenderItem* renderItem)
{

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();

	const ModelSubMesh* entitySubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);

	Mesh* mesh = entitySubMesh->mMesh;
	D3D12_VERTEX_BUFFER_VIEW vbv = mesh->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW ibv = mesh->GetIndexBufferView();

	graphicsCommandList->IASetVertexBuffers(0, 1, &vbv);
	graphicsCommandList->IASetIndexBuffer(&ibv);

	UINT instanceCount = renderItem->mInstanceCount;
	UINT indexStart = renderItem->mSubMesh->mIndexRange.first;
	UINT indexEnd = renderItem->mSubMesh->mIndexRange.second;
	UINT vertexStart = renderItem->mSubMesh->mVertexOffset;
	UINT indexCount = indexEnd - indexStart;
	graphicsCommandList->DrawIndexedInstanced(indexCount, instanceCount, indexStart, vertexStart, 0);

}

void Quad::RenderPassSystem::DrawLineBaseRenderItem(RenderItem* renderItem)
{
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList = mGraphicsCommandObject->GetGraphicsCommandList();

	LineBase* lineBase = (LineBase*)renderItem->mSceneElement;
	D3D12_VERTEX_BUFFER_VIEW vbv = lineBase->GetVertexBufferView();
	graphicsCommandList->IASetVertexBuffers(0, 1, &vbv);

	size_t vertexNum = renderItem->mSubMesh->mVertexNum;
	size_t vertexOffset = renderItem->mSubMesh->mVertexOffset;//占쌓삼옙 0占싹곤옙占싱댐옙占싹댐옙.

	graphicsCommandList->DrawInstanced(vertexNum, 1, vertexOffset, 0);


}

void Quad::RenderPassSystem::UploadDataToConstantBuffer(ShaderResourceConstantBuffer* shaderConstantBuffer,  RenderItem* renderItem,  const const PassData& passData)
{

	const std::vector< ShaderResourceVariable>& shaderResourceVariableVector = shaderConstantBuffer->GetShaderResourceVariableVector();



	Object* object = nullptr;
	if (renderItem != nullptr)
	{
		if (renderItem->mSceneElement->GetSceneElementType() == ESceneElementType::eObject)
			object = static_cast<Object*>(renderItem->mSceneElement);
		else
			return;

	}


	for (int variableIndex = 0; variableIndex < shaderResourceVariableVector.size(); ++variableIndex)
	{

		const std::wstring& name = shaderResourceVariableVector[variableIndex].mName;
		UINT mDataOffsetInBuffer = shaderResourceVariableVector[variableIndex].mOffsetInShaderResource;
		UINT mDataSize = shaderResourceVariableVector[variableIndex].mSize;
		D3D_SHADER_VARIABLE_CLASS variableClass = shaderResourceVariableVector[variableIndex].mClass;
		int elementNum = shaderResourceVariableVector[variableIndex].mElementNum;



		void* src = nullptr;
		if (name == L"gWorld")
		{
			DirectX::XMFLOAT4X4 gWorldMatrix = object->GetTransform().GetWorldMatrix();

			DirectX::XMFLOAT4X4 gWorldMatrixTransPose;
			DirectX::XMStoreFloat4x4(&gWorldMatrixTransPose, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&gWorldMatrix)));
			src = &gWorldMatrixTransPose;
		//	memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gWorldMatrixTransPose, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, src,sizeof(gWorldMatrixTransPose));
		}
		else if (name == L"gWorldInvTrans")
		{

			DirectX::XMFLOAT4X4 gWorldMatrix = object->GetTransform().GetWorldMatrix();

			DirectX::XMFLOAT4X4 gWorldMatrixInvTransPose;
			DirectX::XMStoreFloat4x4(&gWorldMatrixInvTransPose, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&gWorldMatrix)));
			src = &gWorldMatrixInvTransPose;
			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], src, mDataSize);

			shaderConstantBuffer->UploadData(variableIndex, src,sizeof(gWorldMatrixInvTransPose));
		}
		else if (name == L"gEye")
		{
			DirectX::XMFLOAT3 gEye = passData.mCamera->GetTransform().GetPositionWorld();
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

			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);

			DirectX::XMFLOAT3 gFresnelR0 = modelSubMesh->mModelMaterial.GetFresnelR0();

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gFresnelR0, mDataSize);
			src = &gFresnelR0;
			shaderConstantBuffer->UploadData(variableIndex, src);

		}
		else if (name == L"gShiness")
		{
			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh *>(renderItem->mSubMesh);
			float gShiness = modelSubMesh->mModelMaterial.GetShiness();

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gShiness, mDataSize);
			
			shaderConstantBuffer->UploadData(variableIndex, &gShiness);
		}
		else if (name == L"gSpecular")
		{
			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
			DirectX::XMFLOAT3 gSpecular = modelSubMesh->mModelMaterial.GetSpecular();

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gSpecular, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &gSpecular);
		}
		else if (name == L"gAmbient")
		{
			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
			DirectX::XMFLOAT3 gAmbient = modelSubMesh->mModelMaterial.GetAmbient();

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gAmbient, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &gAmbient);
		}
		else if (name == L"gColor")
		{


			DirectX::XMFLOAT3 gColor;

			if (renderItem->mSceneElement->GetSceneElementType() == ESceneElementType::eCollider)
			{
				const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
				gColor = modelSubMesh->mModelMaterial.GetColor();
			}
			else if (renderItem->mSceneElement->GetSceneElementType() == ESceneElementType::eObject)
			{
				Object* object = static_cast<Object*>(renderItem->mSceneElement);

				switch (object->GetObjectType())
				{
				case EObjectType::eEntity:
				case EObjectType::eUiEntity:
				{
					const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
					gColor = modelSubMesh->mModelMaterial.GetColor();
					if (gColor.x == 0)
					{
						int a = 2;
					}
				}
				break;
				case EObjectType::eLine:
				{
					const LineModelSubMesh* modelSubMesh = static_cast<const LineModelSubMesh*>(renderItem->mSubMesh);
					LineBase* lineBase = (LineBase*)renderItem->mSceneElement;
					gColor = modelSubMesh->mModelMaterial.GetColor();
				}
				break;
				}
			}
			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gColor, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &gColor);


		}
		else if (name == L"gColorIntensity")
		{
			DirectX::XMFLOAT3 gColorIntensity;

			if (renderItem->mSceneElement->GetSceneElementType() == ESceneElementType::eObject)
			{
				Object* object = static_cast<Object*>(renderItem->mSceneElement);
				switch (object->GetObjectType())
				{
				case EObjectType::eEntity:
				case EObjectType::eUiEntity:
				{
					const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
					gColorIntensity = modelSubMesh->mModelMaterial.GetColorIntensity();
					
				}
				break;
				}
			}

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &gColorIntensity, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &gColorIntensity);


		}
		else if (name == L"gTextureTrans0")
		{
			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
			DirectX::XMFLOAT4X4  textureTransformMatrix = modelSubMesh->mModelMaterial.GetTextureTransformMatrix(0);
			DirectX::XMStoreFloat4x4(&textureTransformMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&textureTransformMatrix)));

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], &textureTransformMatrix, mDataSize);
			shaderConstantBuffer->UploadData(variableIndex, &textureTransformMatrix);

		}
		else if (name == L"gDiffuseMapGammaFlag")
		{
			const ModelSubMesh* modelSubMesh = static_cast<const ModelSubMesh*>(renderItem->mSubMesh);
			ETextureFormat textureFormat = modelSubMesh->mModelMaterial.GetDiffuseMap()->GetTextureFormat();
			float flag;

			if (textureFormat == ETextureFormat::eRgb)
			{
				flag = 0.0f;
			}
			else if (textureFormat == ETextureFormat::eSRgb)
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


			Entity* entity = dynamic_cast <Entity*> (object);
			if (entity == nullptr)
			{
				OutputDebugString(L"Entity가아닌 오브젝트에 에니메이션effect를 사용할려고한다.\n");
				assert(0);
			}

			const AnimationComponent& animComponent = *entity->GetModel()->GetAnimationComponent();
			const std::vector<DirectX::XMFLOAT4X4>& finalTransformVector = animComponent.GetFinalTransformMatrixVector();
			//transpose가 된 상태(여기서 복사해서 전치하면 비용발생하니) 
			if (finalTransformVector.size() > elementNum)
			{
				OutputDebugString(L"뼈대팔렛트행렬의수가 HLSL에 정의된 수보다 많다.\n");
				assert(0);
			}

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], finalTransformVector.data(), sizeof(DirectX::XMFLOAT4X4) * finalTransformVector.size());
			src = const_cast<DirectX::XMFLOAT4X4*>(finalTransformVector.data());
			shaderConstantBuffer->UploadData(variableIndex, src,sizeof(DirectX::XMFLOAT4X4)* finalTransformVector.size());
		}
		else if (name == L"gBoneAnimationInverseTransposeMatrix")
		{

			Entity* entity = dynamic_cast <Entity*> (object);
			if (entity == nullptr)
			{
				OutputDebugString(L"Entity가아닌 오브젝트에 에니메이션effect를 사용할려고한다.\n");
				assert(0);
			}

			const AnimationComponent& animComponent = *entity->GetModel()->GetAnimationComponent();
			const std::vector<DirectX::XMFLOAT4X4>& finalInverseTransposeTransformVector = animComponent.GetFinalInverseTransposeTransformMatrixVector();
			//transpose가 된 상태(여기서 복사해서 전치하면 비용발생하니) 
			if (finalInverseTransposeTransformVector.size() > elementNum)
			{
				OutputDebugString(L"뼈대팔렛트행렬의수가 HLSL에 정의된 수보다 많다.\n");
				assert(0);
			}

			//memcpy(&mDataTempBuffer[mDataOffsetInBuffer], finalInverseTransposeTransformVector.data(), sizeof(DirectX::XMFLOAT4X4) * finalInverseTransposeTransformVector.size());

			src = const_cast<DirectX::XMFLOAT4X4*>(finalInverseTransposeTransformVector.data());
			shaderConstantBuffer->UploadData(variableIndex, src, sizeof(DirectX::XMFLOAT4X4) * finalInverseTransposeTransformVector.size());

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

				DirectX::XMFLOAT3 positionW = (*pLightItemVector)[elementIndex]->mLight->GetPosition();
				//memcpy(&dataTempBuffer[structOffset + offset], &positionW, structVariableVector[structVariableIndex].mSize);
				shaderResourceConstantBuffer->UploadStructData(variableIndex, elementIndex, structAlignedSize, offset, &positionW, structVariableVector[structVariableIndex].mSize);
			}

		}
	}




}
