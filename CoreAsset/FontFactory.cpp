#include "FontFactory.h"
#include <CoreAsset/Font.h>
#include <CoreAsset/IntermediateAsset.h>

CoreAsset::FontFactory *CoreAsset::FontFactory::GetInstance()
{

    static FontFactory instance;
    return &instance;
}

CoreAsset::Asset *CoreAsset::FontFactory::CreateEmptyAsset(EAssetType assetType)
{

    return new Font;
}

CoreAsset::Asset *CoreAsset::FontFactory::CreateAssetFromData(const IntermediateAsset &intermediateAsset)
{

    const IntermediateFont &intermediateFont = static_cast<const IntermediateFont &>(intermediateAsset);

    Font *font = new Font;

    font->RegisterFontList(intermediateFont.mGlyphVector);
    font->SetFontMatrix(intermediateFont.mFontMatrix);
    font->SetFontAltas(intermediateFont.mFontAltas);
    font->SetGlyphAltas(intermediateFont.mGlyphAltasID);

    return font;
}
