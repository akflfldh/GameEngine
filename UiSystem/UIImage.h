#pragma once

#include "UiSystem/UIElement.h"

namespace CoreAsset
{
class Texture;
}

namespace UI
{

class UIRenderableComponent;
class UISYSTEM_API UIImage : public UIElement
{
  public:
    UIImage(UIElementID id, const std::string &name);
    virtual ~UIImage();

    virtual void Begin() override;
    virtual void Update(float deltaTime) override;

    UIRenderableComponent *mRenderableComponent;

  protected:
  private:
};

} // namespace UI
