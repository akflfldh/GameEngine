#include "Core/Font.h"
#include"ObjectFactory/TextFactory.h"
#include"ResourceManager/TextureManager/TextureManager.h"


Quad::Font::~Font()
{

	FT_Done_Face(mFT_Face);
}

int Quad::Font::GetFontSizeInPixel() const
{
	return mFontSizeInPixel;
}

Quad::Font::Font(const std::wstring& name,
	FT_Face ft_Face, int fontSizeInPixel)
	:mName(name),mFT_Face(ft_Face),mFontSizeInPixel(fontSizeInPixel)
{
	
	ft_Face->size;
	mHB_Face = hb_ft_face_create_referenced(mFT_Face);
	mHB_Font = hb_ft_font_create_referenced(mFT_Face);
	
	hb_font_extents_t fontExtents;
	hb_font_get_h_extents(mHB_Font, &fontExtents);

	unsigned int upem =  hb_face_get_upem(mHB_Face);
	
	mAscender =(float)fontExtents.ascender / upem * fontSizeInPixel;
	mDescender = (float)fontExtents.descender / upem * fontSizeInPixel;




	for (int i = 13; i <= 126; ++i)
	{
		FT_UInt glyphIndex = FT_Get_Char_Index(mFT_Face,i);
		FT_Load_Glyph(mFT_Face, glyphIndex, FT_LOAD_DEFAULT);

		if(mFT_Face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
			FT_Render_Glyph(mFT_Face->glyph, FT_RENDER_MODE_NORMAL);


		GlyphMetrics metrics;
		metrics.mWidth = mFT_Face->glyph->metrics.width /64;
		metrics.mHeight = mFT_Face->glyph->metrics.height /64;
		metrics.mHoriBearingX = mFT_Face->glyph->metrics.horiBearingX / 64;
		metrics.mHoriBearingY = mFT_Face->glyph->metrics.horiBearingY / 64;
		metrics.mHoriAdvance = mFT_Face->glyph->metrics.horiAdvance / 64;
		metrics.mVertBearingX = mFT_Face->glyph->metrics.vertBearingX / 64;
		metrics.mVertBearingY = mFT_Face->glyph->metrics.vertBearingY / 64;
		metrics.mVertAdvance = mFT_Face->glyph->metrics.vertAdvance / 64;
		//mGlyphMetricsTable	mFT_Face->glyph->metrics.
		mGlyphMetricsTable.insert({ glyphIndex,metrics });

		//여기도 textureManager에도 포인터가저장되어있고 삭제된다면 댕글링포인터의 문제는?
		//이벤트를 통해서 삭제할때마다 알리거나, 핸들을 사용하거나,등등
		
		
		if (i > 32)	//스페이스바
		{
			Texture* texture = CreateGlyphBitmap(mFT_Face->glyph);
			if (texture != nullptr)
			{
				//texture->SetEngineContentItemFlag(true);
				mGlyphTextureTable.insert({ glyphIndex,texture });
			}
		}

		if (i == 13)
		{
			i = 31;//엔터다음은 스페이스바
		}

	}

	//엔터, 스페이스바 
	mEnterGlyphIndex = FT_Get_Char_Index(mFT_Face, L'\r');
	mSpaceGlyphIndex = FT_Get_Char_Index(mFT_Face, L' ');






}



Quad::Texture* Quad::Font::CreateGlyphBitmap(FT_GlyphSlot glyph)
{
	Texture * texture = TextFactory::CreateGlyphBitmap(glyph->bitmap);
	if(texture!=nullptr)
		mGlyphTextureTable.insert({ glyph->glyph_index,texture });

	return texture;
}

Quad::Texture* Quad::Font::GetGlyphBitmap(FT_UInt glyphIndex)
{
	std::unordered_map<FT_UInt, Texture*>::iterator	 it = mGlyphTextureTable.find(glyphIndex);
	if (it == mGlyphTextureTable.end())
		return nullptr;
	
	return it->second;
}

FT_Face Quad::Font::GetFT_Face() const
{
	return mFT_Face;
}

hb_font_t* Quad::Font::GetHB_Font() const
{
	return mHB_Font;
}

const std::wstring& Quad::Font::GetName() const
{
	return mName;
	// TODO: 여기에 return 문을 삽입합니다.
}

int Quad::Font::GetFontSize() const
{
	return mFontSizeInPixel;
}

float Quad::Font::GetAscender() const
{
	return mAscender;
}

float Quad::Font::GetDescender() const
{
	return mDescender;
}

const Quad::GlyphMetrics* Quad::Font::GetGlyphMetrics(FT_UInt glyphIndex)
{

	std::unordered_map<FT_UInt, GlyphMetrics>::iterator	 it = mGlyphMetricsTable.find(glyphIndex);
	if (it == mGlyphMetricsTable.end())
		return nullptr;

	return &it->second;
}

void Quad::Font::AddGlyphMetrics(FT_UInt glyphIndex, const GlyphMetrics & glyphMetrics)
{
	mGlyphMetricsTable.insert({ glyphIndex,glyphMetrics });


}

int Quad::Font::GetEnterGlyphIndex() const
{
	return mEnterGlyphIndex;
}

int Quad::Font::GetSpaceGlyphIndex() const
{
	return mSpaceGlyphIndex;
}


