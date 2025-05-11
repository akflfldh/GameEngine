#include "ResourceManager/AnimationClipManager.h"
#include"Asset/Animation/AnimationClip.h"



Quad::AnimationClipManager::AnimationClipManager()
	:mPoolAllocator(10000)
{

	mPoolAllocator.Initialize();

}

Quad::AnimationClipManager::~AnimationClipManager()
{



}

Quad::AnimationClip* Quad::AnimationClipManager::CreateAnimationClip(const std::string& name)
{
	
	auto instance = GetInstance();

	if (GetAnimationClip(name) != nullptr)
		return nullptr;

	AnimationClip* animClip = instance->mPoolAllocator.GetInstance();

	animClip->SetName(name);
	instance->mIDTable.Register(animClip);
	instance->mNameTable.Register(name,animClip->GetUniqueID());

	return animClip;
}

Quad::AnimationClip* Quad::AnimationClipManager::CreateAnimationClipFromFile(const std::string& name, unsigned long long id)
{
	auto instance = GetInstance();
	if (GetAnimationClip(name) != nullptr)
		return nullptr;

	AnimationClip* animClip = instance->mPoolAllocator.GetInstance();
	animClip->SetName(name);
	instance->mIDTable.RegisterFromFile(animClip,id);
	instance->mNameTable.Register(name, animClip->GetUniqueID());

	return animClip;
}

Quad::AnimationClip* Quad::AnimationClipManager::GetAnimationClip(const std::string& name)
{

	auto instance = GetInstance();
	unsigned long long id =	instance->mNameTable.GetID(name);
	
	return GetAnimationClip(id);


}

Quad::AnimationClip* Quad::AnimationClipManager::GetAnimationClip(unsigned long long id)
{
	auto instance = GetInstance();
	if (id == 0)
		return nullptr;

	AnimationClip* animClip = instance->mIDTable.GetElement(id);

	return animClip;
}
