#include "Font.h"

CoreAsset::Font::Font() : Asset(CoreAsset::EAssetType::eFont) {}

CoreAsset::Font::~Font() {}

bool CoreAsset::Font::CopyDataFrom(const Asset &source, std::string *failureReason)
{
    const Font *sourceFont = dynamic_cast<const Font *>(&source);
    if (!sourceFont)
    {
        if (failureReason)
            *failureReason = "Font 에셋이 필요합니다.";
        return false;
    }

    // glyph/metrics는 독립 복사하지만 atlas texture는 별도 에셋이므로 동일 참조를 유지한다.
    mFontGlyphTable = sourceFont->mFontGlyphTable;
    mFontMatrix = sourceFont->mFontMatrix;
    mFontAltas = sourceFont->mFontAltas;
    mGlyphAltas = sourceFont->mGlyphAltas;
    return Asset::CopyDataFrom(source, failureReason);
}

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
