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

  private:
    Object *mObject;
    UIDropTargetComponent *mDropTargetComponent;
    UIDragSourceComponent *mDragSourceComponent;
};
