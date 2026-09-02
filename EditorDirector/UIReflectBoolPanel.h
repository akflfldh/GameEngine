#pragma once

#include <EditorDirector/IPropertyBindable.h>
#include <EditorDirector/UIBoolPanel.h>

#include "UIReflectBoolPanel.generated.h"

class REFLECT_CLASS(EngineClass) UIReflectBoolPanel : public UIBoolPanel, public IPropertyBindable
{
    GENERATED_BODY(UIReflectBoolPanel)
  public:
    UIReflectBoolPanel();
    virtual ~UIReflectBoolPanel();
    virtual void OnBegin() override;
    void BindProperty(void *targetMemory, Quad::PropertyInfo *propertyInfo) override;
    virtual void Release() override;

    void ClearBindCallback();

  private:
    void *mTargetMemory;
    Quad::PropertyInfo *mPropertyInfo;
};
