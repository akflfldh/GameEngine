#pragma once

#include <UiSystem/IUIComponent.h>

#include "UISplitterComponent.generated.h"

class UISplitterPanel;

// 분할 막대기 기능 Com
class REFLECT_CLASS(EngineClass) UISplitterComponent : public UI::IUIComponent
{
    GENERATED_BODY(UISplitterComponent)

  public:
    UISplitterComponent();
    virtual ~UISplitterComponent() override;

    void SetSplitterPanel(UISplitterPanel *panel);
    virtual int IsPointInside(float x, float y) const override;
    virtual void Update(float deltaTime) override;

    virtual void OnHover(int x, int y) override;
    virtual void OnReleaseHover() override;
    virtual void OnMouseDown(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                             bool &bConsume) override;
    virtual void OnMouseUp(const Quad::RawInputData &inputData, float worldPosX, float worldPosY,
                           bool &bConsume) override;
    virtual void OnMouseMove(const Quad::RawInputData &inputData, float worldPosX, float worldPosY) override;
    virtual bool IsHovered() const override;

  private:
    bool mHover;
    bool mPress;

    UISplitterPanel *mSplitterPanel;
    float mMouseOffset;
};
