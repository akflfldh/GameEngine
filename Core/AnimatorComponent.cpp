#include "AnimatorComponent.h"
#include <Core/AnimationSystem.h>
#include <CoreAsset/AnimationClip.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/Skeleton.h>
#include <CoreBase/Arch.h>

AnimatorComponent::AnimatorComponent() {}

AnimatorComponent::~AnimatorComponent() {}

void AnimatorComponent::BindSkeleton(CoreAsset::AssetPtr skeleton)
{
    mSkeleton = skeleton;

    auto animSystem = Core::AnimationSystem::GetInstance();

    if (mSkeleton.As<CoreAsset::Skeleton>() != nullptr)
    {
        bool ret = animSystem->SetSkeleton(mHandle, mSkeleton);
    }
}

const CoreAsset::AssetPtr &AnimatorComponent::GetSkeleton() const
{
    return mSkeleton;
}

void AnimatorComponent::Serialize(Arch &arch)
{
    Component::Serialize(arch);

    // 재생 handle은 맵 등록 시 새로 생성되므로 저장하지 않고 Skeleton 에셋 참조만 복원한다.
    // 로드된 참조는 OnOwnerObjectAddedToMap에서 새 handle에 적용된다.
    arch << mSkeleton;
}

bool AnimatorComponent::BuildFinalMatrix(const CoreAsset::SkinBinding &skinBinding,
                                         std::vector<CoreMath::Matrix4X4> &oFinalMatrixList)
{
    // animationSystem - > build
    auto animSystem = Core::AnimationSystem::GetInstance();
    return animSystem->BuildFinalMatrix(mHandle, skinBinding, oFinalMatrixList);
}

void AnimatorComponent::OnOwnerObjectAddedToMap()
{

    Component::OnOwnerObjectAddedToMap();

    // handle
    auto animSystem = Core::AnimationSystem::GetInstance();

    mHandle = animSystem->Register();

    if (mSkeleton.As<CoreAsset::Skeleton>() != nullptr)
    {
        bool ret = animSystem->SetSkeleton(mHandle, mSkeleton);
    }
}

void AnimatorComponent::OnOwnerObjectRemovedFromMap()
{

    Component::OnOwnerObjectRemovedFromMap();

    auto animSystem = Core::AnimationSystem::GetInstance();
    animSystem->UnRegister(mHandle);
}

bool AnimatorComponent::PlayClip(CoreAsset::AssetPtr clip, Core::EAnimationReplayPolicy replayPolicy)
{

    // anim system -  > 전달

    auto animSystem = Core::AnimationSystem::GetInstance();

    return animSystem->ChangeAnimClip(mHandle, clip, replayPolicy);
}

bool AnimatorComponent::PlayClip(const std::string &clipAssetName, Core::EAnimationReplayPolicy replayPolicy)
{

    //

    auto pAnimClip = CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(clipAssetName.c_str());

    if (pAnimClip.Get() == nullptr)
    {
        return false;
    }

    return PlayClip(pAnimClip, replayPolicy);
}

bool AnimatorComponent::SetClipPaused(bool paused)
{
    return Core::AnimationSystem::GetInstance()->SetPaused(mHandle, paused);
}

bool AnimatorComponent::SetManuallyDriven(bool manuallyDriven)
{
    return Core::AnimationSystem::GetInstance()->SetManuallyDriven(mHandle, manuallyDriven);
}

bool AnimatorComponent::AdvanceClip(float deltaTime)
{
    return Core::AnimationSystem::GetInstance()->UpdateSlot(mHandle, deltaTime);
}

bool AnimatorComponent::IsClipFinished() const
{
    return Core::AnimationSystem::GetInstance()->IsClipFinished(mHandle);
}

bool AnimatorComponent::SetClipTime(float time)
{
    return Core::AnimationSystem::GetInstance()->SeekClip(mHandle, time);
}

Core::AnimPlaybackState AnimatorComponent::GetPlayBackState() const
{
    Core::AnimPlaybackState playbackState;

    auto animSystem = Core::AnimationSystem::GetInstance();

    playbackState.mCurrClip = animSystem->GetCurrentClip(mHandle).As<CoreAsset::AnimationClip>();
    playbackState.mCurrPlayTime = animSystem->GetCurrentPlayTime(mHandle);
    playbackState.bPause = animSystem->GetPauseState(mHandle);
    playbackState.bLoop = animSystem->GetLoopState(mHandle);

    return playbackState;
}

void AnimatorComponent::ApplyPlayBackState(const Core::AnimPlaybackState &playbackState)
{

    auto animSystem = Core::AnimationSystem::GetInstance();

    animSystem->ChangeAnimClip(mHandle, playbackState.mCurrClip, Core::EAnimationReplayPolicy::eKeepIfSame);
    animSystem->SetLoopState(mHandle, playbackState.bLoop);
    animSystem->SetPaused(mHandle, playbackState.bPause);

    animSystem->SetReserveCurrTime(mHandle, playbackState.mCurrPlayTime);
}
