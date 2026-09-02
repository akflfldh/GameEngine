#include "UIProjectBrowser.h"
#include <UIScrollBox.h>
#include <UiSystem/UIButton.h>

#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>
UIProjectBrowser::UIProjectBrowser()
    : mProjectGenerationButton(nullptr), mProjectPathButton(nullptr), mMessageText(nullptr),
      mMessageBoxExitButton(nullptr)
{
}

UIProjectBrowser::~UIProjectBrowser() {}

void UIProjectBrowser::OnBegin()
{
    UI::UIElement::OnBegin();

    SetSize(1000, 1500);

    auto canvas = GetDestCanvas();
    //    SetUseScissorRect(true);

    mProjectContentScrollBox = canvas->CreateUIElement<UIScrollBox>("ProjectContentScrollBox");
    mProjectContentScrollBox->SetLayout(EUIScrollLayout::eGrid);
    // mProjectContentScrollBox->SetItemHeight(20.0f);
    mProjectContentScrollBox->SetParent(this);
    mProjectContentScrollBox->SetWidth(1000.0f);
    mProjectContentScrollBox->SetHeight(1000.0f);
    mProjectContentScrollBox->SetBackgrounColor(1.0f, 0.4, 0.4);
    mProjectContentScrollBox->SetItemSize(200, 250);

    InitProjectList();

    CreateProjectGenerationBar();
    CreateMessageBox();
}

void UIProjectBrowser::CreateProjectGenerationBar()
{

    auto canvas = GetDestCanvas();
    auto bar = canvas->CreateUIElement<UI::UIImage>("ProjectGenerationBar");

    bar->SetUseScissorRect(false);
    bar->SetParent(this);
    bar->SetWidth(1000.0f);
    bar->SetHeight(150.0f);
    bar->SetColor({0.5f, 0.5f, 0.5f});
    bar->SetPositionLocal({0, 1000.0f});

    float xMargin = 20.0f;
    float yMargin = 10.0f;

    auto projectPathTag = bar->CreateChildUIElement<UI::UIText>("ProjectPathTagText");
    projectPathTag->SetHeight(40.0f);
    projectPathTag->SetFontSize(35.0f);
    projectPathTag->SetTextColor({1, 1, 1});
    projectPathTag->SetText("프로젝트 경로 : ");
    projectPathTag->SetPositionLocal(xMargin, yMargin);

    auto projectPathEditBox = bar->CreateChildUIElement<UI::UIEditBox>("ProjectPathEditBox");
    projectPathEditBox->SetWidth(600.0F);
    projectPathEditBox->SetHeight(50.0f);
    projectPathEditBox->SetBackgroundColor(1.0f, 1.0F, 1.0f);
    projectPathEditBox->SetTextColor(0, 0, 0);
    projectPathEditBox->SetPositionLocal(xMargin + 250.0f, yMargin);
    projectPathEditBox->SetFontSize(35.0f);
    projectPathEditBox->SetClipingMode(UI::EUITextClipingMode::eScissor);
    projectPathEditBox->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);

    mProjectPathButton = bar->CreateChildUIElement<UI::UIButton>("ProjectPathButton");
    mProjectPathButton->SetSize(70, 50);
    mProjectPathButton->mUIImageComponent->SetColor(0.3F, 0.3F, 0.3F);
    mProjectPathButton->SetPositionLocal(projectPathEditBox->mTransform.GetLocalPosition().x +
                                             projectPathEditBox->mTransform.GetSize().x + xMargin,
                                         yMargin);

    auto projectNameTag = bar->CreateChildUIElement<UI::UIText>("ProjectNameTagText");
    projectNameTag->SetHeight(40.0f);
    projectNameTag->SetFontSize(35.0f);
    projectNameTag->SetTextColor({1, 1, 1});
    projectNameTag->SetText("프로젝트 이름 : ");
    projectNameTag->SetPositionLocal(xMargin, 75.0f + yMargin);

    auto projectNameEditBox = bar->CreateChildUIElement<UI::UIEditBox>("ProjectNameEditBox");
    projectNameEditBox->SetWidth(600.0F);
    projectNameEditBox->SetHeight(50.0f);
    projectNameEditBox->SetBackgroundColor(1.0f, 1.0F, 1.0f);
    projectNameEditBox->SetTextColor(0, 0, 0);
    projectNameEditBox->SetPositionLocal(xMargin + 250.0f, 75.0f + yMargin);
    projectNameEditBox->SetFontSize(35.0f);
    projectNameEditBox->SetClipingMode(UI::EUITextClipingMode::eScissor);
    projectNameEditBox->SetOverflowMode(UI::EUITextOverflowMode::eScrollHorizontal);

    mProjectGenerationButton = bar->CreateChildUIElement<UI::UITextButton>("ProjectGenerationButton");
    mProjectGenerationButton->SetSize(70, 50);
    mProjectGenerationButton->SetPositionLocal(projectNameEditBox->mTransform.GetLocalPosition().x +
                                                   projectNameEditBox->mTransform.GetSize().x + xMargin,
                                               75.0F + yMargin);
    mProjectGenerationButton->mTextComponent->SetFontSize(30.0F);
    mProjectGenerationButton->mTextComponent->SetPaddingLeft(5.0f);
    mProjectGenerationButton->mTextComponent->SetText("생성");

    mProjectGenerationButton->mTextComponent->SetClipingMode(UI::EUITextClipingMode::eScissor);
    mProjectGenerationButton->mTextComponent->SetOverflowMode(UI::EUITextOverflowMode::eOverflow);
    mProjectGenerationButton->mTextComponent->SetColor({1, 1, 1});
    mProjectGenerationButton->mUIImageComponent->SetColor(0.3f, 0.3f, 0.3f);

    mProjectGenerationButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(

        [this, projectPathEditBox, projectNameEditBox](float, float)
        {
            std::string projectPath = projectPathEditBox->GetText();
            std::string projectName = projectNameEditBox->GetText();

            mOnProjectGeneartionButtonDownCallbackSystem.ExecuteCallbacks(projectPath, projectName);
        });
}

void UIProjectBrowser::SetProjectList(const std::vector<std::string> &projectList)
{

    mProjectList = projectList;
}

void UIProjectBrowser::InitProjectList()
{

    std::vector<UI::UIElement *> itemList;
    for (const std::string &projectName : mProjectList)
    {
        auto uiProjectItem = CreateUIProjectItem(projectName);
        itemList.push_back(uiProjectItem);
    }
    mProjectContentScrollBox->AddItemList(itemList);
}

UI::UIButton *UIProjectBrowser ::CreateUIProjectItem(const std::string &projectName)
{
    auto canvas = GetDestCanvas();
    auto item = canvas->CreateUIElement<UI::UIButton>("projectItem");
    item->SetSize({400, 250});
    item->mUIImageComponent->SetColor(0.4f, 0.4f, 0.4f);
    item->mHoverImageColor = {0.3f, 0.3f, 0.3f, 1.0f};
    item->mReleaseHoverImageColor = {0.4f, 0.4f, 0.4f};
    item->SetUseHoverImageColor(true);

    item->mUIButtonComponent->mButtonClickCallbackSystem.Register([this, projectName](float, float)
                                                                  { OnSelectedProjectItem(projectName); });

    return item;
}

void UIProjectBrowser::ShowMessageBox(const std::string &text)
{
    mMessageText->SetText(text);
    mMessageBox->SetActiveFlag(true);
    mMessageBox->RequestMouseCaptureInput(nullptr);
}

void UIProjectBrowser::ExitMessageBox()
{
    mMessageBox->SetActiveFlag(false);
    mMessageBox->ReleaseMouseCaptureInput();
}

void UIProjectBrowser::CreateMessageBox()
{
    auto canvas = GetDestCanvas();
    mMessageBox = canvas->CreateUIElement<UI::UIImage>("MessageBox");
    float width = 400;
    float height = 250;

    mMessageBox->SetSize(width, height);
    mMessageBox->SetColor({0.1f, 0.1f, 0.1f});
    mMessageBox->SetPositionLocal(500 - width / 2, 500);

    mMessageBox->SetActiveFlag(false);

    UI::UIButton *exitButton = mMessageBox->CreateChildUIElement<UI::UIButton>("ExitButton");
    exitButton->SetSize(40, 40);
    exitButton->SetPositionLocal(width - 40, 0);
    exitButton->mUIImageComponent->UseTexture();
    exitButton->mUIImageComponent->SetTexture("Engine/Exit");
    exitButton->mUIButtonComponent->mButtonClickCallbackSystem.Register([this](float, float) { ExitMessageBox(); });

    mMessageBoxExitButton = exitButton;

    mMessageText = mMessageBox->CreateChildUIElement<UI::UIText>("Text");
    mMessageText->SetPositionLocal(20, height * 0.6f);
    mMessageText->SetFontSize(15.0f);
    mMessageText->SetTextColor({1, 1, 1});
}

void UIProjectBrowser::OnSelectedProjectItem(const std::string &projectName)
{
    mOnSelectedProjectItemCallbackSystem.ExecuteCallbacks(projectName);
}
