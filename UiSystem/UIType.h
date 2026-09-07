#pragma once

#include "UiSystem/UISystemDllMacro.h"
#include <CoreAsset/AssetPtr.h>
#include <CoreBase/CoreBaseType.h>
#include <InputSystem/InputType.h>
#include <UiSystem/UIType.h>
#include <glm/glm.hpp>
#include <optional>
#include <stdint.h>

namespace CoreAsset
{
class Material;
class Texture;
} // namespace CoreAsset

namespace UI
{
using UIChannelID = uint32_t;
using UICanvasID = uint32_t;
using UIElementID = uint32_t;
using UIPopupScopeID = uint32_t;

class UIElement;
class IUIComponent;
#define InvaildUIChannelID 0
#define InvaildUICanvasID 0
#define InvaildUIElementID 0

enum class ECanvasSizeMode
{
    // 고정된 사이즈
    eFixSize = 0,

    // 스크린사이즈(스크린의 사이즈가 변하면 같이변하는 모드 )
    eScreenSize
};

struct UIVertex
{
    glm::vec2 mPos;
    glm::vec2 mTex;
    uint32_t mColor;
    float mCommonOne;
    float mCommonTwo;
    float mCommonThree;
    float mCommonFour;
    uint32_t mCommonFive;
};

struct UIColor
{
    float mR = 1.0f;
    float mG = 1.0f;
    float mB = 1.0f;
    float mA = 1.0f;

    static const UIColor White;
    static const UIColor LightGray;
    static const UIColor Gray;
    static const UIColor DarkGray;
    static const UIColor DimGray;
    static const UIColor DarkYellow;
};

inline const UIColor UIColor::White = {1.0f, 1.0f, 1.0f, 1.0f};
inline const UIColor UIColor::LightGray = {0.8f, 0.8f, 0.8f, 1.0f};
inline const UIColor UIColor::Gray = {0.4f, 0.4f, 0.4f, 1.0f};
inline const UIColor UIColor::DarkGray = {0.2f, 0.2f, 0.2f, 1.0f};
inline const UIColor UIColor::DimGray = {0.1f, 0.1f, 0.1f, 1.0f};
inline const UIColor UIColor::DarkYellow = {0.5f, 0.5f, 0.0f, 1.0f};

class UIColorUtility
{
  public:
    static uint32_t PackColor(float r, float g, float b, float a)
    {

        uint8_t ur = (r * 255.0f);
        uint8_t ug = (g * 255.0f);
        uint8_t ub = (b * 255.0f);
        uint8_t ua = (a * 255.0f);

        return (ua << 24) | (ub << 16) | (ug << 8) | (ur);
    };

    static uint32_t PackColor(const glm::vec4 &color)
    {
        return PackColor(color.r, color.g, color.b, color.a);
    }
};

struct UIMeshComponent
{
    CoreAsset::Material *mUIMaterial;
    //	CoreAsset::AssetPtr<CoreAsset::Texture> mTexture;           // 진짜 UI 요소별로 다른 것
    glm::vec4 mColor;
};
struct UIMouseInputScopeContext
{
    UIElement *mRoot = nullptr;
};
// 현재 입력에따른 ui매니저에서 유지하는 정보(//정확히 마우스, 향후 이름수정)
struct UIManagerInputStateContext
{
    UI::UIElement *mPreHoverUIElement = nullptr;
    UI::UIElement *mCurrHoverUIElement = nullptr;
    UI::UIElement *mCurrMouseCapturedUIElement = nullptr;
    UI::UIElement *mCurrKeyboardCapturedUIElement = nullptr;
    bool mCaptureEnterFlag = false;

    std::vector<UI::UIMouseInputScopeContext> mMouseInputScopeStack;
};

// 현재 입력에따른 uiElement내에서 유지하는 정보(정확히 마우스,향후 이름수정)
struct UIElementInputStateContext
{
    UI::IUIComponent *mPreHoverUIComponent = nullptr;
    UI::IUIComponent *mCurrHoverUIComponent = nullptr;
    UI::IUIComponent *mCurrMouseCapturedUIComponent = nullptr;
    UI::IUIComponent *mCurrKeyboardCapturedUIComponent = nullptr;
};

enum class EUIMouseHoverType
{
    eNone = 0, // 아무상태도아님
    eHeld,     // hover상태가 유지됨
    eEnter,    // 처음 hover상태로 진입
    eRelease   // hover상태에서 빠져나옴
};

enum class EUIMouseCaptureType
{
    eNone = 0, // 아무상태도아님
    eHeld,     // 캡처가 유지되는 상태
    eEnter,    // 캡처요청이 성공하여 캡처상태로 진입
    eRelease   // 캡처요청해제로 인해 캡처해제상태
};

struct UIManagerMouseInputContext
{
    EUIMouseHoverType mHoverState = EUIMouseHoverType::eNone;
    EUIMouseCaptureType mCaptureState = EUIMouseCaptureType::eNone;
    Quad::MouseContext mMouseContext;
};

enum class EUITextClipingMode
{
    eNone = 0,
    eScissor,
    eEllipsis
};

enum class EUITextOverflowMode
{
    eOverflow = 0, // 계속 옆으로 영역을 벗어나도 (한줄)
    eEllipsis,     // 영역을벗어나는것은 draw되지않는다(한줄)
    eWordWrap,     // 자동으로 다음라인으로 넘어간다. 크기를 조정한다.엔터시 텍스트입력 종료
    eMultiLine,    // 엔터시 줄바꿈
    eScrollHorizontal
};

enum class EUITextAlignment
{
    eLeft = 0,
    eCenter,
    eRight
};

enum class EUITextInputType
{
    eString = 0,
    eNumber, // 실수형 ( 0~ 9 , '-' '.' )
    eInteger // 정수형 (0~9  , '-')
};

enum class EUIPosPivotHorizontal : uint8_t
{
    eNone = 0,
    eLeft,
    eRight
};

enum class EUIPosPivotVertical : uint8_t
{
    eNone = 0,
    eTop,
    eBottom
};

struct UIPosPivotContext
{
    bool mPosPivotActive = false;
    bool mUpdateDirty = false; // 업데이트 여부

    EUIPosPivotHorizontal mPivotHorizontal = EUIPosPivotHorizontal::eNone;
    EUIPosPivotVertical mPivotVertical = EUIPosPivotVertical::eNone;

    float mHorizontalOffset = 0.0f;
    float mVerticalOffset = 0.0f;
};

enum class EUIRenderLayer
{
    eNormal = 0,
    ePopup
};

struct UIPalette
{
    UI::UIColor mWindowBackground;
    UI::UIColor mPanelBackground;
    UI::UIColor mControlBackground;

    UI::UIColor mHover;
    UI::UIColor mPressed;
    UI::UIColor mSelected;
    UI::UIColor mDisabled;

    UI::UIColor mText;
    UI::UIColor mDisabledText;
    UI::UIColor mBorder;
    UI::UIColor mAccent;
};

struct UIMetrics
{
    float mPropertyRowHeight = 32.0f;
    float mControlHeight = 26.0f;

    float mSmallPadding = 4.0f;
    float mPadding = 8.0f;
    float mLargePadding = 12.0f;

    float mItemSpacing = 4.0f;
    float mSectionSpacing = 10.0f;

    float mLabelWidth = 180.0f;
    float mBorderThickness = 1.0f;
    //   float CornerRadius = 2.0f;

    float mDefaultFontSize = 15.0f;
    float mSectionFontSize = 17.0f;
};

enum class EUIStyleRole : uint8_t
{
    eNone = 0,
    eWindow,
    ePanel,
    ePropertyRow,
    eSectionHeader,
    eListItem,
    eButton,
    eTextButton,
    eText,
    eInputBox,
    eCheckBox,
    eDropdown,
    eDropdownItem,
    eScrollBar,
    eSeparator,
    ePopup,
    eIcon,
    eCount,
};

constexpr size_t StyleRoleCount = (uint8_t)EUIStyleRole::eCount + 1;

enum class EUIVisualState : uint8_t
{
    eNormal = 0,
    eHovered,
    eSelected,
    ePressed,
    eFocused,
    eDisabled
};

struct UIControlStyle
{
    UI::UIColor mBackgroundColor;
    UI::UIColor mHoverColor;
    UI::UIColor mPressedColor;
    UI::UIColor mSelectedColor;
    UI::UIColor mDisabledColor;

    UI::UIColor mTextColor;
    UI::UIColor mBorderColor;

    float mHeight = 0.0f;
    float mLeftPadding = 0.0f;
    float mTopPadding = 0.0f;
    float mPadding = 0.0f;
    float mBorderThickness = 0.0f;
    float mFontSize = 0.0f;
};

struct UIControlStyleOverride
{

    std::optional<UI::UIColor> mBackgroundColor;
    std::optional<UI::UIColor> mHoverColor;
    std::optional<UI::UIColor> mPressedColor;
    std::optional<UI::UIColor> mSelectedColor;
    std::optional<UI::UIColor> mDisabledColor;

    std::optional<float> mHeight;
    std::optional<float> mLeftPadding;
    std::optional<float> mTopPadding;
    std::optional<float> mFontSize;
    ;

    void ApplyTo(UIControlStyle &style)
    {
        if (mBackgroundColor)
            style.mBackgroundColor = *mBackgroundColor;

        if (mHoverColor)
            style.mHoverColor = *mHoverColor;

        if (mPressedColor)
            style.mPressedColor = *mPressedColor;

        if (mSelectedColor)
            style.mSelectedColor = *mSelectedColor;

        if (mDisabledColor)
            style.mDisabledColor = *mDisabledColor;

        if (mHeight)
            style.mHeight = *mHeight;

        if (mLeftPadding)
            style.mLeftPadding = *mLeftPadding;

        if (mTopPadding)
            style.mTopPadding = *mTopPadding;

        if (mFontSize)
            style.mFontSize = *mFontSize;
    }
};

} // namespace UI