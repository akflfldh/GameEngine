#pragma once

#include <UiSystem/UIImage.h>

#include "UIProjectItem.generated.h"

class REFLECT_CLASS(EngineClass) UIProjectItem : public UI::UIImage
{
    GENERATED_BODY(UIProjectItem)
  public:
    UIProjectItem();
    virtual ~UIProjectItem();

  protected:
    virtual void OnBegin() override;

  private:
};
