#pragma once

#include "UiSystem/UIRenderableComponent.h"

#include "UiSystem/Generated/UIImageComponent.generated.h"

namespace UI
{

class UISYSTEM_API REFLECT_CLASS UIImageComponent : public UIRenderableComponent
{
    GENERATED_BODY(UIImageComponent)

  public:
    UIImageComponent();
    virtual ~UIImageComponent();
    void SetTexture(CoreAsset::Texture *texture);
    void SetTemplateMaterial(CoreAsset::AssetID texID);

    // buffer의 사이즈는 4개이여야 한다.
    inline virtual size_t GetVertexNum() const override;
    virtual void GetVertices(UIVertex *oUIVertices) const override;

    inline virtual size_t GetIndexNum() const override;
    virtual void GetIndices(uint32_t *oIndices) const override;






  private:
};

} // namespace UI