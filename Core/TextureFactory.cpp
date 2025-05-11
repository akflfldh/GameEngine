#include "ResourceFactory/TextureFactory/TextureFactory.h"
//#include"Allocator/DestructingPoolAllocator.h"
#include"Asset/Texture/RenderTargetTexture.h"
//#include"Asset/Texture/Texture.h"

#include"HeapManager/DescriptorHeapManagerMaster.h"
//#include"TextureManager.h"


Quad::TextureFactory::TextureFactory()
	:mTextureAllocator(nullptr), mDevice(nullptr), mDescriptorHeapManagerMaster(nullptr), mGraphicsCommandObject(nullptr)
{

	mTextureAllocator = new DestructingPoolAllocator<Texture>(1000);


	mTextureAllocator->Initialize();


	mDtorTable["Texture"] = [allocator = mTextureAllocator, heapManager = mDescriptorHeapManagerMaster](Texture* texture) {

		if (texture->GetViewIndex() != -1)
		{
			heapManager->ReleaseCbvSrvUav(texture->GetViewIndex());
		}

		allocator->ReleaseInstance(texture);
		};

	mDtorTable["RenderTargetTexture"] = [heapManager = mDescriptorHeapManagerMaster](Texture* texture) {

		RenderTargetTexture* renderTargetTexture = static_cast<RenderTargetTexture*>(texture);

		heapManager->ReleaseCbvSrvUav(renderTargetTexture->GetViewIndex());
		heapManager->ReleaseRtv(renderTargetTexture->GetRendedrTargetViewIndex());

		delete renderTargetTexture;
		};







}

void Quad::TextureFactory::Initialize(Microsoft::WRL::ComPtr<ID3D12Device>& device, DescriptorHeapManagerMaster* descriptorHeapManagerMaster, GraphicCommand* graphicsCommandObject)
{
	mDevice = device;
	mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;
	mGraphicsCommandObject = graphicsCommandObject;

}



Quad::Texture* Quad::TextureFactory::CreateTexture()
{
	auto instance = GetInstance();
	
	Texture* newTexture = instance->mTextureAllocator->GetInstance();
	return newTexture;
		
}




Quad::RenderTargetTexture* Quad::TextureFactory::CreateRenderTargetTexture(int width, int height)
{
	auto instance = GetInstance();


	RenderTargetTexture* renderTargetTexture = new RenderTargetTexture();
	//TextureManager::CreateTexture()

	ID3D12Resource* texture = nullptr;



	D3D12_HEAP_PROPERTIES heapPropertiesDesc;
	heapPropertiesDesc.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDesc.CreationNodeMask = 0;
	heapPropertiesDesc.VisibleNodeMask = 0;



	
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality= 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


	//instance->mDevice->CreateCommittedResource(&heapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue, /IID_PPV_ARGS(&texture));
	instance->mDevice->CreateCommittedResource(&heapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, &clearValue, IID_PPV_ARGS(&texture));



	//srv
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp= 0;
	srvDesc.Texture2D.PlaneSlice= 0;


	ViewIndex srvIndex= instance->mDescriptorHeapManagerMaster->CreateSrv(texture, &srvDesc);
	renderTargetTexture->SetViewIndex(srvIndex);



	//rtv
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;


	ViewIndex rtvIndex = instance->mDescriptorHeapManagerMaster->CreateRtv(texture, &rtvDesc);
	renderTargetTexture->SetRenderTargetViewIndex(rtvIndex);



	renderTargetTexture->SetTextureResource(texture);

	 

	return renderTargetTexture;










}

Quad::Texture* Quad::TextureFactory::CreateDepthStencilBuffer(int width, int height)
{


	auto instance = GetInstance();

	/*if (instance->mNameTable.GetID(name) != 0 )
		return nullptr;*/


	Texture* depthStencilBuffer = CreateTexture();


	ID3D12Resource* texture = nullptr;


	D3D12_HEAP_PROPERTIES heapPropertiesDesc;
	heapPropertiesDesc.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDesc.CreationNodeMask = 0;
	heapPropertiesDesc.VisibleNodeMask = 0;




	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags =D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	D3D12_CLEAR_VALUE clearDepthStencilValue;
	
	clearDepthStencilValue.DepthStencil.Depth = 1.0f;
	clearDepthStencilValue.DepthStencil.Stencil = 0.0f;
	clearDepthStencilValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;



	instance->mDevice->CreateCommittedResource(&heapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearDepthStencilValue, IID_PPV_ARGS(&texture));



	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Texture2D.MipSlice = 0;

	ViewIndex viewIndex = instance->mDescriptorHeapManagerMaster->CreateDsv(texture, &dsvDesc);
	


	depthStencilBuffer->SetViewIndex(viewIndex);
	depthStencilBuffer->SetTextureResource(texture);

	return depthStencilBuffer;
}



void Quad::TextureFactory::CreateSrvTex2D(Texture* texture)
{
	auto instance = GetInstance();
	//VIEW 생성  //일단은 2차원texture나 buffer만 생각한다.
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = texture->GetResource();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = resource->GetDesc().Format;
	srvDesc.ViewDimension = ConvertSRVDimensionEnum(resource->GetDesc().Dimension);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (srvDesc.ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2D)
		FillSRVTEX2DDesc(srvDesc, resource);

	texture->SetTextureFormat(ConvertTextureFormatEnum(srvDesc.Format));

	ViewIndex viewIndex = instance->mDescriptorHeapManagerMaster->CreateSrv(resource, &srvDesc);
	texture->SetViewIndex(viewIndex);

}

void Quad::TextureFactory::ReleaseTexture(Texture* texture)
{
	
	auto instance = GetInstance();
	instance->mDtorTable[texture->GetClassTypeName()](texture);
}


D3D12_SRV_DIMENSION Quad::TextureFactory::ConvertSRVDimensionEnum(D3D12_RESOURCE_DIMENSION dimension)
{

	switch (dimension)
	{
	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		return D3D12_SRV_DIMENSION_TEXTURE2D;

	case D3D12_RESOURCE_DIMENSION::D3D12_RESOURCE_DIMENSION_BUFFER:
		return D3D12_SRV_DIMENSION_BUFFER;

	}
	MessageBox(nullptr, L"srv dimension변환에서 처리미룬것이 있다", L"error", 0);
	assert(-1);

}


void Quad::TextureFactory::FillSRVTEX2DDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& oSrv, Microsoft::WRL::ComPtr<ID3D12Resource> resource)
{
	oSrv.Texture2D.MipLevels = resource->GetDesc().MipLevels;
	oSrv.Texture2D.MostDetailedMip = 0;
	oSrv.Texture2D.ResourceMinLODClamp = 0.0;
	oSrv.Texture2D.PlaneSlice = 0;

}

Quad::ETextureFormat Quad::TextureFactory::ConvertTextureFormatEnum(DXGI_FORMAT format)
{

	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:

		return ETextureFormat::eRgb;

	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return ETextureFormat::eSRgb;
	}

}
