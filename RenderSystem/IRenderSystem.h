#pragma once

#include "RenderSystem/RenderSystemDllMacro.h"
#include "RenderSystem/RenderType.h"
#include <CoreDevice/CommandContext.h>
namespace Render
{

class RENDER_SYSTEM_API IRenderSystem
{
  public:
    virtual ~IRenderSystem();

    static IRenderSystem *GetInstance();
    static void SetRenderSystemImpl(IRenderSystem *pImpl);

    virtual void RegisterWindow(const CreationRenderChannelInfo &creationChannelInfo) = 0;

    // 가장 먼저호출할것
    virtual void StartWindow(Core::CommandContext *commandContext, WindowHandle windowHandle, int backBufferIndex) = 0;

    // StartWindow이후 각 채널별로 한번씩 호출
    virtual void BeginFrame(Render::RenderChannelID channelID, const Render::FrameContext &frameContext) = 0;

    // renderItem제출 (여러번호출가능 EndFrame전까지)
    virtual void SubmitRenderItems(Render::RenderChannelID channelID,
                                   const std::vector<Render::RenderItem> &renderItemVector) = 0;
    // virtual void SubmitRenderItems(Render::RenderChannelID channelID, std::vector<Render::RenderItem>&&
    // renderItemVector) = 0;

    // 최종실제 draw
    virtual void EndFrame(Render::RenderChannelID channelID) = 0;

    // 최종 출력 EndFrame이후 호출(한창의 여러개의 channel들의에대해 EndFrame까지 완료하고나면 호출)
    virtual void PresentWindow(Core::CommandContext *commandContext, WindowHandle windowHandle,
                               int backBufferIndex) = 0;

    // new current backbuffer index 리턴
    virtual int WindowResize(WindowHandle windowHandle) = 0;

#pragma region Refactoring

    virtual void StartFrame(WindowHandle windowHandle) = 0;
    // virtual void StartView(Render::RenderChannelID channelID) = 0;
    //   virtual void EndView(Render::RenderChannelID channelID) = 0;

    // StartView 호출후, Draw호출전, 전역옵션 설정
    virtual void SetUpPassData(Core::CommandContext *commandContext, const FrameContext &passData) = 0;
    virtual void SetUpStencilValue(Core::CommandContext *commandContext, uint32_t value) = 0;
    virtual void Draw(Core::CommandContext *commandContext, const std::vector<RenderItem> &renderItemList) = 0;
    virtual void DrawFullScreen(Core::CommandContext *commandContext, const Render::RenderItem &renderItem) = 0;

    virtual void ExecuteResourceBarriers(Core::CommandContext *commandContext,
                                         std::vector<ResourceBarrier> &resourceBarriers) = 0;
    virtual void ExecuteResourceBarrier(const ResourceBarrier &resourceBarrier) = 0;

    virtual void ClearRenderTarget(Core::CommandContext *commandContext, GRM::IGpuResource *renderTarget,
                                   float color[4], RECT rect) = 0;
    virtual void ClearDepthStencil(Core::CommandContext *commandContext, GRM::IGpuResource *renderTarget, float value,
                                   RECT rect) = 0;
    virtual void SetRenderTarget(Core::CommandContext *commandContext, GRM::IGpuResource *rendertarget,
                                 GRM::IGpuResource *depthStenci) = 0;

#pragma endregion

  protected:
    IRenderSystem();

  private:
    static IRenderSystem *mImpl;
};

} // namespace Render