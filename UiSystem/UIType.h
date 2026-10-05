#pragma once

#include "UiSystem/UISystemDllMacro.h"
#include <CoreAsset/AssetPtr.h>
#include <CoreBase/CoreBaseType.h>
#include <CoreMath/CoreMath.h>
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

    static const UIColor Red;
    static const UIColor Blue;
    static const UIColor White;
    static const UIColor LightGray;
    static const UIColor Gray;
    static const UIColor DarkGray;
    static const UIColor DimGray;
    static const UIColor DarkYellow;

    CoreMath::Vector3 ConvertVector3()
    {
        return {mR, mG, mB};
    }
    CoreMath::Vector4 ConvertVector4()
    {
        return {mR, mG, mB, mA};
    }
};

inline const UIColor UIColor::White = {1.0f, 1.0f, 1.0f, 1.0f};
inline const UIColor UIColor::Red = {1.0f, 0.0f, 0.0f, 1.0f};
inline const UIColor UIColor::Blue = {0.0f, 0.0f, 1.0f, 1.0f};
inline const UIColor UIColor::LightGray = {0.3f, 0.3f, 0.3f, 1.0f};
inline const UIColor UIColor::Gray = {0.1f, 0.1f, 0.1f, 1.0f};
inline const UIColor UIColor::DarkGray = {0.05f, 0.05f, 0.05f, 1.0f};
inline const UIColor UIColor::DimGray = {0.01f, 0.01f, 0.01f, 1.0f};
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

// 부모(최상위 요소는 Canvas) 영역의 비율 기준점과 절대 이동량을 보관한다.
// 실제 크기와 부모의 수명은 소유하지 않으며, 위치 적용은 UIElement가 담당한다.
struct UIPosAnchorContext
{
    bool mPosAnchorActive = false;
    bool mUpdateDirty = false; // 업데이트 여부

    // 한 축만 고정한 기존 UI는 나머지 축의 수동/레이아웃 위치를 유지한다.
    bool mHorizontalAnchorActive = false;
    bool mVerticalAnchorActive = false;

    CoreMath::Vector2 mAnchor = {0.0f, 0.0f}; // 부모 영역의 [0, 1] 비율 기준점.
    CoreMath::Vector2 mOffset = {0.0f, 0.0f}; // +X는 오른쪽, +Y는 아래쪽인 절대 이동량.
    // 자체 정렬 기준점은 후속 구현 대상이다. 현재는 좌상단 (0, 0)으로 두고 계산에는 사용하지 않는다.
    CoreMath::Vector2 mPivot = {0.0f, 0.0f};
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

// 공통 UI가 입력 상태에 따라 적용하는 시각 정보만 보관한다. 크기·폰트·배치는 소유하지 않는다.
struct UIControlStyle
{
    UI::UIColor mBackgroundColor;
    UI::UIColor mHoverColor;
    UI::UIColor mPressedColor;
    UI::UIColor mSelectedColor;
    UI::UIColor mDisabledColor;

    UI::UIColor mTextColor;
    UI::UIColor mBorderColor;
};

struct UIControlStyleOverride
{

    std::optional<UI::UIColor> mBackgroundColor;
    std::optional<UI::UIColor> mHoverColor;
    std::optional<UI::UIColor> mPressedColor;
    std::optional<UI::UIColor> mSelectedColor;
    std::optional<UI::UIColor> mDisabledColor;

    void ApplyTo(UIControlStyle &style) const
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
    }
};

enum class UIRenderRole
{
    eImage = 0,
    eFont
};

} // namespace UI
