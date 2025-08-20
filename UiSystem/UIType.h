#pragma once

#include "UiSystem/UISystemDllMacro.h"
#include <CoreAsset/AssetPtr.h>
#include <CoreMath/CoreMath.h>
#include <InputSystem/InputType.h>
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
    CoreMath::Vector2 mPos;
    CoreMath::Vector2 mTex;
    CoreMath::Vector4 mColor;
};

struct UIMeshComponent
{
    CoreAsset::Material *mUIMaterial;
    //	CoreAsset::AssetPtr<CoreAsset::Texture> mTexture;           // 진짜 UI 요소별로 다른 것
    CoreMath::Vector4 mColor;
};

// 현재 입력에따른 ui매니저에서 유지하는 정보(//정확히 마우스, 향후 이름수정)
struct UIManagerInputStateContext
{
    UI::UIElement *mPreHoverUIElement = nullptr;
    UI::UIElement *mCurrHoverUIElement = nullptr;
    UI::UIElement *mCurrCapturedUIElement = nullptr;
    bool mCaptureEnterFlag = false;
};

// 현재 입력에따른 uiElement내에서 유지하는 정보(정확히 마우스,향후 이름수정)
struct UIElementInputStateContext
{
    UI::IUIComponent *mPreHoverUIComponent = nullptr;
    UI::IUIComponent *mCurrHoverUIComponent = nullptr;
    UI::IUIComponent *mCurrCapturedUIComponent = nullptr;
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

} // namespace UI