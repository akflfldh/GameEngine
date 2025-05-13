#pragma once



#include"header.h"
#include<Shlwapi.h>

//#include"Type.h"
#include"Utility.h"
//#include"Texture.h"
#include"Predeclare.h"
#include"WICTextureLoader12.h"
#include"DDSTextureLoader12.h"
#include"Core/CoreDllExport.h"
#include"Utility/Singleton.h"
namespace Quad
{
	class CORE_API_LIB TextureLoader:public Singleton<TextureLoader>
	{
	public:
		TextureLoader() = default;
		~TextureLoader() = default;

		void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device,
			GraphicCommand* graphicsCommandObject);

		bool LoadTextureFile(const std::string & filePath, Texture& oTexture);
		bool LoadDDSFileFromMemory(uint8_t * blob, size_t size , Texture & oTexture);



		void ReleaseUploadBuffer();

		void test();

	private:
		void UploadTexture(Microsoft::WRL::ComPtr<ID3D12Resource> & defaultTexture,
		std::vector<D3D12_SUBRESOURCE_DATA> &  subresourceData,
			Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);




	private:
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mUploadBuffers;

	private:
		GraphicCommand* mGraphicsCommandObject;
		//2D만 지원
		Microsoft::WRL::ComPtr<ID3D12Resource> LoadTextureFromDDS(const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

		//2D
		Microsoft::WRL::ComPtr<ID3D12Resource> LoadTextureFromBITMAP(const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);



	};

}