#pragma once

#include <RenderFrontend/RenderFrontendDllMarco.h>
#include <RenderFrontend/RenderFrontendType.h>
#include <RenderSystem/RenderType.h>
#include <Utility/Utility.h>
#include <optional>
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
        hash = CoreUtility::HashValue(hash, key.mRenderMaterialContext.mShadingModel);
        hash = CoreUtility::HashValue(hash, key.mRenderMaterialContext.mTransparent);

        return hash;
    }
};

using VariantKeyTable = std::unordered_map<RenderMaterialVariantKey, MaterialID, RenderMaterialVariantKeyHash>;

class RENDER_FRONTEND_API RenderMaterialResolver
{
  public:
    static RenderMaterialResolver *GetInstance();
    RenderMaterialResolver();
    ~RenderMaterialResolver();

    void Initialize();

    MaterialID Resolve(CoreAsset::AssetID materialAssetID, const RenderMaterialContext &context,
                       ERenderPassType passType);
    MaterialID Resolve(const RenderMaterialContext &renderMaterialContext, ERenderPassType passType);

    void RegisterGpuMaterial(const RenderMaterialContext &renderMaterialContext, ERenderPassType passType,
                             MaterialID id);

    // 특수하게 asset mat id 와 매핑되는 gpu mat (일반적으로  editor 전용 오브젝트들을 위해서 )
    void RegisterAssetMaterialOverride(CoreAsset::AssetID materialAssetID, const RenderMaterialVariantKey &variantKey,
                                       MaterialID gpuMaterialID);

    void UnregisterAssetMaterialOverrides(CoreAsset::AssetID materialAssetID);

    // gpu material 생성,등록후 asset mat 과 매핑
    MaterialID CreateAndRegisterAssetMaterialOverride(CoreAsset::AssetID materialAssetID,
                                                      const RenderMaterialContext &context, ERenderPassType passType,
                                                      const MaterialGenerationInfo &generationInfo);

    MaterialID ResolveSystemGpuMaterial(ESystemMaterialRole role) const;

  private:
    std::optional<MaterialID> FindAssetMaterialOverride(CoreAsset::AssetID materialAssetID,
                                                        const RenderMaterialVariantKey &variantKey) const;

    // gpu mat request 생성
    MaterialID CreateGpuMaterial(const MaterialGenerationInfo &info);

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
    void BuildShadowGpuMaterial();
    void BuildToneMappingGpuMaterial();
    void BuildBloomGpuMaterial();

    void RegisterSystemGpuMaterial(ESystemMaterialRole role, MaterialID gpuMaterialID);

    // std::unordered_map<std::string, MaterialID> mGpuMaterialIDTable;
    std::unordered_map<RenderMaterialVariantKey, MaterialID, RenderMaterialVariantKeyHash> mGpuMaterialIDTable;

    // editor 전용 key 테이블
    std::unordered_map<CoreAsset::AssetID, VariantKeyTable> mOverrideGpuMaterialKeyTable;

    // system 전용 gpu mat 테이블 (어떤 렌더항목이든 동일한 gpu mat 사용 하는경우)
    std::unordered_map<ESystemMaterialRole, MaterialID> mSystemGpuMaterialTable;

  private:
    IMaterialManager *mGpuMaterialManager = nullptr;
    bool mInitialized = false;
};

} // namespace Render
