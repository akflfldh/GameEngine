#pragma once

#include <EditorDirector/UIHierarchyItem.h>

#include <CoreBase/CallbackSystem.h>

#include "ObjectHierarchyItem.generated.h"

class ObjectHierarchyItem;

using OnDroppedObjectItemCallbackSystem = Core::MultiCallbackSystem<ObjectHierarchyItem *>;

class Object;
class UIDropTargetComponent;
class UIDragSourceComponent;

namespace UI
{
class UIVerticalLayoutComponent;
}

class REFLECT_CLASS(EngineClass) ObjectHierarchyItem : public UIHierarchyItem
{
    GENERATED_BODY(ObjectHierarchyItem)
  public:
    ObjectHierarchyItem();
    virtual ~ObjectHierarchyItem();

    void OnBegin() override;

    void SetObject(Object *object);
    Object *GetObject() const;

    OnDroppedObjectItemCallbackSystem mOnDroppedObjectItemCallbackSystem;

    void OnSelectedState(bool state);

  protected:
    // style 변화에서 호출
    virtual void ApplyLayoutStyle(const UI::UIControlStyle &style) override;

    // style일변화 , hover,등 상태변화 에서 호출
    virtual void ApplyVisualStyle(const UI::UIControlStyle &style, UI::EUIVisualState state) override;

  private:
    Object *mObject;
    UIDropTargetComponent *mDropTargetComponent;
    UIDragSourceComponent *mDragSourceComponent;
};
