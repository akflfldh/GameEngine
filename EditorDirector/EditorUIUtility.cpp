#include "EditorUIUtility.h"
#include <EditorDirector/UIBoolPanel.h>
#include <EditorDirector/UIDropdown.h>
#include <EditorDirector/UIFoldoutPanel.h>
#include <EditorDirector/UIReflectFloatPanel.h>
#include <EditorDirector/UIReflectVector3Panel.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIHorizontalLayoutComponent.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextButton.h>
#include <UiSystem/UITextComponent.h>

UI::UITheme EditorUIUtility::CreateVisualTheme()
{
    UI::UITheme theme;
    UI::UIControlStyle icon;
    icon.mBackgroundColor = UI::UIColor::White;
    theme.SetStyle(UI::EUIStyleRole::eIcon, icon);

    UI::UIControlStyle button;
    button.mBackgroundColor = UI::UIColor::Gray;
    button.mHoverColor = UI::UIColor::DarkGray;
    button.mSelectedColor = UI::UIColor::LightGray;
    theme.SetStyle(UI::EUIStyleRole::eButton, button);

    UI::UIControlStyle textButton;
    textButton.mBackgroundColor = UI::UIColor::DarkGray;
    textButton.mHoverColor = UI::UIColor::Gray;
    textButton.mSelectedColor = UI::UIColor::DimGray;
    theme.SetStyle(UI::EUIStyleRole::eTextButton, textButton);
    theme.SetStyle(UI::EUIStyleRole::eSectionHeader, textButton);
    theme.SetStyle(UI::EUIStyleRole::eListItem, textButton);

    UI::UIControlStyle panel;
    panel.mBackgroundColor = UI::UIColor::DarkGray;
    theme.SetStyle(UI::EUIStyleRole::ePanel, panel);
    theme.SetStyle(UI::EUIStyleRole::ePropertyRow, panel);

    UI::UIControlStyle text;
    text.mTextColor = UI::UIColor::White;
    theme.SetStyle(UI::EUIStyleRole::eText, text);
    UI::UIControlStyle input;
    input.mBackgroundColor = UI::UIColor::DarkGray;
    theme.SetStyle(UI::EUIStyleRole::eInputBox, input);
    return theme;
}

void EditorUIUtility::ApplyTextPreset(UI::UITextComponent *text)
{
    if (!text)
        return;
    text->SetFontSize(FontSize);
    text->SetColor(UI::UIColor::White);
}

void EditorUIUtility::ApplyPreset(UI::UIElement *element, UI::EUIStyleRole role)
{
    if (!element)
        return;
    element->SetStyleRole(role);
    if (role == UI::EUIStyleRole::eNone)
        return;

    // 생성 기본값은 에디터에서 한 번만 적용한다. 상태 갱신에는 색상만 전달한다.
    const auto visual = CreateVisualTheme().GetStyle(role);
    UI::UIControlStyleOverride visualOverride;
    visualOverride.mBackgroundColor = visual.mBackgroundColor;
    visualOverride.mHoverColor = visual.mHoverColor;
    visualOverride.mPressedColor = visual.mPressedColor;
    visualOverride.mSelectedColor = visual.mSelectedColor;
    visualOverride.mDisabledColor = visual.mDisabledColor;
    element->SetStyleOverride(visualOverride);

    UI::UITextComponent *text = nullptr;
    element->GetComponents(&text, 1);
    ApplyTextPreset(text);
    if (auto *editBox = dynamic_cast<UI::UIEditBox *>(element))
    {
        // 입력 커서의 높이도 함께 맞추고, EditBox 자체가 관리하는 배경에 기본색을 지정한다.
        editBox->SetFontSize(FontSize);
        editBox->SetBackgroundColor(visual.mBackgroundColor.mR, visual.mBackgroundColor.mG, visual.mBackgroundColor.mB);
    }

    switch (role)
    {
    case UI::EUIStyleRole::eButton:
        element->SetSize(ButtonSize, ButtonSize);
        break;
    case UI::EUIStyleRole::eText:
        element->SetHeight(ControlHeight);
        if (text)
        {
            // text->SetPaddingLeft(10.0f);
            // text->SetPaddingTop(5.0f);
        }
        break;
    case UI::EUIStyleRole::eTextButton:
        element->SetHeight(ControlHeight);
        if (text)
            text->SetPaddingLeft(10.0f);
        break;
    case UI::EUIStyleRole::eInputBox:
        element->SetHeight(ControlHeight);
        if (text)
            text->SetPaddingTop(5.0f);
        break;
    case UI::EUIStyleRole::eIcon:
        element->SetSize(ControlHeight, ControlHeight);
        break;
    case UI::EUIStyleRole::eSectionHeader:
        element->SetHeight(SectionHeight);
        break;
    case UI::EUIStyleRole::eListItem:
        element->SetHeight(ListItemHeight);
        break;
    case UI::EUIStyleRole::ePropertyRow:
        element->SetHeight(PropertyRowHeight);
        break;
    default:
        // 컨테이너의 전체 크기는 부모/레이아웃이 결정한다.
        break;
    }
}

UI::UIButton *EditorUIUtility::CreateSmallButton(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateSizedButton<UI::UIButton>(canvas, instanceName, ButtonSize, ButtonSize);
}

UI::UIButton *EditorUIUtility::CreateSmallButton(UI::UIElement *parent, const char *instanceName)
{
    return CreateSizedButton<UI::UIButton>(parent, instanceName, ButtonSize, ButtonSize);
}

UI::UITextButton *EditorUIUtility::CreateSmallTextButton(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateSizedButton<UI::UITextButton>(canvas, instanceName, 120.0f, ControlHeight);
}

UI::UITextButton *EditorUIUtility::CreateSmallTextButton(UI::UIElement *parent, const char *instanceName)
{
    return CreateSizedButton<UI::UITextButton>(parent, instanceName, 120.0f, ControlHeight);
}

UI::UIButton *EditorUIUtility::CreateMediumButton(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateSizedButton<UI::UIButton>(canvas, instanceName, MediumButtonSize, MediumButtonSize);
}

UI::UIButton *EditorUIUtility::CreateMediumButton(UI::UIElement *parent, const char *instanceName)
{
    return CreateSizedButton<UI::UIButton>(parent, instanceName, MediumButtonSize, MediumButtonSize);
}

UI::UITextButton *EditorUIUtility::CreateMediumTextButton(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateSizedButton<UI::UITextButton>(canvas, instanceName, 160.0f, MediumTextButtonHeight);
}

UI::UITextButton *EditorUIUtility::CreateMediumTextButton(UI::UIElement *parent, const char *instanceName)
{
    return CreateSizedButton<UI::UITextButton>(parent, instanceName, 160.0f, MediumTextButtonHeight);
}

UI::UIButton *EditorUIUtility::CreateLargeButton(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateSizedButton<UI::UIButton>(canvas, instanceName, LargeButtonSize, LargeButtonSize);
}

UI::UIButton *EditorUIUtility::CreateLargeButton(UI::UIElement *parent, const char *instanceName)
{
    return CreateSizedButton<UI::UIButton>(parent, instanceName, LargeButtonSize, LargeButtonSize);
}

UI::UITextButton *EditorUIUtility::CreateLargeTextButton(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateSizedButton<UI::UITextButton>(canvas, instanceName, 200.0f, LargeTextButtonHeight);
}

UI::UITextButton *EditorUIUtility::CreateLargeTextButton(UI::UIElement *parent, const char *instanceName)
{
    return CreateSizedButton<UI::UITextButton>(parent, instanceName, 200.0f, LargeTextButtonHeight);
}

UI::UIEditBox *EditorUIUtility::CreateInput(UI::UICanvas *canvas, const char *instanceName, bool bNumber)
{
    if (!canvas)
        return nullptr;
    // 입력 종류도 Begin 전에 정한다. getter/setter 바인딩과 입력 결과 처리는 호출부 책임이다.
    return static_cast<UI::UIEditBox *>(canvas->CreateUIElement(
        UI::UIEditBox::GetStaticClassName(), instanceName,
        [bNumber](UI::UIElement *element)
        {
            ApplyPreset(element, UI::EUIStyleRole::eInputBox);
            if (bNumber)
                static_cast<UI::UIEditBox *>(element)->SetTextInputType(UI::EUITextInputType::eNumber);
        }));
}

UI::UIEditBox *EditorUIUtility::CreateInput(UI::UIElement *parent, const char *instanceName, bool bNumber)
{
    if (!parent)
        return nullptr;
    auto *element = CreateInput(parent->GetDestCanvas(), instanceName, bNumber);
    if (element)
        element->SetParent(parent);
    return element;
}

UI::UIText *EditorUIUtility::CreateLabel(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UI::UIText>(canvas, instanceName, UI::EUIStyleRole::eText);
}

UI::UIText *EditorUIUtility::CreateLabel(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UI::UIText>(parent, instanceName, UI::EUIStyleRole::eText);
}

UI::UIEditBox *EditorUIUtility::CreateTextInput(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateInput(canvas, instanceName, false);
}

UI::UIEditBox *EditorUIUtility::CreateTextInput(UI::UIElement *parent, const char *instanceName)
{
    return CreateInput(parent, instanceName, false);
}

UI::UIEditBox *EditorUIUtility::CreateNumberInput(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateInput(canvas, instanceName, true);
}

UI::UIEditBox *EditorUIUtility::CreateNumberInput(UI::UIElement *parent, const char *instanceName)
{
    return CreateInput(parent, instanceName, true);
}

UI::UIImage *EditorUIUtility::CreatePanel(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UI::UIImage>(canvas, instanceName, UI::EUIStyleRole::ePanel);
}

UI::UIImage *EditorUIUtility::CreatePanel(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UI::UIImage>(parent, instanceName, UI::EUIStyleRole::ePanel);
}

UI::UIImage *EditorUIUtility::CreateSectionHeader(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UI::UIImage>(canvas, instanceName, UI::EUIStyleRole::eSectionHeader);
}

UI::UIImage *EditorUIUtility::CreateSectionHeader(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UI::UIImage>(parent, instanceName, UI::EUIStyleRole::eSectionHeader);
}

UI::UIImage *EditorUIUtility::CreatePropertyRow(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UI::UIImage>(canvas, instanceName, UI::EUIStyleRole::ePropertyRow);
}

UI::UIImage *EditorUIUtility::CreatePropertyRow(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UI::UIImage>(parent, instanceName, UI::EUIStyleRole::ePropertyRow);
}

UI::UIImage *EditorUIUtility::CreateIcon(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UI::UIImage>(canvas, instanceName, UI::EUIStyleRole::eIcon);
}

UI::UIImage *EditorUIUtility::CreateIcon(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UI::UIImage>(parent, instanceName, UI::EUIStyleRole::eIcon);
}

UIBoolPanel *EditorUIUtility::CreateBoolField(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UIBoolPanel>(canvas, instanceName, UI::EUIStyleRole::ePropertyRow);
}

UIBoolPanel *EditorUIUtility::CreateBoolField(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UIBoolPanel>(parent, instanceName, UI::EUIStyleRole::ePropertyRow);
}

UIReflectFloatPanel *EditorUIUtility::CreateFloatField(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UIReflectFloatPanel>(canvas, instanceName, UI::EUIStyleRole::ePropertyRow);
}

UIReflectFloatPanel *EditorUIUtility::CreateFloatField(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UIReflectFloatPanel>(parent, instanceName, UI::EUIStyleRole::ePropertyRow);
}

UIReflectVector3Panel *EditorUIUtility::CreateVector3Field(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UIReflectVector3Panel>(canvas, instanceName, UI::EUIStyleRole::eListItem);
}

UIReflectVector3Panel *EditorUIUtility::CreateVector3Field(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UIReflectVector3Panel>(parent, instanceName, UI::EUIStyleRole::eListItem);
}

UIFoldoutPanel *EditorUIUtility::CreateFoldoutPanel(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UIFoldoutPanel>(canvas, instanceName, UI::EUIStyleRole::ePanel);
}

UIFoldoutPanel *EditorUIUtility::CreateFoldoutPanel(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UIFoldoutPanel>(parent, instanceName, UI::EUIStyleRole::ePanel);
}

UIDropdown *EditorUIUtility::CreateDropdown(UI::UICanvas *canvas, const char *instanceName)
{
    return Create<UIDropdown>(canvas, instanceName);
}

UIDropdown *EditorUIUtility::CreateDropdown(UI::UIElement *parent, const char *instanceName)
{
    return Create<UIDropdown>(parent, instanceName);
}

UI::UIButton *EditorUIUtility::CreateSectionHeaderButton(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UI::UIButton>(canvas, instanceName, UI::EUIStyleRole::eSectionHeader);
}

UI::UIButton *EditorUIUtility::CreateSectionHeaderButton(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UI::UIButton>(parent, instanceName, UI::EUIStyleRole::eSectionHeader);
}

UI::UITextButton *EditorUIUtility::CreateListItemButton(UI::UICanvas *canvas, const char *instanceName)
{
    return CreateWithRole<UI::UITextButton>(canvas, instanceName, UI::EUIStyleRole::eListItem);
}

UI::UITextButton *EditorUIUtility::CreateListItemButton(UI::UIElement *parent, const char *instanceName)
{
    return CreateWithRole<UI::UITextButton>(parent, instanceName, UI::EUIStyleRole::eListItem);
}

UI::UIText *EditorUIUtility::CreateLabel(UI::UIElement *parent, const char *instanceName, const std::string &text,
                                         float width)
{
    auto *element = CreateLabel(parent, instanceName);
    if (element)
    {
        element->SetText(text);
        element->SetWidth(width);
    }
    return element;
}

UI::UIText *EditorUIUtility::CreateText(UI::UIElement *parent, const char *instanceName, const std::string &text,
                                        float width)
{
    return CreateLabel(parent, instanceName, text, width);
}

UI::UITextButton *EditorUIUtility::CreateSmallTextButton(UI::UIElement *parent, const char *instanceName,
                                                         const std::string &text, float width)
{
    auto *element = CreateSmallTextButton(parent, instanceName);
    if (element)
    {
        element->SetText(text);
        element->SetWidth(width);
        element->mUIImageComponent->SetUseBorderFlag(true);
    }
    return element;
}

UI::UIElement *EditorUIUtility::CreateHorizontalRow(UI::UIElement *parent, const std::string &instanceName, float width,
                                                    bool bBackground, bool bRightAlign, float itemPaddingX)
{
    if (!parent)
        return nullptr;
    UI::UIElement *row =
        bBackground ? CreatePanel(parent, instanceName.c_str()) : Create<UI::UIElement>(parent, instanceName.c_str());
    if (!row)
        return nullptr;
    UI::UIHorizontalLayoutComponent *com = row->CreateUIComponent<UI::UIHorizontalLayoutComponent>("HoriCom");
    row->SetWidth(width == 0.0f ? parent->GetWidth() : width);

    if (bRightAlign)
        com->SetRightAlign(true);

    com->SetItemPaddingX(itemPaddingX);

    return row;
}
