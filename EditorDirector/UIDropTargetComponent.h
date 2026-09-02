#pragma once

#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/GlobalOverlayType.h>
#include <UiSystem/IUIComponent.h>

#include "UIDropTargetComponent.generated.h"

using OnDroppedPayloadCallbackSystem = Core::MultiCallbackSystem<const DragPayload &>;

class REFLECT_CLASS(EngineClass) UIDropTargetComponent : public UI::IUIComponent
{
    GENERATED_BODY(UIDropTargetComponent)
  public:
    UIDropTargetComponent();
    virtual ~UIDropTargetComponent();

    virtual void Update(float deltaTime) override;
    virtual int IsPointInside(float x, float y) const override;
    virtual bool IsHovered() const override;
    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;

    void SetDragDropPayloadType(EDragDropType type);

    // hover
    void OnDragDropEnter(const DragPayload &payload);
    void OnDragDropLeave(const DragPayload &payload);
    void OnDroppedPayload(const DragPayload &payload);

    OnDroppedPayloadCallbackSystem mOnDroppedPayloadCallbackSystem;

  private:
    bool mHover;
    bool mDragHover = false;
    EDragDropType mDragPayloadType;
};
