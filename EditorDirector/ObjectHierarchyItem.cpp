#include "ObjectHierarchyItem.h"
#include <UiSystem/UIVerticalLayoutComponent.h>

#include <UIDragSourceComponent.h>
#include <UIDropTargetComponent.h>

#include <Core/Object.h>
#include <EditorDirector/EditorObjectIconResolver.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UIImageComponent.h>
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

    mIconImage->UseTexture(true);
}

void ObjectHierarchyItem::SetObject(Object *object)
{

    mObject = object;
    if (mObject)
    {

        mIconImage->SetTexture(EditorObjectIconResolver::Resolve(*object));
    }
}

Object *ObjectHierarchyItem::GetObject() const
{
    return mObject;
}

// style 변화에서 호출
void ObjectHierarchyItem::ApplyLayoutStyle(const UI::UIControlStyle &style)
{

    UIHierarchyItem::ApplyLayoutStyle(style);
}

// style일변화 , hover,등 상태변화 에서 호출
void ObjectHierarchyItem::ApplyVisualStyle(const UI::UIControlStyle &style, UI::EUIVisualState state)
{

    UIHierarchyItem::ApplyVisualStyle(style, state);
}

void ObjectHierarchyItem::OnSelectedState(bool state)
{

    if (mHeaderPanel)
    {
        mHeaderPanel->mUIButtonComponent->SetSelected(state);
    }
}