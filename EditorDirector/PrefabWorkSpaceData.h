#pragma once

#include <EditorDirector/ComponentScrollPanel.h>

namespace UI
{
class UICanvas;
class UIImage;
class UIButton;

} // namespace UI

class Object;
class UIReflectPanel;
class BaseSelectionManager;
class UISearchSelectBox;

using OnRequestAddComponentCallbackSystem = Core::MultiCallbackSystem<const std::string &>;

namespace Quad
{

class PrefabComponentPanel
{
  public:
    PrefabComponentPanel();
    ~PrefabComponentPanel();

    void Initialize(UI::UICanvas *canvas, BaseSelectionManager *selectionManager);
    void Update(float DeltaTime);

    void SetPositionLocal(float x, float y);
    void SetObject(Object *object);

    void OnSelectedComponent(Component *com);

    UI::UIImage *mTopPanel = nullptr;
    ComponentScrollPanel mComponentScrollPanel;
    UI::UIButton *mComponentGenerationButton = nullptr;

    OnRequestAddComponentCallbackSystem mOnRequestAddComponentCallbackSystem;

  private:
    void RebuildComponentList();
    void CreateComponentAddSearchSelectBox(UI::UIElement *parent, float posX, float posY);
    void OnClickedComponentGenerationButton();

    void AddComponentToPrefab(const std::string &componentName);

  private:
    UI::UICanvas *mCanvas = nullptr;
    Object *mTargetObject = nullptr;
    UISearchSelectBox *mComponentAddSearchSelectBox = nullptr;
    std::vector<std::string> mComponentList;
    uint32_t mSelectedComponentItemID = UINT_MAX;
};

class PrefabPropertyPanel
{
  public:
    PrefabPropertyPanel();
    ~PrefabPropertyPanel();

    void Initialize(UI::UICanvas *canvas);
    void SetPositionLocal(float x, float y);
    void SetObject(Object *object);
    void OnSelectedComponent(Component *com);

  private:
    UIReflectPanel *mReflectPanel = nullptr;
};

} // namespace Quad