#pragma once

#include <RenderFrontend/RenderFrontendType.h>
#include <RenderSystem/RenderType.h>
#include <Utility/Utility.h>
#include <unordered_map>
namespace Render
{
class IMaterialManager;

struct RenderMaterialVariantKey
{

    ERenderPassType mRenderPassType;
    RenderMaterialContext mRenderMaterialContext;

    bool operator==(const RenderMaterialVariantKey &other) const
    {
        return mRenderPassType == other.mRenderPassType && mRenderMaterialContext == other.mRenderMaterialContext;
    }
};

struct RenderMaterialVariantKeyHash
{
    size_t operator()(const RenderMaterialVariantKey &key) const
    {
        size_t hash = 0xcbf29ce484222325;
        size_t prime = 0x00000100000001b3;

        hash = CoreUtility::HashValue(hash, key.mRenderPassType);
        hash = CoreUtility::HashValue(hash, key.mRenderMaterialContext.mGeometryType);
        hash = CoreUtility::HashValue(hash, key.mRenderMaterialContext.mTransparent);

        return hash;
    }
};

class RenderMaterialResolver
{
  public:
    static RenderMaterialResolver *GetInstance();
    RenderMaterialResolver();
    ~RenderMaterialResolver();

    void Initialize();

    MaterialID Resolve(const RenderMaterialContext &renderMaterialContext, ERenderPassType passType);

    void RegisterGpuMaterial(const RenderMaterialContext &renderMaterialContext, ERenderPassType passType,
                             MaterialID id);

  private:
    MaterialID GetGpuMaterialID(const RenderMaterialVariantKey &key) const;

    void BuildStaticMeshOpaqueGpuMaterial();

    void BuildStaticMeshOutlineWriteStencilGpuMaterial();
    void BuildStaticMeshOutlineDrawGpuMaterial();
    void BuildGrayScaleGpuMaterial();
    void BuildDebugGridGpuMaterial();
    void BuildBillboardGpuMaterial();
    void BuildDebugLineGpuMaterial();
    void BuildUIGpuMaterial();
    void BuildSkySphereGpuMaterial();
    // std::unordered_map<std::string, MaterialID> mGpuMaterialIDTable;
    std::unordered_map<RenderMaterialVariantKey, MaterialID, RenderMaterialVariantKeyHash> mGpuMaterialIDTable;
    // pass -

  private:
    IMaterialManager *mGpuMaterialManager = nullptr;
    bool mInitialized = false;
};

} // namespace Render