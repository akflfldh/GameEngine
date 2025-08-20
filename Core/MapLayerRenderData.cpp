#include "MapLayerRenderData.h"
#include "ViewportController.h"

#include "Asset/Texture/RenderTargetTexture.h"
#include "Effect/Effect.h"
#include "ResourceManager/TextureManager/TextureManager.h"

Quad::MapLayerRenderData::MapLayerRenderData() : mDefaultRenderTarget(nullptr), mDefaultDepthStencilBuffer(nullptr) {}

Quad::MapLayerRenderData::~MapLayerRenderData()
{

    // effect처리
}

void Quad::MapLayerRenderData::SetDefaultRenderTarget(RenderTargetTexture *defaultRenderTarget)
{

    mDefaultRenderTarget = defaultRenderTarget;
}

void Quad::MapLayerRenderData::SetDefaultDepthStencilBuffer(Texture *defaultDepthStencilBuffer)
{
    mDefaultDepthStencilBuffer = defaultDepthStencilBuffer;
}

void Quad::MapLayerRenderData::AddEffect(Effect *effect)
{
    if (effect == nullptr)
        return;

    if (GetEffect(effect->GetName()) != nullptr)
        return;

    mEffectTable[effect->GetName()] = effect;
}

void Quad::MapLayerRenderData::SetViewport(D3D12_VIEWPORT viewport)
{

    mViewport = viewport;
}

void Quad::MapLayerRenderData::SetViewportGlobal(D3D12_VIEWPORT viewport)
{
    mViewportGlobal = viewport;
}

Quad::RenderTargetTexture *Quad::MapLayerRenderData::GetDefaultRenderTarget() const
{
    return mDefaultRenderTarget;
}

Quad::Texture *Quad::MapLayerRenderData::GetDefaultDepthStencilBuffer() const
{
    return mDefaultDepthStencilBuffer;
}

D3D12_VIEWPORT Quad::MapLayerRenderData::GetViewport() const
{

    return mViewport;
}

D3D12_VIEWPORT Quad::MapLayerRenderData::GetViewportGlobal() const
{

    return mViewportGlobal;
}

Quad::Effect *Quad::MapLayerRenderData::GetEffect(const std::string &name) const
{

    EffectTable::const_iterator it = mEffectTable.find(name);

    return it == mEffectTable.end() ? nullptr : it->second;
}

void Quad::MapLayerRenderData::OnResize(UINT clientWidth, UINT clientHeight)
{
    // clientWidth,clientHeight는 이 mapLayer가 속한 system의 크기

    // RenderTarget, depthStencilBuffer 사이즈도  새롭게 설정
    // 이미 mapLayer별로 viewport,viewportGlobal의 값은 새롭게 설정된상태

    mViewportGlobal.Width = clientWidth;
    mViewportGlobal.Height = clientHeight;

    if (mDefaultRenderTarget)
    {
        TextureManager::ResizeTexture(mDefaultRenderTarget, mViewportGlobal.Width, mViewportGlobal.Height);
    }

    if (mDefaultDepthStencilBuffer)
    {
        TextureManager::ResizeTexture(mDefaultDepthStencilBuffer, mViewportGlobal.Width, mViewportGlobal.Height);
    }

    for (auto &effectElement : mEffectTable)
    {
        Effect *effect = effectElement.second;

        effect->OnResize(clientWidth, clientHeight);
    }
}

void Quad::MapLayerRenderData::ResetEffectResource()
{
    // effect들 shaderResourceRest

    for (auto &effectElement : mEffectTable)
    {
        Effect *effect = effectElement.second;

        effect->ResetRenderPassShaderResource();
    }
}
