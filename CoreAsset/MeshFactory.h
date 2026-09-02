#pragma once
#include <CoreAsset/IAssetFactory.h>
namespace CoreAsset
{
class Mesh;

class CORE_ASSET_API MeshFactory : public IAssetFactory
{
  public:
    static MeshFactory *GetInstance();
    MeshFactory();
    virtual ~MeshFactory();
    virtual Asset *CreateEmptyAsset(EAssetType assetType) override;
    virtual Asset *CreateAssetFromData(const IntermediateAsset &intermediateAsset) override;

  private:
    Mesh *CreateMesh(EAssetType type);

  private:
};

} // namespace CoreAsset