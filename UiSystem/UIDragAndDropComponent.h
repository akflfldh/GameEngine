#pragma once

#include <CoreBase/CallbackSystem.h>
#include <UiSystem/IUIComponent.h>

#include "UIDragAndDropComponent.generated.h"

//<target element>
using OnDroppedCallbackSystem = Core::MultiCallbackSystem<UI::UIElement *>;

namespace UI

{
class UISYSTEM_API REFLECT_CLASS(EngineClass) UIDragAndDropComponent : public IUIComponent
{
    GENERATED_BODY(UIDragAndDropComponent)
  public:
    UIDragAndDropComponent();
    virtual ~UIDragAndDropComponent();

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;

    virtual bool IsHovered() const override;

    OnDroppedCallbackSystem mOnDroppedCallbackSystem;

  private:
    bool mHover;
    bool mMouseDown;
};

} // namespace UI