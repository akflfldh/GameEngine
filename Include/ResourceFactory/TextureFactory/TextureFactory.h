#pragma once

#include"Predeclare.h"

#include"header.h"
#include"Utility/Singleton.h"
//#include"Texture.h"
#include"Utility/NameTable.h"
#include"Allocator/DestructingPoolAllocator.h"
#include<functional>
#include"Asset/Texture/TextureFormat.h"

#include"Core/CoreDllExport.h"



namespace Quad
{

	class CORE_API_LIB TextureFactory:public Singleton<TextureFactory>
	{
		friend class TextureManager;
		friend class ResourceStorer;
	public:
		TextureFactory();
		~TextureFactory() = default;

		
		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device>& device, DescriptorHeapManagerMaster* descriptorHeapManagerMaster, GraphicCommand* graphicsCommandObject);

	
		static Texture* CreateTexture();		//처음생성하면 Mesh에 아무것도 설정되지않을상태
		

		//실제 리소스생성
		static Texture* CreateTexture(int width, int height, ETextureFormat textureFormat =ETextureFormat::DXGI_FORMAT_R8G8B8A8_UNORM);

		//실제 리소스생성
		static RenderTargetTexture* CreateRenderTargetTexture(int width, int height);
		


		static Texture* CreateDepthStencilBuffer(int width, int height);


		//static void CreateTextureResource(Texture * texture ,int width, int height);
		static void CreateRenderTargetTextureResource(RenderTargetTexture* renderTargetTexture, int width, int height);
		static void CreateDepthStencilBufferResource(Texture* depthStencilBuffer, int width, int height);



		static void CreateSrvTex2D(Texture* texure);



		static void ReleaseTexture(Texture* texture);



		//texture의 내부 리소스,view만 제거한다.
		static void ReleaseTextureResource(Texture* texture);


	private:

		
		static D3D12_SRV_DIMENSION ConvertSRVDimensionEnum(D3D12_RESOURCE_DIMENSION dimension);
		static void FillSRVTEX2DDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& oSrv, Microsoft::WRL::ComPtr<ID3D12Resource> resource);
		static EColorFormat ConvertTextureFormatEnum(DXGI_FORMAT format);
		


		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;
		GraphicCommand* mGraphicsCommandObject;

		DestructingPoolAllocator<Texture>* mTextureAllocator;
		//UniqueIDTable<Texture> mIDTable;
	//	NameTable mNameTable;




		std::unordered_map<ETextureType ,std::function<void(Texture*)>> mDtorTable;





	};


}