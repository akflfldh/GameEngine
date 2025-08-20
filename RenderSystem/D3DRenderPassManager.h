#pragma once

#ifdef D3DX

#include <d3d12.h>
#include <memory>
#include <vector>
#include <wrl.h>
namespace GRM
{
class IGpuResourceManager;
}
namespace D3DGRM
{
class D3DGpuResourceManager;
}

namespace D3DRender
{
class ID3DRenderPass;
class D3DWindowRenderData;
class D3DMaterialManager;

class D3DRenderPassManager
{
  public:
    D3DRenderPassManager(Microsoft::WRL::ComPtr<ID3D12Device> device, D3DGRM::D3DGpuResourceManager *gpuResourceManager,
                         std::shared_ptr<D3DMaterialManager> materialManager);
    ~D3DRenderPassManager();

    std::vector<std::unique_ptr<ID3DRenderPass>> CreateDefaultRenderPipeline(
        std::shared_ptr<D3DRender::D3DWindowRenderData> windowRenderData);

  private:
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    D3DGRM::D3DGpuResourceManager *mGpuResourceManager;
    std::shared_ptr<D3DMaterialManager> mMaterialManager;
};

} // namespace D3DRender

#endif