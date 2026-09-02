#pragma once

#include <UiSystem/UIElement.h>

#include "UISplitterPanel.generated.h"

enum class EUISplitterPanelOrientation
{
    eHorizontal,
    eVertical
};

class REFLECT_CLASS(EngineClass) UISplitterPanel : public UI::UIElement
{

    GENERATED_BODY(UISplitterPanel)
  public:
    UISplitterPanel();
    virtual ~UISplitterPanel();

    virtual void Update(float deltaTime) override;

    void SetFirstChildElement(UI::UIElement *child);
    void SetSecondChildElement(UI::UIElement *child);

    void UpdateLayout();

    void OnSplitterDragged(float delta);

    void SetHandleWidth(float width);

  private:
    virtual void OnBegin() override;

  private:
    EUISplitterPanelOrientation mOrientation;

    UI::UIElement *mFirstChildElement;
    UI::UIElement *mSecondChildElement;
    UI::UIElement *mHandleElement; // 조절막대

    float mHandleWidth = 10.0f;

    //[0.0f, 1.0f]
    float mSplitRatio = 0.3f;
};
