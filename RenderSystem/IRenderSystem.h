#pragma once

#include "RenderSystem/RenderSystemDllMacro.h"
#include "RenderSystem/RenderType.h"

namespace Render
{

class RENDER_SYSTEM_API IRenderSystem
{
  public:
    virtual ~IRenderSystem();

    static IRenderSystem *GetInstance();
    static void SetRenderSystemImpl(IRenderSystem *pImpl);

    virtual RenderChannelID RegisterRenderChannel(const CreationRenderChannelInfo &creationChannelInfo) = 0;

    // 가장 먼저호출할것
    virtual void StartWindow(WindowHandle windowHandle) = 0;

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
    virtual void PresentWindow(WindowHandle windowHandle) = 0;

    virtual void WindowResize(WindowHandle windowHandle) = 0;

  protected:
    IRenderSystem();

  private:
    static IRenderSystem *mImpl;
};

} // namespace Render