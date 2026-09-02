#pragma once

#include <Core/CoreDllExport.h>

#include <RenderSystem/MaterialType.h>

namespace CoreAsset
{
class Material;
}

namespace Core
{

class CORE_API_LIB MaterialSystem
{
  public:
    static MaterialSystem *GetInstance();
    MaterialSystem();
    ~MaterialSystem();

    // 에셋 머티리얼 -> GPU 머티리얼 생성
    void BuildGpuMaterial(CoreAsset::Material *assetMaterial);

    // 그냥 HLSL Shader코드를 넘겨버리는 형태 (따라서 HLSL 연산이 담긴 노드리스트가 필요없다)
    // 엔진에서 빌트인으로 기본제공할,엔진객체들에서만사용할 머터리얼을 항상제공하고 바로 생성하기위해서 정의
    void BuildGpuMaterialDirectly(CoreAsset::Material *assetMaterial, uint8_t *pShader, size_t shaderSize,
                                  Render::MaterialRenderSettingInfo mRenderSettingInfo);

    // 수정 시 재생성
    void RebuildGpuMaterial(CoreAsset::Material *assetMaterial);

    // asset 머터리얼  없이 생성한다.
    Render::MaterialID CreateSystemGpuMaterial(
        uint8_t *pShader, size_t shaderSize, Render::MaterialRenderSettingInfo mRenderSettingInfo,
        const std::vector<Render::ShaderSourceInfo> &shaderInfo,
        Render::EInputLayoutType inputType = Render::EInputLayoutType::eStaticMesh);

  private:
    Render::MaterialRenderSettingInfo mRenderDefaultSettingInfo;
};

} // namespace Core