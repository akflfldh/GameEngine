#pragma once

#include <CoreAsset/CoreAssetDLLMacro.h>
#include <CoreAsset/IAssetFactory.h>

namespace CoreAsset
{
class Texture;
struct IntermediateTexture;
class CORE_ASSET_API TextureFactory : public IAssetFactory
{
  public:
    static TextureFactory *GetInstance();
    TextureFactory();
    virtual ~TextureFactory();

    virtual Asset *CreateEmptyAsset(EAssetType assetType) override;
    virtual Asset *CreateAssetFromData(const IntermediateAsset &intermediateAsset) override;

  private:
    Texture *CreateTexture();

    void SetPropertyMetaData(Texture *texture, const IntermediateTexture &intermediateTexture);
};

} // namespace CoreAsset