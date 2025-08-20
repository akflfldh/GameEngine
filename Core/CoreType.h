#pragma once

namespace Core
{

enum class EViewportMode
{
    // 앵커모드 ( 상대적비율- 픽셀모드로 나누어짐)
    eAnchored = 0,
    // 고정모드(위치,사이즈가 픽셀값으로 고정됨)
    eFixed
};

enum class EViewportAnchoredMode
{
    eRelative = 0,
    ePixel
};

// 논리적윈도우(하나의창-하나의vieport와 연결된)별 장치입력에 대한 정보
struct LogicalWindowInputContext
{
    bool mMouseCaptureFlag = false;
};

} // namespace Core