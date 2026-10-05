#include "AnimationSystem.h"

#include <CoreAsset/AnimationClip.h>
#include <CoreAsset/Skeleton.h>
#include <CoreAsset/SkinningMesh.h>

#include <algorithm>
#include <cmath>

Core::AnimationSystem *Core::AnimationSystem::GetInstance()
{

    static AnimationSystem instance;
    return &instance;
}

Core::AnimationSystem::AnimationSystem() {}

Core::AnimationSystem::~AnimationSystem() {}

void Core::AnimationSystem::Update(float deltaTime)
{
    for (auto &state : mRuntimeStateList)
    {
        if (!state.mManuallyDriven)
            UpdateState(state, deltaTime);
    }
}

bool Core::AnimationSystem::UpdateSlot(AnimRuntimeSlotHandle handle, float deltaTime)
{
    if (!CheckVaildHandle(handle))
        return false;

    // 에디터 미리보기처럼 특정 Animator만 갱신할 때도 일반 재생과 같은 샘플링 규칙을 사용한다.
    UpdateState(mRuntimeStateList[handle.mIndex], deltaTime);
    return true;
}

bool Core::AnimationSystem::SeekClip(AnimRuntimeSlotHandle handle, float timeSeconds)
{
    if (!CheckVaildHandle(handle) || !std::isfinite(timeSeconds))
        return false;

    auto &state = mRuntimeStateList[handle.mIndex];
    const auto *skeleton = state.mSkeleton.As<CoreAsset::Skeleton>();
    const auto *clip = state.mCurrAnimClip.As<CoreAsset::AnimationClip>();
    if (skeleton == nullptr || clip == nullptr || !clip->IsCompatible(*skeleton) || !clip->GetValid())
        return false;

    const float duration = clip->GetDurationSeconds();
    float sampleTime = 0.0f;
    if (duration > 0.0f)
    {
        if (state.mLoop)
        {
            // loop 계약은 [0, duration)이므로 끝 시각과 음수 입력도 동일한 구간으로 정규화한다.
            sampleTime = std::fmod(timeSeconds, duration);
            if (sampleTime < 0.0f)
                sampleTime += duration;
        }
        else
        {
            // non-loop 미리보기는 마지막 sample을 표시할 수 있도록 닫힌 구간으로 제한한다.
            sampleTime = std::clamp(timeSeconds, 0.0f, duration);
        }
    }

    // 일시정지 상태에서도 즉시 자세가 바뀌어야 하므로 UpdateState의 시간 전진 경로를 사용하지 않는다.
    // 실패 시 기존 시간과 pose를 유지하도록 별도 버퍼에서 완성한 후 교체한다.
    std::vector<CoreMath::Matrix4X4> pose;
    if (!EvaluatePose(state, sampleTime, pose))
        return false;

    state.mCurrTime = sampleTime;
    state.mGlobalPoseBuffer.swap(pose);
    return true;
}

void Core::AnimationSystem::UpdateState(AnimRuntimeState &state, float deltaTime)
{
    if (!state.mSlotActive || state.mPause)
        return;

    CoreAsset::Skeleton *skeleton = state.mSkeleton.As<CoreAsset::Skeleton>();
    CoreAsset::AnimationClip *currAnimClip = state.mCurrAnimClip.As<CoreAsset::AnimationClip>();
    if (skeleton == nullptr || currAnimClip == nullptr)
        return;

    float clipEndTime = currAnimClip->GetDurationSeconds();

    if (state.mReserveCurrTimeState)
    {
        state.mCurrTime = state.mReserveCurrTime;
        state.mReserveCurrTimeState = false;
    }
    else
    {
        state.mCurrTime += deltaTime;
    }

    if (clipEndTime < state.mCurrTime)
    {
        if (state.mLoop)
            state.mCurrTime -= clipEndTime;
        else
            state.mCurrTime = clipEndTime;
    }

    EvaluatePose(state, state.mCurrTime, state.mGlobalPoseBuffer);
}

bool Core::AnimationSystem::EvaluatePose(const AnimRuntimeState &state, float timeSeconds,
                                         std::vector<CoreMath::Matrix4X4> &outGlobalPose) const
{
    const auto *skeleton = state.mSkeleton.As<CoreAsset::Skeleton>();
    const auto *currAnimClip = state.mCurrAnimClip.As<CoreAsset::AnimationClip>();
    if (skeleton == nullptr || currAnimClip == nullptr || !currAnimClip->GetValid() ||
        !currAnimClip->IsCompatible(*skeleton))
        return false;

    const auto &joints = skeleton->GetJoints();
    outGlobalPose.resize(joints.size());
    for (size_t i = 0; i < joints.size(); ++i)
    {
        const auto &joint = joints[i];

        CoreAsset::AnimationLocalTransform sampledJointLocalTransform;
        std::string failReason;

        bool ret = currAnimClip->SampleJoint(joint.mStableKey, timeSeconds, state.mLoop, joint.mReferenceLocalPose,
                                             sampledJointLocalTransform, &failReason);

        if (ret == false)
            return false;

        outGlobalPose[i] =
            CoreMath::Matrix4X4::MakeTransform(sampledJointLocalTransform.mPosition,
                                               sampledJointLocalTransform.mRotation, sampledJointLocalTransform.mScale);

        // Skeleton의 parent-first 순서에 따라 local을 부모 global에 합성한다.
        if (joint.mParentIndex != joint.NoParent)
            outGlobalPose[i] = outGlobalPose[joint.mParentIndex] * outGlobalPose[i];
    }

    return true;
}

Core::AnimRuntimeSlotHandle Core::AnimationSystem::Register()
{

    AnimRuntimeSlotHandle handle = GetAvailableSlotHandle();

    if (mRuntimeStateList.size() <= handle.mIndex)
    {
        AnimRuntimeState state;
        mRuntimeStateList.push_back(std::move(state));
    }
    else
    {
        // 재사용 slot의 재생·일시정지·수동 갱신 설정을 새 Animator가 물려받지 않도록 초기화한다.
        mRuntimeStateList[handle.mIndex] = AnimRuntimeState{};
    }
    mRuntimeStateList[handle.mIndex].mSlotActive = true;

    return handle;
}

void Core::AnimationSystem::UnRegister(AnimRuntimeSlotHandle handle)
{

    // Gen 체크

    if (CheckVaildHandle(handle) == false)
        return;

    mRuntimeStateList[handle.mIndex].mSlotActive = false;
    mRuntimeStateList[handle.mIndex].mCurrAnimClip = nullptr;
    mRuntimeStateList[handle.mIndex].mSkeleton = nullptr;

    mAvailableIndexList.push_back(handle.mIndex);
}

bool Core::AnimationSystem::BuildFinalMatrix(AnimRuntimeSlotHandle handle, const CoreAsset::SkinBinding &skinBinding,
                                             std::vector<CoreMath::Matrix4X4> &oFinalMatrix)
{

    if (CheckVaildHandle(handle) == false)
        return false;

    CoreAsset::AnimationClip *currAnimClip =
        mRuntimeStateList[handle.mIndex].mCurrAnimClip.As<CoreAsset::AnimationClip>();

    if (currAnimClip == nullptr)
        return false;

    const auto &globalMatrixList = mRuntimeStateList[handle.mIndex].mGlobalPoseBuffer;

    // if (skinBinding.mInverseBindMatrices.size() == 0 ||
    //     skinBinding.mInverseBindMatrices.size() != skinBinding.mPaletteToSkeletonJoint.size())
    //     return false;

    auto skeleton = mRuntimeStateList[handle.mIndex].mSkeleton.As<CoreAsset::Skeleton>();

    if (skeleton == nullptr)
        return false;

    if (!skinBinding.Validate(*skeleton))
        return false;

    oFinalMatrix.resize(skinBinding.mInverseBindMatrices.size());

    for (size_t i = 0; i < skinBinding.mInverseBindMatrices.size(); ++i)
    {

        uint32_t jointIndex = skinBinding.mPaletteToSkeletonJoint[i];

        oFinalMatrix[i] =
            mRuntimeStateList[handle.mIndex].mGlobalPoseBuffer[jointIndex] * skinBinding.mInverseBindMatrices[i];
    }

    return true;
}

bool Core::AnimationSystem::ChangeAnimClip(AnimRuntimeSlotHandle handle, CoreAsset::AssetPtr pAnimClip,
                                           Core::EAnimationReplayPolicy replayPolicy)
{

    if (CheckVaildHandle(handle) == false)
        return false;

    auto &state = mRuntimeStateList[handle.mIndex];

    CoreAsset::Skeleton *skeleton = state.mSkeleton.As<CoreAsset::Skeleton>();
    if (skeleton == nullptr)
        return false;

    CoreAsset::AnimationClip *animClip = pAnimClip.As<CoreAsset::AnimationClip>();

    if (animClip == nullptr || animClip->IsCompatible(*skeleton) == false)
    {
        return false;
    }

    if (replayPolicy == EAnimationReplayPolicy::eKeepIfSame &&
        animClip->GetID() == mRuntimeStateList[handle.mIndex].mCurrAnimClip.GetAssetID() && !IsClipFinished(handle))
    {

        return true;
    }

    mRuntimeStateList[handle.mIndex].mCurrAnimClip = animClip;
    mRuntimeStateList[handle.mIndex].mCurrTime = 0.0f;
    mRuntimeStateList[handle.mIndex].mPause = false;
    mRuntimeStateList[handle.mIndex].mLoop = animClip->GetLoop();

    return true;
}

bool Core::AnimationSystem::SetPaused(AnimRuntimeSlotHandle handle, bool paused)
{
    if (!CheckVaildHandle(handle))
        return false;

    auto &state = mRuntimeStateList[handle.mIndex];
    if (state.mCurrAnimClip.As<CoreAsset::AnimationClip>() == nullptr)
        return false;

    // 시간을 변경하지 않아 재개 시 마지막으로 샘플링한 자세에서 이어진다.
    state.mPause = paused;
    return true;
}

bool Core::AnimationSystem::SetLoopState(AnimRuntimeSlotHandle handle, bool loop)
{
    if (!CheckVaildHandle(handle))
        return false;

    auto &state = mRuntimeStateList[handle.mIndex];
    if (state.mCurrAnimClip.As<CoreAsset::AnimationClip>() == nullptr)
        return false;

    if (state.mLoop == loop)
        return true;

    // 반복은 이 slot의 재생 설정이며 공유 클립 에셋의 설정은 변경하지 않는다.
    // SeekClip으로 loop [0, duration), non-loop [0, duration] 규칙을 적용하여
    // 일시정지 중에도 변경된 반복 모드에 맞는 시간과 자세를 즉시 유지한다.
    const bool previousLoop = state.mLoop;
    state.mLoop = loop;
    if (!SeekClip(handle, state.mCurrTime))
    {
        // SeekClip은 실패 시 시간과 자세를 유지하므로 반복 설정도 함께 복원한다.
        state.mLoop = previousLoop;
        return false;
    }

    return true;
}

bool Core::AnimationSystem::SetManuallyDriven(AnimRuntimeSlotHandle handle, bool manuallyDriven)
{
    if (!CheckVaildHandle(handle))
        return false;

    // 동시에 활성화된 다른 World가 전역 Update를 실행해도 미리보기 slot을 중복 전진시키지 않는다.
    mRuntimeStateList[handle.mIndex].mManuallyDriven = manuallyDriven;
    return true;
}

bool Core::AnimationSystem::IsClipFinished(AnimRuntimeSlotHandle handle) const
{
    if (!CheckVaildHandle(handle))
        return false;

    const auto &state = mRuntimeStateList[handle.mIndex];
    const auto *clip = state.mCurrAnimClip.As<CoreAsset::AnimationClip>();
    return clip != nullptr && !state.mLoop && state.mCurrTime >= clip->GetDurationSeconds();
}

bool Core::AnimationSystem::SetSkeleton(AnimRuntimeSlotHandle handle, CoreAsset::AssetPtr pSkeleton)
{

    if (CheckVaildHandle(handle) == false)
        return false;

    CoreAsset::Skeleton *skeleton = pSkeleton.As<CoreAsset::Skeleton>();

    if (skeleton == nullptr)
    {
        return false;
    }

    mRuntimeStateList[handle.mIndex].mSkeleton = pSkeleton;

    mRuntimeStateList[handle.mIndex].mGlobalPoseBuffer.resize(skeleton->GetJointNum());

    return true;
}

CoreAsset::AssetPtr Core::AnimationSystem::GetCurrentClip(AnimRuntimeSlotHandle handle) const
{

    if (CheckVaildHandle(handle) == false)
        return nullptr;

    return mRuntimeStateList[handle.mIndex].mCurrAnimClip;
}

float Core::AnimationSystem::GetCurrentPlayTime(AnimRuntimeSlotHandle handle) const
{
    if (CheckVaildHandle(handle) == false)
        return 0.0f;

    return mRuntimeStateList[handle.mIndex].mCurrTime;
}

bool Core::AnimationSystem::GetPauseState(AnimRuntimeSlotHandle handle) const
{
    if (CheckVaildHandle(handle) == false)
        return false;

    return mRuntimeStateList[handle.mIndex].mPause;
}

bool Core::AnimationSystem::GetLoopState(AnimRuntimeSlotHandle handle) const
{
    if (CheckVaildHandle(handle) == false)
        return false;

    return mRuntimeStateList[handle.mIndex].mLoop;
}

void Core::AnimationSystem::SetReserveCurrTime(AnimRuntimeSlotHandle handle, float time)
{

    if (CheckVaildHandle(handle) == false)
        return;

    mRuntimeStateList[handle.mIndex].mReserveCurrTimeState = true;
    mRuntimeStateList[handle.mIndex].mReserveCurrTime = time;
}

Core::AnimRuntimeSlotHandle Core::AnimationSystem::GetAvailableSlotHandle()
{
    uint32_t index = 0;
    if (mAvailableIndexList.empty() == false)
    {
        index = mAvailableIndexList.back();
        mAvailableIndexList.pop_back();
    }
    else
    {
        index = mRuntimeStateList.size();
    }

    if (index >= mGenList.size())
    {
        mGenList.push_back(0);
    }

    mGenList[index]++;

    AnimRuntimeSlotHandle handle;

    handle.mIndex = index;
    handle.mGen = mGenList[index];

    return handle;
}

bool Core::AnimationSystem::CheckVaildHandle(AnimRuntimeSlotHandle handle) const
{
    if (mGenList.size() <= handle.mIndex || mGenList[handle.mIndex] != handle.mGen)
        return false;

    // 이미 비활성화
    if (mRuntimeStateList.size() <= handle.mIndex || mRuntimeStateList[handle.mIndex].mSlotActive == false)
        return false;

    return true;
}
