#include "Font.h"

CoreAsset::Font::Font() : Asset(CoreAsset::EAssetType::eFont) {}

CoreAsset::Font::~Font() {}

void CoreAsset::Font::RegisterFontGlyph(uint32_t unicode, const FontGlyph &glyph)
{

    mFontGlyphTable[unicode] = glyph;
}

void CoreAsset::Font::RegisterFontList(const std::vector<FontGlyph> &fontGlyphList)
{

    for (const auto &fontGlyph : fontGlyphList)
    {
        RegisterFontGlyph(fontGlyph.mUnicode, fontGlyph);
    }
}

void CoreAsset::Font::SetFontMatrix(const FontMatrix &fontMatrix)
{

    mFontMatrix = fontMatrix;
}

void CoreAsset::Font::SetFontAltas(const FontAltas &fontAltas)
{
    mFontAltas = fontAltas;
}

void CoreAsset::Font::SetGlyphAltas(CoreAsset::AssetID id)
{
    mGlyphAltas.SetAsset(id);
}

const CoreAsset::FontGlyph *CoreAsset::Font::GetGlyph(uint32_t unicode) const
{
    auto it = mFontGlyphTable.find(unicode);
    if (it == mFontGlyphTable.cend())
    {
        return nullptr;
    }

    return &it->second;
}

CoreAsset::AssetPtr CoreAsset::Font::GetGlyphAltas() const
{
    return mGlyphAltas;
}

const CoreAsset::FontAltas &CoreAsset::Font::GetFontAltas() const
{

    return mFontAltas;
}

const CoreAsset::FontMatrix &CoreAsset::Font::GetFontMatrix() const
{

    return mFontMatrix;
}