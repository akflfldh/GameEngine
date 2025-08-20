#pragma once

#include "RenderSystem/D3DRenderType.h"
#include <d3d12.h>
#include <memory>
#include <wrl.h>

namespace D3DRender
{
class D3DMaterialManager;
class ID3DRenderPass
{
  public:
    ID3DRenderPass(std::shared_ptr<D3DRender::D3DMaterialManager>);
    virtual ~ID3DRenderPass() = 0;

    // 여러가지 설정등
    virtual void BeginPass(ID3D12GraphicsCommandList *commandList,
                           const D3DRenderChannelFrameContext &frameContext) = 0;

    // renderItem들을 받아서 실제 드로우
    virtual void Draw(ID3D12GraphicsCommandList *commandList,
                      const std::vector<Render::InternalRenderItem> &renderItems) = 0;

    // 어떤 리소스 상태전환등등
    virtual void EndPass(ID3D12GraphicsCommandList *commandList) = 0;

  protected:
    std::shared_ptr<D3DRender::D3DMaterialManager> mMaterialManager;
};

} // namespace D3DRender
