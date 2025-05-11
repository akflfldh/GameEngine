#include "ResourceManager/SkeletonManager.h"
#include"Asset/Skeleton/Skeleton.h"
Quad::SkeletonManager::SkeletonManager()
    :mSkeletonAllocator(1000)
{
    mSkeletonAllocator.Initialize();


}
Quad::Skeleton* Quad::SkeletonManager::CreateSkeleton(const std::string& name)
{
    auto instance = GetInstance();


    if (GetSkeleton(name) != nullptr)
        return nullptr;

    Skeleton * skeleton =   instance->mSkeletonAllocator.GetInstance();
    skeleton->SetName(name);


    instance->mIDTable.Register(skeleton);
    instance->mNameTable.Register(name, skeleton->GetUniqueID());


    return skeleton;

}
Quad::Skeleton* Quad::SkeletonManager::CreateSkeletonFromFile(const std::string& name, unsigned long long id)
{
    auto instance = GetInstance();


    Skeleton* skeleton = instance->mSkeletonAllocator.GetInstance();
    skeleton->SetName(name);


    instance->mIDTable.RegisterFromFile(skeleton, id);
    instance->mNameTable.Register(name, id);

    return skeleton;
}
//bool Quad::SkeletonManager::AddSkeleton(Skeleton* skeleton)
//{
//
//    auto instance = GetInstance();
//    
//      std::pair<std::unordered_map<std::string,Skeleton*>::iterator,bool> ret =
//          instance->mSkeletonUnMap.try_emplace(skeleton->GetName(), skeleton);
//
//      if (!ret.second)
//      {
//          ;
//      } //디버그출력;
//
//  return ret.second;
//
//
//}

Quad::Skeleton* Quad::SkeletonManager::GetSkeleton(const std::string& name)
{
    auto instance = GetInstance();
    unsigned long long id = instance->mNameTable.GetID(name);

    return GetSkeleton(id);
}

Quad::Skeleton* Quad::SkeletonManager::GetSkeleton(unsigned long long id)
{
    auto instance = GetInstance();
    return instance->mIDTable.GetElement(id);
}
