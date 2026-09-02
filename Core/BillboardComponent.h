#pragma once

#include <Core/SceneComponent.h>
#include <CoreAsset/AssetPtr.h>
#include <memory>
#include <string>

#include "BillboardComponent.generated.h"

namespace Core
{

class BillboardRenderProxy;

}
namespace CoreAsset
{
class Texture;
class AssetPtr;
} // namespace CoreAsset

class CORE_API_LIB REFLECT_CLASS(EngineClass) BillboardComponent : public SceneComponent
{
    GENERATED_BODY(BillboardComponent)
  public:
    BillboardComponent();
    virtual ~BillboardComponent();

    void SetTexture(CoreAsset::Texture *texture);
    void SetTexture(const std::string &textureName);
    void SetTexture(CoreAsset::AssetID textureID);

    void SetSize(const CoreMath::Vector2 &size);
    void SetWidth(float w);
    void SetHeight(float h);

    virtual void OnTransformChanged() override;
    virtual void FlushPropertyDirty() override;

  protected:
    virtual void OnOwnerObjectAddedToMap() override;
    virtual void OnActiveStateChanged(bool state) override;

    virtual void EndTick(float DeltaTime) override;

  private:
    void UpdateRenderProxy();

    void RegisterProxy();
    void UnRegisterProxy();

  private:
    std::unique_ptr<Core::BillboardRenderProxy> mRenderProxy;
    CoreAsset::AssetPtr mTexture;

    CoreMath::Vector2 mSize;

    bool mProxyRegisterState = false;
};
