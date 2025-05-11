#pragma once




#include"Predeclare.h"
#include"header.h"
#include<ft2build.h>
#include"harfBuzzsrc/hb-ft.h"
#include"DescriptorHeapType.h"
#include<string>

#include FT_FREETYPE_H

#include"Core/CoreDllExport.h"
namespace Quad
{

	struct GlyphMetrics
	{
		float  mWidth;
		float  mHeight;

		float  mHoriBearingX;
		float  mHoriBearingY;
		float  mHoriAdvance;

		float  mVertBearingX;
		float  mVertBearingY;
		float  mVertAdvance;
	};

	class CORE_API_LIB Font
	{
		friend class TextFactory;
	public:
		~Font();
		int GetFontSizeInPixel() const;

		Texture* CreateGlyphBitmap(FT_GlyphSlot glyph);
		Texture* GetGlyphBitmap(FT_UInt glyphIndex);

		FT_Face GetFT_Face() const;
		hb_font_t* GetHB_Font() const;


		const std::wstring& GetName() const;
		int GetFontSize() const;

		float GetAscender() const;
		float GetDescender() const;


		const GlyphMetrics* GetGlyphMetrics(FT_UInt glyphIndex);
		void AddGlyphMetrics(FT_UInt glyphIndex, const GlyphMetrics& glyphMetrics);

		int GetEnterGlyphIndex()const;
		int GetSpaceGlyphIndex()const;

	private:
		Font(const std::wstring  & name, FT_Face ft_Face,int fontSizeInPixel);


	private:
		std::wstring mName;
		FT_Face mFT_Face;
		hb_font_t* mHB_Font;
		hb_face_t* mHB_Face;
		int mFontSizeInPixel;

		std::unordered_map<FT_UInt, Texture *> mGlyphTextureTable;
		std::unordered_map<FT_UInt, GlyphMetrics > mGlyphMetricsTable;

		float mAscender;
		float mDescender;



		int mEnterGlyphIndex;
		int mSpaceGlyphIndex;
		
	};


}

