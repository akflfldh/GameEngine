#include "Asset/Texture/RenderTargetTexture.h"

Quad::RenderTargetTexture::RenderTargetTexture(const std::string& name)
	:Texture(name)
{
	SetTextureType(ETextureType::eRenderTargetTexture);
}

void Quad::RenderTargetTexture::SetRenderTargetViewIndex(ViewIndex index)
{
	mRenderTargetViewIndex = index;
}

ViewIndex Quad::RenderTargetTexture::GetRendedrTargetViewIndex() const
{
	return mRenderTargetViewIndex;
}
