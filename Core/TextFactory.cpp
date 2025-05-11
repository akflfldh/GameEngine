#include "ObjectFactory/TextFactory.h"
#include"harfBuzzsrc/hb-ft.h"
//#include"Character.h"
#include"ResourceManager/MeshManager.h"
#include"ResourceManager/TextureManager/TextureManager.h"
#include<string>
#include"Asset/Texture/Texture.h"
#include"GraphicCommand.h"
#include"HeapManager/DescriptorHeapManagerMaster.h"
//#include"Text.h"
//#include"TextCursor.h"
#include"Font.h"

#include"Object/RegisterObjectHelper.h"
namespace Quad
{
	FT_Library TextFactory::mFT_Library;
	hb_font_t* TextFactory::mHB_Font;
	UINT TextFactory::mSystemDPI = GetDpiForSystem();

	TextFactory::TextFactory(int characterMaxNum)
		:mTextCharacterPanelEntityPoolAllocator(characterMaxNum)
		
	{
		RegisterObjectHelper textCtorDtor("TextCharacterPanelEntity", []()->Object* {

			Object* object = TextFactory::GetTextCharacterPanelEntity();
			return object;},
			[](Object* ob) {
				TextFactory::ReleaseTextCharacterPanelEntity((TextCharacterPanelEntity*)ob);
			});







	}

	TextFactory::~TextFactory()
	{
		for (auto it = mFontTable.begin(); it != mFontTable.end(); ++it)
		{
			std::vector<Font*>& fontVector = it->second;
			for (int i = 0; i < fontVector.size(); ++i)
				delete fontVector[i];
		}



	}

	bool TextFactory::CreateNewFT_Face(FT_Face & oFT_Face , FT_Byte * pData,
		FT_Long size)
	{
			FT_Error error =FT_New_Memory_Face(mFT_Library, pData, size, 0, &oFT_Face);

			if (!FT_HAS_VERTICAL(oFT_Face))
			{
				OutputDebugString(L"Font가 vertical text layout을 지원하지않는다(한글을 지원하지않는다)\n");
			}



			if (error == FT_Err_Unknown_File_Format)
			{
				MessageBox(nullptr, L"FontFile의 font를 알수없다.", L"error", 0);
				return false;
			}
			else if (error)
			{
				MessageBox(nullptr, L"Face 오브젝트 생성중 에러발생", L"error", 0);
				return false;
			}
			return true;
		
		return false;
	}

	void TextFactory::ReadFontFile(const std::wstring& fontFilePath)
	{

		std::ifstream fin(fontFilePath, std::ios_base::binary);
		if (!fin.is_open())
		{
			MessageBox(nullptr, L"font file open error", L"error", 0);
			return;
		}

		fin.seekg(0, std::ios_base::end);
		std::streampos end = fin.tellg();
		fin.seekg(std::ios_base::beg);
		UINT fileSize = end - fin.tellg();
		FT_Byte* buffer = new FT_Byte[fileSize];
		fin.read((char*)buffer, (fileSize));

		FT_Face ft_face12,ft_face24,ft_face48,ft_face72;
		CreateNewFT_Face(ft_face12,buffer, fileSize);
		CreateNewFT_Face(ft_face24,buffer,fileSize);
		CreateNewFT_Face(ft_face48,buffer,fileSize);
		CreateNewFT_Face(ft_face72,buffer,fileSize);

		FT_Set_Pixel_Sizes(ft_face12, ((float)12 / 72 ) *  mSystemDPI  , 0);
		FT_Set_Pixel_Sizes(ft_face24, ((float)24 / 72) * mSystemDPI, 0);
		FT_Set_Pixel_Sizes(ft_face48, ((float)48 / 72) * mSystemDPI, 0);
		FT_Set_Pixel_Sizes(ft_face72, ((float)72 / 72) * mSystemDPI, 0);
		

		std::wstring  fontName = PathFindFileName(fontFilePath.c_str());

		Font* font12 = new Font(fontName,ft_face12, ((float)12 / 72) * mSystemDPI);
		Font* font24 = new Font(fontName,ft_face24, ((float)24 / 72) * mSystemDPI);
		Font* font48 = new Font(fontName,ft_face48, ((float)48 / 72) * mSystemDPI);
		Font* font72 = new Font(fontName,ft_face72, ((float)72 / 72) * mSystemDPI);

		std::vector<Font*> fontVector(4,nullptr);
		fontVector[0] = font12;
		fontVector[1] = font24;
		fontVector[2] = font48;
		fontVector[3] = font72;


		TextFactory * factory 	=  GetInstance();
		std::pair<std::unordered_map<std::wstring,std::vector<Font*>>::iterator,bool> ret =
			factory->mFontTable.try_emplace(fontName, std::move(fontVector));


	}

	void Quad::TextFactory::initialize(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		GraphicCommand* graphicCommandObject, TextureManager* textureManager,
		DescriptorHeapManagerMaster* descriptorHeapManagerMaster)
	{
		mDevice = device;
		mTextureManager = textureManager;
		mGraphicCommandObject = graphicCommandObject;
		mDescriptorHeapManagerMaster = descriptorHeapManagerMaster;
		
		FT_Error error = FT_Init_FreeType(&mFT_Library);
		if (error)
		{
			MessageBox(nullptr, L"FT_Libaray초기화중 에러발생", L"error", 0);
			return;
		}
	
		mTextCharacterPanelEntityPoolAllocator.Initialize();
		
		//error = FT_Set_Pixel_Sizes(
		//	mFT_Face,    /* face 객체 핸들 */
		//	0,       /* pixel_width */
		//	72);    /* pixel_height */
		//72pt , 1인치 == 72pt , 1인치 == 72픽셀 따라서   
		//72포인트크기인데 그거는 72픽셀이다 -> 72픽셀을 월드공간(직교투영)72유닛으로1:1대응한다.
		//문자 rect의 가로 세로 너비를 72유닛으로 잡으면된다.
		//그리고 당연히 존재해야하는 위치도 픽셀==유닛 1:1이니 그대로 지정하면된다.	

	}

	Quad::Texture* Quad::TextFactory::CreateGlyphBitmap(FT_Bitmap bitmap)
	{
		TextFactory * instance = GetInstance();

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBufferVector;
		if(!instance->mGraphicCommandObject->GetCloseState())
		{
			instance->mGraphicCommandObject->ExecuteCommandList();
		}

		instance->mGraphicCommandObject->ResetCommandList(nullptr);

		static uint64_t nameNum = 0;

		std::string texName = "glyph_" + std::to_string(nameNum);


		//얘가 texture manger에서 관리가되어야 이 텍스처가 제거될때 올바르게 srv인덱스도반환하는등의 적절한 처리가
		//수행될수있다.
		//Texture* glyphTexture =	TextureFactory::CreateTexture(texName);
		Texture* glyphTexture = TextureManager::CreateTexture(texName);
		if (glyphTexture == nullptr)
		{
			MessageBox(nullptr, L"글리프텍스트Texture이름중복", L"error", 0);
			return nullptr;
		}
		nameNum++;

		Microsoft::WRL::ComPtr<ID3D12Resource> texResource = instance->CreateTextureResource(bitmap, uploadBufferVector);


		instance->mGraphicCommandObject->ExecuteCommandList();
		instance->mGraphicCommandObject->FlushCommandQueue();

		glyphTexture->SetTextureResource(texResource);
		ViewIndex index = instance->CreateSrv(texResource);
		//viewIndex가실패했다면 텍스처를 없애야한다.
		if (index != -1)
		{
			glyphTexture->SetViewIndex(index);
			glyphTexture->SetEngineContentItemFlag(true);
			return glyphTexture;
		}
		else
		{
			//텍스처 리소스 제거
			
		}
		




		return nullptr;
	}

	Font* TextFactory::GetFont(std::wstring fontName, int sizeNum)
	{
		TextFactory * instance = 	GetInstance();
		std::unordered_map<std::wstring,std::vector<Font*>> ::iterator it =	instance->mFontTable.find(fontName);
		if (it == instance->mFontTable.end() || (sizeNum<0) || (sizeNum >3 ))
		{
			return nullptr;
		}

		return	it->second[sizeNum];
		
	}

	UINT TextFactory::GetSystemDPI()
	{

		return mSystemDPI;
	}

	TextCharacterPanelEntity* TextFactory::GetTextCharacterPanelEntity()
	{
		auto instance = GetInstance();
		TextCharacterPanelEntity* newInstance = instance->mTextCharacterPanelEntityPoolAllocator.GetInstance();
		return newInstance;
	}

	void TextFactory::ReleaseTextCharacterPanelEntity(TextCharacterPanelEntity* ob)
	{
		auto instance = GetInstance();
		instance->mTextCharacterPanelEntityPoolAllocator.ReleaseInstance(ob);
		return;
	}


	Microsoft::WRL::ComPtr<ID3D12Resource> TextFactory::CreateTextureResource(FT_Bitmap bitmap,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> texResource;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommmandList = mGraphicCommandObject->GetGraphicsCommandList();
		//texture
		D3D12_RESOURCE_DESC texDesc;
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = bitmap.width;
		texDesc.Height = bitmap.rows;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES uploadHeapPropertiesTex;
		uploadHeapPropertiesTex.Type = D3D12_HEAP_TYPE_DEFAULT;
		uploadHeapPropertiesTex.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapPropertiesTex.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapPropertiesTex.CreationNodeMask = 0;
		uploadHeapPropertiesTex.VisibleNodeMask = 0;


		ThrowIfFailed(mDevice->CreateCommittedResource(&uploadHeapPropertiesTex, D3D12_HEAP_FLAG_NONE,
			&texDesc, D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr, IID_PPV_ARGS(texResource.GetAddressOf())));




		D3D12_PLACED_SUBRESOURCE_FOOTPRINT subFootPrint;
		UINT numRow;
		UINT64 rowSizeInByte;
		UINT64 totalByte;
		mDevice->GetCopyableFootprints(
			&texDesc, 0, 1, 0, &subFootPrint,
			&numRow, &rowSizeInByte, &totalByte);




		//default heap의  texture로 업로드



		//uploadbuffer만들기


		D3D12_RESOURCE_DESC uploadBufferDesc;
		uploadBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadBufferDesc.Alignment = 0;
		uploadBufferDesc.Width = totalByte;// bitmap.width* bitmap.rows;// bitmap.width * bitmap.rows;
		uploadBufferDesc.Height = 1;
		uploadBufferDesc.DepthOrArraySize = 1;
		uploadBufferDesc.MipLevels = 1;
		uploadBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadBufferDesc.SampleDesc.Count = 1;
		uploadBufferDesc.SampleDesc.Quality = 0;
		uploadBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadBufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES uploadHeapProperties;
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapProperties.CreationNodeMask = 0;
		uploadHeapProperties.VisibleNodeMask = 0;


		ThrowIfFailed(mDevice->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


		//uploadbuffer로 복사
		D3D12_RANGE range{ 0,0 };
		uint8_t* pData = nullptr;
		//uploadBuffer->Map(0, &range, (void**)&pData);
		uploadBuffer->Map(0, nullptr, (void**)&pData);

		// memset(pData, 255, bitmap.pitch * bitmap.rows);

		for (int row = 0; row < bitmap.rows; ++row)
		{
			memcpy(&pData[subFootPrint.Footprint.RowPitch * row], &bitmap.buffer[bitmap.width * row], bitmap.width);
		}






		uploadBuffer->Unmap(0, nullptr);

		D3D12_RESOURCE_BARRIER uploadBufferTransferState;
		uploadBufferTransferState.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		uploadBufferTransferState.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		uploadBufferTransferState.Transition.pResource = uploadBuffer.Get();
		uploadBufferTransferState.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		uploadBufferTransferState.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		uploadBufferTransferState.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		graphicsCommmandList->ResourceBarrier(1, &uploadBufferTransferState);

		//texture resource 생성



	/*	D3D12_SUBRESOURCE_FOOTPRINT uploadBufferFootprint;
		uploadBufferFootprint.Format = DXGI_FORMAT_R8_UNORM;
		uploadBufferFootprint.Depth = 1;
		uploadBufferFootprint.Height = bitmap.rows;
		uploadBufferFootprint.Width = bitmap.width;
		uploadBufferFootprint.RowPitch = 256;
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT uploadbufferPlacedFootPrint;
		uploadbufferPlacedFootPrint.Offset = 0;
		uploadbufferPlacedFootPrint.Footprint = uploadBufferFootprint;*/

		D3D12_TEXTURE_COPY_LOCATION src{ uploadBuffer.Get(),D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT ,subFootPrint };

		D3D12_TEXTURE_COPY_LOCATION dst{ texResource.Get(),D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,(UINT64)0 };

		graphicsCommmandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

		// mGraphicscommandList->CopyResource(mTextTexture.Get(), uploadBuffer.Get());


		D3D12_RESOURCE_BARRIER texTransferState;
		texTransferState.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		texTransferState.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		texTransferState.Transition.pResource = texResource.Get();
		texTransferState.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		texTransferState.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		texTransferState.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		graphicsCommmandList->ResourceBarrier(1, &texTransferState);


		return texResource;


	}

	ViewIndex TextFactory::CreateSrv(Microsoft::WRL::ComPtr<ID3D12Resource> texResource)
	{

		D3D12_SHADER_RESOURCE_VIEW_DESC srv;
		srv.Format = DXGI_FORMAT_R8_UNORM;
		srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
			D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0);

		srv.Texture2D.MipLevels = 1;
		srv.Texture2D.MostDetailedMip = 0;
		srv.Texture2D.ResourceMinLODClamp = 0;
		srv.Texture2D.PlaneSlice = 0;


		return mDescriptorHeapManagerMaster->CreateSrv(texResource, &srv);
	}



}