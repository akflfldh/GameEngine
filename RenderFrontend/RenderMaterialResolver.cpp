#include "RenderMaterialResolver.h"
#include <Logger/Logger.h>
#include <RenderFrontend/ShaderSourceLibrary.h>
#include <RenderSystem/IMaterialManager.h>
#include <initializer_list>
#include <utility>

namespace
{
// Bloom의 Compute 입력 전환은 이번 작업에서 보류한다.
// TODO: Compute 바이트코드 입력을 연결할 때 이 기존 소스 조회 경로도 함께 제거한다.
bool GetShaderSource(Render::ShaderSourceLibrary *library, const char *fileName, std::vector<uint8_t> &buffer)
{
    // if (!library || !library->GetShaderSource(fileName, buffer) || buffer.empty())
    //{
    //     LOG_MESSAGE_ERROR("RenderMaterialResolver", std::string("HLSL 소스를 조회할 수 없거나 비어 있습니다: ") +
    //     fileName); return false;
    // }

    //// ShaderSourceInfo는 바이트를 소유하지 않는다. Bloom의 로컬 버퍼는 동기 컴파일/PSO 생성이
    //// 모두 끝날 때까지 유지하며, 파일 바이트 길이를 그대로 전달한다(문자열의 null 종단은 필요 없다).
    return true;
}

bool GetShaderBytecode(Render::ShaderSourceLibrary *library, const Render::ShaderVariantKey &key,
                       Render::ShaderBytecodeInfo &bytecodeInfo)
{
    const auto *buffer = library ? library->GetShaderBytecode(key) : nullptr;
    if (!buffer || buffer->empty())
    {
        LOG_MESSAGE_ERROR("RenderMaterialResolver", std::string("컴파일된 셰이더를 조회할 수 없거나 비어 있습니다: ") +
                                                        key.mShaderPath + " [" + key.mEntryPoint + ", " + key.mTarget +
                                                        "]");
        return false;
    }

    // 라이브러리의 버퍼를 복사하지 않는다. 동기 PSO 생성이 끝날 때까지 테이블을 교체하거나
    // 버퍼를 해제하지 않아야 하며, 컴파일 조건은 조회 키에서만 사용한다.
    bytecodeInfo = {buffer->data(), buffer->size(), key.mStage};
    return true;
}

bool SetShaderBytecodeList(Render::ShaderSourceLibrary *library, Render::MaterialGenerationInfo &generationInfo,
                           std::initializer_list<Render::ShaderVariantKey> keys)
{
    std::vector<Render::ShaderBytecodeInfo> bytecodeInfoList;
    bytecodeInfoList.reserve(keys.size());
    for (const auto &key : keys)
    {
        Render::ShaderBytecodeInfo bytecodeInfo;
        if (!GetShaderBytecode(library, key, bytecodeInfo))
            return false;
        bytecodeInfoList.push_back(bytecodeInfo);
    }

    // 필요한 stage를 모두 조회한 뒤 반영하여 누락된 변형으로 PSO 생성 요청을 하지 않는다.
    generationInfo.mShaderByteCodeInfoList = std::move(bytecodeInfoList);
    return true;
}

bool SetShaderBytecode(Render::ShaderSourceLibrary *library, Render::ComputeMaterialGenerationInfo &generationInfo,
                       const Render::ShaderVariantKey &key)
{

    Render::ShaderBytecodeInfo bytecodeInfo;
    if (!GetShaderBytecode(library, key, bytecodeInfo))
        return false;

    // 필요한 stage를 모두 조회한 뒤 반영하여 누락된 변형으로 PSO 생성 요청을 하지 않는다.
    generationInfo.mComputeShaderInfo = std::move(bytecodeInfo);
    return true;
}

bool ConfigureSkinningVertexVariant(Render::ShaderSourceLibrary *library,
                                    Render::MaterialGenerationInfo &generationInfo, const char *fileName,
                                    const char *entryPoint)
{
    Render::ShaderBytecodeInfo skinningVertexInfo;
    if (!GetShaderBytecode(library,
                           {fileName, entryPoint, "vs_5_1", Render::EShaderStage::eVertex, {{"ENABLE_SKINNING", "1"}}},
                           skinningVertexInfo))
        return false;

    // 매크로는 이미 컴파일에 반영되어 있다. PS 등 다른 stage와 렌더 상태는 유지하고,
    // VS 바이트코드와 정점 레이아웃만 함께 스키닝 변형으로 교체한다.
    for (auto &shaderInfo : generationInfo.mShaderByteCodeInfoList)
    {
        if (shaderInfo.mStage == Render::EShaderStage::eVertex)
        {
            shaderInfo = skinningVertexInfo;
            generationInfo.mInputLayoutType = Render::EInputLayoutType::eSkinningMesh;
            return true;
        }
    }

    LOG_MESSAGE_ERROR("RenderMaterialResolver", "스키닝 변형으로 교체할 버텍스셰이더가 없습니다.");
    return false;
}
} // namespace

Render::RenderMaterialResolver *Render::RenderMaterialResolver::GetInstance()
{

    static RenderMaterialResolver instance;

    return &instance;
}

Render::RenderMaterialResolver::RenderMaterialResolver() {}

Render::RenderMaterialResolver::~RenderMaterialResolver() {}

void Render::RenderMaterialResolver::Initialize(ShaderSourceLibrary *shaderSourceLibrary)
{
    if (mInitialized)
        return;

    if (!shaderSourceLibrary)
    {
        LOG_MESSAGE_ERROR("RenderMaterialResolver", "ShaderSourceLibrary가 주입되지 않았습니다.");
        return;
    }

    mShaderSourceLibrary = shaderSourceLibrary;

    mGpuMaterialManager = IMaterialManager::GetInstance();

    if (mGpuMaterialManager)
    {
        BuildStaticMeshOpaqueGpuMaterial();
        BuildStaticMeshOutlineWriteStencilGpuMaterial();
        BuildStaticMeshOutlineDrawGpuMaterial();
        BuildGrayScaleGpuMaterial();
        BuildDebugLineGpuMaterial();
        BuildBillboardGpuMaterial();
        BuildDebugGridGpuMaterial();
        BuildUIGpuMaterial();
        BuildSkySphereGpuMaterial();

        BuildShadowGpuMaterial();
        BuildToneMappingGpuMaterial();
        BuildBloomGpuMaterial();
    }

    mInitialized = true;
}

Render::MaterialID Render::RenderMaterialResolver::Resolve(CoreAsset::AssetID materialAssetID,
                                                           const RenderMaterialContext &context,
                                                           ERenderPassType passType)
{

    RenderMaterialVariantKey key;
    key.mRenderMaterialContext = context;
    key.mRenderPassType = passType;

    std::optional<Render::MaterialID> matID = FindAssetMaterialOverride(materialAssetID, key);

    return matID.value_or<Render::MaterialID>(Resolve(context, passType));
}

Render::MaterialID Render::RenderMaterialResolver::Resolve(const RenderMaterialContext &renderMaterialContext,
                                                           ERenderPassType passType)
{
    RenderMaterialVariantKey key;
    key.mRenderMaterialContext = renderMaterialContext;
    key.mRenderPassType = passType;
    return GetGpuMaterialID(key);
}

void Render::RenderMaterialResolver::RegisterGpuMaterial(const RenderMaterialContext &renderMaterialContext,
                                                         ERenderPassType passType, MaterialID id)
{
    RenderMaterialVariantKey key;
    key.mRenderMaterialContext = renderMaterialContext;
    key.mRenderPassType = passType;

    mGpuMaterialIDTable[key] = id;
}

void Render::RenderMaterialResolver::RegisterAssetMaterialOverride(CoreAsset::AssetID materialAssetID,
                                                                   const RenderMaterialVariantKey &variantKey,
                                                                   MaterialID gpuMaterialID)
{

    VariantKeyTable &keyTable = mOverrideGpuMaterialKeyTable[materialAssetID];

    keyTable[variantKey] = gpuMaterialID;
}

void Render::RenderMaterialResolver::UnregisterAssetMaterialOverrides(CoreAsset::AssetID materialAssetID)
{

    auto keyIt = mOverrideGpuMaterialKeyTable.find(materialAssetID);

    if (keyIt == mOverrideGpuMaterialKeyTable.end())
        return;

    mOverrideGpuMaterialKeyTable.erase(keyIt);
}

Render::MaterialID Render::RenderMaterialResolver::CreateAndRegisterAssetMaterialOverride(
    CoreAsset::AssetID materialAssetID, const RenderMaterialContext &context, ERenderPassType passType,
    const MaterialGenerationInfo &generationInfo)
{

    Render::MaterialID gpuMatID = CreateGpuMaterial(generationInfo);

    if (gpuMatID == MaterialIDNone)
    {
        return MaterialIDNone;
    }

    RenderMaterialVariantKey key;
    key.mRenderMaterialContext = context;
    key.mRenderPassType = passType;
    RegisterAssetMaterialOverride(materialAssetID, key, gpuMatID);

    return gpuMatID;
}

void Render::RenderMaterialResolver::RegisterSystemGpuMaterial(ESystemMaterialRole role, MaterialID gpuMaterialID)
{

    mSystemGpuMaterialTable[role] = gpuMaterialID;
}

Render::MaterialID Render::RenderMaterialResolver::ResolveSystemGpuMaterial(ESystemMaterialRole role) const
{
    auto it = mSystemGpuMaterialTable.find(role);
    return it != mSystemGpuMaterialTable.end() ? it->second : MaterialIDNone;
}

Render::MaterialID Render::RenderMaterialResolver::CreateGpuMaterial(const MaterialGenerationInfo &info)
{

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(info);

    return matID;
}

std::optional<Render::MaterialID> Render::RenderMaterialResolver::FindAssetMaterialOverride(
    CoreAsset::AssetID materialAssetID, const RenderMaterialVariantKey &variantKey) const
{

    auto keyIt = mOverrideGpuMaterialKeyTable.find(materialAssetID);

    if (keyIt == mOverrideGpuMaterialKeyTable.end())
    {

        return {};
    }

    auto gpuMatIt = keyIt->second.find(variantKey);

    if (gpuMatIt == keyIt->second.end())
    {
        return {};
    }

    return gpuMatIt->second;
}

Render::MaterialID Render::RenderMaterialResolver::GetGpuMaterialID(const RenderMaterialVariantKey &key) const
{
    auto it = mGpuMaterialIDTable.find(key);

    return it != mGpuMaterialIDTable.end() ? it->second : 0;
}

void Render::RenderMaterialResolver::BuildStaticMeshOpaqueGpuMaterial()
{
    {
        // Static
        MaterialGenerationInfo gpuMaterialGenerationInfo;
        gpuMaterialGenerationInfo.mHLSLGenerationInfo.mAlbedoNum = 1;
        gpuMaterialGenerationInfo.mHLSLGenerationInfo.mHasNormalMap = false;
        gpuMaterialGenerationInfo.mRenderSettingInfo.mRenderTargetFormat[0] = GRM::ETextureFormat::eR16G16B16A16_FLOAT;

        gpuMaterialGenerationInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

        if (!SetShaderBytecodeList(mShaderSourceLibrary, gpuMaterialGenerationInfo,
                                   {{"DefaultStaticMesh.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}},
                                    {"DefaultStaticMesh.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}}}))
            return;

        gpuMaterialGenerationInfo.mName = "StaticMeshOpaque";

        MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(gpuMaterialGenerationInfo);

        RenderMaterialContext rmc;
        rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
        rmc.mTransparent = false;
        RegisterGpuMaterial(rmc, Render::ERenderPassType::eMain, matID);

        if (!ConfigureSkinningVertexVariant(mShaderSourceLibrary, gpuMaterialGenerationInfo, "DefaultStaticMesh.hlsl",
                                            "VS"))
            return;
        gpuMaterialGenerationInfo.mName = "SkinningMeshOpaque";
        MaterialID skinningMatID = mGpuMaterialManager->CreateMaterialDirectly(gpuMaterialGenerationInfo);

        RenderMaterialContext skinningRmc;
        skinningRmc.mGeometryType = ERenderGeometryType::eSkinnedMesh;
        skinningRmc.mTransparent = false;
        RegisterGpuMaterial(skinningRmc, Render::ERenderPassType::eMain, skinningMatID);
    }

    // Unlit 버전
    {
        MaterialGenerationInfo gpuMaterialGenerationInfo;
        gpuMaterialGenerationInfo.mHLSLGenerationInfo.mAlbedoNum = 1;
        gpuMaterialGenerationInfo.mHLSLGenerationInfo.mHasNormalMap = false;
        gpuMaterialGenerationInfo.mRenderSettingInfo.mRenderTargetFormat[0] = GRM::ETextureFormat::eR16G16B16A16_FLOAT;

        gpuMaterialGenerationInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

        if (!SetShaderBytecodeList(mShaderSourceLibrary, gpuMaterialGenerationInfo,
                                   {{"DefaultStaticMesh.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}},
                                    {"DefaultStaticMesh_Unlit.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}}}))
            return;

        gpuMaterialGenerationInfo.mName = "StaticMeshOpaque_Unlit";

        MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(gpuMaterialGenerationInfo);

        RenderMaterialContext rmc;
        rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
        rmc.mTransparent = false;
        rmc.mShadingModel = CoreAsset::EShadingModel::eUnlit;
        RegisterGpuMaterial(rmc, Render::ERenderPassType::eMain, matID);

        if (!ConfigureSkinningVertexVariant(mShaderSourceLibrary, gpuMaterialGenerationInfo, "DefaultStaticMesh.hlsl",
                                            "VS"))
            return;
        gpuMaterialGenerationInfo.mName = "SkinningMeshOpaque_Unlit";
        MaterialID skinningMatID = mGpuMaterialManager->CreateMaterialDirectly(gpuMaterialGenerationInfo);

        RenderMaterialContext skinningRmc;
        skinningRmc.mGeometryType = ERenderGeometryType::eSkinnedMesh;
        skinningRmc.mTransparent = false;
        skinningRmc.mShadingModel = CoreAsset::EShadingModel::eUnlit;
        RegisterGpuMaterial(skinningRmc, Render::ERenderPassType::eMain, skinningMatID);
    }
}

void Render::RenderMaterialResolver::BuildStaticMeshOutlineWriteStencilGpuMaterial()
{
    // outline stencil gpu Material

    MaterialGenerationInfo mgInfo;

    MaterialRenderSettingInfo &staticMeshOutlineWriteStencilRenderSettingInfo = mgInfo.mRenderSettingInfo;
    staticMeshOutlineWriteStencilRenderSettingInfo.mCullMode = ECullMode::eBack;
    staticMeshOutlineWriteStencilRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
    staticMeshOutlineWriteStencilRenderSettingInfo.mCCW = false;
    staticMeshOutlineWriteStencilRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eNone;
    staticMeshOutlineWriteStencilRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eDisabled;
    staticMeshOutlineWriteStencilRenderSettingInfo.mStencilWriteMode = EStencilWriteMode::eEnabled;
    staticMeshOutlineWriteStencilRenderSettingInfo.mStencilFrontCompareMode = EDepthStencilCompareMode::eAlways;
    staticMeshOutlineWriteStencilRenderSettingInfo.mStencilFrontPassOp = EStencilOP::eReplace;
    staticMeshOutlineWriteStencilRenderSettingInfo.mStencilFrontFailOp = EStencilOP::eKeep;

    if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                               {{"OutlineStaticMesh.hlsl", "VS_Stencil", "vs_5_1", EShaderStage::eVertex, {}}}))
        return;
    mgInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    RegisterGpuMaterial(rmc, Render::ERenderPassType::eOutlineStencil, matID);

    // outline 상태는 공유하고 정점 입력/VS만 스키닝 변형으로 분리한다.
    if (!ConfigureSkinningVertexVariant(mShaderSourceLibrary, mgInfo, "OutlineStaticMesh.hlsl", "VS_Stencil"))
        return;
    mgInfo.mName = "SkinningMeshOutlineStencil";
    MaterialID skinningMatID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);
    rmc.mGeometryType = ERenderGeometryType::eSkinnedMesh;
    RegisterGpuMaterial(rmc, Render::ERenderPassType::eOutlineStencil, skinningMatID);
}

void Render::RenderMaterialResolver::BuildStaticMeshOutlineDrawGpuMaterial()
{
    MaterialGenerationInfo mgInfo;

    MaterialRenderSettingInfo &staticMeshOutlineDrawRenderSettingInfo = mgInfo.mRenderSettingInfo;
    staticMeshOutlineDrawRenderSettingInfo.mCullMode = ECullMode::eBack;
    staticMeshOutlineDrawRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
    staticMeshOutlineDrawRenderSettingInfo.mCCW = false;
    staticMeshOutlineDrawRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eNone;
    staticMeshOutlineDrawRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eDisabled;
    staticMeshOutlineDrawRenderSettingInfo.mStencilWriteMode = EStencilWriteMode::eEnabled;
    staticMeshOutlineDrawRenderSettingInfo.mStencilFrontCompareMode = EDepthStencilCompareMode::eNotEqual;
    staticMeshOutlineDrawRenderSettingInfo.mStencilFrontPassOp = EStencilOP::eZero;
    staticMeshOutlineDrawRenderSettingInfo.mStencilFrontFailOp = EStencilOP::eKeep;

    if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                               {{"OutlineStaticMesh.hlsl", "VS_DrawOutline", "vs_5_1", EShaderStage::eVertex, {}},
                                {"OutlineStaticMesh.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}}}))
        return;

    mgInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    RegisterGpuMaterial(rmc, ERenderPassType::eOutlineDraw, matID);

    if (!ConfigureSkinningVertexVariant(mShaderSourceLibrary, mgInfo, "OutlineStaticMesh.hlsl", "VS_DrawOutline"))
        return;
    mgInfo.mName = "SkinningMeshOutlineDraw";
    MaterialID skinningMatID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);
    rmc.mGeometryType = ERenderGeometryType::eSkinnedMesh;
    RegisterGpuMaterial(rmc, ERenderPassType::eOutlineDraw, skinningMatID);
}

void Render::RenderMaterialResolver::BuildGrayScaleGpuMaterial()
{
    MaterialGenerationInfo mgInfo;
    // GrayScale gpuMaterial
    MaterialRenderSettingInfo &grayScaleRenderSettingInfo = mgInfo.mRenderSettingInfo;
    grayScaleRenderSettingInfo.mCullMode = ECullMode::eNone;
    grayScaleRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
    grayScaleRenderSettingInfo.mCCW = false;
    grayScaleRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eNone;
    grayScaleRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eDisabled;
    grayScaleRenderSettingInfo.mBlendSrc = EBlend::eBLEND_SRC_ALPHA;
    grayScaleRenderSettingInfo.mBlendDest = EBlend::eBLEND_INV_SRC_ALPHA;
    grayScaleRenderSettingInfo.mBlendOp = EBlendOp::eADD;

    if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                               {{"GrayScale.hlsl", "VSMain", "vs_5_1", EShaderStage::eVertex, {}},
                                {"GrayScale.hlsl", "PSMain", "ps_5_1", EShaderStage::ePixel, {}}}))
        return;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = true;

    RegisterGpuMaterial(rmc, ERenderPassType::eGrayScale, matID);
}

void Render::RenderMaterialResolver::BuildDebugGridGpuMaterial()
{
    MaterialGenerationInfo mgInfo;

    // DebugGrid GpuMaterial
    MaterialRenderSettingInfo &debugGridRenderSettingInfo = mgInfo.mRenderSettingInfo;
    debugGridRenderSettingInfo.mCullMode = ECullMode::eNone;
    debugGridRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
    debugGridRenderSettingInfo.mCCW = false;
    debugGridRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eLess;
    debugGridRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eDisabled;
    debugGridRenderSettingInfo.mBlendMode = EBlendMode::eAlphaBlend;
    debugGridRenderSettingInfo.mBlendSrc = EBlend::eBLEND_SRC_ALPHA;
    debugGridRenderSettingInfo.mBlendDest = EBlend::eBLEND_INV_SRC_ALPHA;
    debugGridRenderSettingInfo.mBlendOp = EBlendOp::eADD;
    debugGridRenderSettingInfo.mRenderTargetFormat[0] = GRM::ETextureFormat::eR16G16B16A16_FLOAT;

    if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                               {{"DebugGrid.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}},
                                {"DebugGrid.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}}}))
        return;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = true;

    RegisterGpuMaterial(rmc, ERenderPassType::eDebugGrid, matID);
}

void Render::RenderMaterialResolver::BuildBillboardGpuMaterial()
{
    MaterialGenerationInfo mgInfo;

    MaterialRenderSettingInfo &gpuRenderSettingInfo = mgInfo.mRenderSettingInfo;
    gpuRenderSettingInfo.mCullMode = ECullMode::eNone;
    gpuRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
    gpuRenderSettingInfo.mCCW = false;
    gpuRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eLess;

    if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                               {{"Billboard.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}},
                                {"Billboard.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}},
                                {"Billboard.hlsl", "GS", "gs_5_1", EShaderStage::eGeometry, {}}}))
        return;

    mgInfo.mInputLayoutType = EInputLayoutType::eBillboard;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eBillboard;
    rmc.mTransparent = false;

    RegisterGpuMaterial(rmc, ERenderPassType::eBillboard, matID);
}

void Render::RenderMaterialResolver::BuildDebugLineGpuMaterial()
{
    MaterialGenerationInfo mgInfo;

    MaterialRenderSettingInfo &gpuRenderSettingInfo = mgInfo.mRenderSettingInfo;
    gpuRenderSettingInfo.mCullMode = ECullMode::eNone;
    gpuRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
    gpuRenderSettingInfo.mCCW = false;
    gpuRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eLess;

    if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                               {{"DebugLine.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}},
                                {"DebugLine.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}}}))
        return;

    mgInfo.mInputLayoutType = EInputLayoutType::eLine;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eDebugLine;
    rmc.mTransparent = false;

    RegisterGpuMaterial(rmc, ERenderPassType::eDebugLine, matID);
}
void Render::RenderMaterialResolver::BuildUIGpuMaterial()
{
    // gpuMaterial 생성

    // defaultUIMat
    MaterialID defaultUIGpuMaterialID;
    {
        MaterialGenerationInfo mgInfo;

        MaterialRenderSettingInfo &defaultUIMatRenderSettingInfo = mgInfo.mRenderSettingInfo;
        defaultUIMatRenderSettingInfo.mCullMode = ECullMode::eNone;
        defaultUIMatRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
        defaultUIMatRenderSettingInfo.mCCW = false;
        defaultUIMatRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eLess;
        defaultUIMatRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eDisabled;
        defaultUIMatRenderSettingInfo.mBlendSrc = EBlend::eBLEND_SRC_ALPHA;
        defaultUIMatRenderSettingInfo.mBlendDest = EBlend::eBLEND_INV_SRC_ALPHA;
        defaultUIMatRenderSettingInfo.mBlendOp = EBlendOp::eADD;

        if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                                   {{"DefaultUI.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}},
                                    {"DefaultUI.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}}}))
            return;

        mgInfo.mInputLayoutType = EInputLayoutType::eUI;

        defaultUIGpuMaterialID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);
    }

    // defaultFontMat
    MaterialID defaultUIFontGpuMaterialID;
    {
        MaterialGenerationInfo mgInfo;

        MaterialRenderSettingInfo &defaultUIFontMatRenderSettingInfo = mgInfo.mRenderSettingInfo;
        defaultUIFontMatRenderSettingInfo.mCullMode = ECullMode::eNone;
        defaultUIFontMatRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
        defaultUIFontMatRenderSettingInfo.mCCW = false;
        defaultUIFontMatRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eLess;
        defaultUIFontMatRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eDisabled;
        defaultUIFontMatRenderSettingInfo.mBlendMode = EBlendMode::eAlphaBlend;
        defaultUIFontMatRenderSettingInfo.mBlendSrc = EBlend::eBLEND_SRC_ALPHA;
        defaultUIFontMatRenderSettingInfo.mBlendDest = EBlend::eBLEND_INV_SRC_ALPHA;
        defaultUIFontMatRenderSettingInfo.mBlendOp = EBlendOp::eADD;

        if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                                   {{"DefaultFont.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}},
                                    {"DefaultFont.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}}}))
            return;

        mgInfo.mInputLayoutType = EInputLayoutType::eUI;

        defaultUIFontGpuMaterialID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);
    }
    RegisterSystemGpuMaterial(ESystemMaterialRole::DefaultUI, defaultUIGpuMaterialID);
    RegisterSystemGpuMaterial(ESystemMaterialRole::DefaultUIFont, defaultUIFontGpuMaterialID);
}

void Render::RenderMaterialResolver::BuildSkySphereGpuMaterial()
{
    MaterialID gpuMaterialID;
    {
        MaterialGenerationInfo mgInfo;

        MaterialRenderSettingInfo &matRenderSettingInfo = mgInfo.mRenderSettingInfo;
        matRenderSettingInfo.mCullMode = ECullMode::eNone;
        matRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
        matRenderSettingInfo.mCCW = false;
        matRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eLess;
        matRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eEnabled;
        matRenderSettingInfo.mDepthWriteMask = false;
        matRenderSettingInfo.mRenderTargetFormat[0] = GRM::ETextureFormat::eR16G16B16A16_FLOAT;

        if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                                   {{"SkySphere.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}},
                                    {"SkySphere.hlsl", "PS", "ps_5_1", EShaderStage::ePixel, {}}}))
            return;

        mgInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

        gpuMaterialID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);
    }
    // Register
    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    rmc.mShadingModel = CoreAsset::EShadingModel::eNone;

    RegisterGpuMaterial(rmc, ERenderPassType::eSkySphere, gpuMaterialID);
}

void Render::RenderMaterialResolver::BuildShadowGpuMaterial()
{
    MaterialGenerationInfo mgInfo;

    MaterialRenderSettingInfo &matRenderSettingInfo = mgInfo.mRenderSettingInfo;
    // matRenderSettingInfo.mCullMode = ECullMode::eNone;
    matRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
    matRenderSettingInfo.mCCW = false;
    matRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eLess;
    matRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eEnabled;
    matRenderSettingInfo.mDepthWriteMask = true;
    // color render target 없음으로 설정합니다.
    matRenderSettingInfo.mRenderTargetCount = 0;
    matRenderSettingInfo.mDepthStencilFormat = GRM::ETextureFormat::eD32_FLOAT;

    matRenderSettingInfo.mDepthBias = 10000;
    matRenderSettingInfo.mSlopeScaledDepthBias = 2.0f;

    if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                               {{"Shadow.hlsl", "VS", "vs_5_1", EShaderStage::eVertex, {}}}))
        return;

    mgInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;
    mgInfo.mName = "StaticMeshShadow";
    MaterialID gpuMaterialID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    rmc.mShadingModel = CoreAsset::EShadingModel::eNone;
    RegisterGpuMaterial(rmc, ERenderPassType::eShadow, gpuMaterialID);

    if (!ConfigureSkinningVertexVariant(mShaderSourceLibrary, mgInfo, "Shadow.hlsl", "VS"))
        return;
    mgInfo.mName = "SkinningMeshShadow";
    MaterialID skinningGpuMaterialID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext skinningRmc;
    skinningRmc.mGeometryType = ERenderGeometryType::eSkinnedMesh;
    skinningRmc.mTransparent = false;
    skinningRmc.mShadingModel = CoreAsset::EShadingModel::eNone;
    RegisterGpuMaterial(skinningRmc, ERenderPassType::eShadow, skinningGpuMaterialID);
}

void Render::RenderMaterialResolver::BuildToneMappingGpuMaterial()
{
    MaterialGenerationInfo mgInfo;
    // GrayScale gpuMaterial
    MaterialRenderSettingInfo &toneMappingRenderSettingInfo = mgInfo.mRenderSettingInfo;
    toneMappingRenderSettingInfo.mCullMode = ECullMode::eNone;
    toneMappingRenderSettingInfo.mFillMode = EFillMode::eSolidMode;
    toneMappingRenderSettingInfo.mCCW = false;
    toneMappingRenderSettingInfo.mDepthCompareMode = EDepthStencilCompareMode::eNone;
    toneMappingRenderSettingInfo.mDepthWriteMode = EDepthWriteMode::eDisabled;
    toneMappingRenderSettingInfo.mRenderTargetCount = 1;
    toneMappingRenderSettingInfo.mRenderTargetFormat[0] = GRM::ETextureFormat::eR8G8B8A8_UNORM_SRGB;

    // grayScaleRenderSettingInfo.mBlendSrc = EBlend::eBLEND_SRC_ALPHA;
    // grayScaleRenderSettingInfo.mBlendDest = EBlend::eBLEND_INV_SRC_ALPHA;
    // grayScaleRenderSettingInfo.mBlendOp = EBlendOp::eADD;

    if (!SetShaderBytecodeList(mShaderSourceLibrary, mgInfo,
                               {{"ToneMapping.hlsl", "VSMain", "vs_5_1", EShaderStage::eVertex, {}},
                                {"ToneMapping.hlsl", "PSMain", "ps_5_1", EShaderStage::ePixel, {}}}))
        return;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;

    RegisterGpuMaterial(rmc, ERenderPassType::eToneMapping, matID);
}

void Render::RenderMaterialResolver::BuildBloomGpuMaterial()
{
    std::vector<uint8_t> horizontalShaderSource;
    std::vector<uint8_t> verticalShaderSource;
    if (!GetShaderSource(mShaderSourceLibrary, "BloomHorizontal.hlsl", horizontalShaderSource) ||
        !GetShaderSource(mShaderSourceLibrary, "BloomVertical.hlsl", verticalShaderSource))
        return;

    RenderMaterialContext rmc = {};

    ComputeMaterialGenerationInfo horizontalMaterialInfo;
    horizontalMaterialInfo.mName = "BloomHorizontalHLSL";
    // horizontalMaterialInfo.mComputeShaderInfo = {};

    if (!SetShaderBytecode(mShaderSourceLibrary, horizontalMaterialInfo,
                           {"BloomHorizontal.hlsl", "CSMain", "cs_5_1", EShaderStage::eCompute}))
        return;

    MaterialID horizontalMaterialID = mGpuMaterialManager->CreateComputeMaterial(horizontalMaterialInfo);
    RegisterGpuMaterial(rmc, ERenderPassType::eBloomHorizontal, horizontalMaterialID);

    ComputeMaterialGenerationInfo verticalMaterialInfo;
    verticalMaterialInfo.mName = "BloomVerticalHLSL";
    // verticalMaterialInfo.mComputeShaderInfo = {verticalShaderSource.data(), verticalShaderSource.size(), "CSMain",
    //                                            "cs_5_1", EShaderStage::eCompute};

    if (!SetShaderBytecode(mShaderSourceLibrary, verticalMaterialInfo,
                           {"BloomVertical.hlsl", "CSMain", "cs_5_1", EShaderStage::eCompute}))
        return;

    MaterialID verticalMaterialID = mGpuMaterialManager->CreateComputeMaterial(verticalMaterialInfo);
    RegisterGpuMaterial(rmc, ERenderPassType::eBloomVertical, verticalMaterialID);
}
