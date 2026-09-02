#pragma once
#include "UIFoldoutPanel.generated.h"
#include <EditorDirector/IPropertyBindable.h>
#include <EditorDirector/UIFoldoutPanel.h>

class REFLECT_CLASS(EngineClass) UIReflectVectorPanel : public UIFoldoutPanel, public IPropertyBindable
{
    GENERATED_BODY(UIReflectVectorPanel)

  public:
    UIReflectVectorPanel();
    virtual ~UIReflectVectorPanel();

    virtual void OnBegin() override;
    virtual void Update(float deltaTime) override;

    virtual void BindProperty(void *targetMemory, Quad::PropertyInfo *property) override;
    virtual void Release() override;

  private:
    void *mTargetMemory;
    std::vector<UI::UIElement *> mElementPanelList;
};
