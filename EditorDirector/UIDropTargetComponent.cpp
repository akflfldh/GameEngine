#include "UIDropTargetComponent.h"
#include <GlobalOverlayManager.h>
UIDropTargetComponent::UIDropTargetComponent() : mHover(false) {}

UIDropTargetComponent::~UIDropTargetComponent() {}

void UIDropTargetComponent::Update(float deltaTime) {}

int UIDropTargetComponent::IsPointInside(float x, float y) const
{
    return IsPointInsideDefault(x, y);
}

bool UIDropTargetComponent::IsHovered() const
{
    return mHover;
}

void UIDropTargetComponent::OnHover(int x, int y)
{

    mHover = true;
}

void UIDropTargetComponent::OnReleaseHover()
{

    mHover = false;
}

void UIDropTargetComponent::OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                      bool &bConsume)
{
    // auto overlayManager = GlobalOverlayManager::GetInstance();

    // if (mHover)
    //{
    //     // TODO
    //     if (overlayManager->GetDragDropState())
    //     { // 콜백호출

    //        // globlaOverlay- >Clear

    //        const DragPayload &payload = overlayManager->GetCurrentDragPayload();

    //        if (payload.mType == mDragPayloadType)
    //        {

    //            mOnDroppedPayloadCallbackSystem.ExecuteCallbacks(payload);

    //            overlayManager->ClearDragDrop();
    //        }
    //        bConsume = true;

    //        return;
    //    }
    //}
}

void UIDropTargetComponent::SetDragDropPayloadType(EDragDropType type)
{

    mDragPayloadType = type;
}
void UIDropTargetComponent::OnDragDropEnter(const DragPayload &payload)
{

    if (mDragPayloadType != payload.mType)
        return;

    mDragHover = true;
    // high light
    // callback
}

void UIDropTargetComponent::OnDragDropLeave(const DragPayload &payload)
{

    if (mDragHover == false)
        return;

    mDragHover = false;
    // high light 해제
    // callback
}

void UIDropTargetComponent::OnDroppedPayload(const DragPayload &payload)
{

    mOnDroppedPayloadCallbackSystem.ExecuteCallbacks(payload);
}