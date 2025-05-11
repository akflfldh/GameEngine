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
		

		static RenderTargetTexture* CreateRenderTargetTexture(int width, int height);
		static Texture* CreateDepthStencilBuffer(int width, int height);

		static void CreateSrvTex2D(Texture* texure);



		static void ReleaseTexture(Texture* texture);



	private:

		
		static D3D12_SRV_DIMENSION ConvertSRVDimensionEnum(D3D12_RESOURCE_DIMENSION dimension);
		static void FillSRVTEX2DDesc(D3D12_SHADER_RESOURCE_VIEW_DESC& oSrv, Microsoft::WRL::ComPtr<ID3D12Resource> resource);
		static ETextureFormat ConvertTextureFormatEnum(DXGI_FORMAT format);
		


		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;
		GraphicCommand* mGraphicsCommandObject;

		DestructingPoolAllocator<Texture>* mTextureAllocator;
		//UniqueIDTable<Texture> mIDTable;
	//	NameTable mNameTable;




		std::unordered_map<std::string ,std::function<void(Texture*)>> mDtorTable;





	};


}