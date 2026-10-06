#pragma once

#include <UiSystem/UICanvas.h>
#include <UiSystem/UIElement.h>
#include <string>

namespace UI
{
class UIButton;
class UIEditBox;
class UIImage;
class UIText;
class UITextButton;
class UITextComponent;
} // namespace UI
class UIBoolPanel;
class UIDropdown;
class UIFoldoutPanel;
class UIReflectFloatPanel;
class UIReflectVector3Panel;

// 에디터의 생성 규격만 소유한다. UI의 수명·입력·레이아웃은 기존 Canvas/컴포넌트가 관리한다.
// 게임 UI에는 이 유틸리티를 사용하지 않으며, 크기와 위치는 생성 후 명시적으로 조정할 수 있다.
class EditorUIUtility
{
  public:
    static constexpr float ControlHeight = 28.0f;
    static constexpr float ButtonSize = 18.0f;
    static constexpr float MediumButtonSize = 30.0f;
    static constexpr float LargeButtonSize = 48.0f;
    static constexpr float MediumTextButtonHeight = 32.0f;
    static constexpr float LargeTextButtonHeight = 40.0f;
    static constexpr float ListItemHeight = 34.0f;
    static constexpr float SectionHeight = 31.0f;
    static constexpr float PropertyRowHeight = 38.0f;
    static constexpr float FontSize = 18.0f;

    static UI::UITheme CreateVisualTheme();
    static void ApplyTextPreset(UI::UITextComponent *text);
    static void ApplyPreset(UI::UIElement *element, UI::EUIStyleRole role);

    template <typename T> static T *Create(UI::UICanvas *canvas, const char *instanceName)
    {
        if (!canvas)
            return nullptr;
        // 복합 UI는 Begin에서 자신의 높이를 참조하므로 기본값을 사후 적용하지 않는다.
        return static_cast<T *>(canvas->CreateUIElement(T::GetStaticClassName(), instanceName,
                                                        [](UI::UIElement *element)
                                                        { ApplyPreset(element, element->GetStyleRole()); }));
    }

    template <typename T> static T *Create(UI::UIElement *parent, const char *instanceName)
    {
        if (!parent)
            return nullptr;
        T *element = Create<T>(parent->GetDestCanvas(), instanceName);
        if (element)
            element->SetParent(parent);
        return element;
    }

    // 아이콘 버튼: 18/30/48 정사각형, 텍스트 버튼: 높이 28/32/40.
    // 크기를 외부 속성 묶음으로 받지 않고 이름으로 선택하며, 폭은 이후 레이아웃에서 조정할 수 있다.
    static UI::UIButton *CreateSmallButton(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIButton *CreateSmallButton(UI::UIElement *parent, const char *instanceName);
    static UI::UITextButton *CreateSmallTextButton(UI::UICanvas *canvas, const char *instanceName);
    static UI::UITextButton *CreateSmallTextButton(UI::UIElement *parent, const char *instanceName);
    static UI::UIButton *CreateMediumButton(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIButton *CreateMediumButton(UI::UIElement *parent, const char *instanceName);
    static UI::UITextButton *CreateMediumTextButton(UI::UICanvas *canvas, const char *instanceName);
    static UI::UITextButton *CreateMediumTextButton(UI::UIElement *parent, const char *instanceName);
    static UI::UIButton *CreateLargeButton(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIButton *CreateLargeButton(UI::UIElement *parent, const char *instanceName);
    static UI::UITextButton *CreateLargeTextButton(UI::UICanvas *canvas, const char *instanceName);
    static UI::UITextButton *CreateLargeTextButton(UI::UIElement *parent, const char *instanceName);

    // 생성 시 공통 규격은 함수 이름으로 선택한다. 컨테이너의 전체 크기와 데이터 바인딩은
    // 호출부/복합 UI가 담당하며, 이 함수들은 콜백이나 게임/에셋 데이터를 소유하지 않는다.
    static UI::UIText *CreateLabel(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIText *CreateLabel(UI::UIElement *parent, const char *instanceName);
    static UI::UIEditBox *CreateTextInput(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIEditBox *CreateTextInput(UI::UIElement *parent, const char *instanceName);
    static UI::UIEditBox *CreateNumberInput(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIEditBox *CreateNumberInput(UI::UIElement *parent, const char *instanceName);
    static UI::UIImage *CreatePanel(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIImage *CreatePanel(UI::UIElement *parent, const char *instanceName);
    static UI::UIImage *CreateSectionHeader(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIImage *CreateSectionHeader(UI::UIElement *parent, const char *instanceName);
    static UI::UIImage *CreatePropertyRow(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIImage *CreatePropertyRow(UI::UIElement *parent, const char *instanceName);
    static UI::UIImage *CreateIcon(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIImage *CreateIcon(UI::UIElement *parent, const char *instanceName);
    static UIBoolPanel *CreateBoolField(UI::UICanvas *canvas, const char *instanceName);
    static UIBoolPanel *CreateBoolField(UI::UIElement *parent, const char *instanceName);
    static UIReflectFloatPanel *CreateFloatField(UI::UICanvas *canvas, const char *instanceName);
    static UIReflectFloatPanel *CreateFloatField(UI::UIElement *parent, const char *instanceName);
    static UIReflectVector3Panel *CreateVector3Field(UI::UICanvas *canvas, const char *instanceName);
    static UIReflectVector3Panel *CreateVector3Field(UI::UIElement *parent, const char *instanceName);
    static UIFoldoutPanel *CreateFoldoutPanel(UI::UICanvas *canvas, const char *instanceName);
    static UIFoldoutPanel *CreateFoldoutPanel(UI::UIElement *parent, const char *instanceName);
    static UIDropdown *CreateDropdown(UI::UICanvas *canvas, const char *instanceName);
    static UIDropdown *CreateDropdown(UI::UIElement *parent, const char *instanceName);
    static UI::UIButton *CreateSectionHeaderButton(UI::UICanvas *canvas, const char *instanceName);
    static UI::UIButton *CreateSectionHeaderButton(UI::UIElement *parent, const char *instanceName);
    static UI::UITextButton *CreateListItemButton(UI::UICanvas *canvas, const char *instanceName);
    static UI::UITextButton *CreateListItemButton(UI::UIElement *parent, const char *instanceName);

    static UI::UIText *CreateLabel(UI::UIElement *parent, const char *instanceName, const std::string &text,
                                   float width);
    static UI::UIText *CreateText(UI::UIElement *parent, const char *instanceName, const std::string &text,
                                  float width);
    static UI::UITextButton *CreateSmallTextButton(UI::UIElement *parent, const char *instanceName,
                                                   const std::string &text, float width);
    static UI::UIElement *CreateHorizontalRow(UI::UIElement *parent, const std::string &instanceName,
                                              float width = 0.0f, bool bBackground = false, bool bRightAlign = false,
                                              float itemPaddingX = 0.0f);

  private:
    template <typename T>
    static T *CreateWithRole(UI::UICanvas *canvas, const char *instanceName, UI::EUIStyleRole role)
    {
        if (!canvas)
            return nullptr;
        // 복합 UI의 Begin 및 부모 레이아웃 참여 전에 해당 역할의 기본 높이/폰트를 확정한다.
        return static_cast<T *>(canvas->CreateUIElement(
            T::GetStaticClassName(), instanceName, [role](UI::UIElement *element) { ApplyPreset(element, role); }));
    }

    template <typename T>
    static T *CreateWithRole(UI::UIElement *parent, const char *instanceName, UI::EUIStyleRole role)
    {
        if (!parent)
            return nullptr;
        auto *element = CreateWithRole<T>(parent->GetDestCanvas(), instanceName, role);
        if (element)
            element->SetParent(parent);
        return element;
    }

    static UI::UIEditBox *CreateInput(UI::UICanvas *canvas, const char *instanceName, bool bNumber);
    static UI::UIEditBox *CreateInput(UI::UIElement *parent, const char *instanceName, bool bNumber);

    template <typename T>
    static T *CreateSizedButton(UI::UICanvas *canvas, const char *instanceName, float width, float height)
    {
        if (!canvas)
            return nullptr;
        // Canvas가 이미 Begin된 경우에도 최초 레이아웃은 선택한 고정 규격으로 시작한다.
        return static_cast<T *>(canvas->CreateUIElement(T::GetStaticClassName(), instanceName,
                                                        [width, height](UI::UIElement *element)
                                                        {
                                                            ApplyPreset(element, element->GetStyleRole());
                                                            element->SetSize(width, height);
                                                        }));
    }

    template <typename T>
    static T *CreateSizedButton(UI::UIElement *parent, const char *instanceName, float width, float height)
    {
        if (!parent)
            return nullptr;
        auto *element = CreateSizedButton<T>(parent->GetDestCanvas(), instanceName, width, height);
        if (element)
            element->SetParent(parent);
        return element;
    }
};
