#pragma once



#include"Predeclare.h"
#include<Shlwapi.h>
#include"header.h"
#include"Utility/Singleton.h"
#include<string>
#include<vector>

#include<ft2build.h>
#include"harfBuzzsrc/hb.h"
#include"DescriptorHeapType.h"
#include"Allocator/DestructingPoolAllocator.h"
#include"TextCharacterPanelEntity.h"

#include FT_FREETYPE_H
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB TextFactory:public Singleton<TextFactory>
	{
	public:
		TextFactory(int characterMaxNum=100000);
		~TextFactory();
		
		bool CreateNewFT_Face(FT_Face& oFT_Face, 
			FT_Byte* pData,FT_Long size);

		void ReadFontFile(const std::wstring& fontFIlePath);

		void initialize(
			Microsoft::WRL::ComPtr<ID3D12Device> device,
			GraphicCommand* graphicCommandObject, TextureManager* textureManager,
			DescriptorHeapManagerMaster* descriptorHeapManagerMaster);

		static Texture* CreateGlyphBitmap(FT_Bitmap bitmap);

		static Font* GetFont(std::wstring fontName, int sizeNum);		//0 :12 , 1: 24 , 2: 48 , 3: 72		
		static UINT GetSystemDPI();



		static	TextCharacterPanelEntity* GetTextCharacterPanelEntity();
		static void ReleaseTextCharacterPanelEntity(TextCharacterPanelEntity* ob);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(FT_Bitmap bitmap,
			Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffe);
		ViewIndex CreateSrv(Microsoft::WRL::ComPtr<ID3D12Resource> texResource);


	private:
		static UINT mSystemDPI;

		TextureManager* mTextureManager;
		Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
		GraphicCommand* mGraphicCommandObject;
		DescriptorHeapManagerMaster* mDescriptorHeapManagerMaster;
		static FT_Library mFT_Library;
		static hb_font_t* mHB_Font;

		std::unordered_map<std::wstring, std::vector<Font*>> mFontTable;
		DestructingPoolAllocator<TextCharacterPanelEntity> mTextCharacterPanelEntityPoolAllocator;
	};

}

