#include "Asset/Texture/RenderTargetTexture.h"

Quad::RenderTargetTexture::RenderTargetTexture(const std::string& name)
	:Texture(name)
{
}

void Quad::RenderTargetTexture::SetRenderTargetViewIndex(ViewIndex index)
{
	mRenderTargetViewIndex = index;
}

ViewIndex Quad::RenderTargetTexture::GetRendedrTargetViewIndex() const
{
	return mRenderTargetViewIndex;
}
