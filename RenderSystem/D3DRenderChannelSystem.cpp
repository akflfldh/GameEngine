#include "D3DRenderChannelSystem.h"

D3DRender::D3DRenderChannelSystem::D3DRenderChannelSystem(const Render::CreationRenderChannelInfo& creationChannelInfo)
	:mWindowHandle(creationChannelInfo.mWindowHandle), mViewport({creationChannelInfo.mLeft,creationChannelInfo.mTop,
		creationChannelInfo.mWidth,creationChannelInfo.mHeight,creationChannelInfo.mMinZ,creationChannelInfo.mMaxZ})
{



}

D3DRender::D3DRenderChannelSystem::~D3DRenderChannelSystem()
{
}
