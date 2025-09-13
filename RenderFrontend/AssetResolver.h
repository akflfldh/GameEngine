#pragma once

#include <CoreAsset/AssetType.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <unordered_map>
namespace CoreAsset
{
class Asset;
class AssetManager;
class Material;
class Texture;

} // namespace CoreAsset

namespace GRM
{
class IGpuResourceManager;
}

namespace Render
{

class RENDER_FRONTEND_API AssetResolver
{
  public:
    static AssetResolver *GetInstance();
    AssetResolver();
    ~AssetResolver();

    void Initialize(CoreAsset::AssetManager *assetManager, GRM::IGpuResourceManager *gpuResourceManager);

    // asset의 rawData를 로드,gpuResource생성 을 처리해준다
    bool ResolveAsset(CoreAsset::Asset *asset);

    GRM::GRMPtr GetGpuResource(CoreAsset::Asset *asset) const;
    GRM::GRMPtr GetGpuResource(CoreAsset::AssetID assetID) const;

  private:
    // void ResolveMesh(CoreAsset::Mesh *mesh);
    bool ResolveMaterial(CoreAsset::Material *material);
    bool ResolveTexture(CoreAsset::Texture *texture);

    void RegisterGpuReosurce(CoreAsset::AssetID assetID, GRM::GRMPtr ptr);

  private:
    CoreAsset::AssetManager *mAssetManager;
    GRM::IGpuResourceManager *mGpuResourceManager;

    // asset id - gpuResource 테이블 필요
    // 드러나도 상관없을듯,내부에서만 사용하기에

    std::unordered_map<CoreAsset::AssetID, GRM::GRMPtr> mAssetGpuResourceTable;
};
} // namespace Render
