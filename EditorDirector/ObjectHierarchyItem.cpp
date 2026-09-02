#include "ObjectHierarchyItem.h"
#include <UiSystem/UIVerticalLayoutComponent.h>

#include <UIDragSourceComponent.h>
#include <UIDropTargetComponent.h>

#include <UiSystem/UITextButton.h>
ObjectHierarchyItem::ObjectHierarchyItem() {}

ObjectHierarchyItem::~ObjectHierarchyItem() {}

void ObjectHierarchyItem::OnBegin()
{

    UIHierarchyItem::OnBegin();

    mDropTargetComponent = mHeaderPanel->CreateUIComponent<UIDropTargetComponent>("DropTargetCom");
    mDragSourceComponent = mHeaderPanel->CreateUIComponent<UIDragSourceComponent>("DragSourceCom");

    // mDropTargetComponent->SetPreviewInputListener(true);
    mDragSourceComponent->SetPreviewInputListener(true);

    mDropTargetComponent->SetDepthValue(1);
    mDragSourceComponent->SetDepthValue(1);

    mDropTargetComponent->SetDragDropPayloadType(EDragDropType::eObjectItem);

    DragPayload dragPayload;
    dragPayload.mType = EDragDropType::eObjectItem;
    dragPayload.mUIElement = this;
    mDragSourceComponent->SetPayload(dragPayload);

    mDropTargetComponent->mOnDroppedPayloadCallbackSystem.Register(
        [this](const DragPayload &payload)
        {
            ObjectHierarchyItem *item = dynamic_cast<ObjectHierarchyItem *>(payload.mUIElement);
            if (item)
            {
                mOnDroppedObjectItemCallbackSystem.ExecuteCallbacks(item);
            }
        });
    ;
}

void ObjectHierarchyItem::SetObject(Object *object)
{

    mObject = object;
}

Object *ObjectHierarchyItem::GetObject() const
{
    return mObject;
}
