#pragma once

#include "RenderSystem/RenderType.h"

#ifdef D3DX

#include "RenderSystem/D3DRenderType.h"
#include "RenderSystem/ID3DRenderPass.h"
#include <d3d12.h>
#include <memory>
#include <vector>
#include <wrl.h>

namespace D3DGRM
{
class D3DGpuResourceManager;
}

namespace D3DRender
{
class ID3DRenderPass;

class D3DRenderChannelSystem
{
  public:
    D3DRenderChannelSystem(const Render::CreationRenderChannelInfo &creationChannelInfo,
                           Microsoft::WRL::ComPtr<ID3D12Device> device,
                           Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
                           D3DGRM::D3DGpuResourceManager *gpuResourceManager,
                           std::vector<std::unique_ptr<ID3DRenderPass>> renderPipeline);
    ~D3DRenderChannelSystem();

    // BeginFrame - SubmitRenderItem - EndFrame 순으로 호출

    // 기본셋팅, + 각 렌더패스 의존성설정
    void BeginFrame(const Render::FrameContext &frameContext);

    // 렌더아이템제출밑 정리
    void SubmitRenderItem(const std::vector<Render::InternalRenderItem> &renderItemVector);
    void SubmitRenderItem(std::vector<Render::InternalRenderItem> &&renderItemVector);

    // 최종 렌더수행
    // 렌더패스를 순회하면서 렌더작업수행
    void EndFrame();

  private:
    HWND mWindowHandle;
    D3D12_VIEWPORT mViewport;

    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

    D3DGRM::D3DGpuResourceManager *mGpuResourceManager;

    // 렌더패스 리스트를 가져야하고
    // 순서대로 랜더패스를 수행한다
    // 최종적으로 창의 백버퍼에 복사가되든,드로우가되든 해야한다.
    D3DRenderChannelFrameContext mChannelFrameContext;
    std::vector<std::unique_ptr<ID3DRenderPass>> mRenderPassVector;

    std::vector<Render::InternalRenderItem> mRenderItemVector;
};
} // namespace D3DRender

#endif
