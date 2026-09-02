#include "MaterialSystem.h"
#include <CoreAsset/Material.h>
#include <RenderSystem/IMaterialManager.h>
#include <RenderSystem/MaterialType.h>
Core::MaterialSystem *Core::MaterialSystem::GetInstance()
{
    static MaterialSystem instance;
    return &instance;
}

Core::MaterialSystem::MaterialSystem() {}

Core::MaterialSystem::~MaterialSystem() {}

void Core::MaterialSystem::BuildGpuMaterial(CoreAsset::Material *assetMaterial)
{

    auto gpuMaterialManager = Render::IMaterialManager::GetInstance();

    Render::MaterialGenerationInfo info;
    info.mHLSLGenerationInfo.mAlbedoNum = assetMaterial->GetAlbedoTexResourceList().size();
    info.mHLSLGenerationInfo.mHasNormalMap = assetMaterial->HasNormalMap();
    info.mInputLayoutType = Render::EInputLayoutType::eStaticMesh; // 일단 정적메시 고정 향후 SkinningMesh 2 도 추가

    info.mShaderInfoList.push_back({nullptr, 0, "VS", "vs_5_1", Render::EShaderStage::eVertex});
    info.mShaderInfoList.push_back({nullptr, 0, "PS", "ps_5_1", Render::EShaderStage::ePixel});

    // 머터리얼 처음생성할때는 항상 디폴트
    info.mRenderSettingInfo = mRenderDefaultSettingInfo;

    Render::MaterialID gpuMatID = gpuMaterialManager->CreateMaterial(info);
    assetMaterial->SetGpuMaterial(gpuMatID);
}

void Core::MaterialSystem::BuildGpuMaterialDirectly(CoreAsset::Material *assetMaterial, uint8_t *pShader,
                                                    size_t shaderSize,
                                                    Render::MaterialRenderSettingInfo renderSettingInfo)
{

    auto gpuMaterialManager = Render::IMaterialManager::GetInstance();

    Render::MaterialGenerationInfo info;
    info.mHLSLGenerationInfo.mAlbedoNum = assetMaterial->GetAlbedoTexResourceList().size();
    info.mHLSLGenerationInfo.mHasNormalMap = assetMaterial->HasNormalMap();
    info.mInputLayoutType = Render::EInputLayoutType::eStaticMesh;

    info.mRenderSettingInfo = renderSettingInfo;
    info.mShaderInfoList.push_back({pShader, shaderSize, "VS", "vs_5_1", Render::EShaderStage::eVertex});
    info.mShaderInfoList.push_back({pShader, shaderSize, "PS", "ps_5_1", Render::EShaderStage::ePixel});

    Render::MaterialID gpuMatID = gpuMaterialManager->CreateMaterialDirectly(info);

    assetMaterial->SetGpuMaterial(gpuMatID);
}

void Core::MaterialSystem::RebuildGpuMaterial(CoreAsset::Material *assetMaterial)
{

    // 기존 GPU 머티리얼 해제
    // BuildGpuMaterial 재호출
}

Render::MaterialID Core::MaterialSystem::CreateSystemGpuMaterial(
    uint8_t *pShader, size_t shaderSize, Render::MaterialRenderSettingInfo mRenderSettingInfo,
    const std::vector<Render::ShaderSourceInfo> &shaderInfo, Render::EInputLayoutType inputType)
{
    auto gpuMaterialManager = Render::IMaterialManager::GetInstance();

    Render::MaterialGenerationInfo info;
    info.mHLSLGenerationInfo.mAlbedoNum = 0;
    info.mHLSLGenerationInfo.mHasNormalMap = false;
    info.mShaderInfoList = shaderInfo;
    info.mInputLayoutType = inputType;

    // 머터리얼 처음생성할때는 항상 디폴트
    info.mRenderSettingInfo = mRenderSettingInfo;

    Render::MaterialID gpuMatID = gpuMaterialManager->CreateMaterialDirectly(info);

    return gpuMatID;
}
