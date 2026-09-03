#pragma once

#include "UiSystem/UISystemDllMacro.h"
#include <CoreAsset/AssetPtr.h>
#include <CoreBase/CoreBaseType.h>
#include <InputSystem/InputType.h>
#include <UiSystem/UIType.h>
#include <glm/glm.hpp>
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
};

struct UIColor
{
    float mR = 1.0f;
    float mG = 1.0f;
    float mB = 1.0f;
    float mA = 1.0f;
};

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

} // namespace UI