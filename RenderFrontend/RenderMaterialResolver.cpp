#include "RenderMaterialResolver.h"
#include <CoreAsset/UIMaterialManager.h>
#include <RenderFrontend/ShaderData.h>
#include <RenderSystem/IMaterialManager.h>

Render::RenderMaterialResolver *Render::RenderMaterialResolver::GetInstance()
{

    static RenderMaterialResolver instance;

    return &instance;
}

Render::RenderMaterialResolver::RenderMaterialResolver() {}

Render::RenderMaterialResolver::~RenderMaterialResolver() {}

void Render::RenderMaterialResolver::Initialize()
{
    if (mInitialized)
        return;

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
    }

    mInitialized = true;
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

Render::MaterialID Render::RenderMaterialResolver::GetGpuMaterialID(const RenderMaterialVariantKey &key) const
{
    auto it = mGpuMaterialIDTable.find(key);

    return it != mGpuMaterialIDTable.end() ? it->second : 0;
}

void Render::RenderMaterialResolver::BuildStaticMeshOpaqueGpuMaterial()
{

    {
        MaterialGenerationInfo gpuMaterialGenerationInfo;
        gpuMaterialGenerationInfo.mHLSLGenerationInfo.mAlbedoNum = 1;
        gpuMaterialGenerationInfo.mHLSLGenerationInfo.mHasNormalMap = false;

        gpuMaterialGenerationInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

        gpuMaterialGenerationInfo.mShaderInfoList.push_back({(uint8_t *)DefaultStaticMeshHLSL,
                                                             sizeof(DefaultStaticMeshHLSL) - 1, "VS", "vs_5_1",
                                                             EShaderStage::eVertex});

        gpuMaterialGenerationInfo.mShaderInfoList.push_back({(uint8_t *)DefaultStaticMeshHLSL,
                                                             sizeof(DefaultStaticMeshHLSL) - 1, "PS", "ps_5_1",
                                                             EShaderStage::ePixel});

        gpuMaterialGenerationInfo.mName = "StaticMeshOpaque";

        MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(gpuMaterialGenerationInfo);

        RenderMaterialContext rmc;
        rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
        rmc.mTransparent = false;
        RegisterGpuMaterial(rmc, Render::ERenderPassType::eMain, matID);
    }

    // Unlit 버전
    {
        MaterialGenerationInfo gpuMaterialGenerationInfo;
        gpuMaterialGenerationInfo.mHLSLGenerationInfo.mAlbedoNum = 1;
        gpuMaterialGenerationInfo.mHLSLGenerationInfo.mHasNormalMap = false;

        gpuMaterialGenerationInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

        gpuMaterialGenerationInfo.mShaderInfoList.push_back({(uint8_t *)DefaultStaticMeshHLSL,
                                                             sizeof(DefaultStaticMeshHLSL) - 1, "VS", "vs_5_1",
                                                             EShaderStage::eVertex});

        gpuMaterialGenerationInfo.mShaderInfoList.push_back({(uint8_t *)DefaultStaticMesh_Unlit_HLSL,
                                                             sizeof(DefaultStaticMesh_Unlit_HLSL) - 1, "PS", "ps_5_1",
                                                             EShaderStage::ePixel});

        gpuMaterialGenerationInfo.mName = "StaticMeshOpaque_Unlit";

        MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(gpuMaterialGenerationInfo);

        RenderMaterialContext rmc;
        rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
        rmc.mTransparent = false;
        rmc.mShadingModel = CoreAsset::EShadingModel::eUnlit;
        RegisterGpuMaterial(rmc, Render::ERenderPassType::eMain, matID);
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

    mgInfo.mShaderInfoList = {{(uint8_t *)OutlineStaticMeshHLSL, sizeof(OutlineStaticMeshHLSL) - 1, "VS_Stencil",
                               "vs_5_1", EShaderStage::eVertex}};
    mgInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    RegisterGpuMaterial(rmc, Render::ERenderPassType::eOutlineStencil, matID);
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

    uint8_t *pShader = (uint8_t *)OutlineStaticMeshHLSL;
    size_t shaderSize = sizeof(OutlineStaticMeshHLSL) - 1;
    mgInfo.mShaderInfoList = {{pShader, shaderSize, "VS_DrawOutline", "vs_5_1", EShaderStage::eVertex},
                              {pShader, shaderSize, "PS", "ps_5_1", EShaderStage::ePixel}};

    mgInfo.mInputLayoutType = EInputLayoutType::eStaticMesh;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eStaticMesh;
    rmc.mTransparent = false;
    RegisterGpuMaterial(rmc, ERenderPassType::eOutlineDraw, matID);
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

    size_t shaderSize = sizeof(GrayScaleHLSL) - 1;
    mgInfo.mShaderInfoList = {{(uint8_t *)GrayScaleHLSL, shaderSize, "VSMain", "vs_5_1", EShaderStage::eVertex},
                              {(uint8_t *)GrayScaleHLSL, shaderSize, "PSMain", "ps_5_1", EShaderStage::ePixel}};

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

    size_t shaderSize = sizeof(DebugGridHLSL) - 1;
    mgInfo.mShaderInfoList = {{(uint8_t *)DebugGridHLSL, shaderSize, "VS", "vs_5_1", EShaderStage::eVertex},
                              {(uint8_t *)DebugGridHLSL, shaderSize, "PS", "ps_5_1", EShaderStage::ePixel}};

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

    uint8_t *pShader = (uint8_t *)BillboardHLSL;
    size_t shaderSize = sizeof(BillboardHLSL) - 1;
    mgInfo.mShaderInfoList = {{pShader, shaderSize, "VS", "vs_5_1", EShaderStage::eVertex},
                              {pShader, shaderSize, "PS", "ps_5_1", EShaderStage::ePixel},
                              {pShader, shaderSize, "GS", "gs_5_1", EShaderStage::eGeometry}};

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

    uint8_t *pShader = (uint8_t *)DebugLineHLSL;
    size_t shaderSize = sizeof(DebugLineHLSL) - 1;
    mgInfo.mShaderInfoList = {{pShader, shaderSize, "VS", "vs_5_1", EShaderStage::eVertex},
                              {pShader, shaderSize, "PS", "ps_5_1", EShaderStage::ePixel}};

    mgInfo.mInputLayoutType = EInputLayoutType::eLine;

    MaterialID matID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);

    RenderMaterialContext rmc;
    rmc.mGeometryType = ERenderGeometryType::eDebugLine;
    rmc.mTransparent = false;

    RegisterGpuMaterial(rmc, ERenderPassType::eDebugLine, matID);
}
void Render::RenderMaterialResolver::BuildUIGpuMaterial()
{

    auto uiMaterialManager = CoreAsset::UIMaterialManager::GetInstance();

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

        uint8_t *pShader = (uint8_t *)DefaultUIHLSL;
        size_t shaderSize = sizeof(DefaultUIHLSL) - 1;

        mgInfo.mShaderInfoList = {{pShader, shaderSize, "VS", "vs_5_1", EShaderStage::eVertex},
                                  {pShader, shaderSize, "PS", "ps_5_1", EShaderStage::ePixel}};

        mgInfo.mInputLayoutType = EInputLayoutType::eUI;

        defaultUIGpuMaterialID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);
    }
    // MaterialID defaultUIGpuMaterialID = materialSystem->CreateSystemGpuMaterial(
    //     (uint8_t *)DefaultUIHLSL, sizeof(DefaultUIHLSL) - 1, defaultUIMatRenderSettingInfo,
    //     defaultUIMatShaderInfoList, EInputLayoutType::eUI);

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

        uint8_t *pShader = (uint8_t *)DefaultFontHLSL;
        size_t shaderSize = sizeof(DefaultFontHLSL) - 1;
        mgInfo.mShaderInfoList = {{pShader, shaderSize, "VS", "vs_5_1", EShaderStage::eVertex},
                                  {pShader, shaderSize, "PS", "ps_5_1", EShaderStage::ePixel}};

        mgInfo.mInputLayoutType = EInputLayoutType::eUI;

        defaultUIFontGpuMaterialID = mGpuMaterialManager->CreateMaterialDirectly(mgInfo);
    }
    // Register

    uiMaterialManager->RegisterDefaultUIGpuMaterialID(defaultUIGpuMaterialID);
    uiMaterialManager->RegsiterDefaultUIFontGpuMaterialID(defaultUIFontGpuMaterialID);
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

        uint8_t *pShader = (uint8_t *)SkySphereHLSL;
        size_t shaderSize = sizeof(SkySphereHLSL) - 1;
        mgInfo.mShaderInfoList = {{pShader, shaderSize, "VS", "vs_5_1", EShaderStage::eVertex},
                                  {pShader, shaderSize, "PS", "ps_5_1", EShaderStage::ePixel}};

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
