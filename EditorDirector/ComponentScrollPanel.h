#pragma once

#include <CoreBase/CallbackSystem.h>
#include <string>
#include <vector>

namespace UI
{
class UIImage;
class UICanvas;
class UIElement;
} // namespace UI

class Object;
class UIScrollBox;
class UIHierarchyItem;
class Component;
class BaseSelectionManager;
class UIDragSourceComponent;
class UIDropTargetComponent;
class ComponentItem;
class SceneComponent;

using OnClickedComponentCallbackSystem = Core::MultiCallbackSystem<Component *>;

//<target com, source com>
using OnDroppedComponentItemCallbackSystem = Core::MultiCallbackSystem<Component *, Component *>;

class ComponentItem
{
  public:
    ComponentItem();
    ~ComponentItem();

    void Initialize(UI::UICanvas *canvas, float width, float height);

    // HierarchyItem의 게층구조가 끊어진다.
    void Reset();
    UIHierarchyItem *GetUIItem() const;
    void SetText(const std::string &text);
    void SetActive(bool flag);

    void SetComponent(Component *com);
    Component *GetComponent() const;

    void UpdateVisual(bool bSelect);

    OnDroppedComponentItemCallbackSystem mDroppedComponentItemCallbackSystem;

  private:
    Component *mComponent = nullptr;
    UIHierarchyItem *mUIItem = nullptr;
    UIDragSourceComponent *mDragSourceComponent = nullptr;
    UIDropTargetComponent *mDropTargetComponent = nullptr;
};

class ComponentScrollPanel
{

  public:
    ComponentScrollPanel();
    ~ComponentScrollPanel();

    void Initialize(UI::UICanvas *canvas, BaseSelectionManager *selectionManager, float scrollBoxWidth,
                    float scrollBoxHeight);

    void Update(float DeltaTime);

    void SetObject(Object *object);

    void SetUIParent(UI::UIElement *parent);
    void SetPositionLocal(float x, float y);

    // OnClickedComponentCallbackSystem mOnClickedComponentCallbackSystem;

    void OnSelectedComponent(Component *com);

  private:
    void Refresh();
    void BindObjectComponentEvents(Object *object);
    void UnbindObjectComponentEvents();

    void OnObjectComponentAdded(Component *com);
    void OnObjectComponentRemoved(Component *com);

    ComponentItem *GetAvailableItem();

    ComponentItem *CreateItem();

    // scrollbox에존재하는 모든 아이템을 제거및 초기화
    void ResetScrollBox();

    void OnClickedComponentItem(ComponentItem *componentItem);

    void ChangeToSelectedComponentState(ComponentItem *comItem, bool state);

    void OnDroppedComponent(Component *targetComponent, Component *sourceComponent);
    bool CanReparentComponent(SceneComponent *childComponent, SceneComponent *parentComponent) const;

  private:
    Object *mObject = nullptr;
    UI::UICanvas *mCanvas = nullptr;
    //  UI::UIImage *mTopPanel = nullptr;
    UIScrollBox *mScrollBox = nullptr;
    std::vector<ComponentItem *> mComponentItemList;
    std::vector<ComponentItem *> mComponentItemPool;

    float mItemHeight;

    Component *mCurrentSelectedComponent = nullptr;

    BaseSelectionManager *mSelectionManager = nullptr;

    Core::CallbackID mComponentAddedCallbackID;
    Core::CallbackID mComponentRemovedCallbackID;
};
