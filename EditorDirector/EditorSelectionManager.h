#pragma once

#include <CoreBase/CallbackSystem.h>

class Object;
class Component;

using OnSelectedObjectCallbackSystem = Core::MultiCallbackSystem<Object *>;
using OnSelectedComponentCallbackSystem = Core::MultiCallbackSystem<Component *>;

class BaseSelectionManager
{
  public:
    BaseSelectionManager();
    ~BaseSelectionManager();

    void SetSelectedObject(Object *object);
    void OnMapObjectRemoved(Object *object);

    Object *GetSelectedObject() const;
    Component *GetSelectedComponent() const;

    // 해당 com만 선택처리(Onwer 오브젝트는 동일하게처리된다 SetSelectedObject동일하게,오브젝트,컴포넌트 두 콜백이 모두
    // 호출된다)
    void SetSelectedComponent(Component *com);

    void ClearSelection();

    OnSelectedObjectCallbackSystem mOnSelectedObjectCallbackSystem;
    OnSelectedComponentCallbackSystem mOnSelectedComponentCallbackSystem;

  private:
    Object *mSelectedObject = nullptr;
    Component *mSelectedComponent = nullptr;
};

class EditorSelectionManager : public BaseSelectionManager
{
  public:
    static EditorSelectionManager *GetInstance();
    EditorSelectionManager();
    ~EditorSelectionManager();

    //  void SetSelectedObject(Object *object);
    //  void OnMapObjectRemoved(Object *object);

    //  // 해당 com만 선택처리(Onwer 오브젝트는 동일하게처리된다 SetSelectedObject동일하게,오브젝트,컴포넌트 두 콜백이
    //  모두
    //  // 호출된다)
    //  void SetSelectedComponent(Component *com);

    //  OnSelectedObjectCallbackSystem mOnSelectedObjectCallbackSystem;
    //  OnSelectedComponentCallbackSystem mOnSelectedComponentCallbackSystem;

    // private:
    //   Object *mSelectedObject;

    // true이면 선택된 object의 component
    // bool mComponentSelectMode = false;
};

class EditorPrefabSelectionManager : public BaseSelectionManager
{

  public:
    static EditorPrefabSelectionManager *GetInstance();
    EditorPrefabSelectionManager();
    ~EditorPrefabSelectionManager();
};

class EditorMaterialSelectionManager : public BaseSelectionManager
{
  public:
    static EditorMaterialSelectionManager *GetInstance();
    EditorMaterialSelectionManager();
    ~EditorMaterialSelectionManager();

  private:
};
