#pragma once

#include "RenderSystem/RenderType.h"
#include <d3d12.h>

namespace D3DRender
{

D3D12_VIEWPORT ConvertToD3DViewport(const Render::Viewport &viewport);

// 채널별 한프레임 렌더문맥
// 해당프레임에서 렌더채널들이 렌더패스들에게 넘겨줄 정보
struct D3DRenderChannelFrameContext
{
    Render::FrameContext mFrameContext;
    int mCurrentWindowBackBufferIndex = 0;
    float mDepthClearValue = 1.0f;
    int mStencilClearValue = 0;
};

} // namespace D3DRender