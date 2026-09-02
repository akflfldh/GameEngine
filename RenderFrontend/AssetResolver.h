#pragma once

#include <CoreAsset/AssetType.h>
#include <D3DGpuResourceManager/GRMPtr.h>
#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <RenderSystem/MaterialType.h>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

namespace CoreAsset
{
class Asset;
class AssetManager;
class Material;
class Texture;
class Mesh;
class StaticMesh;

} // namespace CoreAsset

namespace GRM
{
class IGpuResourceManager;
}

namespace Render
{

struct MeshGpuResourceContext
{

    GRM::GRMPtr mIndexBuffer;
    GRM::GRMPtr mVertexBuffer;
};

constexpr const char *OutlineStencilMat = "OutlineStencilMat";
constexpr const char *OutlineDrawMat = "OutlineDrawMat";
constexpr const char *GrayScaleMat = "GrayScaleMat";
constexpr const char *DebugGridMat = "DebugGridMat";
constexpr const char *BillboardMat = "BillboardMat";
constexpr const char *DebugLineMat = "DebugLineMat";

class RENDER_FRONTEND_API AssetResolver
{
  public:
    static AssetResolver *GetInstance();
    AssetResolver();
    ~AssetResolver();

    void Initialize(CoreAsset::AssetManager *assetManager, GRM::IGpuResourceManager *gpuResourceManager);

    bool RequestResolveAsset(CoreAsset::Asset *asset);

    GRM::GRMPtr GetGpuResource(CoreAsset::Asset *asset) const;
    // GRM::GRMPtr GetGpuResource(CoreAsset::AssetID assetID) const;
    MeshGpuResourceContext GetMeshGpuResourceContext(CoreAsset::Asset *asset) const;
    MeshGpuResourceContext GetMeshGpuResourceContext(CoreAsset::AssetID assetID) const;

    void SetDefaultTexture(CoreAsset::Asset *asset);

    void EndResourceResolveThread();

    Render::MaterialID GetPassGpuMaterialID(const std::string &name);

    int GetLightStructuredGpuBufferID() const;
    int GetBillboardStructuredGpuBufferID() const;

    GRM::GRMPtr GetDefaultTexture() const;

    // 호출 스레드가 blocking 된다.
    //  asset의 rawData를 로드,gpuResource생성 을 처리해준다
    bool ResolveAsset(CoreAsset::Asset *asset) const;
    bool ResolveAsset(CoreAsset::AssetID asset) const;

  private:
    // void ResolveMesh(CoreAsset::Mesh *mesh);
    bool ResolveMaterial(CoreAsset::Material *material) const;
    bool ResolveTexture(CoreAsset::Texture *texture) const;
    bool ResolveStaticMesh(CoreAsset::StaticMesh *staticMesh) const;
    bool ResolveSkinningMesh(CoreAsset::Mesh *mesh) const;

    void RegisterGpuReosurce(CoreAsset::AssetID assetID, GRM::GRMPtr ptr) const;
    void RegisterMeshGpuResourceContext(CoreAsset::AssetID, MeshGpuResourceContext context) const;

    void WokerThreadLoop();
    void PushResolveRequest(CoreAsset::Asset *asset);

    void RegisterBuiltInAsset();
    void BuildBillboardGpuMaterial();
    void BuildDebugLineGpuMaterial();
    void BuildUIGpuMaterial();
    void BuildGpuBuffers();

  private:
    CoreAsset::AssetManager *mAssetManager;
    GRM::IGpuResourceManager *mGpuResourceManager;

    // asset id - gpuResource 테이블 필요
    // 드러나도 상관없을듯,내부에서만 사용하기에

    mutable std::unordered_map<CoreAsset::AssetID, GRM::GRMPtr> mAssetGpuResourceTable;
    mutable std::unordered_map<CoreAsset::AssetID, MeshGpuResourceContext> mMeshGpuResourceTable;

    std::queue<CoreAsset::Asset *> mRequestQueue;

    std::thread mWorkerThread;
    std::mutex mRequestQueueMutex;
    std::condition_variable mCV;

    bool mIsWorkerRunning;

    mutable std::shared_mutex mAssetTableMutex;

    // TODO Pass용 GPU머터리얼 ID 테이블
    std::unordered_map<std::string, Render::MaterialID> mPassGpuMaterialTable;

    int mLightGpuBufferContextID = 4;
    int mBillboardGpuBufferContextID = 5;
};
} // namespace Render
