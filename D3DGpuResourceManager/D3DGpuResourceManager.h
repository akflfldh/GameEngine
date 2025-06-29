#pragma once



#include"IGpuResourceManager.h"
#include"D3DGpuResource.h"


#include<d3d12.h>
#include<wrl.h>

#include<memory>
#include<unordered_map>

namespace D3DGRM
{
	class D3DGpuBuffer;
	class D3DGpuDescriptorHeapManager;

	class D3DGpuResourceManager:public GRM::IGpuResourceManager
	{

	public:
		D3DGpuResourceManager(ID3D12Device * device, ID3D12CommandQueue* commandQueue);
		~D3DGpuResourceManager();


		GRM::IGpuResource* CreateBuffer(const GRM::BufferDesc& bufferDesc) override;
		

		GRM::IGpuResource* Create2DTextureResource() override;

		void RegisterSwapChainBackBufferView(void* resoure) override;	//ID3D12Resource * 타입의 후면버퍼를 전달한다.
		void ReleaseSwapChainBackBufferView(void* resoure)override;
		 //void RegisterDescriptorView(D3DGpuResource* gpuResource, /*descriptor type*/);

		//registerRtvSwapChainBackBuffer(id3d12resource);


	private:

		
		D3DGpuBuffer* CreateBuffer(ID3D12Resource * resource , const GRM::BufferDesc& bufferDesc);
		
		//내부리소스버퍼생성과 CreateBuffer을통해  내부리소스를가지는 D3DGpuBuffer생성
		D3DGpuBuffer* CreateDefaultBufferResource(const GRM::BufferDesc& bufferDesc);
		D3DGpuBuffer* CreateUploadBufferResource(const GRM::BufferDesc& bufferDesc);

		void FlushCommandQueue();

		void CreateBufferDescriptor(D3DGpuBuffer* buffer, const GRM::BufferDesc& bufferDesc);


	private:

		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		size_t mCurrentFence;


		std::unique_ptr<D3DGpuDescriptorHeapManager> mCSUHeapManager;
		std::unique_ptr<D3DGpuDescriptorHeapManager> mRTVHeapManager;
		std::unique_ptr<D3DGpuDescriptorHeapManager> mDSVHeapManager;
		std::unique_ptr<D3DGpuDescriptorHeapManager> mSMPHeapManager;




		std::unordered_map< ID3D12Resource*, D3DDescriptorHandle> mSwapChainBackBufferHandleTable;//수많은 창들의 스왑체인백버퍼가 등록될수있다.

	};


}