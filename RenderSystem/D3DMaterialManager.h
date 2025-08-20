#pragma once

#include "D3DGpuResourceManager/D3DGpuResourceManager.h"
#include "RenderSystem/D3DShaderReflectSystem.h"
#include "RenderSystem/IMaterialManager.h"
#include "RenderSystem/MaterialType.h"
#include "RenderSystem/RenderSystemDllMacro.h"
#include <GRMPtr.h>
#include <d3d12.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <wrl.h>

namespace D3DRender
{

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

class RENDER_SYSTEM_API D3DMaterialManager : public Render::IMaterialManager
{
  public:
    D3DMaterialManager(Microsoft::WRL::ComPtr<ID3D12Device> device, D3DGRM::D3DGpuResourceManager *gpuResourceManager);
    virtual ~D3DMaterialManager();

    D3DMaterialManager(const D3DMaterialManager &) = delete;
    D3DMaterialManager &operator=(const D3DMaterialManager &) = delete;

    Render::MaterialID CreateMaterial(const Render::CreationMaterialInfo &creationMaterialInfo);

    D3DMaterialItem *GetMaterialItem(Render::MaterialID materialID) const;

    virtual const Render::ShaderResourceInfoSet &GetMaterialShaderResourceInfo(Render::MaterialID matID) override;

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

    D3D12_DESCRIPTOR_RANGE_TYPE ConvertToDescriptorRangeType(Render::EShaderResourceType shaderResourceType);

    D3D12_CULL_MODE ConvertToD3DCullMode(Render::ECullMode cullMode);
    D3D12_FILL_MODE ConvertToD3DFillMode(Render::EFillMode fillMode);
    bool ConvertToBlendEnableFlag(Render::EBlendMode blendMode);
    D3D12_COMPARISON_FUNC ConvertToD3DCmpFun(Render::EDepthStencilCompareMode cmpMode);
    bool ConvertToDepthWriteModeEnableFlag(Render::EDepthWriteMode writeMode);

  private:
    // 새로운 다음머터리얼ID를가져오고 소비한다.
    Render::MaterialID GetNextMaterialID() const;

  private:
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    D3DGRM::D3DGpuResourceManager *mGpuResourceManager;

  private:
    std::unordered_map<Render::MaterialID, std::unique_ptr<D3DMaterialItem>> mMaterialItemTable;

  private:
    mutable Render::MaterialID mNextMaterialID;
    // 머터리얼id를 부여할때 이벡터에 원소가있다면 마지막원소를 꺼내 id를부여한다.
    mutable std::vector<Render::MaterialID> mFreeMaterialIDVector;

    // GRM::GRMPtr mDefaultSampler;		//테이블로 확장가능
};

} // namespace D3DRender
