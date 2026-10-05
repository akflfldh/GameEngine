#pragma once
#include <CoreAsset/AssetPtr.h>
#include <stdint.h>

namespace Core
{
struct AnimRuntimeSlotHandle
{
    uint32_t mIndex = 0;
    uint32_t mGen = 0; // 세대
};

enum class EAnimationReplayPolicy
{
    eKeepIfSame, // 같은 클립이 현재 재생 중이면 시간 유지
    eRestart     // 같은 클립이라도 처음부터 시작
};

struct AnimPlaybackState
{
    CoreAsset::AssetPtr mCurrClip = nullptr;
    float mCurrPlayTime = 0.0f;
    bool bPause = false;
    bool bLoop = false;
};

} // namespace Core