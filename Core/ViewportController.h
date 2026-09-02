#pragma once

#include "Core/CoreType.h"
#include "CoreDllExport.h"
#include <RenderSystem/RenderType.h>
namespace Core
{
struct ViewportFixedContext
{
    int mLeft = 0.0f;
    int mTop = 0.0f;
    int mWidth = 0.0f;
    int mHeight = 0.0f;
};

struct ViewportAnchor
{
    bool mAnchorOn = false;
    EViewportAnchoredMode mAnchorMode;

    // 각 화면사각형의 모서리에서 얼만큼떨어졌는가
    // ex) bottom의 mRel = 0.2이면 viewport의 bottom이 화면bottom모서리에서 height * 0.2 만큼 떨어진다.
    float mRel = 0.0f;   // 상대적모드일때 사용
    uint32_t mPixel = 0; // pixel 모드일때 사용
};

// 축의 단일앵커만 활성화시 적용되는 사이즈
struct ViewportAnchorSize
{
    // 사이즈도 창의 상대적으로 비례할수도있고
    // 고정된 사이즈일수도있다.
    EViewportAnchoredMode mAnchorMode;

    float mRel = 0.0f;
    uint32_t mPixel = 0;
};

struct ViewportAnchoredContext
{
    ViewportAnchor mLeftAnchor;
    ViewportAnchor mRightAnchor;
    ViewportAnchor mTopAnchor;
    ViewportAnchor mBottomAnchor;

    // 각축에대해 두 앵커가 모두 활성화 -> width 무시

    // 각 축에대해서 단일 앵커만 활성활될때 ->width 적용.
    // 각 축의 두 앵커모두 비활성화라면 Fixed모드의 값을 자동 사용
    ViewportAnchorSize mWidth;
    ViewportAnchorSize mHeight;
};

class CORE_API_LIB ViewportController
{
  public:
    ViewportController();
    ~ViewportController();

    void SetViewportMode(EViewportMode mode);

    void UpdateWindowSize(uint32_t width, uint32_t height);
    Render::Viewport GetViewport() const;
    float GetAspect() const;

    void SetFixedPos(int left, int top);
    void SetFixedSize(int width, int height);

    void SetAnchorLeftState(bool state);
    void SetAnchorRightState(bool state);
    void SetAnchorTopState(bool state);
    void SetAnchorBottomState(bool state);

    void SetAnchorLeftMode(EViewportAnchoredMode mode);
    void SetAnchorRightMode(EViewportAnchoredMode mode);
    void SetAnchorTopMode(EViewportAnchoredMode mode);
    void SetAnchorBottomMode(EViewportAnchoredMode mode);

    // 앵커의 상대적위치값 설정
    void SetAnchorLeftRelValue(float value);
    void SetAnchorRightRelValue(float value);
    void SetAnchorTopRelValue(float value);
    void SetAnchorBottomRelValue(float value);

    void SetAnchorLeftPixelValue(uint32_t value);
    void SetAnchorRightPixelValue(uint32_t value);
    void SetAnchorTopPixelValue(uint32_t value);
    void SetAnchorBottomPixelValue(uint32_t value);

    void SetAnchorWidthSizeMode(EViewportAnchoredMode mode);
    void SetAnchorHeightSizeMode(EViewportAnchoredMode mode);

    void SetAnchorWidthRelValue(float value);
    void SetAnchorHeightRelValue(float value);

    void SetAnchorWidthPixelValue(uint32_t value);
    void SetAnchorHeightPixelValue(uint32_t value);

    // get

    // ViewportController.h (추가)

    EViewportMode GetViewportMode() const;

    // 정확히 LogicalWindow의 크기
    std::pair<uint32_t, uint32_t> GetWindowSize() const; // 필요시

    // Fixed
    int GetFixedLeft() const;
    int GetFixedTop() const;
    int GetFixedWidth() const;
    int GetFixedHeight() const;

    // Anchors - state
    bool GetAnchorLeftState() const;
    bool GetAnchorRightState() const;
    bool GetAnchorTopState() const;
    bool GetAnchorBottomState() const;

    // Anchors - mode
    EViewportAnchoredMode GetAnchorLeftMode() const;
    EViewportAnchoredMode GetAnchorRightMode() const;
    EViewportAnchoredMode GetAnchorTopMode() const;
    EViewportAnchoredMode GetAnchorBottomMode() const;

    // Anchors - values
    float GetAnchorLeftRelValue() const;
    float GetAnchorRightRelValue() const;
    float GetAnchorTopRelValue() const;
    float GetAnchorBottomRelValue() const;

    uint32_t GetAnchorLeftPixelValue() const;
    uint32_t GetAnchorRightPixelValue() const;
    uint32_t GetAnchorTopPixelValue() const;
    uint32_t GetAnchorBottomPixelValue() const;

    // Anchor size mode
    EViewportAnchoredMode GetAnchorWidthSizeMode() const;
    EViewportAnchoredMode GetAnchorHeightSizeMode() const;

    // Anchor size values
    float GetAnchorWidthRelValue() const;
    float GetAnchorHeightRelValue() const;
    uint32_t GetAnchorWidthPixelValue() const;
    uint32_t GetAnchorHeightPixelValue() const;

    // Utility
    // 현재 viewport에대해서 창에대한 위치를 받아 ndc공간으로 변환한것을 돌려준다.
    void ConvertToNdc(float &oClientPosX, float &oClientPosY, float viewportOffsetTopLeftX = 0.0f,
                      float viewportOffsetTopLeftY = 0.0f) const;

    // 점의 교차판정
    bool IntersectPoint(int px, int py, float viewportOffsetTopLeftX = 0.0f, float viewportOffsetTopLeftY = 0.0f);

  private:
    void Update() const;

    // left,right
    void UpdateAnchoredModeAxisX() const;
    // top,bottom
    void UpdateAnchoredModeAxisY() const;

    //[0 1]
    float ClampRelAnchor(float relValue);

    //[0,windowWidth],[0,windowHeight]
    int ClampPixelWidthValue(int value);
    int ClampPixelHeightValue(int value);

  private:
    // 클라이언트영역
    uint32_t mWindowWidth = 0.0f;
    uint32_t mWindowHeight = 0.0f;

    // 앵커모드인 절대고정모드인지

    // 앵커모드이면 크기 width,height가 의미가없어질것

    // 앵커모드에는 창 사이즈에 상대적인 모드와  픽셀모드 가 존재

    // 앵커모드가 아니고 절대고정모드이면 left,top ,width, height를 그대로 설정
    // 또는 left top, bottom, right로 처리해도됨

    EViewportMode mViewportMode;
    EViewportAnchoredMode mViewportAnchoredMode;
    ViewportAnchoredContext mAnchoredContext;
    // true: Fixed , false : Relative
    // viewport의 위치 고정용, 비율용

    // true : Fixed , false :Relative;
    // viewport 사이즈 고정용, 비율용

    ViewportFixedContext mFixedContext;

    mutable Render::Viewport mViewport;
    mutable bool mDirtyFlag;
};

} // namespace Core