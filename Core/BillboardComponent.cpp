#include "BillboardComponent.h"
#include <Core/IRenderProxyManager.h>
#include <Core/Object.h>
#include <Core/map.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Texture.h>

BillboardComponent::BillboardComponent()
    : mRenderProxy(std::make_unique<Core::BillboardRenderProxy>()), mSize(1.0f, 1.0f)
{
}

BillboardComponent::~BillboardComponent()
{
    UnRegisterProxy();
}

void BillboardComponent::OnOwnerObjectAddedToMap()
{
    SceneComponent::OnOwnerObjectAddedToMap();

    UpdateRenderProxy();
    Object *ownerObject = GetOwnerObject();
    if (ownerObject)
    {
        if (Map *map = ownerObject->GetMap())
        {

            mRenderProxy->mRenderID = map->GetRenderID();

            if (ownerObject->GetActive())
                RegisterProxy();
        }
    }
}

void BillboardComponent::UpdateRenderProxy()
{

    if (mRenderProxy == nullptr)
    {
        return;
    }

    mRenderProxy->mTexture = static_cast<CoreAsset::Texture *>(mTexture.Get());
    mRenderProxy->mTransform = GetTransformWorld();
    mRenderProxy->mSize = mSize;
    //    mRenderProxy->mSize;
}

void BillboardComponent::SetTexture(CoreAsset::Texture *texture)
{

    if (texture == nullptr)
        return;

    mTexture = texture;

    UpdateRenderProxy();
}

void BillboardComponent::SetTexture(const std::string &textureName)
{

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    CoreAsset::AssetPtr pTexture = assetManager->GetAsset<CoreAsset::Texture>(textureName.c_str());

    SetTexture(static_cast<CoreAsset::Texture *>(pTexture.Get()));
}

void BillboardComponent::SetTexture(CoreAsset::AssetID textureID)
{

    auto assetManager = CoreAsset::AssetManager::GetInstance();
    CoreAsset::AssetPtr pTexture = assetManager->GetAsset<CoreAsset::Texture>(textureID);

    SetTexture(static_cast<CoreAsset::Texture *>(pTexture.Get()));
}

void BillboardComponent::FlushPropertyDirty()
{
    SceneComponent::FlushPropertyDirty();

    UpdateRenderProxy();
}

void BillboardComponent::OnTransformChanged()
{

    SceneComponent::OnTransformChanged();

    if (mRenderProxy)
    {
        mRenderProxy->mTransform = GetTransformWorld();
    }
}

void BillboardComponent::OnActiveStateChanged(bool state)
{

    if (state)
    {
        RegisterProxy();
    }
    else
    {

        UnRegisterProxy();
    }
}

void BillboardComponent::SetSize(const CoreMath::Vector2 &size)
{

    mSize = size;
    MarkPropertyDirty();
}

void BillboardComponent::SetWidth(float w)
{

    mSize.X = w;
    MarkPropertyDirty();
}

void BillboardComponent::SetHeight(float h)
{
    mSize.Y = h;
    MarkPropertyDirty();
}

void BillboardComponent::EndTick(float deltaTime)
{
    SceneComponent::EndTick(deltaTime);

    if (GetPropertyDirty())
    {
        UpdateRenderProxy();
    }
}

void BillboardComponent::RegisterProxy()
{

    if (mProxyRegisterState)
        return;

    Core::IRenderProxyManager::GetInstance()->RegisterBillboardProxy(mRenderProxy.get());

    mProxyRegisterState = true;
}

void BillboardComponent::UnRegisterProxy()
{
    if (mProxyRegisterState == false)
        return;
    Core::IRenderProxyManager::GetInstance()->UnRegisterBillboardProxy(mRenderProxy.get());
    mProxyRegisterState = false;
}
