#pragma once

#include <string>
#include <vector>

namespace UI
{
class UIElement;
}

namespace Quad
{
struct PropertyInfo;
}

class UIReflectSinglePrimitivePanel;
class UIReflectVector3Panel;
class UIReflectBoolPanel;
class UIReflectVectorPanel;
class StaticMeshComponentUIReflectPanel;

class UIReflectPanelFactory
{
  public:
    static UIReflectPanelFactory *GetInstance();
    UIReflectPanelFactory();
    ~UIReflectPanelFactory();
    std::vector<UI::UIElement *> GetReflectPanel(void *targetMemory, UI::UIElement *parentElement,
                                                 Quad::PropertyInfo *property, const std::string &tagName);

    void ReleaseReflectPanel(UI::UIElement *element);

    StaticMeshComponentUIReflectPanel *GetStaticMeshPanel(UI::UIElement *parentElement);

  private:
    void ActivatePanel(UI::UIElement *element, UI::UIElement *parentElement);

  private:
    std::vector<UIReflectSinglePrimitivePanel *> mSinglePrimitivePanelPool;
    std::vector<UIReflectVector3Panel *> mVector3PanelPool;
    std::vector<UIReflectBoolPanel *> mBoolPanelPool;
    std::vector<UIReflectVectorPanel *> mVectorPanelPool;
    std::vector<StaticMeshComponentUIReflectPanel *> mStaticMeshPanelPool;
};
