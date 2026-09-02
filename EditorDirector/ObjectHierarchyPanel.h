#pragma once
#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIElement.h>
#include <UiSystem/UIElementPtr.h>

#include <ObjectHierarchyPanel.generated.h>

class Map;
class Object;
using OnSelectedObjectCallbackSystem = Core::MultiCallbackSystem<Object *>;

class UIScrollBox;
class BaseSelectionManager;

namespace UI
{
class UIImageComponent;
class UIVerticalLayoutComponent;
} // namespace UI

class ObjectHierarchyItem;

class REFLECT_CLASS(EngineClass) ObjectHierarchyPanel : public UI::UIElement
{
    GENERATED_BODY(ObjectHierarchyPanel)
  public:
    ObjectHierarchyPanel();
    virtual ~ObjectHierarchyPanel();

    void Initialize(BaseSelectionManager *selectionManager);

    void InitMap(Map *map);

    virtual void OnBegin() override;

    // void SetScrollPanelPosition(float x, float y);
    void SetScrollPanelSize(float w, float h);

    void SetScrollPanelColor(float r, float g, float b);

    void InitList();

    void OnAddedObjectToMap(Object *object);
    void OnRemovedObjectFromMap(Object *object);

    void OnSelectedObject(Object *object);

    // bNotify : Object선택에대한 SelectionManager에게 알림여부
    void OnSelectedItem(ObjectHierarchyItem *item, bool bNotify = true);
    virtual void OnKeyDown(const Quad::EKeyCode &key) override;

    // 선택됬을때 호출
    OnSelectedObjectCallbackSystem mOnSelectedObjectCallbackSystem;
    // 선택된 오브젝트가 제거되었을때 호출
    OnSelectedObjectCallbackSystem mOnSelectedObjectRemovedCallbackSystem;

    void ChangeObjectParent(ObjectHierarchyItem *childItem, ObjectHierarchyItem *newParentItem);

#pragma region Input
    // virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;

#pragma endregion

  private:
    // Pool에없다면 생성
    ObjectHierarchyItem *GetObjectHierarchyItem();
    // Pool에 반납
    void ReleaseObjectHierarchyItem(ObjectHierarchyItem *newtem);

    void UpdateItemVisual(ObjectHierarchyItem *newItem);

    ObjectHierarchyItem *FindObjectItem(Object *object);

  private:
    UIScrollBox *mScrollPanel;
    UI::UIImageComponent *mImageCom;
    UI::UIVerticalLayoutComponent *mVerticalLayoutCom;

    std::vector<ObjectHierarchyItem *> mItemPool;

    Map *mMap = nullptr;

    Core::CallbackID mAddedCallbackID = Core::CallbackIDNone;
    Core::CallbackID mRemovedCallbackID = Core::CallbackIDNone;
    ObjectHierarchyItem *mSelectedItem;
    Object *mSelectedObject;

    // SelectedItem 재설정 더티플래그
    bool bSelectedItemDirty;

    BaseSelectionManager *mSelectionManager = nullptr;
};
