#pragma once

#include "RenderSystem/D3DRenderType.h"
#include "RenderSystem/ID3DRenderPass.h"
#include <memory>

namespace GRM
{
class IGpuResourceManager;
}
namespace D3DGRM
{
class D3DGpuTexture;
class D3DGpuResourceManager;
} // namespace D3DGRM
namespace Render
{
struct RenderItem;
}

namespace D3DRender
{

class D3DMaterialManager;
struct D3DMainRenderPassInfo;
class D3DOpaqueRenderPass : public ID3DRenderPass
{
  public:
    D3DOpaqueRenderPass(Microsoft::WRL::ComPtr<ID3D12Device> device, D3DGRM::D3DGpuResourceManager *resourceManager,
                        std::shared_ptr<D3DMaterialManager> materialManager, D3DGRM::D3DGpuTexture *windowBackBuffer[2],
                        D3DGRM::D3DGpuTexture *windowDepthStencilBuffer);

    virtual ~D3DOpaqueRenderPass() = default;

    // 여러가지 설정등
    virtual void BeginPass(ID3D12GraphicsCommandList *commandList,
                           const D3DRender::D3DRenderChannelFrameContext &channelFrameContext) override;

    // renderItem들을 받아서 실제 드로우
    virtual void Draw(ID3D12GraphicsCommandList *commandList,
                      const std::vector<Render::InternalRenderItem> &renderItems) override;

    // 어떤 리소스 상태전환등등
    virtual void EndPass(ID3D12GraphicsCommandList *commandList) override;

  private:
    // pso, rootsignature 비교후 바인딩
    void BindPSOIfNeeded(ID3D12GraphicsCommandList *commandList, const Render::InternalRenderItem *beforeRenderItem,
                         const Render::InternalRenderItem *currRenderItem,
                         const D3DMainRenderPassInfo *beforeMatMainPass, const D3DMainRenderPassInfo *currMatMainPass);

    void BindScissorRectIfNeeded(ID3D12GraphicsCommandList *commandList,
                                 const Render::InternalRenderItem *beforeRenderItem,
                                 const Render::InternalRenderItem *currRenderItem);

    void BindMeshBufferIfNeeded(ID3D12GraphicsCommandList *commandList,
                                const Render::InternalRenderItem *beforeRenderItem,
                                const Render::InternalRenderItem *currRenderItem);

    // 무조건 새롭게 바인딩 수행
    void BindShaderResources(ID3D12GraphicsCommandList *commandList, const Render::InternalRenderItem *currRenderItem,
                             const D3DMainRenderPassInfo *currMatMainPass);

    void DrawRenderItem(ID3D12GraphicsCommandList *commandList, const Render::InternalRenderItem *currRenderItem);

  private:
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    D3DGRM::D3DGpuResourceManager *mResourceManager;
    D3DGRM::D3DGpuTexture *mWindowBackBuffer[2];
    D3DGRM::D3DGpuTexture *mWindowDepthStencilBuffer;

    D3DRender::D3DRenderChannelFrameContext mChannelFrameContext;
};

} // namespace D3DRender
