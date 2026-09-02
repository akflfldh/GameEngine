#pragma once

#include "UiSystem/UIRenderableComponent.h"

#include "UIImageComponent.generated.h"

namespace UI
{

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIImageComponent : public UIRenderableComponent
{
    GENERATED_BODY(UIImageComponent)

  public:
    UIImageComponent();
    virtual ~UIImageComponent();

    void SetTexture(CoreAsset::Texture *texture);
    void SetTexture(const std::string &path);
    void SetTexture(CoreAsset::AssetID id);
    void SetTemplateMaterial(CoreAsset::AssetID texID);

    // buffer의 사이즈는 4개이여야 한다.
    inline virtual size_t GetVertexNum() const override;
    virtual uint32_t GetVertices(UIVertex *oUIVertices) const override;

    inline virtual size_t GetIndexNum() const override;
    virtual void GetIndices(uint32_t *oIndices) const override;

    void UseTexture();
    void NotUseTexture();

    void SetColor(float r, float g, float b);
    void SetColor(glm::vec3 color);
    void SetColor(const UI::UIColor &color);
    glm::vec3 GetColor() const;

  private:
    bool mUseTexture;

    float mColorR;
    float mColorG;
    float mColorB;
};

} // namespace UI