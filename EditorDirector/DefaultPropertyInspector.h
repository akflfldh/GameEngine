#pragma once
#include <EditorDirector/IInspector.h>

class PropertyPanel;

namespace UI
{
class UICanvas;
}

class DefaultPropertyInspector : public IInspector
{
  public:
    static DefaultPropertyInspector *GetInstance();

    DefaultPropertyInspector();
    virtual ~DefaultPropertyInspector();

    void Initialize(UI::UICanvas *canvas);
    virtual void BeginUI() override;

    virtual void ActivateInspector() override;
    virtual void DeActivateInspector() override;
    virtual void SetInspectorContext(const InspectorContext &inspectorContext) override;

  private:
    UI::UICanvas *mCanvas = nullptr;
    PropertyPanel *mPropertyPanel = nullptr;
};
