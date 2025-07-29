#pragma once



#include"IGpuResourceManager.h"
#include"D3DGpuResource.h"


#include<d3d12.h>
#include<wrl.h>

#include<memory>
#include<unordered_map>
#include"GpuResourceDllMacro.h"
namespace D3DGRM
{
	class D3DGpuBuffer;
	class D3DGpuTexture;
	class D3DGpuDescriptorHeapManager;


	class GPURESOURCE_MANAGER_API D3DGpuResourceManager:public GRM::IGpuResourceManager
	{

	public:
		D3DGpuResourceManager(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr < ID3D12CommandQueue> commandQueue);
		~D3DGpuResourceManager();


		GRM::GRMPtr CreateBuffer(const GRM::BufferDesc& bufferDesc) override;
		
		GRM::GRMPtr CreateTexture(const GRM::TextureDesc& textureDesc) override;

		//내부 리소스는 새로운 크기의 리소스일것
		void ResizeSwapChainBackBuffer(const GRM::GRMPtr & resource, void* innerResoure) override;
		GRM::GRMPtr RegisterSwapChainBackBuffer(void* resoure) override;	//ID3D12Resource * 타입의 후면버퍼를 전달한다.

		//후면버퍼를 새로운사이즈의 버퍼로 교체하기전 먼저호출
		// 내부 리소스를 IGPUResource와의 연결을끊는다. 또한 RTV를 제거한다.
		void ReleaseSwapChainBackBuffer(const GRM::GRMPtr &  resoure)override;
		void ChangeTextureData(const GRM::GRMPtr & texture, const GRM::TextureDesc& textureDesc) override;

	

		virtual bool UploadBufferData(const GRM::GRMPtr & buffer, void* data, size_t elementSize, size_t elementNum,
			size_t bufferOffset=0) override;



		virtual GRM::GRMPtr CreateSampler() override;

		
		D3DGpuDescriptorHeapManager* GetCSUHeapManager() const;
		D3DGpuDescriptorHeapManager* GetSMPHeapManager() const;


		void TransitionResourceState(GRM::IGpuResource * resource , D3D12_RESOURCE_STATES afterState);




		virtual void Release(GRM::IGpuResource* resource) override;

		virtual void FlushGarbageCollect()override;

	private:

		D3DGpuBuffer* CreateBuffer(ID3D12Resource * resource , const GRM::BufferDesc& bufferDesc);
		
		//내부리소스버퍼생성과 CreateBuffer을통해  내부리소스를가지는 D3DGpuBuffer생성
		D3DGpuBuffer* CreateDefaultBuffer(const GRM::BufferDesc& bufferDesc);
		D3DGpuBuffer* CreateUploadBuffer(const GRM::BufferDesc& bufferDesc);

		//내부 텍스처 리소스생성
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const GRM::TextureDesc& textureDesc);

		void FlushCommandQueue();

		void CreateBufferDescriptor(D3DGpuBuffer* buffer, const GRM::BufferDesc& bufferDesc);
		
		void CreateTextureDescriptor(D3DGpuTexture* texture, const GRM::TextureDesc& textureDesc);



		void CreateRtvHandle(D3DGpuTexture* texture, const GRM::TextureDesc& textureDesc);
		void CreateSrvHandle(D3DGpuTexture* texture, const GRM::TextureDesc& textureDesc);
		void CreateDsvHandle(D3DGpuTexture* texture, const GRM::TextureDesc& textureDesc);

		

		D3DGpuTexture* GenerateTextureObject(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource);


		Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadBufferResource(const GRM::BufferDesc& bufferDesc);

		//uploadBuffer에 데이터를 업로드한다.
		bool UploadBufferData(Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer, void* data, size_t elementSize, size_t elementNum,
			size_t bufferOffset = 0);


		void ReleaseDescriptorHandle(ED3DResourceDescriptorType type, D3DDescriptorHandle handle);


		void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> textureResource,const GRM::TextureDesc & textureDesc,
			D3D12_RESOURCE_STATES preState);

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
		



		std::vector<D3DGpuResource*> mGarbageVector;




	};


}