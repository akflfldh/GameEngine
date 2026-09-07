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
    void SetColor(const UI::UIColor &color);

    glm::vec3 GetColor() const;

    void SetTexture(CoreAsset::AssetID id);
    void SetTexture(const char *path);
    void UseTexture(bool flag);
    UI::UIImageComponent *mImageCom;

  protected:
    virtual void ApplyLayoutStyle(const UIControlStyle &style) override;
    // style일변화 , hover,등 상태변화 에서 호출
    virtual void ApplyVisualStyle(const UIControlStyle &style, EUIVisualState visualState) override;

  private:
};

} // namespace UI
