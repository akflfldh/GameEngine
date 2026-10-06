#pragma once

#include <Core/AnimHeader.h>
#include <Core/Component.h>
#include <CoreAsset/AssetPtr.h>
#include <CoreMath/CoreMath.h>
#include <string>
#include <vector>

#include "AnimatorComponent.generated.h"

namespace CoreAsset
{
struct SkinBinding;
}

/// 오브젝트의 Skeleton 에셋 선택을 보관하고 맵에 등록된 동안 AnimationSystem의 재생 slot과 연결한다.
/// Skeleton 에셋이나 최종 skin palette는 소유하지 않으며, slot handle은 직렬화하지 않는다.
class CORE_API_LIB REFLECT_CLASS(EngineClass) AnimatorComponent : public Component
{
    GENERATED_BODY(AnimatorComponent)
  public:
    AnimatorComponent();
    ~AnimatorComponent() override;

    void BindSkeleton(CoreAsset::AssetPtr skeleton);
    const CoreAsset::AssetPtr &GetSkeleton() const;

    bool BindTransitionSet(CoreAsset::AssetPtr set);
    CoreAsset::AssetPtr GetTransitionSet() const;

    void Serialize(Arch &arch) override;

    bool BuildFinalMatrix(const CoreAsset::SkinBinding &skinBinding,
                          std::vector<CoreMath::Matrix4X4> &oFinalMatrixList);

    bool PlayClip(CoreAsset::AssetPtr clip,
                  Core::EAnimationReplayPolicy replayPolicy = Core::EAnimationReplayPolicy::eKeepIfSame);
    bool PlayClip(const std::string &clipAssetName,
                  Core::EAnimationReplayPolicy replayPolicy = Core::EAnimationReplayPolicy::eKeepIfSame);
    bool SetClipPaused(bool paused);
    /// 미리보기에서 전역 Update와 이 Animator의 개별 갱신이 중복되지 않게 한다.
    bool SetManuallyDriven(bool manuallyDriven);
    /// 에디터 미리보기처럼 전역 AnimationSystem::Update와 분리된 slot만 전진시킬 때 사용한다.
    bool AdvanceClip(float deltaTime);
    bool IsClipFinished() const;

    /// 절대 시각(초)으로 이동하고 일시정지 중에도 즉시 pose를 갱신한다.
    bool SetClipTime(float time);

    Core::AnimPlaybackState GetPlayBackState() const;
    void ApplyPlayBackState(const Core::AnimPlaybackState &playbackState);

  protected:
    virtual void OnOwnerObjectAddedToMap();
    virtual void OnOwnerObjectRemovedFromMap();

  private:
    /// 프리팹 복제·저장 시 ID로 유지되며 맵 등록 후 런타임 slot에 적용된다.
    CoreAsset::AssetPtr mSkeleton;
    CoreAsset::AssetPtr mAnimTransitionSet;

    Core::AnimRuntimeSlotHandle mHandle;
};
