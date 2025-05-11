#include "AnimationClipSplitter.h"
#include"Asset/Animation/AnimationClip.h"
#include"ResourceManager/AnimationClipManager.h"

Quad::AnimationClip* Quad::AnimationClipSplitter::SplitAnimationClip(AnimationClip* sourceAnimationClip, const std::string& newAnimationClipName, int startFrame, int endFrame)
{
    if (sourceAnimationClip == nullptr)
        return nullptr;

    if (AnimationClipManager::GetAnimationClip(newAnimationClipName) != nullptr)
    {

        MessageBox(nullptr, L"에니메이션클립이름중복생성", L"에러", 0);
        return nullptr;
    }

    int sourceEndFrame = sourceAnimationClip->GetEndFrameCount();
    if (startFrame >= endFrame)
    {   
        MessageBox(nullptr, L"에니메이션분할 프레임시작끝범위에러", L"에러", 0);
        return nullptr;
    }

    if (startFrame > sourceEndFrame)
    {
        MessageBox(nullptr, L"에니메이션분할 시작프레임>원본프레임끝", L"에러", 0);
        return nullptr;
    }


    if (startFrame < 0 )
        startFrame = 0;

    if (sourceEndFrame < endFrame)
        endFrame = sourceEndFrame;
    







    const  std::vector<BoneAnimationClip>& boneAnimationClipVector = sourceAnimationClip->GetBoneAnimationClipVector();
    std::vector<BoneAnimationClip> newBoneAnimationClipVector(boneAnimationClipVector.size());
    

    //각 boneAnimationClip마다 keyFrameVector생성(startFrame ~ endFrame)
    int keyFrameNum = endFrame - startFrame +1;


    for (int i = 0; i < newBoneAnimationClipVector.size(); ++i)
    {

        newBoneAnimationClipVector[i].SetBoneID(boneAnimationClipVector[i].GetBoneID());

    

        //본 에니메이션클립에대한 keyframe vector생성 , 복사 
        const std::vector<AnimationKeyFrame>& keyFrameVector = boneAnimationClipVector[i].GetKeyFrameVector();
        std::vector<AnimationKeyFrame> newKeyFrameVector(keyFrameNum);
        
        if (keyFrameVector.size() == 0)
            continue;


        float newStartTime = keyFrameVector[startFrame].mTime;
        std::copy(keyFrameVector.begin() + startFrame, keyFrameVector.begin() + endFrame + 1, newKeyFrameVector.begin());
       
        //새로운클립의 keyframe time의 시작을 0으로 맞추어야함으로 시작시간을 뺴준다 
        std::for_each(newKeyFrameVector.begin(), newKeyFrameVector.end(), [newStartTime](AnimationKeyFrame& keyFrame) mutable { keyFrame.mTime -= newStartTime; });
        newBoneAnimationClipVector[i].SetAnimationKeyFrameVector(std::move(newKeyFrameVector));

    }

    //float newEndTime = endFrame
    
    
   // AnimationClip* newAnimationClip = new AnimationClip;
    AnimationClip* newAnimationClip = AnimationClipManager::CreateAnimationClip(newAnimationClipName);
    newAnimationClip->SetBoneAnimationClipVector(std::move(newBoneAnimationClipVector));
  //  newAnimationClip->SetName(newAnimationClipName);
    newAnimationClip->SetTimeMode(sourceAnimationClip->GetTimeMode());
    newAnimationClip->SetFrameEndCount(keyFrameNum - 1);  
    
    newAnimationClip->SetDefaultMesh(sourceAnimationClip->GetDefaultMesh());
    newAnimationClip->SetDefaultSkeleton(sourceAnimationClip->GetDefaultSkeleton());
    

    return newAnimationClip;
}
