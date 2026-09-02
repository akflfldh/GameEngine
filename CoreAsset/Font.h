#pragma once
#include <CoreAsset/Asset.h>
#include <CoreAsset/AssetPtr.h>
#include <CoreAsset/AssetType.h>
#include <unordered_map>
#include <vector>
namespace CoreAsset
{

class CORE_ASSET_API Font : public Asset
{
    friend class FontFactory;

  public:
    Font();
    ~Font();

    const FontGlyph *GetGlyph(uint32_t unicode) const;

    AssetPtr GetGlyphAltas() const;

    const FontAltas &GetFontAltas() const;
    const FontMatrix &GetFontMatrix() const;

  private:
    void RegisterFontGlyph(uint32_t unicode, const FontGlyph &glyph);
    void RegisterFontList(const std::vector<FontGlyph> &fontGlyphList);
    void SetFontMatrix(const FontMatrix &fontMatrix);
    void SetFontAltas(const FontAltas &fontAltas);
    void SetGlyphAltas(CoreAsset::AssetID id);
    // 각 문자들에대한 metaData Table

    //<unicode value , FontGlyph>
    std::unordered_map<uint32_t, FontGlyph> mFontGlyphTable;
    FontMatrix mFontMatrix;
    FontAltas mFontAltas;
    AssetPtr mGlyphAltas;
};

AssetClassName(Font)
} // namespace CoreAsset
