#pragma once

#include "UiSystem/UIElement.h"

#include "UIImage.generated.h"

namespace UI
{

class UIImageComponent;

class UISYSTEM_API REFLECT_CLASS(EngineClass) UIImage : public UIElement
{

    GENERATED_BODY(UIImage)
  public:
    UIImage();
    virtual ~UIImage();

    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    void SetColor(float r, float g, float b);
    void SetColor(glm::vec3 color);
    glm::vec3 GetColor() const;

    void SetTexture(CoreAsset::AssetID id);
    void UseTexture(bool flag);
    UI::UIImageComponent *mImageCom;

  protected:
  private:
};

} // namespace UI
