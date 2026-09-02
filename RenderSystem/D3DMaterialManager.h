#pragma once

#include "D3DGpuResourceManager/D3DGpuResourceManager.h"
#include "RenderSystem/D3DShaderReflectSystem.h"
#include "RenderSystem/IMaterialManager.h"
#include "RenderSystem/MaterialType.h"
#include "RenderSystem/RenderSystemDllMacro.h"
#include <GRMPtr.h>
#include <d3d12.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl.h>

namespace D3DRender
{

static constexpr int MAX_TEXTURE_SLOT = 4;
static constexpr int MAX_SAMPLER_SLOT = 1;

struct D3DRenderPassInfo
{
    // 엔진에서 제공하는 기본 pass인지의 여부
    // true라면 pso,rootSignature를 엔진의 컨테이너에서 참조하면된다.
    bool mIsEnginePass;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

    // 어떤 자원을 사용하고 , 그 자원이 몇번 루트파라미터에 바인딩되는지에대한 정보도있어야한다.
    //  resource name,  root paramter index
    std::unordered_map<std::string, int> mShaderResourceRootParameterBindingInfo;
};

struct D3DMainRenderPassInfo : public D3DRenderPassInfo
{
    bool mOpaqueFlag = true;
    int mInputType = 0;
};

struct D3DMaterialItem
{

    std::string mName;
    Render::MaterialID mID;
    // 사용하는 전체 shaderResource정보가 담겨있다
    //  일단 그대로사용
    // 근데 필요없는게 너무 많아.
    // ShaderResourceInfoSet만 가져도 될거같다.
    Render::ShaderResourceInfoSet mShaderResourceInfoSet;
    // Render::CreationMaterialInfo mCreationSourceInfo;

    // main pass 와 user이 post pass는 각각 pso, rootsignature가 존재해야하고
    D3DMainRenderPassInfo mMainPass;
    D3DRenderPassInfo mPostPass;

    // 각 renderpass마다 pso, rootsignature가 존재해야해
    // 더 정확히는 기본엔진에서 제공하는 pass의 이름만 유지하면되고
    // 해당 default pass를 사용하는지의 여부
    // 원소가없으면 사용안함, 있더라도 false로 바꾸면 해당 패스의 사용을 끌수있음.
    std::unordered_map<std::string, bool> mDefaultPassTable;
};

struct MaterialItem
{
    Render::MaterialID mID;
    ID3D12PipelineState *mPso;
};

class RENDER_SYSTEM_API D3DMaterialManager : public Render::IMaterialManager
{
  public:
    D3DMaterialManager(Microsoft::WRL::ComPtr<ID3D12Device> device, D3DGRM::D3DGpuResourceManager *gpuResourceManager);
    virtual ~D3DMaterialManager();

    D3DMaterialManager(const D3DMaterialManager &) = delete;
    D3DMaterialManager &operator=(const D3DMaterialManager &) = delete;

    //  Render::MaterialID CreateMaterial(const Render::CreationMaterialInfo &creationMaterialInfo);

    // D3DMaterialItem *GetMaterialItem(Render::MaterialID materialID) const;
    //  D3DMaterialItem *GetMaterialItem(const char *materialName) const;

    virtual const Render::ShaderResourceInfoSet &GetMaterialShaderResourceInfo(Render::MaterialID matID) override;
    virtual const Render::ShaderResourceInfoSet &GetMaterialShaderResourceInfo(const char *materialName) const override;
    virtual Render::ShaderResourceInfoSet &GetMaterialShaderResourceInfo(const char *materialName) override;

#pragma region refactoring
    virtual Render::MaterialID CreateMaterial(const Render::MaterialGenerationInfo &info) override;
    virtual Render::MaterialID CreateMaterialDirectly(const Render::MaterialGenerationInfo
                                                          &info /*, uint8_t *pShader,
                      size_t shaderSize*/) override;

    MaterialItem *GetMaterialItem(Render::MaterialID materialID) const;

    ID3D12RootSignature *GetMasterRootSignature() const;

#pragma endregion
  private:
    bool BuildMainPass(const Render::CreationMaterialInfo &creationMaterialInfo, D3DMainRenderPassInfo &oMaterialItem,
                       Render::ShaderResourceInfoSet oShaderResourceInfo);

  private:
    bool CompileShader(const Render::ShaderSourceInfo &shaderSourceInfo, const std::vector<uint8_t> &shaderCode,
                       ID3DBlob *&oCompiledBlob);

    // 리플렉션을통해 얻는 리소스정보와 세이더파일에 명시된 리소스정보가 일치하는지 검증
    bool VertifyShaderResource(
        const std::unordered_map<std::string, ShaderResourceReflectInfo> &shaderResourceReflectInfoTable,
        const Render::CreationMaterialInfo &creationMaterialInfo);

    ID3D12RootSignature *BuildRootSignature(
        const std::unordered_map<std::string, ShaderResourceReflectInfo> &shaderResourceReflectInfoTable,
        D3DRenderPassInfo &oRenderPassInfo);

    ID3D12PipelineState *BuildPipelineState(
        ID3D12RootSignature *rootSignature, const Render::MainRenderPass &mainRenderPass,
        const std::unordered_map<Render::EShaderStage, ID3DBlob *> &compiledShaderTable);

    void BuildIMainInputLayout(const Render::MainRenderPass mainRenderPass,
                               std::vector<D3D12_INPUT_ELEMENT_DESC> &oInputElementDescVector);

    void BuildIMainInputLayout(Render::EInputLayoutType inputLayoutType,
                               std::vector<D3D12_INPUT_ELEMENT_DESC> &oInputElementDescVector);

    D3D12_DESCRIPTOR_RANGE_TYPE ConvertToDescriptorRangeType(Render::EShaderResourceType shaderResourceType);

    D3D12_CULL_MODE ConvertToD3DCullMode(Render::ECullMode cullMode);
    D3D12_FILL_MODE ConvertToD3DFillMode(Render::EFillMode fillMode);
    bool ConvertToBlendEnableFlag(Render::EBlendMode blendMode);
    D3D12_BLEND ConvertToD3DBlend(Render::EBlend blend) const;
    D3D12_BLEND_OP ConvertToD3DBlendOp(Render::EBlendOp blendOp) const;
    D3D12_COMPARISON_FUNC ConvertToD3DCmpFun(Render::EDepthStencilCompareMode cmpMode);
    bool ConvertToDepthWriteModeEnableFlag(Render::EDepthWriteMode writeMode);
    D3D12_DEPTH_WRITE_MASK ConvertToDepthWriteMask(bool writemask);
    bool ConvertToStencilWriteModeEnableFlag(Render::EStencilWriteMode writeMode);

    D3D12_STENCIL_OP ConvertToD3DStencilOP(Render::EStencilOP op);

  private:
#pragma region refactoring
    Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootMasterSignature();

    // HLSL생성 + 컴파일

    // Material 구조체에서 사용될 변수, or그냥 HLSL코드에 들어갈 상수
    // 텍스처 리소스이름정보,  텍스처 값 샘플링해서 가져오면 어떻게 사용할것인지에대한 서술
    // 값들이 어떻게 사용되고 어떻한 연산을 거치는지 그런정보 가 순서대로 담겨있어야할듯?
    // 혹은 이것도 결국 그래프로 나타는거니깐.-> 위상정렬같은거지.
    // 위상정렬해서 그 정렬된 순서대로 연산 (행동)을 나타내는 노드를 하나씩 가져오면서 그 연산에 해당하는 코드를 삽입
    // 코드삽입 반복 등등 이런느낌?
    bool CreateHLSL(const Render::MaterialHLSLGenerationInfo &hlslGenerationInfo, std::vector<uint8_t> &oShader);

    bool CompileHLSL(const Render::MaterialGenerationInfo &info /*, uint8_t *pShader, size_t size,*/,
                     std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>> &oShaderTable);

    ID3D12PipelineState *CreatePSO(const Render::MaterialGenerationInfo &info,
                                   const std::unordered_map<Render::EShaderStage, Microsoft::WRL::ComPtr<ID3DBlob>>
                                       &shaderTable /*필요한 데이터 받는다*/);

    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(Render::EInputLayoutType inputLayoutType);

#pragma endregion

  private:
    // 새로운 다음머터리얼ID를가져오고 소비한다.
    Render::MaterialID GetNextMaterialID() const;

    void InitD3DBlendTable();
    void InitD3DBlendOpTable();

  private:
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    D3DGRM::D3DGpuResourceManager *mGpuResourceManager;

  private:
    std::unordered_map<Render::MaterialID, std::unique_ptr<D3DMaterialItem>> mMaterialItemTable;

    std::unordered_map<Render::MaterialID, std::unique_ptr<MaterialItem>> mMaterialItemsTable;

    std::unordered_map<std::string, D3D12_BLEND> mD3DBlendTable;
    std::unordered_map<std::string, D3D12_BLEND_OP> mD3DBlendOpTable;

  private:
    mutable Render::MaterialID mNextMaterialID;
    // 머터리얼id를 부여할때 이벡터에 원소가있다면 마지막원소를 꺼내 id를부여한다.
    mutable std::vector<Render::MaterialID> mFreeMaterialIDVector;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mMasterRootSignature = nullptr;

    // GRM::GRMPtr mDefaultSampler;		//테이블로 확장가능
};

} // namespace D3DRender
