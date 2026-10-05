#pragma once

#include <Core/AnimHeader.h>
#include <Core/CoreDllExport.h>
#include <CoreAsset/AssetPtr.h>
#include <CoreMath/CoreMath.h>
#include <stack>
#include <stdint.h>
#include <vector>

namespace CoreAsset
{
struct SkinBinding;
}

namespace Core
{

struct AnimRuntimeState
{
    bool mSlotActive = true; // 실제 사용되는 slot 여부

    /*재생 시간 */
    float mCurrTime = 0.0f;

    // 미리 예약한 시간  currTime , deltaTime계산이 무시된다.
    bool mReserveCurrTimeState = false;
    float mReserveCurrTime = 0.0f;
    float mEndTime = 0.0f;
    bool mPause = false;
    bool mLoop = false;
    bool mManuallyDriven = false; ///< 전역 Update 대신 에디터 미리보기 등에서 이 slot만 갱신한다.

    CoreAsset::AssetPtr mSkeleton;
    CoreAsset::AssetPtr mCurrAnimClip;

    std::vector<CoreMath::Matrix4X4> mGlobalPoseBuffer;
};

class CORE_API_LIB AnimationSystem
{
  public:
    // 일단싱글톤으로 향후 시스템들을 공급하는 하나의 공통 통로역할 시스템을 만들자.
    static AnimationSystem *GetInstance();
    AnimationSystem();
    ~AnimationSystem();

    void Update(float deltaTime);
    bool UpdateSlot(AnimRuntimeSlotHandle handle, float deltaTime);
    /// 재생 상태와 관계없이 지정한 절대 시각의 pose를 즉시 평가한다.
    bool SeekClip(AnimRuntimeSlotHandle handle, float timeSeconds);

    AnimRuntimeSlotHandle Register();
    void UnRegister(AnimRuntimeSlotHandle handle);

    bool BuildFinalMatrix(AnimRuntimeSlotHandle handle, const CoreAsset::SkinBinding &skinBinding,
                          std::vector<CoreMath::Matrix4X4> &oFinalMatrix);

    // Anim Clip 교체 요청 메서드
    bool ChangeAnimClip(AnimRuntimeSlotHandle handle, CoreAsset::AssetPtr animClip,
                        Core::EAnimationReplayPolicy replayPolicy);
    bool SetPaused(AnimRuntimeSlotHandle handle, bool paused);
    /// 클립 에셋은 변경하지 않고 해당 slot의 반복 여부와 현재 재생 시각을 갱신한다.
    bool SetLoopState(AnimRuntimeSlotHandle handle, bool loop);
    bool SetManuallyDriven(AnimRuntimeSlotHandle handle, bool manuallyDriven);
    bool IsClipFinished(AnimRuntimeSlotHandle handle) const;

    bool SetSkeleton(AnimRuntimeSlotHandle handle, CoreAsset::AssetPtr skeleton);

    CoreAsset::AssetPtr GetCurrentClip(AnimRuntimeSlotHandle handle) const;
    float GetCurrentPlayTime(AnimRuntimeSlotHandle handle) const;
    bool GetPauseState(AnimRuntimeSlotHandle handle) const;
    bool GetLoopState(AnimRuntimeSlotHandle handle) const;

    void SetReserveCurrTime(AnimRuntimeSlotHandle handle, float time);

  private:
    void UpdateState(AnimRuntimeState &state, float deltaTime);
    bool EvaluatePose(const AnimRuntimeState &state, float timeSeconds,
                      std::vector<CoreMath::Matrix4X4> &outGlobalPose) const;

    AnimRuntimeSlotHandle GetAvailableSlotHandle();

    bool CheckVaildHandle(AnimRuntimeSlotHandle handle) const;

  private:
    // 향후 포인터리스트로 변경을 고려
    std::vector<AnimRuntimeState> mRuntimeStateList;
    std::vector<uint32_t> mGenList;

    std::vector<uint32_t> mAvailableIndexList;
};

} // namespace Core
