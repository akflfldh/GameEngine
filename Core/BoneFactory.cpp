#include "BoneFactory.h"

Quad::BoneFactory::BoneFactory()
    :mPoolAllocator(50000)
{
    mPoolAllocator.Initialize();

}

Quad::BoneFactory::~BoneFactory()
{


}

Quad::Bone* Quad::BoneFactory::CreateBone()
{
    auto instance = GetInstance();
    Bone * bone = instance->mPoolAllocator.GetInstance();

    if(bone == nullptr)
    {
        OutputDebugString(L"bone생성실패 , 할당자최대개수초과\n");
        abort();
    }

    return bone;

}

void Quad::BoneFactory::ReleaseBone(Bone* bone)
{
    auto instance = GetInstance();

    instance->ReleaseBone(bone);
        
}
