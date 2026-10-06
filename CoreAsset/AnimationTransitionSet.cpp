#include "AnimationTransitionSet.h"
#include <CoreAsset/Skeleton.h>
#include <algorithm>

namespace CoreAsset
{

AnimationTransitionSet::AnimationTransitionSet(AssetID id) : Asset(EAssetType::eAnimationTransitionSet, id) {}

EAssetType AnimationTransitionSet::GetAssetType()
{
    return EAssetType::eAnimationTransitionSet;
}

void AnimationTransitionSet::Serialize(Arch &arch)
{
    // 편집용 임시 상태가 아닌 에셋에 적용된 Skeleton과 전이 목록만 저장한다.
    Asset::Serialize(arch);

    arch << mSkeleton;
    arch << mTransitionDataList;
}

bool AnimationTransitionSet::CopyDataFrom(const Asset &source, std::string *failureReason)
{
    if (dynamic_cast<const AnimationTransitionSet *>(&source) == nullptr)
    {
        if (failureReason)
            *failureReason = "AnimationTransitionSet 에셋이 필요합니다.";
        return false;
    }

    // ID와 이름은 복제의 입구인 AssetManager가 처리하며, 여기서는 공통 에셋 데이터만 복사한다.
    bool ret = Asset::CopyDataFrom(source, failureReason);

    if (ret == false)
        return false;

    const AnimationTransitionSet &sourceAnimTransitionData = static_cast<const AnimationTransitionSet &>(source);

    mSkeleton = sourceAnimTransitionData.mSkeleton;
    mTransitionDataList = sourceAnimTransitionData.mTransitionDataList;
    return true;
}

bool AnimationTransitionSet::AddTransitionData(const AnimationTransitionData &data, std::string *oMessage)
{

    if (data.mPreAnimClip == NoneAssetID || data.mNextAnimClip == NoneAssetID)
    {
        if (oMessage)
        {
            *oMessage = "AnimClip 설정이 되지않았습니다.";
        }

        return false;
    }

    for (const auto &entry : mTransitionDataList)
    {
        if (entry.mPreAnimClip == data.mPreAnimClip && entry.mNextAnimClip == data.mNextAnimClip)
        {
            if (oMessage)
            {
                *oMessage = "중복되는AnimTransitionData";
            }

            return false;
        }
    }

    mTransitionDataList.push_back(data);

    return true;
}

bool AnimationTransitionSet::RemoveTransitionData(CoreAsset::AssetID preAnimClip, CoreAsset::AssetID nextAnimClip,
                                                  std::string *oMessage)
{
    for (auto it = mTransitionDataList.begin(); it != mTransitionDataList.end(); ++it)
    {
        if (it->mPreAnimClip == preAnimClip && it->mNextAnimClip == nextAnimClip)
        {
            std::iter_swap(it, mTransitionDataList.end() - 1);
            mTransitionDataList.pop_back();
            return true;
        }
    }

    if (oMessage)
    {
        *oMessage = "해당 하는 TransitionData가 없기에 삭제할 수 없습니다.";
    }

    return false;
}

bool AnimationTransitionSet::OverWriteTransitionData(const AnimationTransitionData &data, std::string *oMessage)
{
    if (data.mPreAnimClip == NoneAssetID || data.mNextAnimClip == NoneAssetID)
    {
        if (oMessage)
        {
            *oMessage = "AnimClip 설정이 되지않았습니다.";
        }
        return false;
    }

    for (auto &entry : mTransitionDataList)
    {
        if (entry.mPreAnimClip == data.mPreAnimClip && entry.mNextAnimClip == data.mNextAnimClip)
        {
            entry = data;

            return true;
        }
    }

    mTransitionDataList.push_back(data);
    return true;
}

void AnimationTransitionSet::SetAnimationSkeleton(AssetPtr skeleton)
{

    if (skeleton.As<CoreAsset::Skeleton>() == nullptr)
    {
        mSkeleton = nullptr;
    }
    else
    {

        mSkeleton = skeleton;
    }
}

const AssetPtr &AnimationTransitionSet::GetAnimationSkeleton() const
{
    return mSkeleton;
}

bool AnimationTransitionSet::GetTransitionData(AssetID preAnimClip, AssetID nextAnimClip,
                                               AnimationTransitionData &oData)
{

    for (const auto &entry : mTransitionDataList)
    {

        if (entry.mPreAnimClip == preAnimClip && entry.mNextAnimClip == nextAnimClip)
        {

            oData = entry;
            return true;
        }
    }

    return false;
}

const std::vector<AnimationTransitionData> &AnimationTransitionSet::GetAnimationTransitionDataList() const
{

    return mTransitionDataList;
}

bool AnimationTransitionSet::SetAnimationTransitionDataList(const std::vector<AnimationTransitionData> &dataList,
                                                           std::string *oMessage)
{
    // 기존 추가 API와 같은 미설정/중복 규칙을 전체 목록에 검사한 뒤 한 번에 교체한다.
    // 실패 시 기존 목록을 유지하며, 클립 쌍 변경으로 이전 전이가 남는 부분 갱신도 피한다.
    for (size_t index = 0; index < dataList.size(); ++index)
    {
        const auto &data = dataList[index];
        if (data.mPreAnimClip == NoneAssetID || data.mNextAnimClip == NoneAssetID)
        {
            if (oMessage)
                *oMessage = "이전/다음 클립이 설정되지 않은 전이가 있습니다.";
            return false;
        }

        for (size_t previous = 0; previous < index; ++previous)
        {
            if (dataList[previous].mPreAnimClip == data.mPreAnimClip &&
                dataList[previous].mNextAnimClip == data.mNextAnimClip)
            {
                if (oMessage)
                    *oMessage = "이전/다음 클립 쌍이 중복되는 전이가 있습니다.";
                return false;
            }
        }
    }

    mTransitionDataList = dataList;
    return true;
}

} // namespace CoreAsset
