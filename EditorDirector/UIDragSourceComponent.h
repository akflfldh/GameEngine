#pragma once

#include <EditorDirector/GlobalOverlayType.h>
#include <UiSystem/IUIComponent.h>

#include "UIDragSourceComponent.generated.h"
class REFLECT_CLASS(EngineClass) UIDragSourceComponent : public UI::IUIComponent
{
    GENERATED_BODY(UIDragSourceComponent)
  public:
    UIDragSourceComponent();
    virtual ~UIDragSourceComponent();
    virtual void Update(float deltaTime) override;

    virtual int IsPointInside(float x, float y) const override;

    virtual bool IsHovered() const override;

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;

    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;

    virtual void OnPreviewMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                                    bool &bSteal) override;

    void OnPreviewMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY, bool &bSteal) override;

    void SetPayload(const DragPayload &payload);

  private:
    bool mHover;
    bool mPress;

    float mMouseDownStartX;
    float mMouseDownStartY;

    DragPayload mPayload;

    bool mDragStart = false;
};
