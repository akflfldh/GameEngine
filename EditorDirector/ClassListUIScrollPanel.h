#pragma once

#include "ClassListUIScrollPanel.generated.h"
#include <CoreBase/CallbackSystem.h>
#include <UiSystem/UIImage.h>
#include <unordered_map>
using OnClickedClassItemCallbackSystem = Core::MultiCallbackSystem<const std::string &>;

class UIScrollBox;
class UIHierarchyItem;

class REFLECT_CLASS(EngineClass) ClassListUIScrollPanel : public UI::UIImage
{
    GENERATED_BODY(ClassListUIScrollPanel)
  public:
    ClassListUIScrollPanel();
    virtual ~ClassListUIScrollPanel();

    virtual void OnBegin() override;

    void SetScrollPanelColor(float r, float g, float b);
    OnClickedClassItemCallbackSystem mOnClickedClassItemCallbackSystem;

    void Refresh();

  private:
    UIHierarchyItem *GetItem();

  private:
    UIScrollBox *mScrollBox;
    UI::UIColor mScrollPanelColor;
    std::unordered_map<std::string, UIHierarchyItem *> mItemTable;
};
