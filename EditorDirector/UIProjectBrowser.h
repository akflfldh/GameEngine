#pragma once

#include <UiSystem/UIElement.h>

#include <CoreBase/CallbackSystem.h>

#include "UIProjectBrowser.generated.h"

//<projectName>
using OnSelectedProjectItemCallbackSysmem = Core::MultiCallbackSystem<const std::string &>;
//<projectPath , projectName>,
using OnProjectGenerationButtonDownCallbackSystem = Core::MultiCallbackSystem<const std::string &, const std::string &>;

namespace UI
{
class UITextButton;
class UIButton;
class UIImage;
class UIText;
} // namespace UI

class UIScrollBox;

class REFLECT_CLASS(EngineClass) UIProjectBrowser : public UI::UIElement
{
    GENERATED_BODY(UIProjectBrowser)
  public:
    UIProjectBrowser();
    virtual ~UIProjectBrowser();

    void SetProjectList(const std::vector<std::string> &projectList);
    OnProjectGenerationButtonDownCallbackSystem mOnProjectGeneartionButtonDownCallbackSystem;

    OnSelectedProjectItemCallbackSysmem mOnSelectedProjectItemCallbackSystem;

    void ShowMessageBox(const std::string &text);
    void ExitMessageBox();

  protected:
    virtual void OnBegin();

    void InitProjectList();
    void CreateProjectGenerationBar();
    UI::UIButton *CreateUIProjectItem(const std::string &projectName);

    void CreateMessageBox();

    void OnSelectedProjectItem(const std::string &projectName);

  private:
    UIScrollBox *mProjectContentScrollBox;
    UI::UITextButton *mProjectGenerationButton;
    UI::UIButton *mProjectPathButton;

    std::vector<std::string> mProjectList;
    UI::UIImage *mMessageBox;
    UI::UIText *mMessageText;
    UI::UIButton *mMessageBoxExitButton;
};
